#include <iostream>
#include <sstream>

#include "GameEngine.h"
#include "ModelRenderer.h"
#include "Camera.h"

namespace GE
{
	GameEngine::GameEngine()
	{
		window = nullptr;
		glContext = nullptr;
		windowWidth = 800;
		windowHeight = 600;
	}

	GameEngine::~GameEngine()
	{

	}

	bool GameEngine::Init(bool vSyncOn)
	{
		#pragma region Setting Up SDL & OpenGL

		// Initialise SDL
		if (SDL_Init(SDL_INIT_VIDEO) < 0)
		{
			DisplayInfoMessage("Failed to initialise SDL");
			return false;
		}

		// Create window
		window = SDL_CreateWindow("Game Engine Architecture", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
		if (window == nullptr)
		{
			DisplayInfoMessage("Failed to create window");
			return false;
		}

		// Set OpenGL version to 4.1
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
		// Set the profile to core, modern OpenGL
		// Means no legacy features for backwards compatibility
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);

		// Create OpenGL context
		glContext = SDL_GL_CreateContext(window);
		if (glContext == nullptr)
		{
			DisplayInfoMessage("Failed to create OpenGL context");
			return false;
		}

		// Initialise GLEW
		GLenum glewError = glewInit();
		if (glewError != GLEW_OK)
		{
			DisplayInfoMessage("Failed to initialise GLEW");
			return false;
		}

		// Try to turn on VSync
		if (vSyncOn == true)
		{
			SDL_GL_SetSwapInterval(1);
		}
		else
		{
			SDL_GL_SetSwapInterval(0);
		}

		// Set clear colour
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		#pragma endregion

		#pragma region Creating Cam

		// Look down Z axis into the screen, effectively looks into the display
		dist = glm::vec3(0.0f, 0.0f, -100.0f);

		// Create Camera
		cam = new Camera(glm::vec3(0.0f, 20.0f, 0.0f), // Position
			glm::vec3(0.0f, 0.0f, 0.0f) + dist, // Look at, with the dist variable added
			glm::vec3(0.0f, 1.0f, 0.0f), // Up Direction
			45.0f, 800.0f / 600.0f, 0.1f, 200.0f); // fov, aspect ratio, near clip, far clip

		fpsController = new FPSCameraController(cam, 0.1f, 0.1f);

		crocCam = new Camera(glm::vec3(-114.0f, 30.0f, 103.0f), // Position
			glm::vec3(125.0f, -45.0f, 0.0f) + dist, // Look at, with the dist variable added
			glm::vec3(0.0f, 1.0f, 0.0f), // Up Direction
			45.0f, 800.0f / 600.0f, 0.1f, 200.0f); // fov, aspect ratio, near clip, far clip

		currentCam = cam;

		#pragma endregion

		#pragma region Ark Loading

		// Create model obj
		ark = new Model();

		// Load model from file
		bool result = ark->LoadFromFile("./assets/models/Ark.obj");

		// Check it was loaded
		if (!result)
		{
			std::cerr << "Failed to load Ark" << std::endl;

			return false;
		}

		arkTex = new Texture("./assets/images/Gold.png");
		arkNorm = new Texture("./assets/images/GoldNorm.png");

		// Create a model renderer passing the model object just loaded
		arkModelRenderer = new ModelRenderer();

		// Initialise the model renderer, reduce the scale
		arkModelRenderer->init();
		arkModelRenderer->setPos(73.0f, 5.75f, -80.0f);
		arkModelRenderer->setScale(0.8f, 0.8f, 0.8f);
		arkModelRenderer->setRot(0.0f, 350.0f, 0.0f);
		arkModelRenderer->setTexture(arkTex);
		arkModelRenderer->setNormalTexture(arkNorm);

		#pragma endregion

		#pragma region Skybox & Skydome

		// Construct a skybox
		skybox = new SkyboxRenderer("./assets/images/front.jpg", "./assets/images/back.jpg",
			"./assets/images/right.jpg", "./assets/images/left.jpg",
			"./assets/images/top.jpg", "./assets/images/bottom.jpg");

		// Contruct a skydome with texture
		//skydome = new Skydome();
		//skydomeTexture = new Texture("./assets/images/skydome.jpg");

		// Model renderer for the skydome
		//mrSkydome = new ModelRenderer();
		//mrSkydome->init();
		//mrSkydome->setTexture(skydomeTexture);

		#pragma endregion

		#pragma region Terrain Loading

		// Construct terrain with heightmap and texture
		terrain = new Terrain("./assets/images/CustomTerrain.png", 10.0f, 200.0f);
		terrainTexture = new Texture("./assets/images/TerrainGrass.png");

		// Create a model renderer for the terrain
		mrTerrain = new ModelRenderer();
		mrTerrain->init();
		mrTerrain->setTexture(terrainTexture);

		#pragma endregion

		#pragma region Temple Loading

		temple = new Model();
		bool templeResult = temple->LoadFromFile("./assets/models/Temple.obj");
		if (!templeResult)
		{
			std::cerr << "Failed to load temple" << std::endl;
			return false;
		}

		templeTex = new Texture("./assets/images/Temple.png");
		templeNorm = new Texture("./assets/images/TempleNorm.png");

		templeModelRenderer = new ModelRenderer();
		templeModelRenderer->init();
		templeModelRenderer->setPos(73.0f, 3.25f, -80.0f);
		templeModelRenderer->setScale(0.1f, 0.1f, 0.1f);
		templeModelRenderer->setRot(0.0f, 260.0f, 0.0f);
		templeModelRenderer->setTexture(templeTex);
		templeModelRenderer->setNormalTexture(templeNorm);

		#pragma endregion

		#pragma region Crocodile Loading

		croc = new Model();

		bool crocResult = croc->LoadFromFileNoNorm("./assets/models/Crocodile.obj");
		if (!crocResult)
		{
			std::cerr << "Failed to load crocodile" << std::endl;
			return false;
		}

		crocTex = new Texture("./assets/images/Shrub.png");

		crocModelRenderer = new ModelRenderer();
		crocModelRenderer->initNoNorm();

		crocCurrentPos = glm::vec3(-73.0f, 1.5f, 80.0f); // or your initial croc position
		crocOriginalPos = crocCurrentPos; // Store the original position
		crocModelRenderer->setPos(-73.0f, 1.5f, 80.0f);
		crocModelRenderer->setScale(2.0f, 2.0f, 2.0f);
		crocModelRenderer->setRot(0.0f, -45.0f, 0.0f);
		crocModelRenderer->setTexture(crocTex);

		#pragma endregion

		#pragma region Big Tree Model & Billboard

		int bigTreeCountLeft = 4;

		for (int i = 0; i < bigTreeCountLeft; i++)
		{
			float randomX = randomFloat(38.0f, 65.0f);
			float randomY = randomFloat(3.1f, 3.3f);
			float randomZ = randomFloat(-85.0f, -10.0f);

			treePositions.push_back(glm::vec3(randomX, randomY, randomZ));
		}

		int bigTreeCountRight = 4;

		for (int i = 0; i < bigTreeCountRight; i++)
		{
			float randomX = randomFloat(80.0f, 100.0f);
			float randomY = randomFloat(3.1f, 3.3f);
			float randomZ = randomFloat(-85.0f, 10.0f);
			treePositions.push_back(glm::vec3(randomX, randomY, randomZ));
		}

		int bigTreeCountBottom = 2;

		for (int i = 0; i < bigTreeCountBottom; i++)
		{
			float randomX = randomFloat(38.0f, 100.0f);
			float randomY = randomFloat(3.1f, 3.3f);
			float randomZ = randomFloat(10.0f, 30.0f);
			treePositions.push_back(glm::vec3(randomX, randomY, randomZ));
		}

		int bigTreeCountTop = 4;

		for (int i = 0; i < bigTreeCountTop; i++)
		{
			float randomX = randomFloat(38.0f, 100.0f);
			float randomY = randomFloat(3.1f, 3.3f);
			float randomZ = randomFloat(-100.0f, -90.0f);
			treePositions.push_back(glm::vec3(randomX, randomY, randomZ));
		}

		// Load a texture for the billboard, tree
		treeBillTex = new Texture("./assets/images/BigTreeBill.png");

		// Default billboard will be at 0, 0, 0 in front of the camera
		treeBill = new Billboard(treeBillTex, 0, 0 - 0.8f, 0);

		// Scale the billboard
		//treeBill->setScaleX(9.0f);
		//treeBill->setScaleY(9.0f);

		// Set it in front of the model
		//treeBill->setZ(1.0f);

		// Create a billboard renderer
		treeBillRenderer = new BillboardRenderer();

		// Initialise the billboard renderer
		treeBillRenderer->init();

		// Load a tree model
		treeModel = new Model();
		bool treeResult = treeModel->LoadFromFile("./assets/models/BigTree.obj");

		if (!treeResult)
		{
			std::cerr << "Failed to load tree" << std::endl;

			return false;
		}

		treeModelTex = new Texture("./assets/images/BigTreeAtlas.png");

		treeModelNorm = new Texture("./assets/images/NewTreeNormal.png");

		// Create a model renderer for the tree
		treeModelRenderer = new ModelRenderer();

		// Initialise the model renderer
		treeModelRenderer->init();
		//treeModelRenderer->setPos(treeX, treeY, treeZ);
		//treeModelRenderer->setScale(1.0f, 1.0f, 1.0f);
		treeModelRenderer->setTexture(treeModelTex);
		treeModelRenderer->setNormalTexture(treeModelNorm);

		#pragma endregion

		#pragma region GUI

		// Create a GUI renderer object
		gr = new GUIRenderer();

		gr->init(windowWidth, windowHeight);

		// Create text object for frame counter
		FPSMsg = new GUIText(0, 555, "FPS goes here", "./assets/fonts/Oswald-Regular.ttf");

		// Load texture for minimap and make GUI image for texture
		Texture* minimapTex = new Texture("./assets/images/terrain-texture-mini-map.png");
		miniMapImg = new GUIImage(0, 0, minimapTex);

		Texture* miniMapPlayerTex = new Texture("./assets/images/player-mini-map-img.png");
		miniMapPlayerImg = new GUIImage(0, 0, miniMapPlayerTex);

		// Load cockpit for hud
		Texture* cockpitTex = new Texture("./assets/images/IndyHatHUD.png");

		cockpitImg = new GUIImage(0, 0, cockpitTex);

		lastTime = current_time = SDL_GetTicks();

		frame_count = 0;

		#pragma endregion

		#pragma region Frame Buffer Object & Post Process

		// Create frame buffer object
		fb = new FrameBuffer(windowWidth, windowHeight);
		fb->init();

		// Create quad model
		quadModel = new Model();
		bool quadResult = quadModel->LoadFromFileNoNorm("./assets/models/Quad.obj");

		if (!quadResult)
		{
			std::cerr << "Failed to load quad model" << std::endl;

			return false;
		}

		// Create model renderer for the quad
		quadModelRenderer = new ModelRenderer();
		quadModelRenderer->initNoNorm();

		// Set model pos and scale
		quadModelRenderer->setPos(0.0f, 20.0f, -10.0f);
		quadModelRenderer->setScale(10.0f, 10.0f, 10.0f);

		// Set the quad model texture to the frame buffer texture
		quadModelRenderer->setTexture(fb->getTexture());

		// Create post process
		ppG = new PostProcessJungle(windowWidth, windowHeight);
		ppG->init();

		#pragma endregion

		#pragma region Instanced Renderer

		instancedModel = new Model();
		bool instRes = instancedModel->LoadFromFileNoNorm("./assets/models/NewTree.obj");

		if (!instRes)
		{
			std::cerr << "Failed to load tree" << std::endl;

			return false;
		}

		instancedTex = new Texture("./assets/images/NewTreeUV.png");

		// Create instanced renderer
		instancedRenderer = new InstancedRenderer();
		instancedRenderer->init();
		instancedRenderer->setTexture(instancedTex);

		std::vector<InstancePosRotScale> instances;

		int treeCountLeft = 50;
		for (int i = 0; i < treeCountLeft; i++)
		{
			float randomX = randomFloat(38.0f, 65.0f);
			float randomY = randomFloat(3.1f, 5.0f);
			float randomZ = randomFloat(-85.0f, -10.0f);
			instances.push_back(InstancePosRotScale{ randomX, randomY, randomZ, 0.0f, 0.0f, 0.0f, 0.3f, 0.3f, 0.3f });
		}

		int treeCountRight = 50;
		for (int i = 0; i < treeCountRight; i++)
		{
			float randomX = randomFloat(80.0f, 100.0f);
			float randomY = randomFloat(3.1f, 5.0f);
			float randomZ = randomFloat(-85.0f, 10.0f);
			instances.push_back(InstancePosRotScale{ randomX, randomY, randomZ, 0.0f, 0.0f, 0.0f, 0.3f, 0.3f, 0.3f });
		}

		int treeCountBottom = 40;
		for (int i = 0; i < treeCountBottom; i++)
		{
			float randomX = randomFloat(38.0f, 100.0f);
			float randomY = randomFloat(3.0f, 5.0f);
			float randomZ = randomFloat(10.0f, 30.0f);
			instances.push_back(InstancePosRotScale{ randomX, randomY, randomZ, 0.0f, 0.0f, 0.0f, 0.3f, 0.3f, 0.3f });
		}

		int treeCountTop = 20;
		for (int i = 0; i < treeCountTop; i++)
		{
			float randomX = randomFloat(38.0f, 100.0f);
			float randomY = randomFloat(3.1f, 5.0f);
			float randomZ = randomFloat(-100.0f, -90.0f);
			instances.push_back(InstancePosRotScale{ randomX, randomY, randomZ, 0.0f, 0.0f, 0.0f, 0.3f, 0.3f, 0.3f });
		}

		instancedRenderer->setInstanceData(instances);

		#pragma endregion

		#pragma region Batch Renderer

		batchRenderer = new BatchRenderer();
		batchRenderer->init();

		int rockCount = 40;
		for (int i = 0; i < rockCount; i++)
		{
			float randomX = randomFloat(-90.0f, 10.0f);
			float randomZ = randomFloat(-100.0f, -40.0f);
			float randomY = randomFloat(9.0f, 10.0f);

			float randomRotY = randomFloat(0.0f, 360.0f);

			float randomScale = randomFloat(2.0f, 3.0f);
			float randomScale2 = randomFloat(0.5f, 0.8f);

			batchRenderer->addModelToBatch("./assets/models/Rock.obj", "./assets/images/Rock.png",
				// Location                           Rotation                           Scale
				glm::vec3(randomX, randomY, randomZ), glm::vec3(0.0f, randomRotY, 0.0f), glm::vec3(randomScale, randomScale, randomScale));
		}

		int treeCount = 20;
		for (int i = 0; i < treeCount; i++)
		{
			float randomX = randomFloat(-90.0f, 10.0f);
			float randomZ = randomFloat(-100.0f, -40.0f);
			float randomY = randomFloat(9.0f, 10.0f);

			float randomRotY = randomFloat(0.0f, 360.0f);

			float randomScale = randomFloat(0.5f, 0.8f);

			batchRenderer->addModelToBatch("./assets/models/BigTree.obj", "./assets/images/BigTreeAtlas.png",
				// Location                           Rotation                           Scale
				glm::vec3(randomX, randomY, randomZ), glm::vec3(0.0f, randomRotY, 0.0f), glm::vec3(randomScale, randomScale, randomScale));
		}

		#pragma endregion

		#pragma region Water Loading

		// Construct terrain with heightmap and texture
		water = new Terrain("./assets/images/CustomWater.png", 15.0f, 200.0f);
		waterTexture = new Texture("./assets/images/TerrainWater.png");

		// Create a model renderer for the terrain
		mrWater = new ModelRenderer();
		mrWater->initWater();
		mrWater->setTexture(waterTexture);

		#pragma endregion

		// If gotten this far, then everything has been initialised
		return true;
	}

	bool GameEngine::KeepRunning()
	{
		// Need to do this keep the event queue up to date
		SDL_PumpEvents();

		SDL_Event event;

		// Check for quit event
		if (SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_QUIT, SDL_QUIT))
		{
			// If user quit program, then return false meaning
			// don't keep running
			return false;
		}

		// Got this far means no quit event so keep running
		return true;
	}

	void GameEngine::Update()
	{
		// Rotate camera at current location
		//glm::mat4 camRot = glm::mat4(1.0f);
		// Create a rotation matrix of a certain degree per frame on y axis
		//camRot = glm::rotate(camRot, glm::radians(0.625f), glm::vec3(0.0f, 1.0f, 0.0f));
		// Create a vec 4 in order to multiply against rot matrix
		// The last value should be 0.0f as it should not move the camera
		//glm::vec4 temp = glm::vec4(dist, 0.0f);
		// Rotate dist
		//temp = camRot * temp;
		// Create a new dist based on the rotation
		//dist = glm::vec3(temp.x, temp.y, temp.z);

		//std::cout << "Cam X: " << cam->getPos().x << " Cam Y: " << cam->getPos().y << " Cam Z: " << cam->getPos().z << std::endl;

		float clampedX = glm::clamp(currentCam->getPos().x + (167.5f / 2), 0.0f, 175.5f);
		float clampedY = glm::clamp(currentCam->getPos().z + (167.5f / 2), 0.0f, 175.5f);

		miniMapPlayerImg->setX(clampedX);
		miniMapPlayerImg->setY(clampedY);

		// Moving the crocodile back and forth
		float lerpSpeed = 0.01f;
		crocTargetPos = glm::vec3(-30.0f, 1.5f, 40.0f);

		// Change between target and og pos based on bool
		glm::vec3 destination = crocMoving ? crocTargetPos : crocOriginalPos;

		// Head to destination
		crocCurrentPos = lerp(crocCurrentPos, destination, lerpSpeed);
		//std::cout << "Croc Pos: " << crocCurrentPos.x << " " << crocCurrentPos.y << " " << crocCurrentPos.z << std::endl;
		crocModelRenderer->setPos(crocCurrentPos.x, crocCurrentPos.y, crocCurrentPos.z); // Update pos

		// Check if close to destination
		if (glm::distance(crocCurrentPos, destination) < 0.1f)
		{
			crocMoving = !crocMoving;
		}

		// Set the rotation of the crocodile based on its movement
		if (crocMoving)
		{
			crocModelRenderer->setRot(0.0f, 225.0f, 0.0f);
		}
		else
		{
			crocModelRenderer->setRot(0.0f, 45.0f, 0.0f);
		}


		//std::cout << "Cam X: " << cam->getPos().x << " Cam Y: " << cam->getPos().y << " Cam Z: " << cam->getPos().z << std::endl;

		current_time = SDL_GetTicks();

		if (current_time - lastTime > 1000)
		{
			std::ostringstream msg;

			// Update the FPS message
			msg << "FPS = " << frame_count;

			FPSMsg->setText(msg.str());

			// Reset frame count
			frame_count = 0;

			lastTime = current_time;
		}

		frame_count++;

		// Update the camera
		fpsController->update();

		// Update water
		mrWater->UpdateWater();

		ChangeCam();
	}

	void GameEngine::Draw()
	{
		// Render to the frame buffer
		fb->beginRender();

		// Specify Colours
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glEnable(GL_DEPTH_TEST);

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Render skybox
		skybox->draw(currentCam);
		//mrSkydome->DrawSkydome(cam, skydome);

		// Render model
		arkModelRenderer->Draw(currentCam, ark);

		// Render Crocodile
		crocModelRenderer->DrawNoNorm(currentCam, croc);

		// Render temple
		templeModelRenderer->Draw(currentCam, temple);

		// Render terrain
		mrTerrain->DrawTerrain(currentCam, terrain);

		mrWater->DrawWater(currentCam, water);

		instancedRenderer->drawInstanced(currentCam, instancedModel);

		batchRenderer->draw(currentCam);

		for (const auto& pos : treePositions)
		{
			// Used for billboards
			float dist = glm::distance(currentCam->getPos(), pos);

			// If the distance is less than 75, render the model
			if (dist < 50.0f)
			{
				treeModelRenderer->setPos(pos.x, pos.y, pos.z);
				treeModelRenderer->setScale(0.6f, 0.6f, 0.6f);
				treeModelRenderer->Draw(currentCam, treeModel);
			}
			else
			{
				treeBill->setX(pos.x);
				treeBill->setY(pos.y - 0.8f);
				treeBill->setZ(pos.z);

				treeBill->setScaleX(15.0f);
				treeBill->setScaleY(20.0f);

				treeBillRenderer->draw(treeBill, currentCam);
			}
		}

		// Draw the GUI after 3D objects, and back to front
		gr->drawImage(cockpitImg);
		gr->drawImage(miniMapImg);
		gr->drawText(FPSMsg);
		gr->drawImage(miniMapPlayerImg);

		// Stop rendering to the frame buffer
		fb->endRender();

		// Post process effect
		ppG->apply(fb->getTexture());

		// Specify Colours
		glClearColor(0.392f, 0.584f, 0.929f, 1.0f);
		glEnable(GL_DEPTH_TEST);

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Render what was rendered to the frame buffer & post process to a quad
		ppG->renderToScreen();

		// Swap buffers
		SDL_GL_SwapWindow(window);
	}

	void GameEngine::Shutdown()
	{
		// Destroy model renderer
		//arkModelRenderer->Destroy();

		skybox->destroy();

		delete skybox;
		//delete skydome;
		//delete mrSkydome;

		delete terrain;
		delete water;

		delete arkModelRenderer;
		delete ark;
		delete arkTex;
		delete arkNorm;
		delete cam;

		delete croc;
		delete crocModelRenderer;
		delete crocTex;

		delete templeModelRenderer;
		delete temple;
		delete templeTex;
		delete templeNorm;

		delete treeBill;
		delete treeBillTex;
		delete treeBillRenderer;

		//GUI stuff
		delete FPSMsg;
		delete miniMapImg;
		delete miniMapPlayerImg;
		delete cockpitImg;
		delete gr;

		// Destroy OpenGL context
		SDL_GL_DeleteContext(glContext);

		// Destroy window
		SDL_DestroyWindow(window);

		window = nullptr;

		// Quit SDL
		SDL_Quit();
	}

	void GameEngine::ChangeCam()
	{
		const Uint8* keyState = SDL_GetKeyboardState(nullptr);
		bool fKeyDown = keyState[SDL_SCANCODE_F] != 0;

		if (fKeyDown && !prevFKeyDown) // Key was just pressed
		{
			if (currentCam == cam)
				currentCam = crocCam;
			else
				currentCam = cam;
		}

		prevFKeyDown = fKeyDown; // Update previous state
	}

	void GameEngine::SetWindowTitle(const char* title)
	{
		SDL_SetWindowTitle(window, title);
	}

	void DisplayInfoMessage(const char* message)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Basic Game Engine", message, nullptr);
	}

	float GameEngine::randomFloat(float min, float max) 
	{
		return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX) / (max - min));
	}

	glm::vec3 GameEngine::lerp(const glm::vec3& a, const glm::vec3& b, float t) 
	{
		return a + (b - a) * t;
	}
}
