using System;
using System.Collections.Generic;
using System.Linq;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Audio;
using Microsoft.Xna.Framework.Content;
using Microsoft.Xna.Framework.GamerServices;
using Microsoft.Xna.Framework.Graphics;
using Microsoft.Xna.Framework.Input;
using Microsoft.Xna.Framework.Media;

namespace Tetris3D
{
    /// <summary>
    /// This is the main type for your game
    /// </summary>
    public class Game1 : Microsoft.Xna.Framework.Game
    {
        GraphicsDeviceManager graphics;
        SpriteBatch spriteBatch;

        public Model blockModel;
        public Model projectionModel;
        public Grid grid;

        //Our rotating camera
        Camera camera;

        KeyboardState KBState_Old;

        // Positie van de camera in 3D coördinaten. Voor 3D coördinaten gebruiken we objecten van klasse 'Vector3'
        // De positie van de camera wordt initieel vastgesteld op (0.0, 80.0, 500.0)
        Vector3 cameraPosition = new Vector3(300.0f, 500.0f, 700.0f);
        // 'aimPosition' is het punt (in 3D) waar de camera naar kijkt.
        Vector3 aimPosition = new Vector3(0.0f, 0.0f, 0.0f);
        private float aspectRatio;

        public Game1()
        {
            graphics = new GraphicsDeviceManager(this);
            Content.RootDirectory = "Content";
            grid = new Grid(this, 4, 10);
            aimPosition = new Vector3(5.0f * Grid.baseSize, 0.0f, 5.0f * Grid.baseSize);
        }

        /// <summary>
        /// Allows the game to perform any initialization it needs to before starting to run.
        /// This is where it can query for any required services and load any non-graphic
        /// related content.  Calling base.Initialize will enumerate through any components
        /// and initialize them as well.
        /// </summary>
        protected override void Initialize()
        {

            camera = new Camera(
                //new Vector3(400.0f, 0.0f, 240.0f),
                //new Vector3(120.0f, 120.0f, 0.0f)
                cameraPosition,
                aimPosition
            );

            // Allow the mouse pointer to be visible in the game window.
            IsMouseVisible = true;
            
            grid.graphics = this.graphics;
            grid.GraphicsDevice = this.GraphicsDevice;

            grid.initialize();

            base.Initialize();
        }

        /// <summary>
        /// LoadContent will be called once per game and is the place to load
        /// all of your content.
        /// </summary>
        protected override void LoadContent()
        {
            blockModel = Content.Load<Model>("models\\cube");
            projectionModel = Content.Load<Model>("models\\blackSquare");

            // Create a new SpriteBatch, which can be used to draw textures.
            spriteBatch = new SpriteBatch(GraphicsDevice);

            // bereken de lengte/breedte verhouding van het venster. Die hebben we later nodig.
            aspectRatio = (float)graphics.GraphicsDevice.Viewport.Width / (float)graphics.GraphicsDevice.Viewport.Height;

            // TODO: use this.Content to load your game content here
        }

        /// <summary>
        /// UnloadContent will be called once per game and is the place to unload
        /// all content.
        /// </summary>
        protected override void UnloadContent()
        {
            // TODO: Unload any non ContentManager content here
        }

        /// <summary>
        /// Allows the game to run logic such as updating the world,
        /// checking for collisions, gathering input, and playing audio.
        /// </summary>
        /// <param name="gameTime">Provides a snapshot of timing values.</param>
        protected override void Update(GameTime gameTime)
        {
            // Allows the game to exit
            if (GamePad.GetState(PlayerIndex.One).Buttons.Back == ButtonState.Pressed)
                this.Exit();

            KeyboardState KBState_Current = Keyboard.GetState();
            if (KBState_Old == null)
                KBState_Old = KBState_Current;

            if (KBState_Current.IsKeyDown(Keys.W) && !KBState_Old.IsKeyDown(Keys.W))
            {
                grid.currentBlock.move(new Vector3(0, 0, -1), new Vector3(0, 0, 0));
            }
            if (KBState_Current.IsKeyDown(Keys.S) && !KBState_Old.IsKeyDown(Keys.S)) // Naar voren lopen
            {
                grid.currentBlock.move(new Vector3(0, 0, 1), new Vector3(0, 0, 0));
            }
            if (KBState_Current.IsKeyDown(Keys.D) && !KBState_Old.IsKeyDown(Keys.D))
            {
                grid.currentBlock.move(new Vector3(1, 0, 0), new Vector3(0, 0, 0));
            }
            if (KBState_Current.IsKeyDown(Keys.A)  && !KBState_Old.IsKeyDown(Keys.A)) // Naar rechts lopen
            {
                grid.currentBlock.move(new Vector3(-1, 0, 0), new Vector3(0, 0, 0));
            }
            if (KBState_Current.IsKeyDown(Keys.Q) && !KBState_Old.IsKeyDown(Keys.Q))
            {
                grid.currentBlock.move(new Vector3(0, 0, 0), new Vector3(1.0f, 0.0f, 0.0f));
            }
            if (KBState_Current.IsKeyDown(Keys.E) && !KBState_Old.IsKeyDown(Keys.E))
            {
                grid.currentBlock.move(new Vector3(0, 0, 0), new Vector3(0.0f, 1.0f, 0.0f));
            }
            if (KBState_Current.IsKeyDown(Keys.R) && !KBState_Old.IsKeyDown(Keys.R))
            {
                grid.currentBlock.move(new Vector3(0, 0, 0), new Vector3(0.0f, 1.0f, 0.0f));
            }

            KBState_Old = KBState_Current;

            grid.update(gameTime);

            // TODO: Add your update logic here

            base.Update(gameTime);
        }

        /// <summary>
        /// This is called when the game should draw itself.
        /// </summary>
        /// <param name="gameTime">Provides a snapshot of timing values.</param>
        protected override void Draw(GameTime gameTime)
        {
            GraphicsDevice.Clear(Color.WhiteSmoke);

            // TODO: Add your drawing code here
            grid.draw();

            base.Draw(gameTime);
        }

        // Teken een model op een zekere positie
        // Hier wordt het een klein beetje ingewikkeld om de 3D effecten uit te voeren.
        // Misschien hoef je deze functie helemaal niet aan te passen, maar het kan natuurlijk wel.
        public void drawModel(Model model, float x, float y, float z)
        {
            // Maak een vector met de gewenste coordinaten.
            Vector3 modelPosition = new Vector3(x, y, z);

            // Zoals bij de introductie uitgelegd, worden modeltransformaties gerepresenteerd met matrices.
            // Een model kan bestaan uit onderdelen 'bones', met elk hun eigen, relatieve transformatie.
            // Die vragen we eerst op uit het model
            // Maake eerst een array van matrices aan
            Matrix[] transforms = new Matrix[model.Bones.Count];
            // Vul deze array met de matrices van de transformaties van elk van de onderdelen van het model.
            model.CopyAbsoluteBoneTransformsTo(transforms);

            // 'view' is een 3D-naar-3D transformatie matrix die bepaalt hoe 3D voorwerpen georienteerd zijn tov een camera in de ruimte.
            // Maak een standaard view matrix op basis van een camera perspectief van een camera op locatie 'cameraPosition' die kijkt naar positie 
            // 'aimPosition' en die horizontaal hangt wanneer de vector 'Vector3.Up' (0,1,) naar boven wijst.
            Matrix view = Matrix.CreateLookAt(cameraPosition, aimPosition, Vector3.Up);
            // 'projection' is een 3D-naar-2D transformatie matrix die bepaalt hoe de 3D voorwerpen in een assenstelsel relatief tov de camera afgebeeld worden
            // op een 2D vlak zoals het scherm.
            // Maak de projectie op basis van een camera met een field-of-view van 45 graden een lengte-breedte verhouding van 'aspectRatio' die alle voorwerpen ziet met een afstand 
            // tussen 1 en 10000 eenheden.
            Matrix projection = Matrix.CreatePerspectiveFieldOfView(MathHelper.ToRadians(45.0f), aspectRatio, 1.0f, 10000.0f);

            // Teken het model. Een model kan bestaan uit meerder 'Meshes', die één voor één getekend moeten worden. Vandaar de volgende lus.
            foreach (ModelMesh mesh in model.Meshes)
            {
                // bepaal de transformatie matrix. Deze begint met de transformatie uit het model zelf, uit de transforms array
                // Daaraan wordt nog een translatie toegevoegd (vermenigvuldiging van de matrices), naar de gewenste model locatie (modelPosition)
                // Rotatie transformaties zijn ook mogelijk, bijv. een rotatie om de Z-as met de functie: Matrix.CreateRotationZ()
                Matrix world = transforms[mesh.ParentBone.Index] * Matrix.CreateTranslation(modelPosition);

                // Het afbeelden ('renderen') van het model gebeurt met één of meerdere 'effecten', gedefinieerd in de mesh.
                // Die moeten één voor één ingesteld worden met de gewenste parameters
                foreach (BasicEffect effect in mesh.Effects)
                {
                    // Hier worden de uiteindelijke rendering parameters bepaald.
                    // belichtingseffect 
                    effect.EnableDefaultLighting();
                    // voeg licht uit een bepaalde richting toe
                    effect.DirectionalLight1.Enabled = true;
                    effect.DirectionalLight1.Direction = new Vector3(0.0f, 300.0f, 0.0f);
                    // zet de gewenste transformatie voor het object, 'world'
                    effect.World = world;
                    // zet de transformatie die het voorwerp in absolute 'wereld' coordinaten omzet in camera coordinaten
                    effect.View = view;
                    // zet de transformatie die de projectie naar een 2D afbeelding uitvoert.
                    effect.Projection = projection;

                    if (model == projectionModel)
                    {
                        GraphicsDevice.BlendState = BlendState.AlphaBlend;
                        effect.Alpha = 0.5f;
                    }

                }
                // Teken uiteindelijk mesh daadwerkelijk op het scherm met de effecten hierboven gedefinieerd.
                mesh.Draw();
            }
        }
    }
}
