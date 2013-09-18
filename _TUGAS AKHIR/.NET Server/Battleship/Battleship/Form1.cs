using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Net;
using System.Net.Sockets;
using System.IO;

namespace Battleship {
  public partial class Form1 : Form {
    public static readonly int _DEFAULT_PANEL_SIZE = 64;
    public static readonly int _TOTAL_SHAPE = 4;
    public static readonly int[,] SHAPES = { 
                                             { 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 }, 
                                             { 2, 1, 0, 1, 0, 0, 0, 0, 0, 0 }, 
                                             { 2, 1, 1, 0, 0, 0, 0, 0, 0, 0 }, 
                                             { 3, 0, 1, 0, 1, 1, 1, 0, 1, 0 } 
                                           };

    private UdpClient udp;
    private int udp_port = 3217;

    private ClientHandle client;

    private Timer timer;
    private DateTime lastPing;
    private int timeout = 10; //seconds

    private int[,] board;
    private Panel[,] pnls;

    private Boolean isEditing;
    private int editShape;
    private int boardGameSize = 0;

    private Dictionary<String, String> strings = new Dictionary<String, String>();

    public Form1() {
      InitializeComponent();
      loadStrings();
      switchToPanel(pnlLogin);
    }

    #region Log Functions
    delegate void LogCallback(string text);
    private void log(String format, params object[] args) {
      log(String.Format(format, args));
    }
    private void log(String message) {
      if(!textBox1.IsDisposed) {
        if(textBox1.InvokeRequired) {
          textBox1.Invoke(new LogCallback(log), message);
        } else {
          textBox1.AppendText(message + Environment.NewLine);
          textBox1.Refresh();
        }
      }
    }
    #endregion

    private void Form1_Load(object sender, EventArgs e) {
      textBox2.Focus();
      lastPing = DateTime.Now;
      log("Initializing Connection.");
      client = new ClientHandle();
      client.OnRead += new ClientHandle.GEventHandler(client_OnRead);
      client.OnDisconnected += new ClientHandle.GEventHandler(client_OnDisconnected);
      udp = new UdpClient(new IPEndPoint(IPAddress.Any, udp_port));
      log("Looking For Server..");
      startListening();

      this.timer = new Timer();
      timer.Interval = 2048;
      timer.Tick += new EventHandler(timer_Tick);
      timer.Start();
    }
    private void Form1_FormClosing(object sender, FormClosingEventArgs e) {
      if(client.Connected) { client.Disconnect(); }
    }

    void timer_Tick(object sender, EventArgs e) {
      TimeSpan diff = DateTime.Now - lastPing;
      if(client.Connected) {
        if(Math.Abs(diff.Seconds) >= timeout) {
          toolStripStatusLabel1.Text = "Server Status: Request Time Out.";
        }
      }
    }

    private void textBox2_KeyPress(object sender, KeyPressEventArgs e) {
      if(e.KeyChar == (char)13) {
        doCommand(((TextBox)sender).Text);
        ((TextBox)sender).Clear();
      }
    }

    private void switchToPanel(Panel pnl) {
      panelGame.Hide();
      pnlLogin.Hide();
      pnl.Show();
    }

    delegate void parseMessageCallback(String msg);
    private void parseMessage(String msg) {
      if(msg.ToUpper().StartsWith("MSG:")) {
        //Cross Thread
        String clean = msg.Substring(5); clean = clean.Substring(0, clean.Length - 1);
        MessageBox.Show(clean, "Information");
        lbStatus.Hide(); tbUsername.Show();
        btnLogin.Enabled = true;
      }
      if(msg.ToUpper().StartsWith("GAME:")) {
        //Cross Thread
        String[] detail = msg.Split('#');
        if(detail[1].ToUpper() == "MSG") {
          lbStatus.Text = strings[detail[2]];
          lblGameStatus.Text = strings[detail[2]];
        }
        if(detail[1].ToUpper() == "SETUP_BOARD") {
          int boardSize = int.Parse(detail[2]);
          lblObject.Visible = false; lblEnemyObject.Visible = false;
          prepareForBoard(boardSize);
          switchToPanel(panelGame);
        }
        if(detail[1].ToUpper() == "BOARD") {
          prepareForGame(detail[2]);
          if(detail.Length > 3) {
            lblObject.Visible = true; lblObject.Text = String.Format("Your Object : {0}", detail[4]);
            lblEnemyObject.Visible = true; lblEnemyObject.Text = String.Format("Enemy Object : {0}", detail[3]);
          }
        }
        if(detail[1].ToUpper() == "HIT") {
          String board = detail[2];
          int x = Int32.Parse(detail[3]); int y = Int32.Parse(detail[4]);
          FormInfo info = new FormInfo(board, x, y, boardGameSize);
          info.ShowDialog();
        }
        if(detail[1].ToUpper() == "YOU_WIN") {
          reloadPanel();
          lblGameStatus.Text = strings["YOU_WIN"];
          //MessageBox.Show(String.Format("{0}{1}{2}", strings["REWARD_MSG"], Environment.NewLine, strings["REWARD_WIN"]));
          MessageBox.Show(String.Format("{0}", strings["YOU_WIN"]));
          btnAction.Enabled = false;
          btnAction.Text = strings["FIRE"];
        }
        if(detail[1].ToUpper() == "YOU_LOSE") {
          reloadPanel();
          lblGameStatus.Text = strings["YOU_LOSE"];
          //MessageBox.Show(String.Format("{0}{1}{2}", strings["REWARD_MSG"], Environment.NewLine, strings["REWARD_LOSE"]));
          MessageBox.Show(String.Format("{0}", strings["YOU_LOSE"]));
          btnAction.Enabled = false;
          btnAction.Text = strings["FIRE"];
        }
        if(detail[1].ToUpper() == "OPPONENT_TURN") {
          reloadPanel();
          lblGameStatus.Text = strings["OPPONENT_TURN"];
          btnAction.Enabled = false;
          btnAction.Text = strings["FIRE"];
        }
        if(detail[1].ToUpper() == "YOUR_TURN") {
          reloadPanel();
          lblGameStatus.Text = strings["YOUR_TURN"];
          btnAction.Enabled = true;
          btnAction.Text = strings["FIRE"];
        }
        if(detail[1].ToUpper() == "OPPONENT_DISCONNECTED") {
          if(panelGame.Visible) {
            MessageBox.Show(strings["OPPONENT_DISCONNECTED"] + Environment.NewLine + strings["WAITING_QUEUE"], "Information", MessageBoxButtons.OK, MessageBoxIcon.Information);
            lbStatus.Text = strings["WAITING_QUEUE"];
            switchToPanel(pnlLogin);
          }
        }
      }
    }

    private void client_OnRead(Object sender, GEventArgs e) {
      log("[<-]::[ID:{0}]::[MSG:{1}]", ((ClientHandle)sender).ConnectionID, e.Message);
      this.Invoke(new parseMessageCallback(parseMessage), e.Message);

    }
    private void client_OnDisconnected(Object sender, GEventArgs e) {
      log("[x]::[ID:{0}]", ((ClientHandle)sender).ConnectionID);
      toolStripStatusLabel1.Text = "Server Status: Disconnected.";
    }

    private void startListening() {
      udp.BeginReceive(HandleAsyncConnection, udp);
    }
    private void HandleAsyncConnection(IAsyncResult res) {
      startListening();
      lastPing = DateTime.Now;
      if(!client.Connected) {
        IPEndPoint ip = null;
        Byte[] buffer = udp.EndReceive(res, ref ip);
        String message = System.Text.Encoding.UTF8.GetString(buffer);
        log("Server Found.");
        log("[<->]::[IP:{0}]::[PORT:{1}]::[NAME:{2}]", ip.Address.ToString(), ip.Port, message);
        ip.Port += 10000;
        log("[o]::[IP:{0}]::[PORT:{1}]", ip.Address.ToString(), ip.Port);
        toolStripStatusLabel1.Text = "Server Status: Connected.";
        client.Connect(ip);
      }
    }

    private void doCommand(String command) {
      if(!String.IsNullOrEmpty(command)) {
        log("> {0}", command);
        String[] commands = command.Split(' ');
        commands[0] = commands[0].ToUpper();
        if(commands[0] == "SEND") {
          String message = "";
          for(int i = 1; i < commands.Length; i++) { message += commands[i] + " "; }
          message = message.Substring(0, message.Length - 1);
          if(!String.IsNullOrEmpty(message)) { client.Send(message); }
        } else if(commands[0] == "EXIT" || commands[0] == "QUIT") {
          Application.Exit();
        } else if(commands[0] == "DISCONNECT") {
          client.Disconnect();
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
      log("> send <message> : Send <message> To Server.");
      log("> exit : Close And Exit Server.");
      log("> quit : Close And Exit Server.");
    }
    private void loadStrings() {
      if(!File.Exists("Strings.ini")) { MessageBox.Show("Strings.ini Not Found!", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error); Application.Exit(); }
      String[] lines = File.ReadAllLines("Strings.ini");
      foreach(String s in lines) { String[] t = s.Split('#'); strings.Add(t[0], t[1]); }
    }

    private void btnLogin_Click(object sender, EventArgs e) {
      doLogin();
    }
    private void tbUsername_KeyPress(object sender, KeyPressEventArgs e) {
      if(e.KeyChar == (char)13) {
        doLogin();
      }
    }
    private void doLogin() {
      if(client.Connected) {
        String username = tbUsername.Text;
        lbStatus.Text = "Logging In..";
        tbUsername.Clear();
        tbUsername.Hide(); lbStatus.Show();
        btnLogin.Enabled = false;
        client.Send(String.Format("LOGIN:#{0}#", username));
      }
    }

    private void btnAction_Click(object sender, EventArgs e) {
      // Edit Mode
      if(isEditing) {
        int used = 0;
        foreach(Object o in panelGameSide.Controls) {
          if(o.GetType() == panelGameSide.GetType()) {
            if(((Panel)o).Enabled == false) { used++; }
          }
        }
        if(used < _TOTAL_SHAPE) {
          MessageBox.Show("Please Place All Items On Board");
        } else {
          String boardRepresentation = "";
          for(int y = 0; y < boardGameSize; y++) {
            for(int x = 0; x < boardGameSize; x++) {
              boardRepresentation += (board[x, y] == 0 ? "0" : "1");
            }
          }
          client.Send("GAME:#BOARD#" + boardRepresentation);
          btnAction.Enabled = false;
        }
      }

      // Game Mode
      if(!isEditing && editShape == -2) {
        int shootCount = 0; int x = 0; int y = 0;
        foreach(Object o in panelGameMain.Controls) {
          if(o.GetType() == panelGameMain.GetType()) {
            if(((Panel)o).BackColor == Color.Pink) {
              shootCount++;
              x = (int)((Panel)o).Tag % boardGameSize;
              y = (int)((Panel)o).Tag / boardGameSize;
              reloadPanel();
              break;
            }
          }
        }
        if(shootCount == 1) {
          client.Send(String.Format("GAME:#SHOOT#{0}#{1}#", x, y));
        }
        reloadPanel();
      }
    }
    private void prepareForBoard(int size) {
      editShape = -1; isEditing = true;

      panelGameMain.Controls.Clear();
      panelGameSide.Controls.Clear();
      lblGameStatus.Text = "Prepare Your Board";
      btnAction.Text = "Done";
      btnAction.Enabled = true;

      board = new int[size, size];
      pnls = new Panel[size, size];
      boardGameSize = size;

      int boardSize = size * size;
      for(int i = 0; i < boardSize; i++) {
        Panel p = new Panel();
        p.Size = new Size(_DEFAULT_PANEL_SIZE, _DEFAULT_PANEL_SIZE);
        p.Location = new Point((i % size) * _DEFAULT_PANEL_SIZE, (i / size) * _DEFAULT_PANEL_SIZE);
        p.Tag = i; p.BorderStyle = BorderStyle.Fixed3D;
        p.BackgroundImage = Image.FromFile("./Images/soil.png");
        p.BackgroundImageLayout = ImageLayout.Stretch;

        p.Click += new EventHandler(panel_MouseClick);
        p.MouseMove += new MouseEventHandler(panel_MouseMove);
        p.MouseLeave += new EventHandler(panel_MouseLeave);

        pnls[i % size, i / size] = p;
        board[i % size, i / size] = 0;

        panelGameMain.Controls.Add(p);
      }

      for(int i = 0; i < _TOTAL_SHAPE; i++) {
        Panel p = new Panel();
        p.Size = new Size(250, 60);
        p.Location = new Point(0, i * (60 + 5));
        p.Tag = i; p.BorderStyle = BorderStyle.None;
        p.BackColor = Color.LightBlue;
        p.BackgroundImage = Image.FromFile(String.Format("./Images/shape{0}.png", i));

        p.Click += new EventHandler(shape_MouseClick);

        panelGameSide.Controls.Add(p);
      }

    }
    private void prepareForGame(String board) {
      isEditing = false; editShape = -2;
      int size = board.Length;
      for(int i = 0; i < size; i++) {
        char current = board[i];
        if(current == '0') { this.board[i % boardGameSize, i / boardGameSize] = -1; }
        if(current == '3') { this.board[i % boardGameSize, i / boardGameSize] = 0; }
        if(current == '2') { this.board[i % boardGameSize, i / boardGameSize] = 1; }
      }
      reloadPanel();
    }

    private void panel_MouseClick(object sender, EventArgs e) {
      // Edit Mode
      if(isEditing && editShape != -1) {
        setBoard(sender);
      }
      if(!isEditing && editShape == -2) {
        if(btnAction.Enabled) {
          reloadPanel();
          Panel p = (Panel)sender;
          p.BackColor = Color.Pink;
          Image img = Image.FromFile("./Images/stone.png");
          p.BackgroundImage = img;
          p.Refresh();
        }
      }
    }
    private void panel_MouseMove(object sender, MouseEventArgs e) {
      if(isEditing) { reloadPanel(); }
      if(editShape != -1 && isEditing) {
        setPanel(sender);
      }
    }
    private void panel_MouseLeave(object sender, EventArgs e) {
      if(isEditing) { reloadPanel(); }
    }
    private void panel_MouseHover(object sender, EventArgs e) {
      if(isEditing) { reloadPanel(); }
      if(editShape != -1 && isEditing) {
        setPanel(sender);
      }
    }

    private void setBoard(Object sender) {
      int shapeSize = SHAPES[editShape, 0];
      int grid = shapeSize * shapeSize;

      Panel tmp = (Panel)sender;
      int x = (int)tmp.Tag % boardGameSize;
      int y = (int)tmp.Tag / boardGameSize;
      if(isValidPlacement(x, y)) {
        for(int i = 0; i < grid; i++) {
          int dx = i % shapeSize; int dy = i / shapeSize;
          if(x + dx < boardGameSize && y + dy < boardGameSize) {
            if(SHAPES[editShape, i + 1] == 1) { board[x + dx, y + dy] = (editShape + 1); }
          }
        }
        foreach(Object o in panelGameSide.Controls) {
          if(o.GetType() == panelGameSide.GetType()) {
            if((int)((Panel)o).Tag == editShape) {
              ((Panel)o).Enabled = false;
              ((Panel)o).BackgroundImage = null;
              ((Panel)o).BorderStyle = BorderStyle.None;
              ((Panel)o).BackColor = Color.LightGray;
            }
          }
        }
        editShape = -1;
      } else {
        MessageBox.Show(strings["INVALID_PLACEMENT"]);
      }
    }
    private void reloadPanel() {
      reloadPanel(true);
    }
    private void reloadPanel(Boolean wipeAll) {
      for(int y = 0; y < boardGameSize; y++) {
        for(int x = 0; x < boardGameSize; x++) {
          Image img = Image.FromFile("./Images/soil.png");
          Color clr = Color.LightGreen; /* soil */
          if(board[x, y] == -1) { clr = Color.LightGray; img = Image.FromFile("./Images/grass.png"); /* grass */ }
          if(board[x, y] >= 1) { clr = Color.Red; img = (isEditing ? Image.FromFile("./Images/strawberry.png") : Image.FromFile("./Images/stone.png")); /* stone or strawberry*/ }
          pnls[x, y].BackColor = (wipeAll ? clr : (pnls[x, y].BackColor == Color.Pink ? Color.Pink : clr));
          pnls[x, y].BackgroundImage = (wipeAll ? img : (pnls[x, y].BackColor == Color.Pink ? Image.FromFile("./Images/stone.png") : img));
          pnls[x, y].Refresh();
        }
      }
    }
    private void setPanel(Object sender) {
      int shapeSize = SHAPES[editShape, 0];
      int grid = shapeSize * shapeSize;

      Panel tmp = (Panel)sender;
      int x = (int)tmp.Tag % boardGameSize;
      int y = (int)tmp.Tag / boardGameSize;
      Image img = Image.FromFile("./Images/strawberry.png");

      for(int i = 0; i < grid; i++) {
        int dx = i % shapeSize; int dy = i / shapeSize;
        if(x + dx < boardGameSize && y + dy < boardGameSize) {
          if(SHAPES[editShape, i + 1] == 1) { pnls[x + dx, y + dy].BackColor = Color.Red; pnls[x + dx, y + dy].BackgroundImage = img; pnls[x + dx, y + dy].Refresh(); }
        }
      }
    }

    private Boolean isValidPlacement(int x, int y) {
      Boolean result = true;
      int shapeSize = SHAPES[editShape, 0];
      int grid = shapeSize * shapeSize;
      for(int i = 0; i < grid; i++) {
        int dx = i % shapeSize; int dy = i / shapeSize;
        if(SHAPES[editShape, i + 1] == 1) {
          if(x + dx < boardGameSize && y + dy < boardGameSize) {
            if(board[x + dx, y + dy] > 0) { result = false; break; }
          } else {
            result = false; break;
          }
        }
      }


      return result;
    }

    private void shape_MouseClick(object sender, EventArgs e) {
      foreach(Object o in panelGameSide.Controls) {
        if(o.GetType() == panelGameSide.GetType()) {
          ((Panel)o).BorderStyle = BorderStyle.None;
        }
      }

      if(editShape == (int)((Panel)sender).Tag) {
        ((Panel)sender).BorderStyle = BorderStyle.None;
        editShape = -1;
      } else {
        ((Panel)sender).BorderStyle = BorderStyle.Fixed3D;
        editShape = (int)((Panel)sender).Tag;
      }
      reloadPanel();
    }

   


  }
}
