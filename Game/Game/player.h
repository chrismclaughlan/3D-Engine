#pragma once
#include "Engine/vector.h"

class Player
{
private:
	float moveDirX = 0.0f;
	float moveDirY = 0.0f;
	float moveDirZ = 0.0f;
	float fYaw = 0.0f;

private:
	// Camera control
	double turningSpeed = 0.005f;
	double movementSpeed = 0.005f;

	const Vector vUp;
	Vector vCamera;
	Vector vLookDir;
	Vector vForward;
	Vector vTarget;
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
		fYaw += moveDirX * turningSpeed;        // left / right
		vCamera.y += moveDirY * movementSpeed;  // up / down
		mCameraRotation.MakeRotationY(fYaw);
		vLookDir = mCameraRotation * vTarget;
		vTarget = vCamera + vLookDir;
		mCamera.MakePointAt(vCamera, vTarget, vUp);
		mCamera.MakeQuickInverse();
	}

	// Movement
	void moveForward(const bool b = true) { b ? moveDirZ += 1.0f : moveDirZ = 0.0f; };
	void moveBackward(const bool b = true) { b ? moveDirZ -= 1.0f : moveDirZ = 0.0f; };
	void moveLeft(const bool b = true) { b ? moveDirX -= 1.0f : moveDirX = 0.0f; };
	void moveRight(const bool b = true) { b ? moveDirX += 1.0f : moveDirX = 0.0f; };
	void moveUpward(const bool b = true) { b ? moveDirY += 1.0f : moveDirY = 0.0f; };
	void moveDownward(const bool b = true) { b ? moveDirY -= 1.0f : moveDirY = 0.0f; };

	const bool isMovingForward() { return moveDirZ > 0.0f; };
	const bool isMovingBackward() { return moveDirZ < 0.0f; };
	const bool isMovingUpward() { return moveDirY > 0.0f; };
	const bool isMovingDownward() { return moveDirY < 0.0f; };
	const bool isMovingLeft() { return moveDirX < 0.0f; };
	const bool isMovingRight() { return moveDirX > 0.0f; };

	const Vector getVCamera() { return vCamera; };
	const Matrix4x4 getMCamera() { return mCamera; };
};