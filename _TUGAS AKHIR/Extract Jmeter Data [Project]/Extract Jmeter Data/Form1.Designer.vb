<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class Form1
    Inherits System.Windows.Forms.Form

    'Form overrides dispose to clean up the component list.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        Try
            If disposing AndAlso components IsNot Nothing Then
                components.Dispose()
            End If
        Finally
            MyBase.Dispose(disposing)
        End Try
    End Sub

    'Required by the Windows Form Designer
    Private components As System.ComponentModel.IContainer

    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerStepThrough()> _
    Private Sub InitializeComponent()
        Me.OpenFileDialog1 = New System.Windows.Forms.OpenFileDialog()
        Me.Button1 = New System.Windows.Forms.Button()
        Me.Button3 = New System.Windows.Forms.Button()
        Me.FolderBrowserDialog1 = New System.Windows.Forms.FolderBrowserDialog()
        Me.raw_radio = New System.Windows.Forms.RadioButton()
        Me.mean_radio = New System.Windows.Forms.RadioButton()
        Me.count_txt = New System.Windows.Forms.TextBox()
        Me.ListBox1 = New System.Windows.Forms.ListBox()
        Me.r_time = New System.Windows.Forms.RadioButton()
        Me.SuspendLayout()
        '
        'OpenFileDialog1
        '
        Me.OpenFileDialog1.FileName = "OpenFileDialog1"
        Me.OpenFileDialog1.Multiselect = True
        '
        'Button1
        '
        Me.Button1.Location = New System.Drawing.Point(12, 17)
        Me.Button1.Name = "Button1"
        Me.Button1.Size = New System.Drawing.Size(75, 23)
        Me.Button1.TabIndex = 3
        Me.Button1.Text = "Open File"
        Me.Button1.UseVisualStyleBackColor = True
        '
        'Button3
        '
        Me.Button3.Location = New System.Drawing.Point(780, 72)
        Me.Button3.Name = "Button3"
        Me.Button3.Size = New System.Drawing.Size(121, 49)
        Me.Button3.TabIndex = 6
        Me.Button3.Text = "Process"
        Me.Button3.UseVisualStyleBackColor = True
        '
        'raw_radio
        '
        Me.raw_radio.AutoSize = True
        Me.raw_radio.Checked = True
        Me.raw_radio.Location = New System.Drawing.Point(780, 128)
        Me.raw_radio.Name = "raw_radio"
        Me.raw_radio.Size = New System.Drawing.Size(73, 17)
        Me.raw_radio.TabIndex = 7
        Me.raw_radio.TabStop = True
        Me.raw_radio.Text = "Raw Data"
        Me.raw_radio.UseVisualStyleBackColor = True
        '
        'mean_radio
        '
        Me.mean_radio.AutoSize = True
        Me.mean_radio.Location = New System.Drawing.Point(780, 151)
        Me.mean_radio.Name = "mean_radio"
        Me.mean_radio.Size = New System.Drawing.Size(91, 17)
        Me.mean_radio.TabIndex = 8
        Me.mean_radio.Text = "Average Data"
        Me.mean_radio.UseVisualStyleBackColor = True
        '
        'count_txt
        '
        Me.count_txt.Location = New System.Drawing.Point(877, 151)
        Me.count_txt.Name = "count_txt"
        Me.count_txt.ReadOnly = True
        Me.count_txt.Size = New System.Drawing.Size(52, 20)
        Me.count_txt.TabIndex = 9
        Me.count_txt.Text = "10"
        '
        'ListBox1
        '
        Me.ListBox1.FormattingEnabled = True
        Me.ListBox1.Location = New System.Drawing.Point(12, 46)
        Me.ListBox1.Name = "ListBox1"
        Me.ListBox1.Size = New System.Drawing.Size(762, 160)
        Me.ListBox1.TabIndex = 10
        '
        'r_time
        '
        Me.r_time.AutoSize = True
        Me.r_time.Location = New System.Drawing.Point(780, 174)
        Me.r_time.Name = "r_time"
        Me.r_time.Size = New System.Drawing.Size(94, 17)
        Me.r_time.TabIndex = 11
        Me.r_time.Text = "Respond Time"
        Me.r_time.UseVisualStyleBackColor = True
        '
        'Form1
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(943, 215)
        Me.Controls.Add(Me.r_time)
        Me.Controls.Add(Me.ListBox1)
        Me.Controls.Add(Me.count_txt)
        Me.Controls.Add(Me.mean_radio)
        Me.Controls.Add(Me.raw_radio)
        Me.Controls.Add(Me.Button3)
        Me.Controls.Add(Me.Button1)
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle
        Me.MaximizeBox = False
        Me.MinimizeBox = False
        Me.Name = "Form1"
        Me.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen
        Me.Text = "Form1"
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents OpenFileDialog1 As System.Windows.Forms.OpenFileDialog
    Friend WithEvents Button1 As System.Windows.Forms.Button
    Friend WithEvents Button3 As System.Windows.Forms.Button
    Friend WithEvents FolderBrowserDialog1 As System.Windows.Forms.FolderBrowserDialog
    Friend WithEvents raw_radio As System.Windows.Forms.RadioButton
    Friend WithEvents mean_radio As System.Windows.Forms.RadioButton
    Friend WithEvents count_txt As System.Windows.Forms.TextBox
    Friend WithEvents ListBox1 As System.Windows.Forms.ListBox
    Friend WithEvents r_time As System.Windows.Forms.RadioButton

End Class
