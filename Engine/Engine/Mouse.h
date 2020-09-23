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
			MoveRelative,
			Invalid,
		};
	private:
		Type type;
		bool leftPressed;
		bool rightPressed;
		int x;
		int y;
	public:
		Event() : type(Type::Invalid), leftPressed(false), rightPressed(false), x(-1), y(-1) {}
		Event(Type type, const Mouse& parent) : type(type), leftPressed(parent.leftPressed), 
			rightPressed(parent.rightPressed), x(parent.x), y(parent.y)
		{}
		Type getType() const noexcept { return type; }
		bool isValid() const noexcept { return type != Type::Invalid; }
		bool isLeftPressed() const noexcept { return leftPressed; }
		bool isRightPressed() const noexcept { return rightPressed; }
		int getX() const noexcept { return x; }
		int getY() const noexcept { return y; }
	};

public:
	Mouse() = default;
	Mouse(const Mouse&) = delete;
	Mouse& operator=(const Mouse&) = delete;
	bool leftIsPressed() const noexcept;
	bool rightIsPressed() const noexcept;
	bool isInWindow() const noexcept;
	int getPosX() const noexcept;
	int getPosY() const noexcept;
	Mouse::Event read() noexcept;
	bool isEmpty() const noexcept;
	void flush() noexcept;

private:
	void move(int newX, int newY) noexcept;
	void moveRelative(const int x, const int y) noexcept;
	void enter() noexcept;
	void leave() noexcept;
	void leftPress(int newX, int newY) noexcept;
	void leftRelease(int newX, int newY) noexcept;
	void rightPress(int newX, int newY) noexcept;
	void rightRelease(int newX, int newY) noexcept;
	void middlePress(int newX, int newY) noexcept;
	void middleRelease(int newX, int newY) noexcept;
	void wheelUp(int newX, int newY) noexcept;
	void wheelDown(int newX, int newY) noexcept;
	void onWheelDelta(int newX, int newY, int delta) noexcept;
	void trimBuffer() noexcept;
private:
	static constexpr uint bufferSize = 16;
	static constexpr int wheelDelta = 120;
	int x = -1;
	int y = -1;
	bool inWindow = false;
	bool leftPressed = false;
	bool rightPressed = false;
	int wheelDeltaCarry = 0;
	std::queue<Event> buffer;
};