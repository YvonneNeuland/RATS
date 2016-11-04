using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace LevelEditor
{
    class Level
    {
        public List<Wave> WaveList = new List<Wave>();

        private int num_Level;

        public int numLevel
        {
            get { return num_Level; }
            set { num_Level = value; }
        }

        private string level_Shape;

        public string levelShape
        {
            get { return level_Shape; }
            set { level_Shape = value; }
        }


    }
}
