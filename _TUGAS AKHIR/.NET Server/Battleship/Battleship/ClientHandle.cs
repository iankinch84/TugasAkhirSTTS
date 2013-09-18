using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System.Threading;

namespace Battleship {
  public class GEventArgs : EventArgs {
    private string _info;
    public GEventArgs(string info) { this._info = info; }
    public String Message { get { return this._info; } }
  }

  enum ClientState { CONNECTING = 0, CONNECTED, AUTHENTICATING, AUTHENTICATED, QUEUEING, QUEUED, READY, IN_GAME, DISCONNECTING, DISCONNECTED, WAITING }

  class ClientHandle {
    public static readonly String _AUTHENTICATE = "0x00";
    public static readonly String _MESSAGE = "0x01";
    public static readonly String _DISCONNECT = "0xFF";

    /// <summary>
    /// Connection ID, Assigned By Server
    /// </summary>
    public String ConnectionID { get; set; }

    /// <summary>
    /// Connected Status
    /// </summary>
    public Boolean Connected { get; set; }

    /// <summary>
    /// Check if client is Authenticated
    /// </summary>
    public Boolean Authenticated { get; set; }

    /// <summary>
    /// Current Client State
    /// </summary>
    public ClientState State { get; set; }

    #region Private Variables
    private TcpClient socket;
    private NetworkStream stream;
    private Byte[] buffer;
#pragma warning disable 0649
    private Thread clientThread;
#pragma warning restore 0649
    #endregion

    /// <summary>
    /// Event Delegates
    /// </summary>
    /// <param name="sender">Object Sender.</param>
    /// <param name="e">Event Arguments.</param>
    public delegate void GEventHandler(Object sender, GEventArgs e);

    /// <summary>
    /// On Read Event, Raised whenever a message is ready
    /// Warning: UNSAFE EVENT, require Invoke
    /// </summary>
    public event GEventHandler OnRead;

    /// <summary>
    /// On Disconnected Event, Raised when client disconnected
    /// Warning: UNSAFE EVENT, require Invoke
    /// </summary>
    public event GEventHandler OnDisconnected;

    /// <summary>
    /// ClientHandle Constructor, Initialize ClientHandle Class
    /// </summary>
    /// <param name="client">TCPClient for the ClientHandle, a newly accepted connection.</param>
    public ClientHandle() {
      this.Connected = false;
    }

    /// <summary>
    /// Connect To Server
    /// </summary>
    /// <param name="address">Server IP Address And Port</param>
    public void Connect(IPEndPoint address) {
      this.socket = new TcpClient();
      this.socket.Connect(address);
      
      this.Connected = true;
      this.Authenticated = false;
      this.State = ClientState.CONNECTING;
      this.stream = this.socket.GetStream();

      buffer = new Byte[socket.ReceiveBufferSize];
      this.beginReadData();

      /* No Longer Used: Exhaustive and Expensive */
      /*this.clientThread = new Thread(new ThreadStart(listen));
      this.clientThread.IsBackground = true;
      this.clientThread.Start();*/
    }

    /// <summary>
    /// Send Message To Client
    /// </summary>
    /// <param name="message">Byte stream to be send</param>
    public void Send(Byte[] message) {
      if(this.Connected && stream.CanWrite && this.socket.Connected) {
        this.stream.Write(message, 0, message.Length);
        this.stream.Flush();
      }
    }
    /// <summary>
    /// Send Message To Client
    /// </summary>
    /// <param name="message">String to be send</param>
    public void Send(String message) {
      Byte[] buffer = System.Text.Encoding.UTF8.GetBytes(message);
      this.Send(buffer);
    }

    /// <summary>
    /// Disconnect Client
    /// </summary>
    public void Disconnect() {
      this.Send(_DISCONNECT);
      if(OnDisconnected != null) { OnDisconnected.Invoke(this, new GEventArgs("Disconnected.")); }
      this.State = ClientState.DISCONNECTING;
      this.Connected = false;
      this.Authenticated = false;

      this.socket.Close();
      this.State = ClientState.DISCONNECTED;

      /*           Unsafe           */
      /* this.clientThread.Abort(); */
    }

    private void beginReadData() {
      if(this.Connected && stream.CanRead) {
        try {
          stream.BeginRead(buffer, 0, buffer.Length, new AsyncCallback(completeRead), null);
        } catch(Exception ex) {
          Console.WriteLine(ex.Message);
        }
      }
    }
    private void completeRead(IAsyncResult result) {
      if(this.Connected) { this.beginReadData(); }
      int bytesRead = -1;
      try {
        bytesRead = stream.EndRead(result);
      } catch(Exception ex) {
        Console.WriteLine(ex.Message);
        bytesRead = -1;
      }
      if(bytesRead > 0) {
        String data = System.Text.Encoding.UTF8.GetString(buffer, 0, bytesRead);
        if(data.StartsWith(_DISCONNECT)) {
          this.Connected = false;
          if(OnDisconnected != null) { OnDisconnected(this, new GEventArgs("Client Disconnected From Server.")); }
        } else if(data.StartsWith(_AUTHENTICATE)) {
          this.ConnectionID = data.Substring(4);
        } else {
          if(OnRead != null) { OnRead.Invoke(this, new GEventArgs(data)); }
        }
      }
    }

    /// <summary>
    ///  Listen to socket, read data if available. No Longer Used: Exhaustive and Expensive
    /// </summary>
    private void listen() {
      int i; String data = null;
      Byte[] buffer = new Byte[socket.ReceiveBufferSize];

      while(this.Connected && this.clientThread.IsAlive) {
        if(socket.Available > 0 && this.Connected && this.socket.Connected) {
          if(this.State == ClientState.CONNECTING) { this.State = ClientState.CONNECTED; }
          data = ""; i = -1;
          try {
            while((i = stream.Read(buffer, 0, buffer.Length)) > 0) {
              data = System.Text.Encoding.UTF8.GetString(buffer, 0, i);

              if(data.StartsWith(_DISCONNECT)) {
                this.Connected = false;
                if(OnDisconnected != null) { OnDisconnected(this, new GEventArgs("Disconnected From Server.")); }
              } else if(data.StartsWith(_AUTHENTICATE)) {
                this.ConnectionID = data.Substring(4);
              } else {
                if(OnRead != null) { OnRead(this, new GEventArgs(data)); }
              }

            }
          } catch(Exception ex) {
            Console.WriteLine(ex.Message);
          }
        }
      }
    }
  }
}
