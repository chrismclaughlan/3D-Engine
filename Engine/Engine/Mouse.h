#pragma once
#include "types.h"
#include <queue>

class Mouse
{
	// TODO error handling

	friend class Window;

public:
	class Event
	{
	public:
		enum class Type
		{
			LPressed, LReleased,
			RPressed, RReleased,
			MPressed, MReleased,
			WheelUp, WheelDown,
			Move, Enter, Leave,
			Invalid,
		};
	private:
		Type type;
		bool leftPressed;
		bool rightPressed;
		int32 x;
		int32 y;
	public:
		Event() : type(Type::Invalid), leftPressed(false), rightPressed(false), x(-1), y(-1) {}
		Event(Type type, const Mouse& parent) : type(type), leftPressed(parent.leftPressed), 
			rightPressed(parent.rightPressed), x(parent.x), y(parent.y)
		{}
		Type getType() const noexcept { return type; }
		bool isValid() const noexcept { return type != Type::Invalid; }
		bool isLeftPressed() const noexcept { return leftPressed; }
		bool isRightPressed() const noexcept { return rightPressed; }
		int32 getX() const noexcept { return x; }
		int32 getY() const noexcept { return y; }
	};

public:
	Mouse() = default;
	Mouse(const Mouse&) = delete;
	Mouse& operator=(const Mouse&) = delete;
	bool leftIsPressed() const noexcept;
	bool rightIsPressed() const noexcept;
	bool isInWindow() const noexcept;
	int32 getPosX() const noexcept;
	int32 getPosY() const noexcept;
	Mouse::Event read() noexcept;
	bool isEmpty() const noexcept;
	void flush() noexcept;

private:
	void move(int32 newX, int32 newY) noexcept;
	void enter() noexcept;
	void leave() noexcept;
	void leftPress(int32 newX, int32 newY) noexcept;
	void leftRelease(int32 newX, int32 newY) noexcept;
	void rightPress(int32 newX, int32 newY) noexcept;
	void rightRelease(int32 newX, int32 newY) noexcept;
	void middlePress(int32 newX, int32 newY) noexcept;
	void middleRelease(int32 newX, int32 newY) noexcept;
	void wheelUp(int32 newX, int32 newY) noexcept;
	void wheelDown(int32 newX, int32 newY) noexcept;
	void onWheelDelta(int32 newX, int32 newY, int32 delta) noexcept;
	void trimBuffer() noexcept;
private:
	static constexpr uint32 bufferSize = 16;
	static constexpr int32 wheelDelta = 120;
	int32 x = -1;
	int32 y = -1;
	bool inWindow = false;
	bool leftPressed = false;
	bool rightPressed = false;
	int32 wheelDeltaCarry = 0;
	std::queue<Event> buffer;
};