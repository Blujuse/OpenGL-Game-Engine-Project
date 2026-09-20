#pragma once
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

namespace GE {
	class Camera 
	{
	public:
		Camera(glm::vec3 camPos, glm::vec3 camForwardDir, glm::vec3 camUpDir, float fov, float aR, float near, float far) 
		{
			// Initialise the camera variables to the values passed on the parameters
			pos = camPos;

			forwardDir = camForwardDir;
			up = camUpDir;

			// Initialise the look direction
			lookDir = forwardDir;

			// Initialise pitch and yaw
			pitch = 0.0f;
			yaw = 0.0f;

			fovy = fov;
			aspectR = aR;
			nearClip = near;
			farClip = far;

			updateCamMatrices();
		}

		~Camera() 
		{

		}

	// Gets & Sets Region
	#pragma region Gets
		// Accessor methods
		// Return the position of the camera in terms of x, y, z
		float getPosX()
		{
			return pos.x;
		}

		float getPosY()
		{
			return pos.y;
		}

		float getPosZ()
		{
			return pos.z;
		}

		// Target or what location the camera is looking at
		const glm::vec3 getForwardDir()
		{
			return forwardDir;
		}

		glm::vec3 getPos() 
		{
			return pos;
		}

		// Up direction
		glm::vec3 getUpDir() const
		{
			return up;
		}

		glm::vec3 getLookDir()
		{
			return lookDir;
		}

		// Return the camera's view matrix.  Used by draw
		// method to send view matrix to vertex shader
		glm::mat4 getViewMatrix() const
		{
			return viewMat;
		}

		// Return camera's projection matrix.  Same reason
		// as above
		glm::mat4 getProjectionMatrix() const
		{
			return projectionMat;
		}

		float getPitch()
		{
			return pitch;
		}

		float getYaw()
		{
			return yaw;
		}

	#pragma endregion

	#pragma region Sets
		// Mutator methods
		// Set position
		void setPosX(float newX)
		{
			pos = glm::vec3(newX, pos.y, pos.z);

			updateCamMatrices();
		}

		void setPosY(float newY)
		{
			pos = glm::vec3(pos.x, newY, pos.z);

			updateCamMatrices();
		}

		void setPosZ(float newZ)
		{
			pos = glm::vec3(pos.x, pos.y, newZ);

			updateCamMatrices();
		}

		// Set position for all axes in one method
		void setPos(glm::vec3 newPos) 
		{
			pos = newPos;
			updateCamMatrices();
		}


		// Set new direction
		void setForwardDir(glm::vec3 newDir)
		{
			forwardDir = newDir;

			updateCamMatrices();
		}

		// Set up direction on the camera
		void setUpDir(glm::vec3 newUp) 
		{
			up = newUp;
			updateCamMatrices();
		}

		// Set the fov
		void setFov(float newFov)
		{
			fovy = newFov;

			updateCamMatrices();
		}

		// Set the aspect ratio.
		// You might use this method if the program's window
		// can be resized meaning the width and height change
		void setAspectRatio(float newAR)
		{
			aspectR = newAR;

			updateCamMatrices();
		}

		// Set clipping planes
		void setNearClip(float newNearClip)
		{
			nearClip = newNearClip;

			updateCamMatrices();
		}

		void setFarClip(float newFarClip)
		{
			farClip = newFarClip;

			updateCamMatrices();
		}

		void setPitch(float newPitch)
		{
			pitch = newPitch;

			if (pitch > 70.0f)
			{
				pitch = 70.0f;
			}

			if (pitch < -70.0f)
			{
				pitch = -70.0f;
			}

			updateCamMatrices();
		}

		void setYaw(float newYaw)
		{
			yaw = newYaw;

			updateCamMatrices();
		}

	#pragma endregion

	private:
		// Update the camera matrices done in response to
		// a member variable variable being updated
		void updateCamMatrices() 
		{
			glm::mat4 rollPitchYawMat = glm::mat4(1.0f);

			rollPitchYawMat = glm::rotate(rollPitchYawMat, glm::radians(-yaw), glm::vec3(0.0f, 1.0f, 0.0f));
			rollPitchYawMat = glm::rotate(rollPitchYawMat, glm::radians(pitch), glm::vec3(1.0f, 0.0f, 0.0f));

			lookDir = rollPitchYawMat * glm::vec4(forwardDir, 0.0f);

			viewMat = glm::lookAt(pos, pos + lookDir, up);
			projectionMat = glm::perspective(glm::radians(fovy), aspectR, nearClip, farClip);
		}

	private:
		// Member variables
		// Camera view variables
		glm::vec3 pos;

		// Consider where the camera is looking at as a direction
		glm::vec3 forwardDir;
		glm::vec3 up;

		// Where the camera is looking
		glm::vec3 lookDir;

		// Cam orientation
		float pitch, yaw;

		// Camera projection variables
		float fovy;
		float aspectR;
		float nearClip;
		float farClip;

		// View and projection matrices
		glm::mat4 viewMat;
		glm::mat4 projectionMat;
	};
}
