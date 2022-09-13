using WinHideExGUI.Properties;

namespace WinHideExGUI
{
    partial class MainForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.NotifyIcon1 = new System.Windows.Forms.NotifyIcon(this.components);
            this.InjectionEnabled = new System.Windows.Forms.CheckBox();
            this.RunOnStartup = new System.Windows.Forms.CheckBox();
            this.InjectList = new System.Windows.Forms.ListBox();
            this.AddProgram = new System.Windows.Forms.Button();
            this.DeleteProgram = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.InjectionWorker = new System.ComponentModel.BackgroundWorker();
            this.backgroundWorker1 = new System.ComponentModel.BackgroundWorker();
            this.SaveButton = new System.Windows.Forms.Button();
            this.RegexList = new System.Windows.Forms.TextBox();
            this.SuspendLayout();
            // 
            // NotifyIcon1
            // 
            this.NotifyIcon1.BalloonTipIcon = System.Windows.Forms.ToolTipIcon.Info;
            this.NotifyIcon1.BalloonTipText = "Click to open the GUI";
            this.NotifyIcon1.Icon = global::WinHideExGUI.Properties.Resources.AppIcon;
            this.NotifyIcon1.Text = "WinHideEx";
            this.NotifyIcon1.Visible = true;
            this.NotifyIcon1.Click += new System.EventHandler(this.NotifyIcon1_Click);
            // 
            // InjectionEnabled
            // 
            this.InjectionEnabled.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.InjectionEnabled.AutoSize = true;
            this.InjectionEnabled.Location = new System.Drawing.Point(12, 432);
            this.InjectionEnabled.Name = "InjectionEnabled";
            this.InjectionEnabled.Size = new System.Drawing.Size(65, 17);
            this.InjectionEnabled.TabIndex = 0;
            this.InjectionEnabled.Text = "Enabled";
            this.InjectionEnabled.UseVisualStyleBackColor = true;
            // 
            // RunOnStartup
            // 
            this.RunOnStartup.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.RunOnStartup.AutoSize = true;
            this.RunOnStartup.Location = new System.Drawing.Point(83, 432);
            this.RunOnStartup.Name = "RunOnStartup";
            this.RunOnStartup.Size = new System.Drawing.Size(98, 17);
            this.RunOnStartup.TabIndex = 1;
            this.RunOnStartup.Text = "Run on Startup";
            this.RunOnStartup.UseVisualStyleBackColor = true;
            // 
            // InjectList
            // 
            this.InjectList.FormattingEnabled = true;
            this.InjectList.Location = new System.Drawing.Point(12, 41);
            this.InjectList.Name = "InjectList";
            this.InjectList.Size = new System.Drawing.Size(260, 225);
            this.InjectList.TabIndex = 2;
            // 
            // AddProgram
            // 
            this.AddProgram.Location = new System.Drawing.Point(12, 12);
            this.AddProgram.Name = "AddProgram";
            this.AddProgram.Size = new System.Drawing.Size(120, 23);
            this.AddProgram.TabIndex = 3;
            this.AddProgram.Text = "Add Application";
            this.AddProgram.UseVisualStyleBackColor = true;
            this.AddProgram.Click += new System.EventHandler(this.AddProgram_Click);
            // 
            // DeleteProgram
            // 
            this.DeleteProgram.Location = new System.Drawing.Point(152, 12);
            this.DeleteProgram.Name = "DeleteProgram";
            this.DeleteProgram.Size = new System.Drawing.Size(120, 23);
            this.DeleteProgram.TabIndex = 4;
            this.DeleteProgram.Text = "Delete Selected";
            this.DeleteProgram.UseVisualStyleBackColor = true;
            this.DeleteProgram.Click += new System.EventHandler(this.DeleteProgram_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 276);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(218, 13);
            this.label1.TabIndex = 6;
            this.label1.Text = "Regular expressions: (newlines equate to \"|\")";
            // 
            // InjectionWorker
            // 
            this.InjectionWorker.DoWork += new System.ComponentModel.DoWorkEventHandler(this.InjectionWorker_DoWork);
            // 
            // SaveButton
            // 
            this.SaveButton.Location = new System.Drawing.Point(187, 428);
            this.SaveButton.Name = "SaveButton";
            this.SaveButton.Size = new System.Drawing.Size(85, 23);
            this.SaveButton.TabIndex = 8;
            this.SaveButton.Text = "Save Changes";
            this.SaveButton.UseVisualStyleBackColor = true;
            this.SaveButton.Click += new System.EventHandler(this.SaveButton_Click);
            // 
            // RegexList
            // 
            this.RegexList.Location = new System.Drawing.Point(12, 292);
            this.RegexList.Multiline = true;
            this.RegexList.Name = "RegexList";
            this.RegexList.Size = new System.Drawing.Size(260, 130);
            this.RegexList.TabIndex = 9;
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(284, 461);
            this.Controls.Add(this.RegexList);
            this.Controls.Add(this.SaveButton);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.DeleteProgram);
            this.Controls.Add(this.AddProgram);
            this.Controls.Add(this.InjectList);
            this.Controls.Add(this.RunOnStartup);
            this.Controls.Add(this.InjectionEnabled);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.Name = "MainForm";
            this.Text = Resources.AppName;
            this.Load += new System.EventHandler(this.MainForm_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.NotifyIcon NotifyIcon1;
        private System.Windows.Forms.CheckBox InjectionEnabled;
        private System.Windows.Forms.CheckBox RunOnStartup;
        private System.Windows.Forms.ListBox InjectList;
        private System.Windows.Forms.Button AddProgram;
        private System.Windows.Forms.Button DeleteProgram;
        private System.Windows.Forms.Label label1;
        private System.ComponentModel.BackgroundWorker InjectionWorker;
        private System.ComponentModel.BackgroundWorker backgroundWorker1;
        private System.Windows.Forms.Button SaveButton;
        private System.Windows.Forms.TextBox RegexList;
    }
}

