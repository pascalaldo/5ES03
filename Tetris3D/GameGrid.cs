using System;
using System.Linq;
using System.Collections.Generic;
using System.Text;
using Microsoft.Xna.Framework;

namespace HighRoller
{
    static class GameGrid
    {
        private Block[] blocks; //The blocks 
        private int width, height; //Width and height of the grid, assuming a width*width base.

        private float totalElapsedTime = 0; //Time elapsed since last step (in seconds)

        private int score = 0;

        public GameGrid(int width, int height)
        {
            this.width = width;
            this.height = height;
            blocks = new Block[width * width * height]; //This is the maximum number of blocks that can be in the grid at the same time.
        }


        //Checks for full layers/rows (whatever term you prefer), removes them and moves blocks down accordingly
        public void fullLayers()
        {
            int[] layerCount = new int[height];
            foreach (Block b in blocks)
            {
                int[] blockCount = b.countCubesOnRows();
                for (int i = 0; i < blockCount.Length; i++)
                {
                    layerCount[i] += blockCount[i];
                }
            }
            for (int i = 0; i < layerCount.Length; i++)
            {
                if (layerCount[i] == width * width)
                {
                    foreach (Block b in blocks)
                    {
                        b.removeRow(i);
                        i--;                            //Since after deleting a layer, all other blocks are moved down, layer i+1 becomes layer i. Therefore,
                                                        //this ensures that with the next iteration, layer i is checked again, 
                        score += width * width;
                    }
                }
            }
        }

        //Speaks for itself
        public void draw()
        {
            //  ..
            //  v
            // ---
            //  |
            // / \
            //I drew a stick-man :D
        }

        //Speaks for itself
        public void Update(float ElapsedTime)
        {
            totalElapsedTime += ElapsedTime;
            if (totalElapsedTime >= 1.0f) //If one full second elapsed, step all the blocks.
            {
                totalElapsedTime -= 1.0f;
                foreach (Block b in blocks)
                {
                    b.step();
                }
            }
        }

        #region GETTERS
        public int getWidth()
        {
            return width;
        }

        public int getHeight()
        {
            return height;
        }

        public Block[] getBlocks()
        {
            return blocks;
        }

        public int getScore()
        {
            return score;
        }
        #endregion

    }
}

