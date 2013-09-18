using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace Battleship {
  public partial class FormInfo : Form {

    private int x;
    private int y;
    private int ctr;
    private Panel[,] pnls;

    public FormInfo() {
      InitializeComponent();
    }
    public FormInfo(String Board, int x, int y, int size) {
      InitializeComponent();

      this.x = x; this.y = y; this.ctr = 0;
      pnls = new Panel[size, size];

      int boardSize = size * size;
      for(int i = 0; i < boardSize; i++) {
        Panel p = new Panel();
        p.Size = new Size(Form1._DEFAULT_PANEL_SIZE, Form1._DEFAULT_PANEL_SIZE);
        p.Location = new Point((i % size) * Form1._DEFAULT_PANEL_SIZE, (i / size) * Form1._DEFAULT_PANEL_SIZE);
        p.BorderStyle = BorderStyle.Fixed3D;
        p.BackgroundImageLayout = ImageLayout.Stretch;
        p.BackgroundImage = Image.FromFile("./Images/soil.png");
        switch(Board[i]) {
          case '1' :
            p.BackgroundImage = Image.FromFile("./Images/strawberry.png");
            break;
          case '2':
            p.BackgroundImage = Image.FromFile("./Images/stone.png");
            break;
        }
        pnls[i % size, i / size] = p;
        panel1.Controls.Add(p);
      }

      int w = 652; int h = 724;
      int dw = 652 - 640; int dh = 724 - 640;
      w = dw + size * 64; h = dh + size * 64;
      this.Size = new Size(w, h);
    }

    private void timer1_Tick(object sender, EventArgs e) {
      this.ctr++;
      pnls[this.x, this.y].BackgroundImage = (ctr % 2 == 0 ? Image.FromFile("./Images/soil.png") : Image.FromFile("./Images/stone.png"));
      pnls[this.x, this.y].Refresh();
    }

    private void FormInfo_Load(object sender, EventArgs e) {
      timer1.Start();
    }
  }
}
