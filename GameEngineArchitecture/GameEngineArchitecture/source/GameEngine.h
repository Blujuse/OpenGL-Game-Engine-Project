#pragma once

#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <vector>

#include "ModelRenderer.h"
#include "Model.h"
#include "Camera.h"
#include "SkyboxRenderer.h"
#include "Terrain.h"
#include "Skydome.h"
#include "FPSCameraController.h"
#include "Billboard.h"
#include "BillboardRenderer.h"
#include "GUIRenderer.h"
#include "FrameBuffer.h"
#include "PostProcessJungle.h"
#include "InstancedRenderer.h"
#include "BatchRenderer.h"

namespace GE
{
	class GameEngine
	{

	public:
		GameEngine();

		virtual ~GameEngine();

		bool Init(bool vSyncOn);
		bool KeepRunning();
		void Update();
		void Draw();
		void Shutdown();

		void ChangeCam();

		void SetWindowTitle(const char*);

		float randomFloat(float min, float max);

		glm::vec3 lerp(const glm::vec3& a, const glm::vec3& b, float t);

	private:
		SDL_Window* window;
		int windowWidth, windowHeight;

		SDL_GLContext glContext;

		// Camera
		Camera* currentCam;
		Camera* cam;
		Camera* crocCam;
		bool prevFKeyDown = false;
		glm::vec3 dist;

		// FPS Camera Controller
		FPSCameraController* fpsController;

		// Temple
		Model* temple;
		Texture* templeTex;
		Texture* templeNorm;
		ModelRenderer* templeModelRenderer;

		// Ark Objs
		Model* ark;
		Texture* arkTex;
		Texture* arkNorm;
		ModelRenderer* arkModelRenderer;

		// Crocodile Objs
		Model* croc;
		Texture* crocTex;
		ModelRenderer* crocModelRenderer;
		glm::vec3 crocOriginalPos;
		glm::vec3 crocCurrentPos;
		glm::vec3 crocTargetPos;
		bool crocMoving = false;

		// Terrain
		Terrain* terrain;
		Texture* terrainTexture;
		ModelRenderer* mrTerrain;

		// Water
		Terrain* water;
		Texture* waterTexture;
		ModelRenderer* mrWater;

		// Skybox
		SkyboxRenderer* skybox;

		// Skydome
		Skydome* skydome;
		Texture* skydomeTexture;
		ModelRenderer* mrSkydome;

		// Billboard objects
		std::vector<glm::vec3> treePositions;
		Model* treeModel;
		Texture* treeModelTex;
		Texture* treeModelNorm;
		ModelRenderer* treeModelRenderer;
		Billboard* treeBill;
		Texture* treeBillTex;
		BillboardRenderer* treeBillRenderer;

		// GUI stuff
		GUIRenderer* gr;
		GUIRenderer* minimapGR;
		GUIText* FPSMsg;
		GUIImage* miniMapImg;
		GUIImage* miniMapPlayerImg;
		GUIImage* cockpitImg;
		Uint32 lastTime, current_time, frame_count;

		// Frame Buffer
		FrameBuffer* fb;
		Model* quadModel;
		ModelRenderer* quadModelRenderer;
		PostProcessJungle* ppG;

		// Instanced Renderer
		InstancedRenderer* instancedRenderer;
		Model* instancedModel;
		Texture* instancedTex;

		// Batch Renderer
		BatchRenderer* batchRenderer;
	};

	void DisplayInfoMessage(const char*);
}