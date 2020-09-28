#pragma once
#include "Engine/utils_vector.h"
#include <ostream>
#include <stack>
#include <typeinfo>

#define MOV_FORWARD		(0b000001)
#define MOV_BACKWARD	(0b000010)
#define MOV_FB			(0b000011)
#define MOV_LEFT		(0b000100)
#define MOV_RIGHT		(0b001000)
#define MOV_LR			(0b001100)
#define MOV_UP			(0b010000)
#define MOV_DOWN		(0b100000)
#define MOV_UD			(0b110000)


enum class PlayerActions : uint8
{
	Invalid, Place, Remove,
};

class InventorySlot
{
private:
	static const int maxSize = 64;
	std::stack<Object*> sObjects;

public:
	InventorySlot(Object* o)
	{
		sObjects.push(o);
	}
	~InventorySlot()
	{
		while (sObjects.size() > 0)
		{
			Object* o = sObjects.top();
			if (o != nullptr)
			{
				delete o;
				o = nullptr;
			}
			sObjects.pop();
		}
	}

	bool push(Object* o)
	{
		if (sObjects.size() == maxSize)
		{
			return false;
		}

		sObjects.push(o);
	}
	/**
	 * \brief Returns top of objects stack or nullptr if empty.
	 * 
	 * \return 
	 */
	Object* pop()
	{
		if (sObjects.empty())
		{
			return nullptr;
		}

		Object* o;
		o = sObjects.top();
		sObjects.pop();

		return o;
	}

	int getSize() { return sObjects.size(); }

	Object* top()
	{
		if (sObjects.empty())
		{
			return nullptr;
		}

		return sObjects.top();
	}

	friend std::ostream& operator<<(std::ostream& os, const InventorySlot& slot)
	{
		if (slot.sObjects.empty())
		{
			os << "Empty";
		}
		else
		{
			os << slot.sObjects.top()->name << "<" << slot.sObjects.size() << "/" << maxSize << ">";
		}

		return os;
	}
};


class Inventory
{
private:
	std::vector<InventorySlot*> slots;

	/**
	 * \brief Translate slot number to vector index.
	 */
	inline InventorySlot* getFromSlot(const int slot) const
	{
		assert((slot >= 1) && (slot <= maxSlots));
		return slots[slot - 1];
	}

public:
	static const int maxSlots = 10;
	int currentSlot = 1;  ///< Controls which item gets popped when placing

	Inventory()
	{
		slots.reserve(maxSlots);
	}

	~Inventory()
	{
		for (auto s : slots)
		{
			delete s;
			s = nullptr;
		}
		slots.clear();
	}

	void reset()
	{
		for (auto s : slots)
		{
			delete s;
			s = nullptr;
		}
		slots.clear();

		currentSlot = 1;
	}

	bool push(Object* o)
	{
		if (slots.size() == maxSlots)
		{
			return false;
		}

		// Check for same type in slots and try to push there
		for (auto s : slots)
		{
			// Check object type
			//if (typeid(o) == typeid(s->top()))

			// Check if slot occupied
			if (s->top() != nullptr)
			{
				if (o->name == s->top()->name)
				{
					if (s->push(o))
					{
						return true;
					}
				}
			}
		}

		// If no valid slot with same type try fill in any existing slots
		for (auto s : slots)
		{
			if (s->top() == nullptr)
			{
				if (s->push(o))
				{
					return true;
				}
			}
		}

		// Valid slot not found, try create new slot
		if (slots.size() < maxSlots)
		{
			slots.push_back(new InventorySlot(o));
			return true;
		}

		// Could not find or create a valid slot (inventory should be full)
		return false;
	}

	/**
	 * \brief Returns object in inventory index if occupied, else returns nullptr.
	 * 
	 * \param index
	 * \return 
	 */
	Object* pop(int slotN)
	{
		if (slotN > slots.size() || slots.empty())
		{
			return nullptr;
		}

		return getFromSlot(slotN)->pop();
	}

	friend std::ostream& operator<<(std::ostream& os, const Inventory& inv)
	{
		os << "Inventory:\n";
		for (std::size_t i = 1; i <= inv.slots.size(); ++i)
		{
			os << "Slot[" << i << "]: " << *inv.getFromSlot(i) << ", ";
		}

		return os;
	}

	void indexIncr()
	{
		if (++currentSlot > maxSlots)
		{
			currentSlot = maxSlots;
		}
	}

	void indexDecr()
	{
		if (--currentSlot < 1)
		{
			currentSlot = 1;
		}
	}
};


class Player
{
public:
	PlayerActions action = PlayerActions::Invalid;
	Inventory inventory;
	ObjectHit objectVisable;
	bool isLookingAtObject = false;

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

	void updateMovement(const float dt)
	{
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

		updateVelocity(vAcceleration * dt * 10.0f);

		clampf(&vVelocity.x, -fMaxVelocity, fMaxVelocity);
		clampf(&vVelocity.y, -fMaxVelocity, fMaxVelocity);
		clampf(&vVelocity.z, -fMaxVelocity, fMaxVelocity);
	}

	void updateVelocity(Vec4f v)
	{
		vVelocity += v;
	}

	void updatePhysics(const float dt)
	{
		Vec4f vGravity = { 0.0f, -1.0f, 0.0f };
		updateVelocity(vGravity * dt * 9.8f);
	}

public:
	Player()
		: vUp(0.0f, 1.0f, 0.0f)
	{
	}

	void resetPosition()
	{
		vCamera.setZero();
		vLookDir.setZero();
		vLookDirLeft.setZero();
		vForward.setZero();
		vLeft.setZero();
		mCameraRotation.setZero();
		mCamera.setZero();
		vVelocity.setZero();
		vAcceleration.setZero();
		accelerationFlags = 0b0;
	}

	void reset()
	{
		resetPosition();
		inventory.reset();
	}

	void updatePosition(const float dt)
	{
		updateMovement(dt);
		//updatePhysics(dt);

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