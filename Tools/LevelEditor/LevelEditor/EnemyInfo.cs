using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace LevelEditor
{
    class EnemyInfo
    {
      
        private int num_Enemies;

        public int numEnemies
        {
            get { return num_Enemies; }
            set { num_Enemies = value; }
        }

       private EnemyType enemy_Type;

        public EnemyType enemyType
        {
            get { return enemy_Type; }
            set { enemy_Type = value; }
        }
    }
}
