using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Xna.Framework.Graphics;
using Microsoft.Xna.Framework;

namespace Tetris3D
{
    public class Grid
    {
        public Grid(Game1 game, int n, int h)
        {
            this.game = game;
            this.width = n;
            this.height = h;
            this.blocks = new List<Block>();
            this.currentBlock = null;
            this.ground = new int[width, width];
        }

        public static float baseSize = 30.0f;
        public static float blockSpeed = 0.002f;

        private float currentFloatPos = 0.0f;
        private float currentIntPos = 0;

        public Game1 game;

        public GraphicsDeviceManager graphics;
        public GraphicsDevice GraphicsDevice;

        public int width { get; private set; }
        public int height { get; private set; }
        public List<Block> blocks { get; private set; }

        public Block currentBlock { get; private set; }

        private int[,] ground;

        public void initialize()
        {
            startGame();
        }

        public void startGame()
        {
            nextBlock();
        }

        public void nextBlock()
        {
            this.currentBlock = addRandomBlock();
            if (currentBlock == null)
                throw new Exception("End of game!");
            this.currentFloatPos = this.currentBlock.origin.Y;
            this.currentIntPos = (int)this.currentBlock.origin.Y;
        }

        public Block addRandomBlock()
        {
            // Going to be random, for now just add StraightBlock
            return addBlock(new StraightBlock(this));
        }

        public Block addBlock(Block b)
        {
            if (b.create())
                return null;
            blocks.Add(b);
            return b;
        }

        public void draw()
        {
            foreach (Block b in blocks)
            {
                b.draw();
            }

            for (int x = 0; x < this.width; x++)
            {
                for (int z = 0; z < this.width; z++)
                {
                    foreach (Vector3 c in currentBlock.cubes)
                    {
                        if (currentBlock.origin.X + c.X == z &&
                            currentBlock.origin.Z + c.Z == x)
                        {
                            game.drawModel(game.projectionModel, Grid.baseSize * (z + 0.2f), Grid.baseSize * 1.0f * (ground[x, z] + 0.16f), Grid.baseSize * x);
                            break;
                        }
                    }
                }
            }
        }

        public void update(GameTime gameTime)
        {
            currentFloatPos -= gameTime.ElapsedGameTime.Milliseconds * blockSpeed;
            //Console.WriteLine("Current float pos: " + currentFloatPos.ToString());
            if ((int)currentFloatPos < currentIntPos)
            {
                currentIntPos = (int)currentFloatPos;
                if (!currentBlock.step())
                {
                    fullLayers();
                    updateGround();
                    nextBlock();
                }
            }
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
                    }
                    for (int j = i; j < layerCount.Length-1; j++)
                    {
                        layerCount[j] = layerCount[j + 1];
                    }
                    layerCount[layerCount.Length-1] = 0;
                    for (int z = 0; z < this.width; z++)
                    {
                        for (int x = 0; x < this.width; x++)
                        {
                            if (ground[z, x] > 0)
                            {
                                ground[z, x]--;
                            }
                        }
                    }
                    i--;  //Since after deleting a layer, all other blocks are moved down, layer i+1 becomes layer i. Therefore,
                    //this ensures that with the next iteration, layer i is checked again, 
                    //score += width * width;
                }
            }
        }

        private void updateGround()
        {
            Block tempBlock = new Block(this);
            Vector3[] nullCubes = new Vector3[]{new Vector3(0, 0, 0)};

            for (int z = 0; z < this.width; z++)
            {
                for (int x = 0; x < this.width; x++)
                {
                    while (tempBlock.checkCollision(nullCubes, new Vector3(x, ground[z, x], z)))
                    {
                        ground[z, x]++;
                        Console.WriteLine(ground[z, x]);
                    }
                }
            }
        }
    }
}
