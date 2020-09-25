#pragma once
#include "Engine/utils_vector.h"

class Player
{
private:
	float moveDirX = 0.0f;
	float moveDirY = 0.0f;
	float moveDirZ = 0.0f;

public:  // temp
	float fYaw = 0.0f;
	float fPitch = 0.0f;

private:
	// Camera control
	float turningSpeed = 0.005f;
	float movementSpeed = 0.005f;

	const Vec4f vUp;
	Vec4f vCamera;
	Vec4f vLookDir;
	Vec4f vForward;
	Vec4f vTarget;
	Matrix4x4 mCameraRotation;
	Matrix4x4 mCamera;

public:
	Player()
		: vUp(0.0f, 1.0f, 0.0f)
	{
	}

	void resetPosition()
	{
		moveDirX = 0.0f;
		moveDirY = 0.0f;
		moveDirZ = 0.0f;
	}

	void resetCamera()
	{
		fYaw = 0.0f;
		vCamera.setZero();
		vLookDir.setZero();
	}

	void updatePosition()
	{
		vTarget = { 0.0f, 0.0f, 1.0f };
		vForward = vLookDir * movementSpeed;
		vCamera += moveDirZ * vForward;		    // forward / backward
		//fYaw += moveDirX * turningSpeed;        // left / right
		vCamera.x += moveDirX * movementSpeed;
		vCamera.y += moveDirY * movementSpeed;  // up / down
		mCameraRotation.MakeRotationX(fPitch);
		mCameraRotation.MakeRotationY(fYaw);
		vLookDir = mCameraRotation * vTarget;
		vTarget = vCamera + vLookDir;
		mCamera.MakePointAt(vCamera, vTarget, vUp);
		mCamera.MakeQuickInverse();
	}

	// Movement
	void moveForward(const bool b = true) { b ? moveDirZ += 1.0f : moveDirZ = 0.0f; };
	void moveBackward(const bool b = true) { b ? moveDirZ -= 1.0f : moveDirZ = 0.0f; };
	void moveLeft(const bool b = true) { b ? moveDirX += 1.0f : moveDirX = 0.0f; };
	void moveRight(const bool b = true) { b ? moveDirX -= 1.0f : moveDirX = 0.0f; };
	void moveUpward(const bool b = true) { b ? moveDirY += 1.0f : moveDirY = 0.0f; };
	void moveDownward(const bool b = true) { b ? moveDirY -= 1.0f : moveDirY = 0.0f; };

	void lookX(const float d)
	{
		fYaw += d * turningSpeed;
	}
	
	void lookY(const float d)
	{
		float maxPitch = 1.5f;

		fPitch -= d * turningSpeed;

		if (fPitch > maxPitch)
		{
			fPitch = maxPitch;
		}
		else if (fPitch < -maxPitch)
		{
			fPitch = -maxPitch;
		}
	}

	const bool isMovingForward() { return moveDirZ > 0.0f; };
	const bool isMovingBackward() { return moveDirZ < 0.0f; };
	const bool isMovingUpward() { return moveDirY > 0.0f; };
	const bool isMovingDownward() { return moveDirY < 0.0f; };
	const bool isMovingLeft() { return moveDirX > 0.0f; };
	const bool isMovingRight() { return moveDirX < 0.0f; };

	const Vec4f getVCamera() { return vCamera; };
	const Matrix4x4 getMCamera() { return mCamera; };
};