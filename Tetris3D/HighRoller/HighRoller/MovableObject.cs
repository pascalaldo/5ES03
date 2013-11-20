using System;
using System.Collections.Generic;
using System.Text;
using Microsoft.Xna.Framework;

namespace HighRoller
{

    // Class representing an object that can move across the field
    public class MovableObject
    {
        #region VARIABLE DECLARATIONS
        // its current position
        private Vector2 positionCurrent;
        // start of a movement
        private Vector2 positionStart;
        //destination of the movement
        private Vector2 positionDestination;

        //Type of movement
        private enum MovingType
        {
            None,
            Linear,
            PathSmooth
        };

        // status of movement, what is it currently doing
        private MovingType movingStatus = MovingType.None;

        //speed of movement
        private static float moveSpeed = 64.0f;

        //time measurement
        private float time = 0.0f;
        private float totalTime = 0.0f;

        //distance to move
        private float MoveDistance;

    	//Moving Direction
        private float direction = 0.0f;

        //Curves for smooth path movement
        //Curve is a standard XNA class
        public Curve curveX;
        public Curve curveY;

        //Width of one path tile
        private static int GridWidth;
        //GridWidth/2
        private static int GridWidthDiv2;

        //Array containing directions lookups
        private static int[] Directions;
        //Array for relative position(regarding to directions) lookup
        private static Point[] LinearMoves;
        #endregion

        #region CONSTRUCTOR
        //Crete a movable object
        public MovableObject(Vector2 StartPos)
        {
            positionCurrent = StartPos;
        
            curveX = new Curve();
            curveY = new Curve();
        }
        #endregion

        #region PROPERTIES
        public Vector2 PositionCurrent
        {
            get { return positionCurrent; }
            set { positionCurrent = value; }
        }
        public float MoveSpeed
        {
            get { return moveSpeed; }
            set { moveSpeed = value; }
        }
        public float Direction
        {
            get { return direction; }
            set { direction = value; }
        }

        #endregion

        // Init MovableObject class
        public static void Initalize(int GridSize, int MoveWidth)
        {
            GridWidth = MoveWidth;
            GridWidthDiv2 = MoveWidth / 2;

            //Lookups for direction and node insertion 
            Directions = new int[8];
            Directions[0] = -GridSize;
            Directions[1] = -GridSize + 1;
            Directions[2] = 1;
            Directions[3] = GridSize + 1;
            Directions[4] = GridSize;
            Directions[5] = GridSize - 1;
            Directions[6] = -1;
            Directions[7] = -GridSize - 1;

            LinearMoves = new Point[8];
            LinearMoves[0] = new Point(GridWidthDiv2, GridWidth);
            LinearMoves[1] = new Point(0, GridWidth);
            LinearMoves[2] = new Point(0, GridWidthDiv2);
            LinearMoves[3] = new Point(0, 0);
            LinearMoves[4] = new Point(GridWidthDiv2, 0);
            LinearMoves[5] = new Point(GridWidth, 0);
            LinearMoves[6] = new Point(GridWidth, GridWidthDiv2);
            LinearMoves[7] = new Point(GridWidth, GridWidth);

        }

        /// <summary>
        /// Stop moving and reset times
        /// </summary>
        public void StopMoving()
        {
            movingStatus = MovingType.None;
            time = 0.0f;
            totalTime = 0.0f;
        }

        /// <summary>
        /// Perform a linear move
        /// </summary>
        /// <param name="startPos">Start position</param>
        /// <param name="endPos">End Position</param>
        public void LinearMove(Vector2 startPos, Vector2 endPos)
        {
            movingStatus = MovingType.Linear;
            time = 0.0f;
            positionStart = startPos;
            positionDestination = endPos;
            Vector2.Distance(ref positionStart, ref positionDestination, out MoveDistance);
            direction = (float)Math.Atan2(endPos.Y - startPos.Y,endPos.X - startPos.X);
        }

        /// <summary>
        /// Performs moving logic
        /// </summary>
        /// <param name="ElapsedTime">Time elapsed form last frame</param>
        public void Update(float ElapsedTime)
        {
            switch (movingStatus)
            {
                case MovingType.Linear:
                    {
                        positionCurrent = Linear(positionStart, positionDestination, time);
                        time += (moveSpeed * ElapsedTime) / MoveDistance;
                        if (time >= 1.0f)
                        {
                            StopMoving();
                            positionCurrent = positionDestination;
                        }
                    }
                    break;
                case MovingType.PathSmooth:
                    {
                        time += (moveSpeed * ElapsedTime);
                        if (time < totalTime)
                        {
                            float newX = curveX.Evaluate(time);
                            float newY = curveY.Evaluate(time);

                            //Trought not used here, this provide moving direction (in radions)
                            direction = (float)Math.Atan2(newY - positionCurrent.Y,  newX-positionCurrent.X);

                            positionCurrent.X = newX;
                            positionCurrent.Y = newY;
                        }
                        else
                        {
                            StopMoving();
                        }
                    }
                    break;
            }
        }

        //Return 2D Postion on line form start and end point at given time
        private static Vector2 Linear(Vector2 startPos, Vector2 endPos, float time)
        {
            return startPos = startPos + (endPos - startPos) * time;
        }

    }
}
