namespace LevelEditor
{
    partial class Form1
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
            this.LevelListBox = new System.Windows.Forms.ListBox();
            this.WaveDataGroupBox = new System.Windows.Forms.GroupBox();
            this.NumEnemiesNumericUpDown = new System.Windows.Forms.NumericUpDown();
            this.NumEnemiesLabel = new System.Windows.Forms.Label();
            this.RemoveEnemyButton = new System.Windows.Forms.Button();
            this.AddEnemyButton = new System.Windows.Forms.Button();
            this.EnemyTypeComboBox = new System.Windows.Forms.ComboBox();
            this.EnemyInfoListBox = new System.Windows.Forms.ListBox();
            this.EnemyTypeLabel = new System.Windows.Forms.Label();
            this.removeLevelButton = new System.Windows.Forms.Button();
            this.LevelGroupBox = new System.Windows.Forms.GroupBox();
            this.AddLevelButton = new System.Windows.Forms.Button();
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.newToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.openToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator = new System.Windows.Forms.ToolStripSeparator();
            this.saveToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
            this.exitToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.WaveOptionsGroupBox = new System.Windows.Forms.GroupBox();
            this.AddWaveButton = new System.Windows.Forms.Button();
            this.WaveListBox = new System.Windows.Forms.ListBox();
            this.RemoveWaveButton = new System.Windows.Forms.Button();
            this.LabelShape = new System.Windows.Forms.Label();
            this.ShapeTextBox = new System.Windows.Forms.TextBox();
            this.WaveDataGroupBox.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.NumEnemiesNumericUpDown)).BeginInit();
            this.LevelGroupBox.SuspendLayout();
            this.menuStrip1.SuspendLayout();
            this.WaveOptionsGroupBox.SuspendLayout();
            this.SuspendLayout();
            // 
            // LevelListBox
            // 
            this.LevelListBox.FormattingEnabled = true;
            this.LevelListBox.ItemHeight = 16;
            this.LevelListBox.Location = new System.Drawing.Point(6, 21);
            this.LevelListBox.Name = "LevelListBox";
            this.LevelListBox.Size = new System.Drawing.Size(305, 148);
            this.LevelListBox.TabIndex = 0;
            this.LevelListBox.SelectedIndexChanged += new System.EventHandler(this.LevelListBox_SelectedIndexChanged);
            // 
            // WaveDataGroupBox
            // 
            this.WaveDataGroupBox.Controls.Add(this.NumEnemiesNumericUpDown);
            this.WaveDataGroupBox.Controls.Add(this.NumEnemiesLabel);
            this.WaveDataGroupBox.Controls.Add(this.RemoveEnemyButton);
            this.WaveDataGroupBox.Controls.Add(this.AddEnemyButton);
            this.WaveDataGroupBox.Controls.Add(this.EnemyTypeComboBox);
            this.WaveDataGroupBox.Controls.Add(this.EnemyInfoListBox);
            this.WaveDataGroupBox.Controls.Add(this.EnemyTypeLabel);
            this.WaveDataGroupBox.Location = new System.Drawing.Point(12, 523);
            this.WaveDataGroupBox.Name = "WaveDataGroupBox";
            this.WaveDataGroupBox.Size = new System.Drawing.Size(328, 317);
            this.WaveDataGroupBox.TabIndex = 1;
            this.WaveDataGroupBox.TabStop = false;
            this.WaveDataGroupBox.Text = "Enemy Options";
            // 
            // NumEnemiesNumericUpDown
            // 
            this.NumEnemiesNumericUpDown.Location = new System.Drawing.Point(110, 228);
            this.NumEnemiesNumericUpDown.Name = "NumEnemiesNumericUpDown";
            this.NumEnemiesNumericUpDown.Size = new System.Drawing.Size(120, 22);
            this.NumEnemiesNumericUpDown.TabIndex = 7;
            // 
            // NumEnemiesLabel
            // 
            this.NumEnemiesLabel.AutoSize = true;
            this.NumEnemiesLabel.Location = new System.Drawing.Point(9, 228);
            this.NumEnemiesLabel.Name = "NumEnemiesLabel";
            this.NumEnemiesLabel.Size = new System.Drawing.Size(95, 17);
            this.NumEnemiesLabel.TabIndex = 6;
            this.NumEnemiesLabel.Text = "Num Enemies";
            // 
            // RemoveEnemyButton
            // 
            this.RemoveEnemyButton.Location = new System.Drawing.Point(159, 275);
            this.RemoveEnemyButton.Name = "RemoveEnemyButton";
            this.RemoveEnemyButton.Size = new System.Drawing.Size(129, 23);
            this.RemoveEnemyButton.TabIndex = 5;
            this.RemoveEnemyButton.Text = "Remove Enemy";
            this.RemoveEnemyButton.UseVisualStyleBackColor = true;
            this.RemoveEnemyButton.Click += new System.EventHandler(this.RemoveEnemyButton_Click);
            // 
            // AddEnemyButton
            // 
            this.AddEnemyButton.Location = new System.Drawing.Point(12, 275);
            this.AddEnemyButton.Name = "AddEnemyButton";
            this.AddEnemyButton.Size = new System.Drawing.Size(129, 23);
            this.AddEnemyButton.TabIndex = 4;
            this.AddEnemyButton.Text = "Add Enemy";
            this.AddEnemyButton.UseVisualStyleBackColor = true;
            this.AddEnemyButton.Click += new System.EventHandler(this.AddEnemyButton_Click);
            // 
            // EnemyTypeComboBox
            // 
            this.EnemyTypeComboBox.FormattingEnabled = true;
            this.EnemyTypeComboBox.Location = new System.Drawing.Point(110, 198);
            this.EnemyTypeComboBox.Name = "EnemyTypeComboBox";
            this.EnemyTypeComboBox.Size = new System.Drawing.Size(202, 24);
            this.EnemyTypeComboBox.TabIndex = 1;
            this.EnemyTypeComboBox.SelectedIndexChanged += new System.EventHandler(this.EnemyTypeComboBox_SelectedIndexChanged);
            // 
            // EnemyInfoListBox
            // 
            this.EnemyInfoListBox.FormattingEnabled = true;
            this.EnemyInfoListBox.ItemHeight = 16;
            this.EnemyInfoListBox.Location = new System.Drawing.Point(7, 34);
            this.EnemyInfoListBox.Name = "EnemyInfoListBox";
            this.EnemyInfoListBox.Size = new System.Drawing.Size(305, 148);
            this.EnemyInfoListBox.TabIndex = 2;
            this.EnemyInfoListBox.SelectedIndexChanged += new System.EventHandler(this.EnemyInfoListBox_SelectedIndexChanged);
            // 
            // EnemyTypeLabel
            // 
            this.EnemyTypeLabel.AutoSize = true;
            this.EnemyTypeLabel.Location = new System.Drawing.Point(9, 198);
            this.EnemyTypeLabel.Name = "EnemyTypeLabel";
            this.EnemyTypeLabel.Size = new System.Drawing.Size(83, 17);
            this.EnemyTypeLabel.TabIndex = 0;
            this.EnemyTypeLabel.Text = "EnemyType";
            // 
            // removeLevelButton
            // 
            this.removeLevelButton.Location = new System.Drawing.Point(159, 212);
            this.removeLevelButton.Name = "removeLevelButton";
            this.removeLevelButton.Size = new System.Drawing.Size(129, 23);
            this.removeLevelButton.TabIndex = 3;
            this.removeLevelButton.Text = "Remove Level";
            this.removeLevelButton.UseVisualStyleBackColor = true;
            this.removeLevelButton.Click += new System.EventHandler(this.removeLevelButton_Click);
            // 
            // LevelGroupBox
            // 
            this.LevelGroupBox.Controls.Add(this.ShapeTextBox);
            this.LevelGroupBox.Controls.Add(this.LabelShape);
            this.LevelGroupBox.Controls.Add(this.AddLevelButton);
            this.LevelGroupBox.Controls.Add(this.LevelListBox);
            this.LevelGroupBox.Controls.Add(this.removeLevelButton);
            this.LevelGroupBox.Location = new System.Drawing.Point(12, 45);
            this.LevelGroupBox.Name = "LevelGroupBox";
            this.LevelGroupBox.Size = new System.Drawing.Size(328, 250);
            this.LevelGroupBox.TabIndex = 4;
            this.LevelGroupBox.TabStop = false;
            this.LevelGroupBox.Text = "Level Options";
            // 
            // AddLevelButton
            // 
            this.AddLevelButton.Location = new System.Drawing.Point(12, 212);
            this.AddLevelButton.Name = "AddLevelButton";
            this.AddLevelButton.Size = new System.Drawing.Size(129, 23);
            this.AddLevelButton.TabIndex = 4;
            this.AddLevelButton.Text = "Add Level";
            this.AddLevelButton.UseVisualStyleBackColor = true;
            this.AddLevelButton.Click += new System.EventHandler(this.AddLevelButton_Click);
            // 
            // menuStrip1
            // 
            this.menuStrip1.ImageScalingSize = new System.Drawing.Size(20, 20);
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(371, 28);
            this.menuStrip1.TabIndex = 5;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // fileToolStripMenuItem
            // 
            this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.newToolStripMenuItem,
            this.openToolStripMenuItem,
            this.toolStripSeparator,
            this.saveToolStripMenuItem,
            this.toolStripSeparator1,
            this.toolStripSeparator2,
            this.exitToolStripMenuItem});
            this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
            this.fileToolStripMenuItem.Size = new System.Drawing.Size(44, 24);
            this.fileToolStripMenuItem.Text = "&File";
            // 
            // newToolStripMenuItem
            // 
            this.newToolStripMenuItem.Image = ((System.Drawing.Image)(resources.GetObject("newToolStripMenuItem.Image")));
            this.newToolStripMenuItem.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.newToolStripMenuItem.Name = "newToolStripMenuItem";
            this.newToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.N)));
            this.newToolStripMenuItem.Size = new System.Drawing.Size(173, 26);
            this.newToolStripMenuItem.Text = "&New";
            this.newToolStripMenuItem.Click += new System.EventHandler(this.newToolStripMenuItem_Click);
            // 
            // openToolStripMenuItem
            // 
            this.openToolStripMenuItem.Image = ((System.Drawing.Image)(resources.GetObject("openToolStripMenuItem.Image")));
            this.openToolStripMenuItem.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.openToolStripMenuItem.Name = "openToolStripMenuItem";
            this.openToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.O)));
            this.openToolStripMenuItem.Size = new System.Drawing.Size(173, 26);
            this.openToolStripMenuItem.Text = "&Open";
            this.openToolStripMenuItem.Click += new System.EventHandler(this.openToolStripMenuItem_Click);
            // 
            // toolStripSeparator
            // 
            this.toolStripSeparator.Name = "toolStripSeparator";
            this.toolStripSeparator.Size = new System.Drawing.Size(170, 6);
            // 
            // saveToolStripMenuItem
            // 
            this.saveToolStripMenuItem.Image = ((System.Drawing.Image)(resources.GetObject("saveToolStripMenuItem.Image")));
            this.saveToolStripMenuItem.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.saveToolStripMenuItem.Name = "saveToolStripMenuItem";
            this.saveToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.S)));
            this.saveToolStripMenuItem.Size = new System.Drawing.Size(173, 26);
            this.saveToolStripMenuItem.Text = "&Save";
            this.saveToolStripMenuItem.Click += new System.EventHandler(this.saveToolStripMenuItem_Click);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(170, 6);
            // 
            // toolStripSeparator2
            // 
            this.toolStripSeparator2.Name = "toolStripSeparator2";
            this.toolStripSeparator2.Size = new System.Drawing.Size(170, 6);
            // 
            // exitToolStripMenuItem
            // 
            this.exitToolStripMenuItem.Name = "exitToolStripMenuItem";
            this.exitToolStripMenuItem.Size = new System.Drawing.Size(173, 26);
            this.exitToolStripMenuItem.Text = "E&xit";
            this.exitToolStripMenuItem.Click += new System.EventHandler(this.exitToolStripMenuItem_Click);
            // 
            // WaveOptionsGroupBox
            // 
            this.WaveOptionsGroupBox.Controls.Add(this.AddWaveButton);
            this.WaveOptionsGroupBox.Controls.Add(this.WaveListBox);
            this.WaveOptionsGroupBox.Controls.Add(this.RemoveWaveButton);
            this.WaveOptionsGroupBox.Location = new System.Drawing.Point(12, 301);
            this.WaveOptionsGroupBox.Name = "WaveOptionsGroupBox";
            this.WaveOptionsGroupBox.Size = new System.Drawing.Size(328, 216);
            this.WaveOptionsGroupBox.TabIndex = 5;
            this.WaveOptionsGroupBox.TabStop = false;
            this.WaveOptionsGroupBox.Text = "Wave Options";
            // 
            // AddWaveButton
            // 
            this.AddWaveButton.Location = new System.Drawing.Point(12, 175);
            this.AddWaveButton.Name = "AddWaveButton";
            this.AddWaveButton.Size = new System.Drawing.Size(129, 23);
            this.AddWaveButton.TabIndex = 4;
            this.AddWaveButton.Text = "Add Wave";
            this.AddWaveButton.UseVisualStyleBackColor = true;
            this.AddWaveButton.Click += new System.EventHandler(this.AddWaveButton_Click);
            // 
            // WaveListBox
            // 
            this.WaveListBox.FormattingEnabled = true;
            this.WaveListBox.ItemHeight = 16;
            this.WaveListBox.Location = new System.Drawing.Point(12, 21);
            this.WaveListBox.Name = "WaveListBox";
            this.WaveListBox.Size = new System.Drawing.Size(305, 148);
            this.WaveListBox.TabIndex = 0;
            this.WaveListBox.SelectedIndexChanged += new System.EventHandler(this.WaveListBox_SelectedIndexChanged_1);
            // 
            // RemoveWaveButton
            // 
            this.RemoveWaveButton.Location = new System.Drawing.Point(159, 175);
            this.RemoveWaveButton.Name = "RemoveWaveButton";
            this.RemoveWaveButton.Size = new System.Drawing.Size(129, 23);
            this.RemoveWaveButton.TabIndex = 3;
            this.RemoveWaveButton.Text = "Remove Wave";
            this.RemoveWaveButton.UseVisualStyleBackColor = true;
            this.RemoveWaveButton.Click += new System.EventHandler(this.RemoveWaveButton_Click);
            // 
            // LabelShape
            // 
            this.LabelShape.AutoSize = true;
            this.LabelShape.Location = new System.Drawing.Point(12, 176);
            this.LabelShape.Name = "LabelShape";
            this.LabelShape.Size = new System.Drawing.Size(49, 17);
            this.LabelShape.TabIndex = 5;
            this.LabelShape.Text = "Shape";
            // 
            // ShapeTextBox
            // 
            this.ShapeTextBox.Location = new System.Drawing.Point(68, 176);
            this.ShapeTextBox.Name = "ShapeTextBox";
            this.ShapeTextBox.Size = new System.Drawing.Size(243, 22);
            this.ShapeTextBox.TabIndex = 6;
            this.ShapeTextBox.TextChanged += new System.EventHandler(this.ShapeTextBox_TextChanged);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(371, 863);
            this.Controls.Add(this.WaveOptionsGroupBox);
            this.Controls.Add(this.LevelGroupBox);
            this.Controls.Add(this.WaveDataGroupBox);
            this.Controls.Add(this.menuStrip1);
            this.MainMenuStrip = this.menuStrip1;
            this.Name = "Form1";
            this.Text = "Form1";
            this.WaveDataGroupBox.ResumeLayout(false);
            this.WaveDataGroupBox.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.NumEnemiesNumericUpDown)).EndInit();
            this.LevelGroupBox.ResumeLayout(false);
            this.LevelGroupBox.PerformLayout();
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            this.WaveOptionsGroupBox.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ListBox LevelListBox;
        private System.Windows.Forms.GroupBox WaveDataGroupBox;
        private System.Windows.Forms.ComboBox EnemyTypeComboBox;
        private System.Windows.Forms.Label EnemyTypeLabel;
        private System.Windows.Forms.ListBox EnemyInfoListBox;
        private System.Windows.Forms.Button removeLevelButton;
        private System.Windows.Forms.GroupBox LevelGroupBox;
        private System.Windows.Forms.NumericUpDown NumEnemiesNumericUpDown;
        private System.Windows.Forms.Label NumEnemiesLabel;
        private System.Windows.Forms.Button RemoveEnemyButton;
        private System.Windows.Forms.Button AddEnemyButton;
        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem newToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem openToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator;
        private System.Windows.Forms.ToolStripMenuItem saveToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
        private System.Windows.Forms.ToolStripMenuItem exitToolStripMenuItem;
        private System.Windows.Forms.Button AddLevelButton;
        private System.Windows.Forms.GroupBox WaveOptionsGroupBox;
        private System.Windows.Forms.Button AddWaveButton;
        private System.Windows.Forms.ListBox WaveListBox;
        private System.Windows.Forms.Button RemoveWaveButton;
        private System.Windows.Forms.TextBox ShapeTextBox;
        private System.Windows.Forms.Label LabelShape;
    }
}

