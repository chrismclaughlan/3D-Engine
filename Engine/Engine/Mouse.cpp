#include "Mouse.h"

bool Mouse::leftIsPressed() const noexcept
{
	return leftPressed;
}
bool Mouse::rightIsPressed() const noexcept
{
	return rightPressed;
}
bool Mouse::isInWindow() const noexcept
{
	return inWindow;
}
int32 Mouse::getPosX() const noexcept
{
	return x;
}
int32 Mouse::getPosY() const noexcept
{
	return y;
}
Mouse::Event Mouse::read() noexcept
{
	if (buffer.size() > 0u)
	{
		Mouse::Event e = buffer.front();
		buffer.pop();
		return e;
	}
	else
	{
		return Mouse::Event();
	}
}

bool Mouse::isEmpty() const noexcept
{
	return buffer.empty();
}

void Mouse::flush() noexcept
{
	buffer = std::queue<Event>();
}

void Mouse::move(int32 newX, int32 newY) noexcept
{
	x = newX;
	y = newY;

	buffer.push(Mouse::Event(Mouse::Event::Type::Move, *this));
	trimBuffer();
}

void Mouse::moveRelative(const int32 newX, const int32 newY) noexcept
{
	x = newX;
	y = newY;

	buffer.push(Mouse::Event(Mouse::Event::Type::MoveRelative, *this));
	trimBuffer();
}

void Mouse::enter() noexcept
{
	inWindow = true;
	buffer.push(Mouse::Event(Mouse::Event::Type::Enter, *this));
	trimBuffer();
}

void Mouse::leave() noexcept
{
	inWindow = false;
	buffer.push(Mouse::Event(Mouse::Event::Type::Leave, *this));
	trimBuffer();
}

void Mouse::leftPress(int32 newX, int32 newY) noexcept
{
	x = newX;
	y = newY;
	leftPressed = true;  //

	buffer.push(Mouse::Event(Mouse::Event::Type::LPressed, *this));
	trimBuffer();
}
void Mouse::leftRelease(int32 newX, int32 newY) noexcept
{
	x = newX;
	y = newY;
	leftPressed = false;  //

	buffer.push(Mouse::Event(Mouse::Event::Type::LReleased, *this));
	trimBuffer();
}
void Mouse::rightPress(int32 newX, int32 newY) noexcept
{
	x = newX;
	y = newY;
	rightPressed = true;  //

	buffer.push(Mouse::Event(Mouse::Event::Type::RPressed, *this));
	trimBuffer();
}
void Mouse::rightRelease(int32 newX, int32 newY) noexcept
{
	x = newX;
	y = newY;
	rightPressed = false;  //

	buffer.push(Mouse::Event(Mouse::Event::Type::RReleased, *this));
	trimBuffer();
}

void Mouse::middlePress(int32 newX, int32 newY) noexcept
{
	x = newX;
	y = newY;

	buffer.push(Mouse::Event(Mouse::Event::Type::MPressed, *this));
	trimBuffer();
}
void Mouse::middleRelease(int32 newX, int32 newY) noexcept
{
	x = newX;
	y = newY;

	buffer.push(Mouse::Event(Mouse::Event::Type::MReleased, *this));
	trimBuffer();
}

void Mouse::wheelUp(int32 newX, int32 newY) noexcept
{
	x = newX;
	y = newY;

	buffer.push(Mouse::Event(Mouse::Event::Type::WheelUp, *this));
	trimBuffer();
}
void Mouse::wheelDown(int32 newX, int32 newY) noexcept
{
	x = newX;
	y = newY;

	buffer.push(Mouse::Event(Mouse::Event::Type::WheelDown, *this));
	trimBuffer();
}
void Mouse::onWheelDelta(int32 newX, int32 newY, int32 delta) noexcept
{
	x = newX;
	y = newY;

	wheelDeltaCarry += delta;
	while (wheelDeltaCarry >= wheelDelta)
	{
		wheelDeltaCarry -= wheelDelta;
		wheelDown(x, y);
	}
	while (wheelDeltaCarry <= -wheelDelta)
	{
		wheelDeltaCarry += wheelDelta;
		wheelUp(x, y);
	}
}
void Mouse::trimBuffer() noexcept
{
	while (buffer.size() > bufferSize)
	{
		buffer.pop();
	}
}