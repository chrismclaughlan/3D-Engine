#pragma once
#include "types.h"
#include <queue>
#include <bitset>

class Keyboard
{
	// TODO error handling

	friend class Window;

public:
	class Event
	{
	public:
		enum class Type
		{
			Press, Release, Invalid,
		};
	private:
		Type type;
		uint8 code;
	public:
		Event() : type(Type::Invalid), code(0) {}
		Event(Type type, uint8 code) noexcept : type(type), code(code) {}
		bool isValid() const noexcept { return type != Type::Invalid; }
		bool isPressed() const noexcept { return type == Type::Press; }
		bool isReleased() const noexcept { return type == Type::Release; }
		uint8 getCode() const noexcept { return code; }
	};

public:
	Keyboard() = default;
	Keyboard(const Keyboard&) = delete;
	Keyboard& operator=(const Keyboard&) = delete;

	Keyboard::Event readKey() noexcept;
	bool keyIsPressed(uint8 keycode) const noexcept;
	bool keyIsEmpty() const noexcept;

	uint8 readChar() noexcept;
	bool charIsEmpty() const noexcept;

	void flushKey() noexcept;
	void flushChar() noexcept;
	void flush() noexcept;

	// Autorepeat control
	void enableAutorepeat() noexcept;
	void disableAutorepeat() noexcept;
	bool autorepeatIsEnabled() const noexcept;
private:
	void keyPressed(uint8 keycode) noexcept;
	void keyReleased(uint8 keycode) noexcept;
	void onChar(uint8 character) noexcept;

	void clearState() noexcept;
	template<typename T>
	static void trimBuffer(std::queue<T>& buffer) noexcept;
private:
	static constexpr uint32 numKeys = 256;
	static constexpr uint32 bufferSize = 16;
	bool autorepeatEnabled = false;
	std::bitset<numKeys> keyStates;
	std::queue<Event> keyBuffer;
	std::queue<uint8> charBuffer;
};