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

    public class ArcBallCamera
    {

        public ArcBallCamera(float aspectRation, Vector3 lookAt)
            : this(aspectRation, MathHelper.PiOver4, lookAt, Vector3.Up, 0.1f, float.MaxValue) { }

        public ArcBallCamera(float aspectRatio, float fieldOfView, Vector3 lookAt, Vector3 up, float nearPlane, float farPlane)
        {
            this.aspectRatio = aspectRatio;
            this.fieldOfView = fieldOfView;
            this.lookAt = lookAt;
            this.nearPlane = nearPlane;
            this.farPlane = farPlane;
        }


        /// <summary>
        /// Recreates our view matrix, then signals that the view matrix
        /// is clean.
        /// </summary>
        private void ReCreateViewMatrix()
        {
            //Calculate the relative position of the camera                        
            position = Vector3.Transform(Vector3.Backward, Matrix.CreateFromYawPitchRoll(yaw, pitch, 0));
            //Convert the relative position to the absolute position
            position *= zoom;
            position += lookAt;

            //Calculate a new viewmatrix
            viewMatrix = Matrix.CreateLookAt(position, lookAt, Vector3.Up);
            viewMatrixDirty = false;
        }

        /// <summary>
        /// Recreates our projection matrix, then signals that the projection
        /// matrix is clean.
        /// </summary>
        private void ReCreateProjectionMatrix()
        {
            projectionMatrix = Matrix.CreatePerspectiveFieldOfView(MathHelper.PiOver4, AspectRatio, nearPlane, farPlane);
            projectionMatrixDirty = false;
        }

        #region HelperMethods

        /// <summary>
        /// Moves the camera and lookAt at to the right,
        /// as seen from the camera, while keeping the same height
        /// </summary>        
        public void MoveCameraRight(float amount)
        {
            Vector3 right = Vector3.Normalize(LookAt - Position); //calculate forward
            right = Vector3.Cross(right, Vector3.Up); //calculate the real right
            right.Y = 0;
            right.Normalize();
            LookAt += right * amount;
        }

        /// <summary>
        /// Moves the camera and lookAt forward,
        /// as seen from the camera, while keeping the same height
        /// </summary>        
        public void MoveCameraForward(float amount)
        {
            Vector3 forward = Vector3.Normalize(LookAt - Position);
            forward.Y = 0;
            forward.Normalize();
            LookAt += forward * amount;
        }

        #endregion

        #region FieldsAndProperties
        //We don't need an update method because the camera only needs updating
        //when we change one of it's parameters.
        //We keep track if one of our matrices is dirty
        //and reacalculate that matrix when it is accesed.
        private bool viewMatrixDirty = true;
        private bool projectionMatrixDirty = true;

        public float MinPitch = -MathHelper.PiOver2 + 0.3f;
        public float MaxPitch = MathHelper.PiOver2 - 0.3f;
        private float pitch;
        public float Pitch
        {
            get { return pitch; }
            set
            {
                viewMatrixDirty = true;
                pitch = MathHelper.Clamp(value, MinPitch, MaxPitch);
            }
        }

        private float yaw;
        public float Yaw
        {
            get { return yaw; }
            set
            {
                viewMatrixDirty = true;
                yaw = value;
            }
        }

        private float fieldOfView;
        public float FieldOfView
        {
            get { return fieldOfView; }
            set
            {
                projectionMatrixDirty = true;
                fieldOfView = value;
            }
        }

        private float aspectRatio;
        public float AspectRatio
        {
            get { return aspectRatio; }
            set
            {
                projectionMatrixDirty = true;
                aspectRatio = value;
            }
        }

        private float nearPlane;
        public float NearPlane
        {
            get { return nearPlane; }
            set
            {
                projectionMatrixDirty = true;
                nearPlane = value;
            }
        }

        private float farPlane;
        public float FarPlane
        {
            get { return farPlane; }
            set
            {
                projectionMatrixDirty = true;
                farPlane = value;
            }
        }

        public float MinZoom = 1;
        public float MaxZoom = float.MaxValue;
        private float zoom = 1;
        public float Zoom
        {
            get { return zoom; }
            set
            {
                viewMatrixDirty = true;
                zoom = MathHelper.Clamp(value, MinZoom, MaxZoom);
            }
        }


        private Vector3 position;
        public Vector3 Position
        {
            get
            {
                if (viewMatrixDirty)
                {
                    ReCreateViewMatrix();
                }
                return position;
            }
        }

        private Vector3 lookAt;
        public Vector3 LookAt
        {
            get { return lookAt; }
            set
            {
                viewMatrixDirty = true;
                lookAt = value;
            }
        }
        #endregion

        #region ICamera Members
        public Matrix ViewProjectionMatrix
        {
            get { return ViewMatrix * ProjectionMatrix; }
        }

        private Matrix viewMatrix;
        public Matrix ViewMatrix
        {
            get
            {
                if (viewMatrixDirty)
                {
                    ReCreateViewMatrix();
                }
                return viewMatrix;
            }
        }

        private Matrix projectionMatrix;
        public Matrix ProjectionMatrix
        {
            get
            {
                if (projectionMatrixDirty)
                {
                    ReCreateProjectionMatrix();
                }
                return projectionMatrix;
            }
        }
        #endregion
    }

    //public class ArcBallCamera
    //{

    //    public Matrix rotation = Matrix.Identity;
    //    public Vector3 position = Vector3.Zero;

    //     //Width and length of the square "playfield" in tiles
    //    private const int PlayfieldWidth = 24;

    //    //Width of one playfield tile
    //    private const float TileWidth = 10.0f; // the 'f' in 10.0f denotes that the number is a single precisoin floating point number


    //    // Simply feed this camera the position of whatever you want its target to be
    //    //public Vector3 targetPosition = Vector3.Zero;
    //    public Vector3 targetPosition = new Vector3(100.0f, 100.0f, 100.0f); 
        
    //    public Matrix viewMatrix = Matrix.Identity;
    //    public Matrix projectionMatrix = Matrix.Identity;
    //    private float zoom = 30.0f;
    //    public float Zoom
    //    {
    //        get
    //        {
    //            return zoom;
    //        }
    //        set
    //        {    // Keep zoom within range
    //            zoom = MathHelper.Clamp(value, zoomMin, zoomMax);
    //        }

    //    }


    //    private float horizontalAngle = 0;
    //    public float HorizontalAngle
    //    {
    //        get
    //        {
    //            return horizontalAngle;
    //        }
    //        set
    //        {    // Keep horizontalAngle between -pi and pi.
    //            horizontalAngle = value;// % MathHelper.Pi;
    //        }
    //    }

    //    private float verticalAngle = 0;
    //    public float VerticalAngle 
    //    {
    //        get
    //        {
    //            return verticalAngle;
    //        }
    //        set
    //        {    // Keep vertical angle within tolerances
    //            verticalAngle = value;//MathHelper.Clamp(value, verticalAngleMin, verticalAngleMax);
    //        }
    //    }

    //    private const float verticalAngleMin = 0.01f;
    //    private const float verticalAngleMax = MathHelper.Pi - 0.01f;
    //    private const float zoomMin = 0.1f;
    //    private const float zoomMax = 50.0f;


    //    // FOV is in radians
    //    // screenWidth and screenHeight are pixel values. They're floats because we need to divide them to get an aspect ratio.
    //    public ArcBallCamera(float FOV, float screenWidth, float screenHeight, float nearPlane, float farPlane)
    //    {

    //        if (screenHeight < float.Epsilon)
    //            throw new Exception("screenHeight cannot be zero or a negative value");

    //        if (screenWidth < float.Epsilon)
    //            throw new Exception("screenWidth cannot be zero or a negative value");

    //        if (nearPlane < 0.1f)
    //            throw new Exception("nearPlane must be greater than 0.1");

    //        this.projectionMatrix = Matrix.CreatePerspectiveFieldOfView(MathHelper.ToRadians(FOV), screenWidth / screenHeight,
    //                                                                    nearPlane, farPlane);
    //    }

    //    public void Update(GameTime gameTime, float elapsedTime, KeyboardState CurrentKeyboardState)
    //    {
    //        //if (CurrentKeyboardState.IsKeyDown(Keys.D))
    //        //    HorizontalAngle.set(moveSpeed * elapsedTime);

    //        // determine moveVector for camera according to keys pressed
    //        if (CurrentKeyboardState.IsKeyDown(Keys.D))
    //            this.HorizontalAngle -= 1.0f * elapsedTime; //-20.0f / 180.0f * (float)Math.PI;
    //        // determine moveVector for camera according to keys pressed
    //        if (CurrentKeyboardState.IsKeyDown(Keys.A))
    //            this.HorizontalAngle += 1.0f * elapsedTime; //-20.0f / 180.0f * (float)Math.PI;
    //        // determine moveVector for camera according to keys pressed
    //        if (CurrentKeyboardState.IsKeyDown(Keys.W))
    //            this.VerticalAngle += 1.0f * elapsedTime; //-20.0f / 180.0f * (float)Math.PI;
         
    //        this.VerticalAngle = MathHelper.PiOver2; ;//MathHelper.PiOver4; //-135.0f / 180.0f * (float)Math.PI; //MathHelper.PiOver2;

    //        // Start with an initial offset
    //        Vector3 cameraPosition = new Vector3(0.0f, zoom, 0.0f);

    //        // Rotate vertically
    //        cameraPosition = Vector3.Transform(cameraPosition, Matrix.CreateRotationX(verticalAngle));

    //        // Rotate horizontally
    //        cameraPosition = Vector3.Transform(cameraPosition, Matrix.CreateRotationY(horizontalAngle));

    //        position = cameraPosition + targetPosition;
    //        this.LookAt(targetPosition);

    //        // Compute view matrix
    //        this.viewMatrix = Matrix.CreateLookAt(this.position,
    //                                              this.position + this.rotation.Forward,
    //                                              this.rotation.Up);
    //    }

    //    /// <summary>
    //    /// Points camera in direction of any position.
    //    /// </summary>
    //    /// <param name="targetPos">Target position for camera to face.</param>
    //    public void LookAt(Vector3 targetPos)
    //    {
    //        Vector3 newForward = targetPos - this.position;
    //        newForward.Normalize();
    //        this.rotation.Forward = newForward;

    //        Vector3 referenceVector = Vector3.UnitY;

    //        // On the slim chance that the camera is pointer perfectly parallel with the Y Axis, we cannot
    //        // use cross product with a parallel axis, so we change the reference vector to the forward axis (Z).
    //        if (this.rotation.Forward.Y == referenceVector.Y || this.rotation.Forward.Y == -referenceVector.Y)
    //        {
    //            referenceVector = Vector3.UnitZ;
    //        }

    //        this.rotation.Right = Vector3.Cross(this.rotation.Forward, referenceVector);
    //        this.rotation.Up = Vector3.Cross(this.rotation.Right, this.rotation.Forward);
    //    }

    //}
}
