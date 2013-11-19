using System;
using System.Linq;
using System.Collections.Generic;
using System.Text;
using Microsoft.Xna.Framework;

namespace TestTetris
{
    public class Block
    {
        protected Grid grid;        //Parent grid,which has to contain: public int width, height; public Block[] blocks;
        protected Vector3[] cubes;  //Defines cube positions.
        protected Vector3 origin;   //relative to this origin.

        /**
         * Creation method.
         * @return true when creating this cube
         * results in a collision: game over.
         * 
         * Actual implementation depends on block shape
         */
        public virtual bool create()
        {
            return this.checkCollision(this.cubes.Select(cube => cube + this.origin).ToArray()); //Returns if this object can stay in place using the move function.
        } //Creates a block. Use extensions for specific block shapes.
     
        public Block(Grid grid)
        {
            this.grid = grid;
        }
        
        /**
         * Move a block with a translation around x, y and z, and a rotation around x, y and z.
         */
        public bool move(Vector3 translate, Vector3 rotate)
        {
            //Calculate theoretical movement

            //Translation: absolute, so origin
            Vector3 newOrigin = origin+translate;

            //Rotation: relative, so cubes matrix.
            Matrix rotX = Matrix.CreateFromAxisAngle(Vector3.UnitX, (float)(rotate.X * Math.PI / 2));
            Matrix rotY = Matrix.CreateFromAxisAngle(Vector3.UnitY, (float)(rotate.Y * Math.PI / 2));
            Matrix rotZ = Matrix.CreateFromAxisAngle(Vector3.UnitZ, (float)(rotate.Z * Math.PI / 2));
            Matrix rotTotal = rotX * rotY * rotZ; //Total rotation
            
            Vector3[] newCubes = new Vector3[cubes.Length];
            Vector3.Transform(cubes,ref rotTotal, newCubes); //Applies rotation matrix
            
            for (int i = 1; i < newCubes.Length; i++) //Round cubes to ints.
            {
                newCubes[i] = new Vector3((int)newCubes[i].X, (int)newCubes[i].Y, (int)newCubes[i].Z);
            }

            //Check collision: add the origin and the cubes together and use that set for collission detection.
            if(checkCollision(newCubes.Select(newCube=>newCube+newOrigin).ToArray())) 
            {
                return false;
            }

            //Perform movement with calculated values
            this.origin = newOrigin;
            this.cubes = newCubes;
            return true; //Succes!
        }

        /**
         * Check if the given set of cubes collides with the walls or other blocks in the grid
         * @param Vector3 cubeSet: the set of 'integer' vectors that define cubes.
         * @return true on collision
         */
        protected bool checkCollision(Vector3[] cubeSet)
        {
            foreach (Vector3 currentCube in cubeSet)
            {
                if (currentCube.X < 0 || currentCube.X > grid.width || //X walls
                    currentCube.Z < 0 || currentCube.Z > grid.width || //Z walls
                    currentCube.Y < 0 || currentCube.Y > grid.height) //Necessary for rotation, even though blocks do not move up directly.
                {
                    return true; //Wall collision detected
                }
                foreach (Block neighbour in grid.blocks)
                {
                    if (neighbour.collidesWith(currentCube))
                    {
                        return true; //Neighbour block collision detected.
                    }
                }

            }
            return false;
        }

        /**
         * Checks whether the give cube collides with any of this Block's cubes.
         * @return true on collision
         */
        public bool collidesWith(Vector3 cube)
        {
            foreach (Vector3 currentCube in cubes)
            {
                if (((int)currentCube.X) == ((int)cube.X) && //Cast to int, because slim chance floats are exactly equal.
                    ((int)currentCube.Y) == ((int)cube.Y) && //Even if they look the same.
                    ((int)currentCube.Z) == ((int)cube.Z))   //These three lines check whether all coordinates mach => collision.
                {
                    return true;
                }
            }
            return false;
        }
        /**
         * Moves the block down 1 step in y direction. 
         * @return false if impossible.
         */
        public bool step()
        {
            return move(new Vector3(0,-1,0),new Vector3(0,0,0));
        }

        /**
         * Removes a certain row and moves all cubes above it one step down.
         * @return false if no cubes left, i.e., marked for deletion.
         */
        public bool removeRow(int row)
        {
            //Remove cube at int row.
            cubes = cubes.Where(cube => cube.Y != row).ToArray();
            //Moves cubes above row one step down.
            for (int i = 1; i < cubes.Length; i++)
            {
                if (cubes[i].Y > row)
                {
                    cubes[i].Y -= 1;
                }
            }
            return cubes.Length > 0; //Return true if cubes still contains at least one cube.
        }

        

        /**
         * Counts the number of cubes on each row,
         * so that the game can determine if any of the
         * rows is full.
         * @return int[grid.height] with values for each row.
         */
        public int[] countCubesOnRows()
        {
            int[] rowCount = new int[grid.height];
            foreach (Vector3 cube in cubes)
            {
                rowCount[(int)cube.Y] += 1;
            }
            return rowCount;
        }

    }

    public class StraightBlock : Block
    {
        public StraightBlock(Grid grid) : base(grid)
        {
            //Nothing to do here lol.
        }
        /**
         * Create a block from x=-1 to x=2 at the top of the field.
         */
        public override bool create()
        {
            this.cubes = new Vector3[] {
                new Vector3(-1,0,0),
                new Vector3(0,0,0),
                new Vector3(1,0,0),
                new Vector3(2,0,0)};
            this.origin = new Vector3((int)(grid.width / 2), grid.height, (int)(grid.width / 2));
            return base.create();
        }
    }

}
