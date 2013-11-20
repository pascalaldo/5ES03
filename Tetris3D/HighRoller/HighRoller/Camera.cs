using System;
using System.Collections.Generic;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Audio;
using Microsoft.Xna.Framework.Graphics;
using Microsoft.Xna.Framework.Input;
using Microsoft.Xna.Framework.Content;
using Microsoft.Xna.Framework.Storage;


namespace HighRoller
{
    // This class represents the moving camera object in the scene
    public class Camera
    {
        public Matrix mRotation;
        public Matrix mView;
        public Matrix mProjection;

        // vecPosition is the location of the camera
        public Vector3 vecPosition;
        // vecTarget is the point the camera is looking at
        public Vector3 vecTarget;
        // vecUp defines what direction is upward to put the camera 'horizontal'
        public Vector3 vecUp;
        // vecAngles is a vector containing the vertical and horizontal angle of the camera.
        public Vector2 vecAngles;

        // moveSpeed define the speed of movement of the camera.
        public float moveSpeed = 128.0f;
        // the window size to compute an appropriate projection matrix
        private float windowWidth = 1024.0f;
        private float windowHeight = 768.0f;

        // position of the mouse pointer to move the camera orientation
        private int mice_x;
        private int mice_y;

        // The constructor, creates a camera object
        public Camera(Vector3 POSITION, Vector2 ANGLES)
        {
            vecPosition = POSITION;
            vecAngles = ANGLES;

            mice_x = 1024 / 2;
            mice_y = 768 / 2;

        }

        // Update is called for every update in the game. Update camera position and orientation depending on keyboard and mouse state.
        public void Update(float elapsedTime, KeyboardState CurrentKeyboardState,
                           MouseState currentMouseState, MouseState previousMouseState, GraphicsDevice device)
        {
            Vector3 moveVector = Vector3.Zero;

            // determine moveVector for camera according to keys pressed
            if (CurrentKeyboardState.IsKeyDown(Keys.D))
                moveVector.X += moveSpeed * elapsedTime;
            if (CurrentKeyboardState.IsKeyDown(Keys.A))
                moveVector.X -= moveSpeed * elapsedTime;
            if (CurrentKeyboardState.IsKeyDown(Keys.S))
                moveVector.Y -= moveSpeed * elapsedTime;
            if (CurrentKeyboardState.IsKeyDown(Keys.W))
                moveVector.Y += moveSpeed * elapsedTime;

            // determine camera rotation due to mouse button and movement
            if ((currentMouseState.RightButton == ButtonState.Pressed) && (previousMouseState.RightButton == ButtonState.Released))
            {
                mice_x = currentMouseState.X;
                mice_y = currentMouseState.Y;
            }

            if (currentMouseState.RightButton == ButtonState.Pressed)
            {
                if (currentMouseState.X != mice_x)
                {
                    vecAngles.Y -= (currentMouseState.X - mice_x) * 0.006f;
                }
                if (currentMouseState.Y != mice_y)
                {
                    vecAngles.X -= (currentMouseState.Y - mice_y) * 0.006f;
                }

                Mouse.SetPosition(mice_x, mice_y);
            }

            // Create rotation matrix according to the x and z angles in veAngles
            // First rotate around x-axis, then around (new) z-axis.
            mRotation = Matrix.CreateRotationX(vecAngles.X) * Matrix.CreateRotationZ(vecAngles.Y);
            // Move the camera position in the right firection according to moveVector and mRotation matrix
            vecPosition += Vector3.Transform(moveVector, mRotation);

            //Set a target for the camera to look at. 
            vecTarget = vecPosition + Vector3.Transform(new Vector3(0, 1, 0), mRotation);
            vecUp = Vector3.Transform(new Vector3(0, 0, 1), mRotation);

            //Create view matrix using a camera position vecPosition, a target to look at vecTarget and
            //a specification of what direction is 'up', vecUp
            mView = Matrix.CreateLookAt(vecPosition, vecTarget, vecUp);

            //Create a projection matrix for a given field-of-view angle Pi/4 and according to the window size
            mProjection = Matrix.CreatePerspectiveFieldOfView(MathHelper.PiOver4, windowWidth / windowHeight, 0.12f, 8096.0f);
        }

    }
}
