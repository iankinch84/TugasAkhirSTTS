namespace Server {
  partial class Form1 {
    /// <summary>
    /// Required designer variable.
    /// </summary>
    private System.ComponentModel.IContainer components = null;

    /// <summary>
    /// Clean up any resources being used.
    /// </summary>
    /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
    protected override void Dispose(bool disposing) {
      if(disposing && (components != null)) {
        components.Dispose();
      }
      base.Dispose(disposing);
    }

    #region Windows Form Designer generated code

    /// <summary>
    /// Required method for Designer support - do not modify
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent() {
      this.tbLog = new System.Windows.Forms.TextBox();
      this.bw_UDPBroadcast = new System.ComponentModel.BackgroundWorker();
      this.bw_GameServer = new System.ComponentModel.BackgroundWorker();
      this.tbCommand = new System.Windows.Forms.TextBox();
      this.SuspendLayout();
      // 
      // tbLog
      // 
      this.tbLog.Dock = System.Windows.Forms.DockStyle.Top;
      this.tbLog.Font = new System.Drawing.Font("Courier New", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.tbLog.Location = new System.Drawing.Point(0, 0);
      this.tbLog.Multiline = true;
      this.tbLog.Name = "tbLog";
      this.tbLog.ReadOnly = true;
      this.tbLog.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
      this.tbLog.Size = new System.Drawing.Size(834, 278);
      this.tbLog.TabIndex = 99;
      this.tbLog.WordWrap = false;
      // 
      // bw_UDPBroadcast
      // 
      this.bw_UDPBroadcast.WorkerReportsProgress = true;
      this.bw_UDPBroadcast.WorkerSupportsCancellation = true;
      this.bw_UDPBroadcast.DoWork += new System.ComponentModel.DoWorkEventHandler(this.bw_UDPBroadcast_DoWork);
      this.bw_UDPBroadcast.ProgressChanged += new System.ComponentModel.ProgressChangedEventHandler(this.bw_UDPBroadcast_ProgressChanged);
      this.bw_UDPBroadcast.RunWorkerCompleted += new System.ComponentModel.RunWorkerCompletedEventHandler(this.bw_UDPBroadcast_RunWorkerCompleted);
      // 
      // bw_GameServer
      // 
      this.bw_GameServer.WorkerReportsProgress = true;
      this.bw_GameServer.WorkerSupportsCancellation = true;
      this.bw_GameServer.DoWork += new System.ComponentModel.DoWorkEventHandler(this.bw_GameServer_DoWork);
      this.bw_GameServer.ProgressChanged += new System.ComponentModel.ProgressChangedEventHandler(this.bw_GameServer_ProgressChanged);
      this.bw_GameServer.RunWorkerCompleted += new System.ComponentModel.RunWorkerCompletedEventHandler(this.bw_GameServer_RunWorkerCompleted);
      // 
      // tbCommand
      // 
      this.tbCommand.Dock = System.Windows.Forms.DockStyle.Bottom;
      this.tbCommand.Font = new System.Drawing.Font("Courier New", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.tbCommand.Location = new System.Drawing.Point(0, 280);
      this.tbCommand.Name = "tbCommand";
      this.tbCommand.Size = new System.Drawing.Size(834, 22);
      this.tbCommand.TabIndex = 0;
      this.tbCommand.WordWrap = false;
      this.tbCommand.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.tbCommand_KeyPress);
      // 
      // Form1
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(834, 302);
      this.Controls.Add(this.tbCommand);
      this.Controls.Add(this.tbLog);
      this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
      this.MaximizeBox = false;
      this.Name = "Form1";
      this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
      this.Text = "Battleship Server";
      this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Form1_FormClosing);
      this.Load += new System.EventHandler(this.Form1_Load);
      this.ResumeLayout(false);
      this.PerformLayout();

    }

    #endregion

    private System.Windows.Forms.TextBox tbLog;
    private System.ComponentModel.BackgroundWorker bw_UDPBroadcast;
    private System.ComponentModel.BackgroundWorker bw_GameServer;
    private System.Windows.Forms.TextBox tbCommand;
  }
}

