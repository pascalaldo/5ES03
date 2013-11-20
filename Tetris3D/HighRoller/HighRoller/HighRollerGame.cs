using System;
using System.Collections.Generic;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Audio;
using Microsoft.Xna.Framework.Content;
using Microsoft.Xna.Framework.GamerServices;
using Microsoft.Xna.Framework.Graphics;
using Microsoft.Xna.Framework.Input;
using Microsoft.Xna.Framework.Net;
using Microsoft.Xna.Framework.Storage;

namespace HighRoller
{
    // This is the main class for your game
    public class HighRollerGame : Microsoft.Xna.Framework.Game
    {

        #region VARIABLE DECLARATIONS

        //for drawing the scene on the screen
        GraphicsDeviceManager graphics;

        //Width and length of the square "playfield" in tiles
        private const int PlayfieldWidth = 24;

        //Width of one playfield tile
        private const float TileWidth = 10.0f; // the 'f' in 10.0f denotes that the number is a single precisoin floating point number

        //Our movement restriction information array - 
        //a value of false means "empty, can move there"
        //true means "cannot move there"
        private bool[,] Walls;

        //Keyboard states
        private KeyboardState KBState_Current;
        private KeyboardState KBState_Previous;
        //Mouse states
        private MouseState MSState_Current;
        private MouseState MSState_Previous;

        //Elapsed game time
        float elapsedTime;

        //Matrix array, containing positions, where to place walls - you could also calculate them on the fly and save some memory, but i think, that
        //it works fine in this way (256kb isnt too much)
        private Matrix[,] WallMatrices;

        //There is a need to scale the ground plane matrix, acording to PlayfieldWidth
        private Matrix PlaneMatrix;

        //Models used in this sample
        private Model Model_Box;
        private Model Model_Plane;
        private Model Model_Admoveo;

        //Box and tile textures
        private Texture2D Texture_Box;
        private Texture2D Texture_Tile;
       
        //Character model world matrix
        private Matrix CharacterMatrix;

        // Are we adding or removing boxes?
        // Start with adding boxes.
        private bool Box_Add = true;

        //The rendering effect
        private Effect effect;

        //Our Free-Fly Camera
        ArcBallCamera ArcCamera;

        //Point of rotation of the arcballcamera
        Vector3 pointRotation;

        //Location in 3D world space where a mouse click has occurred
        Vector3 Click;

        //A movable object, see the file MovableObject.cs
        MovableObject Character;

        //Is it allowed to move?
        // Start with ok to move.
        bool CanMove = true;

        #endregion

        public HighRollerGame()
        {
            // the code below executes only once, when the game starts

            //create the graphics device that will visualize the game
            graphics = new GraphicsDeviceManager(this);

            //set properties of the graphics device
            // Window size.
            graphics.PreferredBackBufferWidth = 1024;
            graphics.PreferredBackBufferHeight = 768;
            // play in a window or full screen?
            graphics.IsFullScreen = false;
            // synchronization with screen required?
            graphics.SynchronizeWithVerticalRetrace = false;
            // variable time steps may be used
            IsFixedTimeStep = false;

            //where to find the visual content for the game.
            //"Content" will be a directory created from the HighRollerContent project, see 'solution explorer'
            Content.RootDirectory = "Content";
        }

        // Allows the game to perform any initialization it needs to before starting to run.
        // This is where it can query for any required services and load any non-graphic
        // related content.  Calling base.Initialize will enumerate through any components
        // and initialize them as well.
        protected override void Initialize()
        {
            // Add your initialization logic here

            // Create the arrays representing the playing field
            // A PlayfieldWidth by PlayfieldWidth array representing the walls.
            // A 0 in the array will mean there is no wall, a 1 will mean there is a wall.
            Walls = new bool[PlayfieldWidth, PlayfieldWidth];
            // An array of transformation matrices that can be used to move an object to the center of the corresponding tile.
            WallMatrices = new Matrix[PlayfieldWidth, PlayfieldWidth];

            //Initialize the walls array, so that it is empty at the start
            //Also initialize our WallMatrices array, so it contains wall positions used for drawing them
            for (int y = 0; y < PlayfieldWidth; y = y + 1)
            {
                for (int x = 0; x < PlayfieldWidth; x = x + 1)
                {
                    // initialize to 0, meaning empty
                    Walls[x, y] = false;
                    //Create a transformation matrix that places the object in the right spot for the tile.
                    //This means a translation to the center of the tile.
                    WallMatrices[x, y] = Matrix.CreateTranslation(new Vector3((x+0.5f) * TileWidth, (y+0.5f) * TileWidth, 0.0f));
                }
            }

            // place a box in two corners, to give an ide of the orientation of the field
            Walls[0, 0] = true;
            Walls[PlayfieldWidth-2, 0] = true;


            //Create a transformation matrix for the ground plane.
            //Compute the scale for the playing field: number of tiles times the width of a tile.
            float Scale = (float) PlayfieldWidth * TileWidth;
            //First do a scaling to make the ground plane the right size, then a translation to the center of the playing field.
            //Note that matrix multiplication has the effect of performing the operations in sequence, from left to right, in this case,
            //first scale, then translate.
            PlaneMatrix = Matrix.CreateScale(Scale) * Matrix.CreateTranslation(new Vector3(Scale / 2, Scale / 2, 0));

            //Create the camera that will be looking onto the scene, using the Camera class that is defined in file Camera.cs
            //The first vector is the location of the camera, above the far corner of the playing field
            //the second vector contains the angle the camera looks at in the vertical plane and in the horizontal plane
            //look 20 degrees down and 135 degrees from the positive y axis.
            //Note that the angles in degrees need to be converted to angels in radians (by dividing by 180 and multiplying with Pi).
            //camera = new Camera(
            //    new Vector3((float)PlayfieldWidth * TileWidth, (float)PlayfieldWidth * TileWidth, 100.0f), 
            //    new Vector2(-20.0f /180.0f * (float)Math.PI, 135.0f /180.0f * (float)Math.PI));

            pointRotation = new Vector3(120.0f, 120.0f, 500.0f);

            ArcCamera = new ArcBallCamera(
                1024.0f/768.0f,
                MathHelper.PiOver4,
                pointRotation,
                new Vector3(0.0f, 0.0f, 0.0f),
                0.12f,
                8096.0f
            );

            // Allow the mouse pointer to be visible in the game window.
            IsMouseVisible = true;

            //Set the grid size and tile size in class MovableObject
            //The class MovableObject is defined in file MovableObject.cs
            MovableObject.Initalize(24, (int)TileWidth);

            //Create a movable object for the Admoveo character, positioned in the center of tile (1,1)
            Character = new MovableObject(new Vector2(0.5f * TileWidth, 0.5f * TileWidth));

            //Call inherited initialization
            //Do not change this one, or everything will break!
            base.Initialize();
        }

        // LoadContent will be called once per game and this is the place where you would load
        // all of your graphics (and sound) content.
        protected override void LoadContent()
        {
            //Load all 3D models used
            Model_Box = Content.Load<Model>("Models\\box");
            Model_Plane = Content.Load<Model>("Models\\plane");
            Model_Admoveo = Content.Load<Model>("models\\" + "admoveo");

            //Load texture of the crate model
            Texture_Box = Content.Load<Texture2D>("Models\\crate");

            //Load texture of the tiles
            Texture_Tile = Content.Load<Texture2D>("Models\\tile");

            //Some 3d models have their texture data integrated. Here we set it manually for the plane object.
            //Try what happens if you remove this code!
            //The original model is set to have to the texture (tile) to occur once over the entire object. 
            //We want instead one texture per two tiles, so we need to repeat the texture 'PlayfieldWidth' times over the object.
            //To do this, we get the vertex data from the model, modify it, and set it back to the model:
            //We know this model has 4 vertices, allocate an arry.
            VertexPositionNormalTexture[] Vertices = new VertexPositionNormalTexture[4];
            //Extract the vertices from the model into 'Vertices'
            Model_Plane.Meshes[0].MeshParts[0].VertexBuffer.GetData<VertexPositionNormalTexture>(Vertices);
            //Modify the texture coordinates of all vertices by scaling the with a factor PlayfieldWidth, causing
            //the texture to repeat PlayfieldWidth times.
            for (int n = 0; n < Vertices.Length; n += 1)
            {
                Vertices[n].TextureCoordinate = Vertices[n].TextureCoordinate * 0.5f * PlayfieldWidth;
            }
            //Write the modified vertex data back into the model.
            Model_Plane.Meshes[0].MeshParts[0].VertexBuffer.SetData<VertexPositionNormalTexture>(Vertices);


            //Load the rendering effect, defined in the ShaderEffect.fx file in Models
            //You may have a look at this file to see if you can make sense of it.
            //It specifies what effects are used to render the model to the screen.
            effect = Content.Load<Effect>("Models\\ShaderEffect");

            //Set the effect parameters that will not change during the game
            //Different types of light colors. Try changing them
            effect.Parameters["AmbientColor"].SetValue(Color.White.ToVector4());
            effect.Parameters["AmbientIntensity"].SetValue(0.1f);
            effect.Parameters["DiffuseDirection"].SetValue(new Vector3(-0.2f, -0.6f, -1.0f));
            effect.Parameters["DiffuseColor"].SetValue(Color.WhiteSmoke.ToVector4());
            effect.Parameters["DiffuseIntensity"].SetValue(0.2f);
            effect.Parameters["TextureIntensity"].SetValue(0.7f);
        }

        // UnloadContent will be called once per game and is the place to unload
        // all content.
        protected override void UnloadContent()
        {
            // OK. Were are too lazy to unload anything. The program is about to end anyway.
        }

        // Allows the game to run logic such as updating the world,
        // checking for collisions, gathering input, and playing audio.
        // gameTime provides a snapshot of timing values. 
        // This function is called over and over again. There is no guarantee however about how much time passes exactly 
        // between two calls. We can see that however from gameTime.ElapsedGameTime
        protected override void Update(GameTime gameTime)
        {
            //To get a constant speed anymation, determine how much time passed since last update (in seconds)
            elapsedTime = (float) gameTime.ElapsedGameTime.TotalSeconds;

            //Get the keyboard state to check if the user pressed any keys.
            //Remember also the previous state, so we can compare and detect new key presses.
            KBState_Previous = KBState_Current;
            KBState_Current = Keyboard.GetState();

            //Get the mouse state to check if the used moved the mouse and/or pressed any mouse buttons.
            //Remember also the previous state, so we can compare and detect new mouse moves / button presses.
            MSState_Previous = MSState_Current;
            MSState_Current = Mouse.GetState();

            //Animate the camera, i.e. move it, depending on mouse and keyboard activity
            //The details are defined in the Update function in the Camera class, see the Camera.cs file.
            //camera.Update(elapsedTime, KBState_Current, MSState_Current, MSState_Previous, graphics.GraphicsDevice);

            float diffAngleYaw = (float)MathHelper.ToRadians(0.25f) % (MathHelper.TwoPi);
            float diffAnglePitch = (float)MathHelper.ToRadians(0.1f) % (MathHelper.TwoPi);
            float currRadius = (float)Math.Sqrt(Math.Pow(ArcCamera.LookAt.X - pointRotation.X, 2) + Math.Pow(ArcCamera.LookAt.Y - pointRotation.Y, 2) + Math.Pow(ArcCamera.LookAt.Z, 2));

            if (KBState_Current.IsKeyDown(Keys.D))
            {
                ArcCamera.MoveCameraRight(currRadius * diffAngleYaw);
                ArcCamera.Yaw += diffAngleYaw;
                //ArcCamera.MoveCameraRight(100.0f * ArcCamera.Yaw);
            }
            if (KBState_Current.IsKeyDown(Keys.A))
            {
                ArcCamera.MoveCameraRight(-currRadius * diffAngleYaw);
                ArcCamera.Yaw -= diffAngleYaw;
                //ArcCamera.MoveCameraRight(-100.0f * ArcCamera.Yaw);
            }
            if (KBState_Current.IsKeyDown(Keys.E))
            {
                ArcCamera.MoveCameraForward(1.0f); //-20.0f / 180.0f * (float)Math.PI;
            }
            if (KBState_Current.IsKeyDown(Keys.Q))
            {
                ArcCamera.MoveCameraForward(-1.0f); //-20.0f / 180.0f * (float)Math.PI;
            }
            if (KBState_Current.IsKeyDown(Keys.W))
            {
                ArcCamera.Pitch += diffAnglePitch; //-20.0f / 180.0f * (float)Math.PI;
            }
            if (KBState_Current.IsKeyDown(Keys.S))
            {
                ArcCamera.Pitch -= diffAnglePitch; //-20.0f / 180.0f * (float)Math.PI;
            }


            // check if the user pressed the 'b' key
            if (IsKeyPush(Keys.B))
            {
                //If yes, toggle the adding/removing boxes state.
                Box_Add = !Box_Add;
            }

            // check if the user pressed the 'o' key
            if (IsKeyPush(Keys.O))
            {
                //If yes, toggle the allow moving state.
                CanMove = !CanMove;
            }

            //Animate the camera, i.e. move it, depending on mouse and keyboard activity
            //The details are defined in the Update function in the Camera class, see the Camera.cs file.
            //camera.Update(elapsedTime, KBState_Current, MSState_Current, MSState_Previous, graphics.GraphicsDevice);

            //Now we move the character if the right mouse button is being pressed
            if (MSState_Current.RightButton == ButtonState.Pressed && MSState_Previous.RightButton == ButtonState.Released)
            {
                // Is the character allowed to move?
                if (CanMove)
                {
                    //Get the coordinates in 3d world of the mouse click
                    //Thus user clicked the mouse on the 2D screen. We need to translate the 2D coordinates to 3D world coordinates.
                    //The details are in the function GetCollision below.
                    //Click = GetCollision();

                    // Check if the selected point is inside the playing field, then move
                    if (Click.X > 0.0f && Click.X < (float)PlayfieldWidth*TileWidth && Click.Y > 0 && Click.Y < (float)PlayfieldWidth*TileWidth)
                    {
                        //Start moving to the clicked position.
                        //Note that the character will not instantaneously change its position, but sets its movement target 
                        //and will then move by small amounts in every Update.
                        Character.LinearMove(Character.PositionCurrent, new Vector2(Click.X, Click.Y));
                    }
                }
            }

            //Update the character (move towards its target)
            Character.Update(elapsedTime);
            //updates the matrix representing the position of the character.
            CharacterMatrix = Matrix.CreateTranslation(Character.PositionCurrent.X, Character.PositionCurrent.Y, 0.0f);

            //Add or Remove walls if the left mouse button is used.
            if (MSState_Current.LeftButton == ButtonState.Pressed)
            {
                //Get the coordinates in 3d world of the mouse click
                //Click = GetCollision();

                //Determine the tile that was clicked on
                Point point = new Point((int)(Click.X / TileWidth), (int)(Click.Y / TileWidth));

                //Is it inside the playing field?
                if (point.X >= 0 && point.Y >= 0 && point.X < PlayfieldWidth && point.Y < PlayfieldWidth)
                {
                    if (Box_Add)
                        // Add a box
                        Walls[point.X, point.Y] = true;
                    else
                        //Remove the box
                        Walls[point.X, point.Y] = false;
                }
            }

            //Must call the inherited Update function.
            //Don't touch this one!
            base.Update(gameTime);
        }

        // A function to test if key 'key' is pressed at this moment
        private bool IsKeyPush(Keys key)
        {
            // it is currently being pressed if it was up in the previous keyboard state
            // and is down in the current state.
            return KBState_Current.IsKeyDown(key) && KBState_Previous.IsKeyUp(key);
        }

        // find out what point on the ground plane the mouse position points to
        // returns the intersection point of a 'ray' pointing into the screen at the mouse coordinates with the
        // ground plane.
        //public Vector3 GetCollision()
        //{
        //    // create points with x and y coordinates of the mouse pointer and z coordinates of 0 and 4096.
        //    // a 'long enough' line pointing into the screen at the mouse
        //    Vector3 startC = new Vector3(MSState_Current.X, MSState_Current.Y, 0.0f);
        //    Vector3 endC = new Vector3(MSState_Current.X, MSState_Current.Y, 4096.0f);

        //    //Convert screen space point 'startC' into the corresponding point in world space. 
        //    Vector3 nearPoint = graphics.GraphicsDevice.Viewport.Unproject(startC,
        //                camera.mProjection, camera.mView, Matrix.Identity);

        //    //Convert screen space point 'endC' into the corresponding point in world space. 
        //    Vector3 farPoint = graphics.GraphicsDevice.Viewport.Unproject(endC,
        //                camera.mProjection, camera.mView, Matrix.Identity);

        //    // find the direction of the vector from nearPoint to farPoint
        //    Vector3 rayDirection = Vector3.Normalize(farPoint - nearPoint);

        //    // compute the cosine of the angle between the positive z axis and the ray direction in the world coordinate system
        //    float cosAlpha = Vector3.Dot(Vector3.UnitZ, rayDirection);

        //    // set deltaD to the z-coordinate of nearPoint
        //    float deltaD = Vector3.Dot(Vector3.UnitZ, nearPoint);

        //    // distance becomes the length of a vector starting from nearPoint, pointing in the direction of farPoint
        //    // and ending at the ground plane (z=0).
        //    float distance = deltaD / cosAlpha;

        //    // calculates the intersection of the ray with the ground plane by subtracting the vector above from the nearPoint.
        //    return nearPoint - (rayDirection * distance);
        //}

        //// The Draw function is called when the game should draw itself.
        ////gameTime provides a snapshot of current time.
        protected override void Draw(GameTime gameTime)
        {
            //Set a background color
            graphics.GraphicsDevice.Clear(Color.DarkBlue);

            ////Set the rendering effect parameters for the view and projection matrices .
            effect.Parameters["View"].SetValue(ArcCamera.ViewMatrix);
            effect.Parameters["Projection"].SetValue(ArcCamera.ProjectionMatrix);

            //Draw ground plane with texture
            //Set the world matrix according to the plane object
            effect.Parameters["World"].SetValue(PlaneMatrix);
            //Set the texture for the plane object (tile)
            effect.Parameters["hasTexture"].SetValue(true);
            effect.Parameters["currentTexture"].SetValue(Texture_Tile);
            //Draw the mesh in the model. Details in function DrawMesh
            DrawMesh(Model_Plane.Meshes[0]);

            //Draw all boxes. Details in function DrawBoxArray
            DrawBoxArray(Model_Box);

            // Draw the character (Admoveo)
            //Set the world matrix according to the character 
            effect.Parameters["World"].SetValue(CharacterMatrix);
            //Details in function DrawModel.
            DrawModel(Model_Admoveo);

            //Always need to call the inherited fucntion Draw.
            //Do not remove!
            base.Draw(gameTime);
        }

        //Draw all the boxes
        public void DrawBoxArray(Model sampleMesh)
        {
            //Draw walls with box texture
            effect.Parameters["hasTexture"].SetValue(true);
            effect.Parameters["currentTexture"].SetValue(Texture_Box);

            //our sample meshes only contain a single part, so we don't need to bother
            //looping over the ModelMesh and ModelMeshPart collections. If the meshes
            //were more complex, we would repeat all the following code for each part
            ModelMesh mesh = sampleMesh.Meshes[0];
            ModelMeshPart meshPart = mesh.MeshParts[0];

            //set the vertex source for drawing to the mesh's vertex buffer (the 3D points in the mesh)
            graphics.GraphicsDevice.SetVertexBuffer(mesh.MeshParts[0].VertexBuffer);

            //set the current index buffer to the sample mesh's index buffer
            graphics.GraphicsDevice.Indices = mesh.MeshParts[0].IndexBuffer;

            // set the current rendering technique (see the ShaderEffect.fx file)
            effect.CurrentTechnique = effect.Techniques["Technique1"];

            //at this point' we're ready to begin drawing
            //To start using any effect, you must call Effect.Apply
            //to start using the current technique (set in LoadGraphicsContent)

            // Iterate over all the tiles in the field
            for (int y = 0; y < PlayfieldWidth; y += 1)
            {
                for (int x = 0; x < PlayfieldWidth; x += 1)
                {
                    if (Walls[x, y]) // there is a box, draw it.
                    {
                        //Set the World matrix to position the box in the right place.
                        effect.Parameters["World"].SetValue(WallMatrices[x, y]);

                        //now we loop through the passes in the rendering technique, drawing each
                        //one in order
                        for (int i = 0; i < effect.CurrentTechnique.Passes.Count; i++)
                        {
                            //EffectPass.Apply will update the device to
                            //begin using the state information defined in the current pass
                            effect.CurrentTechnique.Passes[i].Apply();

                            //sampleMesh contains all of the information required to draw
                            //the current mesh       
                            graphics.GraphicsDevice.DrawIndexedPrimitives(
                                PrimitiveType.TriangleList, meshPart.VertexOffset, 0,
                                meshPart.NumVertices, meshPart.StartIndex, meshPart.PrimitiveCount);
                        }
                    }

                }
            }
        }


        public void DrawModel(Model model)
        {

            // Draw the model. A model can have multiple meshes, so loop.
            foreach (ModelMesh mesh in model.Meshes)
            {
                // This is where the mesh orientation is set, as well 
                // as our camera and projection.
                foreach (BasicEffect eff in mesh.Effects)
                {
                    eff.EnableDefaultLighting();
                    eff.World = effect.Parameters["World"].GetValueMatrix();
                    eff.View = effect.Parameters["View"].GetValueMatrix();
                    eff.Projection = effect.Parameters["Projection"].GetValueMatrix();
                }
                // Draw the mesh, using the effects set above.
                mesh.Draw();
            }
        }

        public void DrawMesh(ModelMesh mesh)
        {
            //our sample meshes only contain a single part, so we don't need to bother
            //looping over the ModelMesh and ModelMeshPart collections. If the meshes
            //were more complex, we would repeat all the following code for each part
            ModelMeshPart meshPart = mesh.MeshParts[0];

            //set the vertex source to the mesh's vertex buffer
            graphics.GraphicsDevice.SetVertexBuffer(mesh.MeshParts[0].VertexBuffer);

            //set the current index buffer to the sample mesh's index buffer
            graphics.GraphicsDevice.Indices = mesh.MeshParts[0].IndexBuffer;

            // set the current technique 
            effect.CurrentTechnique = effect.Techniques["Technique1"];

            //now we loop through the passes in the technique, drawing each
            //one in order
            for (int i = 0; i < effect.CurrentTechnique.Passes.Count; i++)
            {
                //EffectPass.Apply will update the device to
                //begin using the state information defined in the current pass
                effect.CurrentTechnique.Passes[i].Apply();

                //sampleMesh contains all of the information required to draw
                //the current mesh       

                graphics.GraphicsDevice.DrawIndexedPrimitives(
                    PrimitiveType.TriangleList, meshPart.VertexOffset, 0,
                    meshPart.NumVertices, meshPart.StartIndex, meshPart.PrimitiveCount);
            }

        }

        static class Program
        {
            /// <summary>
            /// The main entry point for the application.
            /// </summary>
            static void Main(string[] args)
            {
                // create an instance of the game class
                using (HighRollerGame game = new HighRollerGame())
                {
                    // and run it.
                    game.Run();
                }
            }
        }
    }
}
