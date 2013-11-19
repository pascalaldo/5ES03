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

        public GameGrid(int width, int height)
        {
            this.width = width;
            this.height = height;
            blocks = new Block[width * width * height]; //This is the maximum number of blocks that can be in the grid at the same time.
        }


        //Ignore for now
        private void newBlock()
        {
            Vector3 beginPosition = new Vector3(width / 2, width / 2, height);
        }


        //Speaks for itself
        public void Update(float ElapsedTime)
        {
            totalElapsedTime += ElapsedTime;
            if (totalElapsedTime >= 1.0f) //If one full second elapsed, step all the blocks.
            {
                totalElapsedTime -= 1.0f;
                foreach(Block b in blocks)
                {
                    b.step();
                }
            }
        }

        public void draw()
        {

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
        #endregion

    }
}

