using System;
using System.Linq;
using System.Collections.Generic;
using System.Text;
using Microsoft.Xna.Framework;

namespace TestTetris
{
    class Block
    {
        protected Grid grid;
        protected Vector3[] cubes;       //Defines cube positions.
        protected Vector3 origin;     //relative to this origin.

        protected bool create()
        {
            return this.checkCollision(this.cubes.Select(cube => cube + this.origin).ToArray()); //Returns if this object can stay in place using the move function.
        } //Creates a block. Use extensions for specific block shapes.
     
        public Block(Grid grid)
        {
            this.grid = grid;
        }
        
        public bool move(Vector3 translate, Vector3 rotate)
        {
            //Calculate theoretical movement
            //Translation: absolute
            Vector3 newOrigin = origin+translate;
            //Rotation: relative.
            Matrix rotX = Matrix.CreateFromAxisAngle(Vector3.UnitX, (float)(rotate.X * Math.PI / 2));
            Matrix rotY = Matrix.CreateFromAxisAngle(Vector3.UnitY, (float)(rotate.Y * Math.PI / 2));
            Matrix rotZ = Matrix.CreateFromAxisAngle(Vector3.UnitZ, (float)(rotate.Z * Math.PI / 2));
            Matrix rotTotal = rotX * rotY * rotZ; //Total rotation
            Vector3[] newCubes = new Vector3[cubes.Length];
            Vector3.Transform(cubes,ref rotTotal, newCubes);
           
            //Check collision
            if(checkCollision(newCubes.Select(newCube=>newCube+newOrigin).ToArray())) //Add the origin and the cubes together and use that set for collission detection.
            {
                return false;
            }


            //Perform movement with calculated values
            this.origin = newOrigin;
            this.cubes = newCubes;
            return true; //Succes!
        }

        //Return true on collision
        protected bool checkCollision(Vector3[] cubeSet)
        {
            foreach (Vector3 currentCube in cubeSet)
            {
                if (currentCube.X < 0 || currentCube.X > grid.width || //X walls
                    currentCube.Z < 0 || currentCube.Z > grid.width || //Z walls
                    currentCube.Y < 0 || currentCube.Y > grid.height)//Necessary for rotation, even though blocks do not move up directly.
                {
                    return true;
                }
                foreach (Block neighbour in grid.Blocks)
                {
                    if (neightbour.collidesWidth(currentCube))
                    {
                        return true;
                    }
                }

            }
            return false;
        }
        public bool step()
        {
            return move(new Vector3(0,-1,0),new Vector3(0,0,0));
        }

        public bool removeRow(int row)
        {
            //Remove cube at int row. Return false if no more cubes, to mark object for deletion.
            cubes = cubes.Where(cube => cube.Y != row).ToArray();
            
            for (int i = 1; i < cubes.Length; i++)
            {
                if (cubes[i].Y > row)
                {
                    cubes[i].Y -= 1;
                }
            }
            return cubes.Length > 0; //Return true if cubes still contains at least one cube.
        }
        public bool collidesWith(Vector3 cube)
        {
            foreach(Vector3 currentCube in cubes)
            {
                if(currentCube.Equals(cube))
                {
                    return true;
                }
            }
            return false;
        }
        
    }

    class StraightBlock : Block
    {
        public bool create()
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
