using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Net;
using System.Net.Sockets;
using ITVaganza;

namespace Server {
  public partial class Form1 : Form {
    private UdpClient udp;
    private int udp_port;
    private IPEndPoint ip_udp;
    private IPEndPoint ip_broadcast;
    private int broadcast_delay = 3;
    private int client_port = 3217;
    private int globalDelay = 127;

    private TcpListener tcp;
    private int tcp_port;

    private Boolean queueEnabled = true;
    private int lastQueueSize = 0;
    private User userDatabase;

    private Dictionary<String, ClientHandle> clientList = new Dictionary<String, ClientHandle>();
    private Dictionary<String, String> userList = new Dictionary<String, String>();
    private Dictionary<String, String> userGame = new Dictionary<String, String>();
    private Dictionary<String, BSGame> gameList = new Dictionary<String, BSGame>();

    public Form1() {
      InitializeComponent();
    }

    #region Log Functions
    delegate void LogCallback(string text);
    private void log(String format, params object[] args) {
      log(String.Format(format, args));
    }
    private void log(String message) {
      if(!tbLog.IsDisposed) {
        if(tbLog.InvokeRequired) {
          tbLog.Invoke(new LogCallback(log), message);
        } else {
          tbLog.AppendText(message + Environment.NewLine);
          tbLog.Refresh();
        }
      }
    }
    #endregion

    #region Form Events
    private void Form1_Load(object sender, EventArgs e) {
      log("Connecting Login Server..");
      this.userDatabase = new User();
      log("Connected To Login Server.");
      Random rnd = new Random(DateTime.Now.Second * 1000 + DateTime.Now.Millisecond);
      this.udp_port = 10000 + rnd.Next(0, 10) * 1000 + rnd.Next(0, 10) * 100 + rnd.Next(0, 10) * 10 + rnd.Next(0, 10);
      this.tcp_port = this.udp_port + 10000;
      bw_UDPBroadcast.RunWorkerAsync();
      bw_GameServer.RunWorkerAsync();
      log("Starting Game Server..");
      this.tcp = new TcpListener(new IPEndPoint(IPAddress.Any, this.tcp_port));
      this.tcp.Start();
      log("Game Server Started On Port : {0}", this.tcp_port);
      log("Begin Accepting Connection.");
      startAcceptConnection();
      tbLog.Focus();
    }
    private void Form1_FormClosing(object sender, FormClosingEventArgs e) {
      bw_UDPBroadcast.CancelAsync();
      disconnectAll();
      this.Refresh();
    }
    private void tbCommand_KeyPress(object sender, KeyPressEventArgs e) {
      if(e.KeyChar == (char)13) {
        doCommand(((TextBox)sender).Text);
        ((TextBox)sender).Clear();
      }
    }
    #endregion

    private void doCommand(String command) {
      if(!String.IsNullOrEmpty(command)) {
        log("> {0}", command);
        String[] commands = command.Split(' ');
        commands[0] = commands[0].ToUpper();
        if(commands[0] == "BROADCAST") {
          String message = "";
          for(int i = 1; i < commands.Length; i++) { message += commands[i] + " "; }
          message = message.Substring(0, message.Length - 1);
          if(!String.IsNullOrEmpty(message)) {
            log("Broadcasting To {0} Client(s).", clientList.Count);
            foreach(KeyValuePair<String, ClientHandle> entry in clientList) {
              if(entry.Value.Connected) {
                entry.Value.Send(message);
                log("[->]::[ID:{0}]::[MSG:{1}]", entry.Value.ConnectionID, message);
              }
            }
          } else { log("> Error: Empty Message"); }
        } else if(commands[0] == "EXIT" || commands[0] == "QUIT") {
          Application.Exit();
        } else if(commands[0] == "LIST") {
          listAll();
        } else if(commands[0] == "GAME") {
          listAllGame();
        } else if(commands[0] == "HELP") {
          displayHelp();
        } else {
          log("> Error: Unrecognized Command \"{0}\"", command);
        }
      } else {
        displayHelp();
      }
    }

    private void displayHelp() {
      log("> -- COMMAND HELP --");
      log("> help : display this message.");
      log("> list : List all connected client. Display User if available");
      log("> game : List all available games, and display user in game.");
      log("> broadcast <message> : Send <message> to all active client.");
      log("> exit : Close and Exit Server.");
      log("> quit : Close and Exit Server.");
    }
    private void disconnectAll() {
      log("Disconnecting {0} Client(s).", clientList.Count);
      foreach(KeyValuePair<String, ClientHandle> entry in clientList) {
        if(entry.Value.Connected) {
          entry.Value.Send(ClientHandle._DISCONNECT);
          log("[x]::[ID:{0}]", entry.Value.ConnectionID);
        }
      }
    }
    private void listAll() {
      log("Listing {0} Client(s).", clientList.Count);
      Dictionary<String, ClientHandle> tmpClient = new Dictionary<String, ClientHandle>(clientList);
      foreach(KeyValuePair<String, ClientHandle> entry in tmpClient) {
        if(entry.Value.Connected) {
          log("[+]::[ID:{0}]{1}", entry.Value.ConnectionID, (userList.ContainsKey(entry.Key) ? String.Format("::[USER:{0}]", userList[entry.Key]) : ""));
          if(userGame.ContainsKey(entry.Key)) {
            log("     +->[GAME:{0}]", userGame[entry.Key]);
          }
        }
      }
    }
    private void listAllGame() {
      log("Listing {0} Game(s)", gameList.Count);
      Dictionary<String, BSGame> tmpGame = new Dictionary<String, BSGame>(gameList);
      foreach(KeyValuePair<String, BSGame> entry in tmpGame) {
        BSGame tmp = entry.Value;
        log("[+]::[GAME]::[ID:{0}]", entry.Key);
        for(int i = 0; i < tmp.PlayerCount; i++) {
          if(userList.ContainsKey(tmp.playerIdForIndex(i))) { log("     +->[USER:{0}]::[OBJECT:{2}]::[ID:{1}]", userList[tmp.playerIdForIndex(i)], tmp.playerIdForIndex(i), tmp.lifeForPlayer(i)); }
        }
      }
    }

    #region Broadcasting Server
    private void bw_UDPBroadcast_DoWork(object sender, DoWorkEventArgs e) {
      byte[] msg = System.Text.Encoding.UTF8.GetBytes(Environment.MachineName);
      IPHostEntry ipEntry = Dns.GetHostEntry(Environment.MachineName);
      IPAddress[] addr = ipEntry.AddressList;
      bw_UDPBroadcast.ReportProgress(0, String.Format("Retaining IP Address of {0}", Environment.MachineName));
      for(int i = 0; i < addr.Length; i++) {
        if(addr[i].AddressFamily == AddressFamily.InterNetwork) {
          ip_udp = new IPEndPoint(addr[i], this.udp_port);
          bw_UDPBroadcast.ReportProgress(0, String.Format("Selected IP Address {0}:{1}", addr[i].ToString(), this.udp_port));
          break;
        }
      }
      bw_UDPBroadcast.ReportProgress(0, "Starting Broadcast Server..");
      this.udp = new UdpClient(ip_udp);
      this.udp.EnableBroadcast = true;
      this.ip_broadcast = new IPEndPoint(IPAddress.Broadcast, this.client_port);
      bw_UDPBroadcast.ReportProgress(0, "Broadcast Server Started.");
      bw_UDPBroadcast.ReportProgress(0, String.Format("Broadcasting every {0} second{1}.", broadcast_delay, (broadcast_delay > 1 ? "s" : "")));

      DateTime start = DateTime.Now;
      DateTime end = DateTime.Now;
      while(!bw_UDPBroadcast.CancellationPending) {
        TimeSpan diff = end.Subtract(start);
        if(diff.Seconds >= broadcast_delay) { start = DateTime.Now; this.udp.Send(msg, msg.Length, ip_broadcast); }
        end = DateTime.Now;
        System.Threading.Thread.Sleep(broadcast_delay * 100);
      }

      bw_UDPBroadcast.ReportProgress(0, "Shutting Down Broadcast Server..");
    }
    private void bw_UDPBroadcast_ProgressChanged(object sender, ProgressChangedEventArgs e) {
      if(e.ProgressPercentage == 0) {
        log((String)e.UserState);
      }
    }
    private void bw_UDPBroadcast_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e) {
      log("Broadcast Server Closed.");
      System.Threading.Thread.Sleep(globalDelay);
    }
    #endregion

    #region TCP Server & Main Game Server
    private void startAcceptConnection() {
      tcp.BeginAcceptTcpClient(HandleAsyncConnection, tcp);
    }
    private void HandleAsyncConnection(IAsyncResult res) {
      startAcceptConnection();
      TcpClient tcpClient = tcp.EndAcceptTcpClient(res);
      ClientHandle client = new ClientHandle(tcpClient);
      log("[o]::[ID:{0}]", client.ConnectionID);
      client.OnRead += new ClientHandle.GEventHandler(client_OnRead);
      client.OnDisconnected +=new ClientHandle.GEventHandler(client_OnDisconnected);
      clientList.Add(client.ConnectionID, client);
    }

    private void client_OnRead(Object sender, GEventArgs e) {
      ClientHandle client = ((ClientHandle)sender);
      log("[<-]::[ID:{0}]::[MSG:{1}]", client.ConnectionID, e.Message);
      String msg = e.Message;

      #region Login Handling
      if(msg.ToUpper().StartsWith("LOGIN:")) {
        String[] info = msg.Split('#');
        String username = info[1];
        Boolean userValid = true;
        userValid = userDatabase.login(username) > 0 ? true : false;

        if(userValid) {
          log("[LOGIN]::[USER:{0}]::[OK]", username);
          userList.Add(client.ConnectionID, username);
          client.State = ClientState.QUEUEING;
          client.Send("GAME:#MSG#WAITING_QUEUE#");
        } else {
          log("[LOGIN]::[USER:{0}]::[INVALID]", username);
          client.Send("MSG:#Invalid Username!#");
        }
      }
      #endregion Login Handling

      #region Logout Handling
      if(msg.ToUpper().StartsWith("LOGOUT:")) {
        String[] info = msg.Split('#');
        String username = info[1];
        if (userList.ContainsKey(client.ConnectionID)) {
          log("[LOGOUT]::[USER:{0}]", username);
          client.Send("GAME:#LOGOUT#");
          client.State = ClientState.WAITING;
          userList.Remove(client.ConnectionID);
        }
      }
      #endregion Logout Handling

      #region Queued Player Handle
      // QUEUED: Ready to Play
      if(msg.ToUpper().StartsWith("GAME:") && client.State == ClientState.QUEUED) {
        String[] detail = msg.Split('#'); String mode = detail[1];
        String GameId = userGame[client.ConnectionID];

        if(mode.ToUpper() == "BOARD") {
          int playerIndex = gameList[GameId].playerIndexForId(client.ConnectionID);
          gameList[GameId].setBoardForPlayer(playerIndex, detail[2]);
          client.Send("GAME:#MSG#WAITING_OPPONENT#");
          client.State = ClientState.READY;
        }
      }
      #endregion Queued Player Handle

      #region Game Play
      // IN_GAME: Playing
      if(msg.ToUpper().StartsWith("GAME:") && client.State == ClientState.IN_GAME) {
        String[] detail = msg.Split('#'); String mode = detail[1];
        String GameId = userGame[client.ConnectionID];

        if (mode.ToUpper() == "SHOOT") {
          int x = int.Parse(detail[2]); int y = int.Parse(detail[3]);
          if(gameList[GameId].Turn == gameList[GameId].playerIndexForId(client.ConnectionID)) {
            Boolean isHit = gameList[GameId].shoot(gameList[GameId].playerIndexForId(client.ConnectionID), x, y);
            if (isHit) {
              announceHit(gameList[GameId], client.ConnectionID, x, y);
            }
            gameList[GameId].changeTurn();
            sendBoardAndSetTurn(gameList[GameId]);
          } else {
            client.Send("GAME:#MSG#NOT_YOUR_TURN#");
          }
        }
      }
      #endregion Game Play

    }

    private void client_OnDisconnected(Object sender, GEventArgs e) {
      ClientHandle client = (ClientHandle)sender;
      if(userGame.ContainsKey(client.ConnectionID)) {
        String GameId = userGame[client.ConnectionID];
        for(int i = 0; i < gameList[GameId].PlayerCount; i++) {
          String player = gameList[GameId].playerIdForIndex(i);
          if(clientList.ContainsKey(player)) { clientList[player].Send("GAME:#OPPONENT_DISCONNECTED#"); }
          if(userGame.ContainsKey(player)) { userGame.Remove(player); }
        }
      }
      if(clientList.ContainsKey(client.ConnectionID)) {
        log("[x]::[ID:{0}]", client.ConnectionID);
        clientList.Remove(client.ConnectionID);
        if (userList.ContainsKey(client.ConnectionID)) { userList.Remove(client.ConnectionID); }
      }
    }

    private void announceHit(BSGame game, String connectionID, int x, int y) {
      int player = 1- game.playerIndexForId(connectionID);
      String sendToID = game.playerIdForIndex(player);
      String Board = game.boardForPlayer(player, false);
      clientList[sendToID].Send(String.Format("GAME:#HIT#{0}#{1}#{2}#", Board, x, y));
      log("[HIT]::[GAME:{0}]::[USER:{1}]::[->]::[USER:{2}]", game.GameID, userList[connectionID], userList[sendToID]);
      System.Threading.Thread.Sleep(globalDelay);
    }
    private void sendBoardAndSetTurn(BSGame game) {
      String Client_One = game.playerIdForIndex(0);
      String Client_Two = game.playerIdForIndex(1);
      int P_One = game.playerIndexForId(Client_One);
      int P_Two = game.playerIndexForId(Client_Two);

      int life_One = game.lifeForPlayer(0);
      int life_Two = game.lifeForPlayer(1);

      // Board Sending
      String Board_Two = game.boardForPlayer(P_Two, true);
      String Board_One = game.boardForPlayer(P_One, true);
      clientList[Client_One].Send(String.Format("GAME:#BOARD#{0}#{1}#{2}#", Board_Two, life_Two, life_One));
      clientList[Client_Two].Send(String.Format("GAME:#BOARD#{0}#{1}#{2}#", Board_One, life_One, life_Two));
      System.Threading.Thread.Sleep(globalDelay);

      if(life_One <= 0 || life_Two <= 0) {
        if(life_One <= 0) { // Player One Turn
          clientList[Client_One].Send("GAME:#YOU_LOSE#");
          clientList[Client_Two].Send("GAME:#YOU_WIN#");
          log("[END]::[GAME:{0}]::[USER:{1}]::[LOSE]::[USER:{2}]::[WIN]", game.GameID, userList[Client_One], userList[Client_Two]);
        } else { // Player Two Turn
          clientList[Client_One].Send("GAME:#YOU_WIN#");
          clientList[Client_Two].Send("GAME:#YOU_LOSE#");
          log("[END]::[GAME:{0}]::[USER:{1}]::[WIN]::[USER:{2}]::[LOSE]", game.GameID, userList[Client_One], userList[Client_Two]);
        }
        if(userGame.ContainsKey(Client_One)) { userGame.Remove(Client_One); }
        if(userGame.ContainsKey(Client_Two)) { userGame.Remove(Client_Two); }
        game.clearPlayer();
        gameList.Remove(game.GameID);
      } else {
        // Turn Changing
        if(game.Turn == P_One) { // Player One Turn
          clientList[Client_One].Send("GAME:#YOUR_TURN#");
          clientList[Client_Two].Send("GAME:#OPPONENT_TURN#");
        } else { // Player Two Turn
          clientList[Client_One].Send("GAME:#OPPONENT_TURN#");
          clientList[Client_Two].Send("GAME:#YOUR_TURN#");
        }
      }
      System.Threading.Thread.Sleep(globalDelay);
    }

    private void processQueue() {
      Queue<String> ready = new Queue<String>();
      try {
        foreach(KeyValuePair<String, ClientHandle> entry in clientList) {
          if(entry.Value.State == ClientState.QUEUEING) { ready.Enqueue(entry.Key); }
        }
#pragma warning disable 168
      } catch(Exception ex) {
#pragma warning restore 168
        /* log("<------------>" + Environment.NewLine + "An Error Occured While Processing Queue : " + Environment.NewLine + ex.Message + Environment.NewLine + "<------------>"); */
      }
      if(ready.Count != lastQueueSize) { log("[QUEUE]::[SIZE:{0}]", ready.Count); }
      while(ready.Count > 1) {
        String Client_One = ready.Dequeue(); String Client_Two = ready.Dequeue();
        BSGame game = new BSGame();
        if(clientList.ContainsKey(Client_One) && clientList.ContainsKey(Client_Two)) {
          game.addPlayer(Client_One); game.addPlayer(Client_Two);
          clientList[Client_One].State = ClientState.QUEUED;
          clientList[Client_Two].State = ClientState.QUEUED;
          log("[READY]::[GAME:{2}]::[USER:{0}]::[USER:{1}]", userList[Client_One], userList[Client_Two], game.GameID);

          gameList.Add(game.GameID, game);
          userGame.Add(Client_One, game.GameID); userGame.Add(Client_Two, game.GameID);
          clientList[Client_One].Send(String.Format("GAME:#SETUP_BOARD#{0}#", BSGame._BOARD_SIZE));
          clientList[Client_Two].Send(String.Format("GAME:#SETUP_BOARD#{0}#", BSGame._BOARD_SIZE));
        } else {
          if(clientList.ContainsKey(Client_One)) { clientList[Client_One].State = ClientState.QUEUEING; }
          if(clientList.ContainsKey(Client_Two)) { clientList[Client_Two].State = ClientState.QUEUEING; }
        }
        System.Threading.Thread.Sleep(globalDelay);
      }
      lastQueueSize = ready.Count;
      ready.Clear();
    }
    private void processReady() {
      List<String> toBeRemoved = new List<String>();
      try {
        foreach(KeyValuePair<String, BSGame> entry in gameList) {
          String Client_One = entry.Value.playerIdForIndex(0);
          String Client_Two = entry.Value.playerIdForIndex(1);
          if(clientList.ContainsKey(Client_One) && clientList.ContainsKey(Client_Two)) {
            if(clientList[Client_One].State == ClientState.READY && clientList[Client_Two].State == ClientState.READY) {
              log("[START]::[GAME:{0}]::[USER:{1}]::[<->]::[USER:{2}]", entry.Key, userList[Client_One], userList[Client_Two]);
              entry.Value.randomTurn();
              sendBoardAndSetTurn(entry.Value);
              clientList[Client_One].State = ClientState.IN_GAME; clientList[Client_Two].State = ClientState.IN_GAME;
            }
          } else {
            toBeRemoved.Add(entry.Key);
            if(clientList.ContainsKey(Client_One)) { clientList[Client_One].State = ClientState.QUEUEING; }
            if(clientList.ContainsKey(Client_Two)) { clientList[Client_Two].State = ClientState.QUEUEING; }
          }
          System.Threading.Thread.Sleep(globalDelay);
        }
#pragma warning disable 168
      } catch(Exception ex) {
#pragma warning restore 168
        /* log("<------------>" + Environment.NewLine + "An Error Occured While Processing Queue : " + Environment.NewLine + ex.Message + Environment.NewLine + "<------------>"); */
      }
      for(int i = 0; i < toBeRemoved.Count; i++) {
        if(gameList.ContainsKey(toBeRemoved[i])) { gameList.Remove(toBeRemoved[i]); }
      }
      toBeRemoved.Clear();
    }

    private void bw_GameServer_DoWork(object sender, DoWorkEventArgs e) {
      bw_GameServer.ReportProgress(0, "Queueing System Initialized.");
      while(!bw_GameServer.CancellationPending && queueEnabled) {
        processQueue(); processReady();
        System.Threading.Thread.Sleep(broadcast_delay * 1000);
      }
      bw_GameServer.ReportProgress(0, "Closing Queueing System..");
    }
    private void bw_GameServer_ProgressChanged(object sender, ProgressChangedEventArgs e) {
      if(e.ProgressPercentage == 0) {
        log((String)e.UserState);
      }
    }
    private void bw_GameServer_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e) {
      log("Queueing System Closed.");
    }
    #endregion


  }
}
