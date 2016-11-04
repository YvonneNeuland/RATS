using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Xml.Serialization;
using System.Xml.Linq;
using System.IO;

namespace LevelEditor
{
    public partial class Form1 : Form
    {
        
        List<Level> Levels = new List<Level>();

        int currentLevel = -1;
        int currentWave = -1;
        int currentEnemyInfo = -1;


        public Form1()
        {
            InitializeComponent();

            EnemyTypeComboBox.DataSource = Enum.GetValues(typeof(EnemyType));
        }


        private void EnemyTypeComboBox_SelectedIndexChanged(object sender, EventArgs e)
        {

        }


        private void AddLevelButton_Click(object sender, EventArgs e)
        {
            Level newLevel = new Level();
            newLevel.numLevel = Levels.Count;
            newLevel.levelShape = ShapeTextBox.Text;
            Levels.Add(newLevel);
            LevelListBox.Items.Add(newLevel.numLevel);
            LevelListBox.SelectedIndex = LevelListBox.Items.Count - 1;
            
        }

        private void removeLevelButton_Click(object sender, EventArgs e)
        {
            if (currentLevel != -1 && LevelListBox.SelectedIndex > -1)
            {
                Levels.RemoveAt(LevelListBox.SelectedIndex);
                LevelListBox.Items.Clear();

                for (int i = 0; i < Levels.Count; i++)
                {
                    Levels[i].numLevel = i;
                    LevelListBox.Items.Add(i);
                }

                WaveListBox.Items.Clear();
                EnemyInfoListBox.Items.Clear();
            }
        }

        //WaveListBox
        private void WaveListBox_SelectedIndexChanged_1(object sender, EventArgs e)
        {
            if (WaveListBox.SelectedIndex <= -1)
            {
                currentWave = -1;
            }
            else
            {
                currentWave = (int)WaveListBox.Items[WaveListBox.SelectedIndex];

                EnemyInfoListBox.Items.Clear();

                for (int i = 0; i < Levels[LevelListBox.SelectedIndex].WaveList[WaveListBox.SelectedIndex].EnemyInfoList.Count; i++)
                {
                    EnemyInfoListBox.Items.Add(Levels[LevelListBox.SelectedIndex].WaveList[WaveListBox.SelectedIndex].EnemyInfoList[i].enemyType);                    
                }
            }
        }

        private void EnemyInfoListBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (EnemyInfoListBox.SelectedIndex <= -1)
            {
                currentEnemyInfo = -1;                
            }
            else
            {
                currentEnemyInfo = EnemyInfoListBox.SelectedIndex;
                EnemyTypeComboBox.SelectedIndex = (int)Levels[LevelListBox.SelectedIndex].WaveList[WaveListBox.SelectedIndex].EnemyInfoList[EnemyInfoListBox.SelectedIndex].enemyType;
                NumEnemiesNumericUpDown.Value = Levels[LevelListBox.SelectedIndex].WaveList[WaveListBox.SelectedIndex].EnemyInfoList[EnemyInfoListBox.SelectedIndex].numEnemies;
            }
        }

        private void LevelListBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (LevelListBox.SelectedIndex <= -1)
            {
                currentLevel = -1;
            }
            else
            {
                currentLevel = (int)LevelListBox.Items[LevelListBox.SelectedIndex];
                WaveListBox.Items.Clear();
                EnemyInfoListBox.Items.Clear();
                ShapeTextBox.Text = Levels[LevelListBox.SelectedIndex].levelShape;

                if (currentLevel != -1)
                {
                    for (int i = 0; i < Levels[LevelListBox.SelectedIndex].WaveList.Count; i++)
                    {
                        WaveListBox.Items.Add(i);                     
                    }
                }
            }
        }

        private void AddWaveButton_Click(object sender, EventArgs e)
        {
            Wave newWave = new Wave();

            if (currentLevel != -1 && LevelListBox.SelectedIndex > -1)
            {
                newWave.numWave = Levels[LevelListBox.SelectedIndex].WaveList.Count;

                Levels[LevelListBox.SelectedIndex].WaveList.Add(newWave);
                WaveListBox.Items.Add(newWave.numWave);

                WaveListBox.SelectedIndex = WaveListBox.Items.Count - 1;
            }

        }

        private void RemoveWaveButton_Click(object sender, EventArgs e)
        {
            if (currentWave != -1)
            {
                Levels[currentLevel].WaveList.RemoveAt(currentWave);
                WaveListBox.Items.Clear();
                EnemyInfoListBox.Items.Clear();
                EnemyTypeComboBox.SelectedIndex = 0;
                NumEnemiesNumericUpDown.Value = 0;

                for (int i = 0; i < Levels[currentLevel].WaveList.Count; i++)
                {
                    Levels[currentLevel].WaveList[i].numWave = i;
                    WaveListBox.Items.Add(i);
                }
            }
        }

        private void AddEnemyButton_Click(object sender, EventArgs e)
        {
            EnemyInfo newEnemyInfo = new EnemyInfo();

            if (currentLevel != -1 && currentWave != -1 && LevelListBox.SelectedIndex > -1 && WaveListBox.SelectedIndex > -1)
            {
                newEnemyInfo.enemyType = (EnemyType)EnemyTypeComboBox.SelectedValue;
                newEnemyInfo.numEnemies = (int)NumEnemiesNumericUpDown.Value;
                Levels[LevelListBox.SelectedIndex].WaveList[WaveListBox.SelectedIndex].EnemyInfoList.Add(newEnemyInfo);
                EnemyInfoListBox.Items.Add(newEnemyInfo.enemyType);
                EnemyInfoListBox.SelectedIndex = EnemyInfoListBox.Items.Count - 1;
            }
        }

        private void RemoveEnemyButton_Click(object sender, EventArgs e)
        {
            Levels[LevelListBox.SelectedIndex].WaveList[WaveListBox.SelectedIndex].EnemyInfoList.RemoveAt(EnemyInfoListBox.SelectedIndex);
            EnemyInfoListBox.Items.RemoveAt(EnemyInfoListBox.SelectedIndex);
        }

        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void saveToolStripMenuItem_Click(object sender, EventArgs e)
        {
             SaveFileDialog dlg = new SaveFileDialog();
            dlg.Filter = "All Files|*.*|Xml Files|*.xml";
            dlg.FilterIndex = 2;
            dlg.DefaultExt = "xml";



            if (DialogResult.OK == dlg.ShowDialog())
            {
                XElement xRoot = new XElement("Root");

                for (int level = 0; level < Levels.Count; level++)
                {
                    XElement xLevel = new XElement("level");
                    xRoot.Add(xLevel);

                    int numLevel = Levels[level].numLevel;
                    XAttribute xNumLevel = new XAttribute("numLevel", numLevel);
                    xLevel.Add(xNumLevel);

                    string levelShape = Levels[level].levelShape;
                    XAttribute xLevelShape = new XAttribute("levelShape", levelShape);
                    xLevel.Add(xLevelShape);

                    for (int wave = 0; wave < Levels[level].WaveList.Count; wave++)
                    {
                        XElement xWave = new XElement("wave");
                        xLevel.Add(xWave);

                        int numWave = Levels[level].WaveList[wave].numWave;
                        XAttribute xNumWave = new XAttribute("numWave", numWave);
                        xWave.Add(xNumWave);

                        for (int enemyInfo = 0; enemyInfo < Levels[level].WaveList[wave].EnemyInfoList.Count; enemyInfo++)
                        {
                            XElement xEnemyInfo = new XElement("enemyInfo");
                            xWave.Add(xEnemyInfo);

                            int enemyType = (int)Levels[level].WaveList[wave].EnemyInfoList[enemyInfo].enemyType;
                            XAttribute xEnemyType = new XAttribute("enemyType", enemyType);
                            xEnemyInfo.Add(xEnemyType);

                            int numEnemies = Levels[level].WaveList[wave].EnemyInfoList[enemyInfo].numEnemies;
                            XAttribute xNumEnemies = new XAttribute("numEnemies", numEnemies);
                            xEnemyInfo.Add(xNumEnemies);
                        }
                    }
                }

                xRoot.Save(dlg.FileName);
            }
        }

        private void newToolStripMenuItem_Click(object sender, EventArgs e)
        {
            LevelListBox.Items.Clear();
            WaveListBox.Items.Clear();
            EnemyInfoListBox.Items.Clear();
            EnemyTypeComboBox.SelectedIndex = 0;
            NumEnemiesNumericUpDown.Value = 0;
            Levels.Clear();
        }

        private void openToolStripMenuItem_Click(object sender, EventArgs e)
        {
            LevelListBox.Items.Clear();
            WaveListBox.Items.Clear();
            EnemyInfoListBox.Items.Clear();
            EnemyTypeComboBox.SelectedIndex = 0;
            NumEnemiesNumericUpDown.Value = 0;
            Levels.Clear();

            OpenFileDialog dlg = new OpenFileDialog();
            dlg.Filter = "All Files|*.*|Xml Files|*.xml";
            dlg.FilterIndex = 2;

            if (DialogResult.OK == dlg.ShowDialog())
            {
                XElement xRoot = XElement.Load(dlg.FileName);

                IEnumerable<XElement> xLevels = xRoot.Elements();


                foreach (XElement xLevel in xLevels)
                {
                    Level newLevel = new Level();

                    XAttribute xNumLevel = xLevel.Attribute("numLevel");
                    int numLevel = Convert.ToInt32(xNumLevel.Value);
                    newLevel.numLevel = numLevel;

                    XAttribute xLevelShape = xLevel.Attribute("levelShape");
                    string levelShape = Convert.ToString(xLevelShape.Value);
                    newLevel.levelShape = levelShape;

                    Levels.Add(newLevel);                    

                    IEnumerable<XElement> xWaves = xLevel.Elements();

                    foreach (XElement xWave in xWaves)
                    {
                        Wave newWave = new Wave();

                        XAttribute xNumWave = xWave.Attribute("numWave");
                        int numWave = Convert.ToInt32(xNumWave.Value);
                        newWave.numWave = numWave;

                        newLevel.WaveList.Add(newWave);

                        IEnumerable<XElement> xEnemyInfos = xWave.Elements();

                        foreach (XElement xEnemyInfo in xEnemyInfos)
                        {
                            EnemyInfo newEnemyInfo = new EnemyInfo();

                            XAttribute xEnemyType = xEnemyInfo.Attribute("enemyType");
                            EnemyType enemyType = (EnemyType)Convert.ToInt32(xEnemyType.Value);
                            newEnemyInfo.enemyType = enemyType;

                            XAttribute xNumEnemies = xEnemyInfo.Attribute("numEnemies");
                            int numEnemies = Convert.ToInt32(xNumEnemies.Value);
                            newEnemyInfo.numEnemies = numEnemies;

                            newWave.EnemyInfoList.Add(newEnemyInfo);
                        }
                    }
                }

                for (int level = 0; level < Levels.Count; level++)
                {
                    LevelListBox.Items.Add(Levels[level].numLevel);
                }

                if (LevelListBox.Items.Count > 0)
                {
                    LevelListBox.SelectedIndex = 0;

                    if (WaveListBox.Items.Count > 0)
                    {
                        WaveListBox.SelectedIndex = 0;

                        if (EnemyInfoListBox.Items.Count > 0)
                        {
                            EnemyInfoListBox.SelectedIndex = 0;
                        }
                    }
                }
            }
        }

        private void ShapeTextBox_TextChanged(object sender, EventArgs e)
        {
            if (LevelListBox.SelectedIndex <= -1)
            {
                currentLevel = -1;
            }
            else
            {
                
            }
        }
    }
}
