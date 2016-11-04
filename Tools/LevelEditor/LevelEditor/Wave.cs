using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace LevelEditor
{
    class Wave
    {
        public List<EnemyInfo> EnemyInfoList = new List<EnemyInfo>();

        private int num_Wave;

        public int numWave
        {
            get { return num_Wave; }
            set { num_Wave = value; }
        }
    }
}
