namespace Battleship {
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
      this.textBox1 = new System.Windows.Forms.TextBox();
      this.textBox2 = new System.Windows.Forms.TextBox();
      this.statusStrip1 = new System.Windows.Forms.StatusStrip();
      this.toolStripStatusLabel1 = new System.Windows.Forms.ToolStripStatusLabel();
      this.tableLayoutPanel1 = new System.Windows.Forms.TableLayoutPanel();
      this.panel1 = new System.Windows.Forms.Panel();
      this.panelContainer = new System.Windows.Forms.Panel();
      this.panelGame = new System.Windows.Forms.Panel();
      this.tableLayoutPanel3 = new System.Windows.Forms.TableLayoutPanel();
      this.panelGameMain = new System.Windows.Forms.Panel();
      this.tableLayoutPanel4 = new System.Windows.Forms.TableLayoutPanel();
      this.panel2 = new System.Windows.Forms.Panel();
      this.lblEnemyObject = new System.Windows.Forms.Label();
      this.lblObject = new System.Windows.Forms.Label();
      this.lblGameStatus = new System.Windows.Forms.Label();
      this.btnAction = new System.Windows.Forms.Button();
      this.panelGameSide = new System.Windows.Forms.Panel();
      this.pnlLogin = new System.Windows.Forms.Panel();
      this.tableLayoutPanel2 = new System.Windows.Forms.TableLayoutPanel();
      this.btnLogin = new System.Windows.Forms.Button();
      this.panel3 = new System.Windows.Forms.Panel();
      this.tbUsername = new System.Windows.Forms.TextBox();
      this.lbStatus = new System.Windows.Forms.Label();
      this.statusStrip1.SuspendLayout();
      this.tableLayoutPanel1.SuspendLayout();
      this.panel1.SuspendLayout();
      this.panelContainer.SuspendLayout();
      this.panelGame.SuspendLayout();
      this.tableLayoutPanel3.SuspendLayout();
      this.tableLayoutPanel4.SuspendLayout();
      this.panel2.SuspendLayout();
      this.pnlLogin.SuspendLayout();
      this.tableLayoutPanel2.SuspendLayout();
      this.panel3.SuspendLayout();
      this.SuspendLayout();
      // 
      // textBox1
      // 
      this.textBox1.Dock = System.Windows.Forms.DockStyle.Top;
      this.textBox1.Font = new System.Drawing.Font("Courier New", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.textBox1.Location = new System.Drawing.Point(0, 0);
      this.textBox1.Multiline = true;
      this.textBox1.Name = "textBox1";
      this.textBox1.ReadOnly = true;
      this.textBox1.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
      this.textBox1.Size = new System.Drawing.Size(726, 186);
      this.textBox1.TabIndex = 99;
      this.textBox1.WordWrap = false;
      // 
      // textBox2
      // 
      this.textBox2.Dock = System.Windows.Forms.DockStyle.Bottom;
      this.textBox2.Font = new System.Drawing.Font("Courier New", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.textBox2.Location = new System.Drawing.Point(0, -21);
      this.textBox2.Name = "textBox2";
      this.textBox2.Size = new System.Drawing.Size(726, 22);
      this.textBox2.TabIndex = 0;
      this.textBox2.WordWrap = false;
      this.textBox2.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.textBox2_KeyPress);
      // 
      // statusStrip1
      // 
      this.statusStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripStatusLabel1});
      this.statusStrip1.Location = new System.Drawing.Point(0, 498);
      this.statusStrip1.Name = "statusStrip1";
      this.statusStrip1.Size = new System.Drawing.Size(732, 22);
      this.statusStrip1.TabIndex = 100;
      this.statusStrip1.Text = "statusStrip1";
      // 
      // toolStripStatusLabel1
      // 
      this.toolStripStatusLabel1.Name = "toolStripStatusLabel1";
      this.toolStripStatusLabel1.Size = new System.Drawing.Size(85, 17);
      this.toolStripStatusLabel1.Text = "Server Status: -";
      // 
      // tableLayoutPanel1
      // 
      this.tableLayoutPanel1.ColumnCount = 1;
      this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
      this.tableLayoutPanel1.Controls.Add(this.panel1, 0, 1);
      this.tableLayoutPanel1.Controls.Add(this.panelContainer, 0, 0);
      this.tableLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
      this.tableLayoutPanel1.Location = new System.Drawing.Point(0, 0);
      this.tableLayoutPanel1.Name = "tableLayoutPanel1";
      this.tableLayoutPanel1.RowCount = 2;
      this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
      this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 0F));
      this.tableLayoutPanel1.Size = new System.Drawing.Size(732, 498);
      this.tableLayoutPanel1.TabIndex = 101;
      // 
      // panel1
      // 
      this.panel1.Controls.Add(this.textBox1);
      this.panel1.Controls.Add(this.textBox2);
      this.panel1.Dock = System.Windows.Forms.DockStyle.Fill;
      this.panel1.Location = new System.Drawing.Point(3, 501);
      this.panel1.Name = "panel1";
      this.panel1.Size = new System.Drawing.Size(726, 1);
      this.panel1.TabIndex = 0;
      // 
      // panelContainer
      // 
      this.panelContainer.Controls.Add(this.panelGame);
      this.panelContainer.Controls.Add(this.pnlLogin);
      this.panelContainer.Dock = System.Windows.Forms.DockStyle.Fill;
      this.panelContainer.Location = new System.Drawing.Point(3, 3);
      this.panelContainer.Name = "panelContainer";
      this.panelContainer.Size = new System.Drawing.Size(726, 492);
      this.panelContainer.TabIndex = 1;
      // 
      // panelGame
      // 
      this.panelGame.Controls.Add(this.tableLayoutPanel3);
      this.panelGame.Dock = System.Windows.Forms.DockStyle.Fill;
      this.panelGame.Location = new System.Drawing.Point(0, 0);
      this.panelGame.Name = "panelGame";
      this.panelGame.Size = new System.Drawing.Size(726, 492);
      this.panelGame.TabIndex = 3;
      // 
      // tableLayoutPanel3
      // 
      this.tableLayoutPanel3.ColumnCount = 2;
      this.tableLayoutPanel3.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 250F));
      this.tableLayoutPanel3.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
      this.tableLayoutPanel3.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 20F));
      this.tableLayoutPanel3.Controls.Add(this.panelGameMain, 1, 0);
      this.tableLayoutPanel3.Controls.Add(this.tableLayoutPanel4, 0, 0);
      this.tableLayoutPanel3.Dock = System.Windows.Forms.DockStyle.Fill;
      this.tableLayoutPanel3.Location = new System.Drawing.Point(0, 0);
      this.tableLayoutPanel3.Name = "tableLayoutPanel3";
      this.tableLayoutPanel3.RowCount = 1;
      this.tableLayoutPanel3.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
      this.tableLayoutPanel3.Size = new System.Drawing.Size(726, 492);
      this.tableLayoutPanel3.TabIndex = 0;
      // 
      // panelGameMain
      // 
      this.panelGameMain.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
      this.panelGameMain.Dock = System.Windows.Forms.DockStyle.Fill;
      this.panelGameMain.Location = new System.Drawing.Point(253, 3);
      this.panelGameMain.Name = "panelGameMain";
      this.panelGameMain.Size = new System.Drawing.Size(470, 486);
      this.panelGameMain.TabIndex = 2;
      // 
      // tableLayoutPanel4
      // 
      this.tableLayoutPanel4.CellBorderStyle = System.Windows.Forms.TableLayoutPanelCellBorderStyle.Single;
      this.tableLayoutPanel4.ColumnCount = 1;
      this.tableLayoutPanel4.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
      this.tableLayoutPanel4.Controls.Add(this.panel2, 0, 0);
      this.tableLayoutPanel4.Controls.Add(this.panelGameSide, 0, 1);
      this.tableLayoutPanel4.Dock = System.Windows.Forms.DockStyle.Fill;
      this.tableLayoutPanel4.Location = new System.Drawing.Point(3, 3);
      this.tableLayoutPanel4.Name = "tableLayoutPanel4";
      this.tableLayoutPanel4.RowCount = 2;
      this.tableLayoutPanel4.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 120F));
      this.tableLayoutPanel4.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
      this.tableLayoutPanel4.Size = new System.Drawing.Size(244, 486);
      this.tableLayoutPanel4.TabIndex = 1;
      // 
      // panel2
      // 
      this.panel2.Controls.Add(this.lblEnemyObject);
      this.panel2.Controls.Add(this.lblObject);
      this.panel2.Controls.Add(this.lblGameStatus);
      this.panel2.Controls.Add(this.btnAction);
      this.panel2.Dock = System.Windows.Forms.DockStyle.Fill;
      this.panel2.Location = new System.Drawing.Point(4, 4);
      this.panel2.Name = "panel2";
      this.panel2.Size = new System.Drawing.Size(236, 114);
      this.panel2.TabIndex = 0;
      // 
      // lblEnemyObject
      // 
      this.lblEnemyObject.AutoSize = true;
      this.lblEnemyObject.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.lblEnemyObject.Location = new System.Drawing.Point(4, 52);
      this.lblEnemyObject.Name = "lblEnemyObject";
      this.lblEnemyObject.Size = new System.Drawing.Size(188, 24);
      this.lblEnemyObject.TabIndex = 3;
      this.lblEnemyObject.Text = "Enemy Object :  99";
      this.lblEnemyObject.Visible = false;
      // 
      // lblObject
      // 
      this.lblObject.AutoSize = true;
      this.lblObject.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.lblObject.Location = new System.Drawing.Point(4, 28);
      this.lblObject.Name = "lblObject";
      this.lblObject.Size = new System.Drawing.Size(167, 24);
      this.lblObject.TabIndex = 2;
      this.lblObject.Text = "Your Object :  99";
      this.lblObject.Visible = false;
      // 
      // lblGameStatus
      // 
      this.lblGameStatus.AutoSize = true;
      this.lblGameStatus.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.lblGameStatus.Location = new System.Drawing.Point(4, 4);
      this.lblGameStatus.Name = "lblGameStatus";
      this.lblGameStatus.Size = new System.Drawing.Size(17, 24);
      this.lblGameStatus.TabIndex = 1;
      this.lblGameStatus.Text = "-";
      // 
      // btnAction
      // 
      this.btnAction.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.btnAction.Location = new System.Drawing.Point(136, 78);
      this.btnAction.Name = "btnAction";
      this.btnAction.Size = new System.Drawing.Size(97, 33);
      this.btnAction.TabIndex = 0;
      this.btnAction.Text = "Done";
      this.btnAction.UseVisualStyleBackColor = true;
      this.btnAction.Click += new System.EventHandler(this.btnAction_Click);
      // 
      // panelGameSide
      // 
      this.panelGameSide.Dock = System.Windows.Forms.DockStyle.Fill;
      this.panelGameSide.Location = new System.Drawing.Point(4, 125);
      this.panelGameSide.Name = "panelGameSide";
      this.panelGameSide.Size = new System.Drawing.Size(236, 357);
      this.panelGameSide.TabIndex = 1;
      // 
      // pnlLogin
      // 
      this.pnlLogin.Controls.Add(this.tableLayoutPanel2);
      this.pnlLogin.Dock = System.Windows.Forms.DockStyle.Fill;
      this.pnlLogin.Location = new System.Drawing.Point(0, 0);
      this.pnlLogin.Name = "pnlLogin";
      this.pnlLogin.Size = new System.Drawing.Size(726, 492);
      this.pnlLogin.TabIndex = 2;
      // 
      // tableLayoutPanel2
      // 
      this.tableLayoutPanel2.ColumnCount = 3;
      this.tableLayoutPanel2.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
      this.tableLayoutPanel2.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 340F));
      this.tableLayoutPanel2.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
      this.tableLayoutPanel2.Controls.Add(this.btnLogin, 1, 3);
      this.tableLayoutPanel2.Controls.Add(this.panel3, 1, 1);
      this.tableLayoutPanel2.Dock = System.Windows.Forms.DockStyle.Fill;
      this.tableLayoutPanel2.Location = new System.Drawing.Point(0, 0);
      this.tableLayoutPanel2.Name = "tableLayoutPanel2";
      this.tableLayoutPanel2.RowCount = 5;
      this.tableLayoutPanel2.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50F));
      this.tableLayoutPanel2.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 40F));
      this.tableLayoutPanel2.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 5F));
      this.tableLayoutPanel2.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 60F));
      this.tableLayoutPanel2.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50F));
      this.tableLayoutPanel2.Size = new System.Drawing.Size(726, 492);
      this.tableLayoutPanel2.TabIndex = 3;
      // 
      // btnLogin
      // 
      this.btnLogin.Dock = System.Windows.Forms.DockStyle.Fill;
      this.btnLogin.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.btnLogin.Location = new System.Drawing.Point(196, 241);
      this.btnLogin.Name = "btnLogin";
      this.btnLogin.Size = new System.Drawing.Size(334, 54);
      this.btnLogin.TabIndex = 1;
      this.btnLogin.Text = "Login";
      this.btnLogin.UseVisualStyleBackColor = true;
      this.btnLogin.Click += new System.EventHandler(this.btnLogin_Click);
      // 
      // panel3
      // 
      this.panel3.Controls.Add(this.tbUsername);
      this.panel3.Controls.Add(this.lbStatus);
      this.panel3.Dock = System.Windows.Forms.DockStyle.Fill;
      this.panel3.Location = new System.Drawing.Point(196, 196);
      this.panel3.Name = "panel3";
      this.panel3.Size = new System.Drawing.Size(334, 34);
      this.panel3.TabIndex = 2;
      // 
      // tbUsername
      // 
      this.tbUsername.AutoCompleteCustomSource.AddRange(new string[] {
            "AFRO_NEKO",
            "ANAKKU_SEDANG_MERANTAU",
            "BENDROID",
            "CURIOUSITY",
            "D_CIBUNX",
            "DE_GRACIA",
            "DGR",
            "DIM",
            "EJW",
            "EL_OSO",
            "FARMYST",
            "FRATORIAN1",
            "FUKURO",
            "HIPHIPHORE",
            "I2ASTM",
            "ITVAGANZA",
            "KAIPANG",
            "KATTO_CENDRAWASIH",
            "KCM2",
            "MAC",
            "MMS",
            "NAONE_61",
            "PHOENIX",
            "R-CORPORATION",
            "RENOVATIO_SANMAR",
            "RUBICK",
            "SC",
            "SC_VAGANZA",
            "SCC_(_SMAELI_CYBER_CLUB)",
            "SCIFI",
            "SMACAR",
            "SMAELI_IT",
            "SMAKSTA_2",
            "SMANTHA",
            "SPEED_DEMON",
            "STAG_1",
            "STAG_2",
            "STAG_3",
            "STAG_4",
            "STAG_6",
            "STAG_7",
            "THOTH",
            "THREE_AN",
            "TISSUE",
            "TRIXIE",
            "VEINE",
            "VERMOGEN",
            "VIKADO",
            "WYRM",
            "X-TRAVAGANZA"});
      this.tbUsername.AutoCompleteMode = System.Windows.Forms.AutoCompleteMode.SuggestAppend;
      this.tbUsername.AutoCompleteSource = System.Windows.Forms.AutoCompleteSource.CustomSource;
      this.tbUsername.CharacterCasing = System.Windows.Forms.CharacterCasing.Upper;
      this.tbUsername.Dock = System.Windows.Forms.DockStyle.Fill;
      this.tbUsername.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.tbUsername.Location = new System.Drawing.Point(0, 0);
      this.tbUsername.Name = "tbUsername";
      this.tbUsername.Size = new System.Drawing.Size(334, 29);
      this.tbUsername.TabIndex = 0;
      this.tbUsername.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
      this.tbUsername.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.tbUsername_KeyPress);
      // 
      // lbStatus
      // 
      this.lbStatus.Dock = System.Windows.Forms.DockStyle.Fill;
      this.lbStatus.Font = new System.Drawing.Font("Microsoft Sans Serif", 21.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.lbStatus.Location = new System.Drawing.Point(0, 0);
      this.lbStatus.Name = "lbStatus";
      this.lbStatus.Size = new System.Drawing.Size(334, 34);
      this.lbStatus.TabIndex = 2;
      this.lbStatus.Text = "Loading..";
      this.lbStatus.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
      this.lbStatus.Visible = false;
      // 
      // Form1
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(732, 520);
      this.Controls.Add(this.tableLayoutPanel1);
      this.Controls.Add(this.statusStrip1);
      this.Name = "Form1";
      this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
      this.Text = "Battleship";
      this.WindowState = System.Windows.Forms.FormWindowState.Maximized;
      this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Form1_FormClosing);
      this.Load += new System.EventHandler(this.Form1_Load);
      this.statusStrip1.ResumeLayout(false);
      this.statusStrip1.PerformLayout();
      this.tableLayoutPanel1.ResumeLayout(false);
      this.panel1.ResumeLayout(false);
      this.panel1.PerformLayout();
      this.panelContainer.ResumeLayout(false);
      this.panelGame.ResumeLayout(false);
      this.tableLayoutPanel3.ResumeLayout(false);
      this.tableLayoutPanel4.ResumeLayout(false);
      this.panel2.ResumeLayout(false);
      this.panel2.PerformLayout();
      this.pnlLogin.ResumeLayout(false);
      this.tableLayoutPanel2.ResumeLayout(false);
      this.panel3.ResumeLayout(false);
      this.panel3.PerformLayout();
      this.ResumeLayout(false);
      this.PerformLayout();

    }

    #endregion

    private System.Windows.Forms.TextBox textBox1;
    private System.Windows.Forms.TextBox textBox2;
    private System.Windows.Forms.StatusStrip statusStrip1;
    private System.Windows.Forms.ToolStripStatusLabel toolStripStatusLabel1;
    private System.Windows.Forms.TableLayoutPanel tableLayoutPanel1;
    private System.Windows.Forms.Panel panel1;
    private System.Windows.Forms.Panel panelContainer;
    private System.Windows.Forms.Panel pnlLogin;
    private System.Windows.Forms.TableLayoutPanel tableLayoutPanel2;
    private System.Windows.Forms.Button btnLogin;
    private System.Windows.Forms.Panel panel3;
    private System.Windows.Forms.TextBox tbUsername;
    private System.Windows.Forms.Label lbStatus;
    private System.Windows.Forms.Panel panelGame;
    private System.Windows.Forms.TableLayoutPanel tableLayoutPanel3;
    private System.Windows.Forms.TableLayoutPanel tableLayoutPanel4;
    private System.Windows.Forms.Panel panel2;
    private System.Windows.Forms.Panel panelGameSide;
    private System.Windows.Forms.Button btnAction;
    private System.Windows.Forms.Label lblGameStatus;
    private System.Windows.Forms.Panel panelGameMain;
    private System.Windows.Forms.Label lblObject;
    private System.Windows.Forms.Label lblEnemyObject;
  }
}

