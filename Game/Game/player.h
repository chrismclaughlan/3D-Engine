#pragma once
#include "Engine/utils_vector.h"

#define MOV_FORWARD		(0b000001)
#define MOV_BACKWARD	(0b000010)
#define MOV_FB			(0b000011)
#define MOV_LEFT		(0b000100)
#define MOV_RIGHT		(0b001000)
#define MOV_LR			(0b001100)
#define MOV_UP			(0b010000)
#define MOV_DOWN		(0b100000)
#define MOV_UD			(0b110000)


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
	float baseTurningSpeed = 0.005f;
	float fMaxVelocity = 1.0f;

	const Vec4f vUp;
	Vec4f vCamera;
	Vec4f vLookDir;
	Vec4f vLookDirLeft;
	Vec4f vForward;
	Vec4f vLeft;
	const Vec4f vTargetLeft = { 1.0f, 0.0f, 0.0f };
	Matrix4x4 mCameraRotation;
	Matrix4x4 mCamera;

	Vec4f vVelocity = { 0.0f, 0.0f, 0.0f };
	Vec4f vAcceleration = { 0.0f, 0.0f, 0.0f };
	uint8 accelerationFlags = 0b000000;

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

	void updatePosition(const float dt)
	{
		/* Physics */
		vAcceleration.setZero();

		if (accelerationFlags)
		{
			// z-axis
			if ((accelerationFlags & MOV_FORWARD) && !(accelerationFlags & MOV_BACKWARD))
			{
				//vVelocity.z += vVelocity.z * dt;
				vAcceleration.z = 1.0f;
			}
			else if ((accelerationFlags & MOV_BACKWARD) && !(accelerationFlags & MOV_FORWARD))
			{
				//vVelocity.z -= vVelocity.z * dt;
				vAcceleration.z = -1.0f;
			}
			else if ((accelerationFlags & MOV_BACKWARD) && (accelerationFlags & MOV_FORWARD))
			{
				vVelocity.z = 0.0f;
			}

			// y-axis
			if ((accelerationFlags & MOV_UP) && !(accelerationFlags & MOV_DOWN))
			{
				vAcceleration.y = 1.0f;
			}
			else if ((accelerationFlags & MOV_DOWN) && !(accelerationFlags & MOV_UP))
			{
				vAcceleration.y = -1.0f;
			}
			else if ((accelerationFlags & MOV_UP) && (accelerationFlags & MOV_DOWN))
			{
				vVelocity.y = 0.0f;
			}
			
			// x-axis
			if ((accelerationFlags & MOV_LEFT) && !(accelerationFlags & MOV_RIGHT))
			{
				vAcceleration.x = 1.0f;
			}
			else if ((accelerationFlags & MOV_RIGHT) && !(accelerationFlags & MOV_LEFT))
			{
				vAcceleration.x = -1.0f;
			}
			else if ((accelerationFlags & MOV_LEFT) && (accelerationFlags & MOV_RIGHT))
			{
				vVelocity.x = 0.0f;
			}
		}
		{
			vVelocity.setZero();
		}

		vVelocity += (vAcceleration * dt * 10.0f);

		clampf(&vVelocity.x, -fMaxVelocity, fMaxVelocity);
		clampf(&vVelocity.y, -fMaxVelocity, fMaxVelocity);
		clampf(&vVelocity.z, -fMaxVelocity, fMaxVelocity);

		Vec4f vTarget	= { 0.0f, 0.0f, 1.0f };
		vCamera			+= vVelocity.z * vLookDir;		// forward / backward
		vCamera			+= vVelocity.x * vLookDirLeft;	// left / right
		vCamera.y		+= vVelocity.y;					// up / down
		mCameraRotation.MakeRotationX(fPitch);
		mCameraRotation.MakeRotationY(fYaw);
		vLookDir = mCameraRotation * vTarget;
		vLookDirLeft = mCameraRotation * vTargetLeft;
		vTarget = vCamera + vLookDir;
		mCamera.MakePointAt(vCamera, vTarget, vUp);
		mCamera.MakeQuickInverse();
	}

	// Movement
	void moveForward(const bool b = true)	{ b ? accelerationFlags |= MOV_FORWARD	: accelerationFlags &= ~MOV_FORWARD; };
	void moveBackward(const bool b = true)	{ b ? accelerationFlags |= MOV_BACKWARD	: accelerationFlags &= ~MOV_BACKWARD; };
	void moveLeft(const bool b = true)		{ b ? accelerationFlags |= MOV_LEFT		: accelerationFlags &= ~MOV_LEFT; };
	void moveRight(const bool b = true)		{ b ? accelerationFlags |= MOV_RIGHT	: accelerationFlags &= ~MOV_RIGHT; };
	void moveUpward(const bool b = true)	{ b ? accelerationFlags |= MOV_UP		: accelerationFlags &= ~MOV_UP; };
	void moveDownward(const bool b = true)	{ b ? accelerationFlags |= MOV_DOWN		: accelerationFlags &= ~MOV_DOWN; };

	void lookX(const float d)
	{
		fYaw += d * baseTurningSpeed;
	}
	
	void lookY(const float d)
	{
		float maxPitch = 1.5f;

		fPitch -= d * baseTurningSpeed;

		if (fPitch > maxPitch)
		{
			fPitch = maxPitch;
		}
		else if (fPitch < -maxPitch)
		{
			fPitch = -maxPitch;
		}
	}

	const bool isMovingForward()	{ return accelerationFlags & MOV_FORWARD; };
	const bool isMovingBackward()	{ return accelerationFlags & MOV_BACKWARD; };
	const bool isMovingUpward()		{ return accelerationFlags & MOV_UP; };
	const bool isMovingDownward()	{ return accelerationFlags & MOV_DOWN; };
	const bool isMovingLeft()		{ return accelerationFlags & MOV_LEFT; };
	const bool isMovingRight()		{ return accelerationFlags & MOV_RIGHT; };

	const float getYaw() { return fYaw; }
	const float getPitch() { return fPitch; }
	const Vec4f getVelocity() { return vVelocity; };
	const uint8 getAcceleration() { return accelerationFlags; };
	const Vec4f getVCamera() { return vCamera; };
	const Matrix4x4 getMCamera() { return mCamera; };
	const Vec4f getVLookDir() { return vLookDir; }
};