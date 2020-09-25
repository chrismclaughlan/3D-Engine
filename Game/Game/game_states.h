#include "game.h"


/* Game State Management Methods */

/**
 * \brief Pushes a game function and a corresponding flag to the
 * sGameStates stack.
 *
 * \param func Pointer to Game function.
 * \param flag Flag determining the behaviour the function should inherit.
 * \return Returns true if function pushed to stack.
 */
const bool Game::gsPush(void (Game::* func)(int), int flag)
{
	if (sGameStates.size() >= MAX_GAME_STATE_SIZE)
	{
		return false;
	}

	sGameStates.push({ func, flag });
	win.keyboard.flush();
	win.mouse.flush();

	return true;
}

/**
 * \brief Only pops function from sGameStates if it's not empty.
 *
 * \return Returns true if function popped from stack.
 */
const bool Game::gsSafePop()
{
	if (sGameStates.empty())
	{
		return false;
	}

	sGameStates.pop();
	return true;
}

/**
 * \brief Tries to pop function from sGameStates.
 *
 * If successfully popped, the keyboard and mouse inputs
 * are flushed to ensure fresh input for the new function.
 *
 * \return Returns true if function popped from sGameStates
 */
const bool Game::gsPop()
{
	if (!gsSafePop())
	{
		return false;
	}

	win.keyboard.flush();
	win.mouse.flush();
	return true;
}

/**
 * \brief Pops game functions from sGameStates until it encounters the given
 * function.
 *
 * If the intended element in the stack is hit, the keyboard and mouse inputs
 * are flushed to ensure fresh input for the new function. The current flag
 * corresponding to the function is then updated (default: FLAG_INIT so that the
 * new game state can initialise itself).
 *
 * If the intended element in the stack is not hit, an error message in cerr
 * is written and false is returned.
 *
 * \important The game states popped from this function will be called with
 * FLAG_DESTROY before being popped to let the function cleanup.
 *
 * \param func Pointer to Game function to pop until.
 * \param flag Flag to pass on to the intended game function (default: FLAG_INIT).
 */
const bool Game::gsPopUntil(void (Game::* func)(int), const int flag = FLAG_INIT)
{
	bool hit = false;

	if (sGameStates.empty())
	{
		std::cerr << "Error popping game states -> Stack empty\n";
		return false;
	}

	while (true)
	{
		std::pair<void (Game::*)(int), int> current = sGameStates.top();
		void (Game:: * currentState)(int) = current.first;
		int currentFlag = current.second;

		// Check if hit intended function
		if (current.first == func)
		{
			gsUpdateCurrentFlag(flag);  // init gs again

			win.keyboard.flush();
			win.mouse.flush();
			hit = true;
			break;
		}

		// Cleanup of top stack function
		(this->*currentState)(FLAG_DESTROY);  // execute method

		// Pop top stack function
		if (!gsSafePop())
		{
			std::cerr << "Error popping game states -> End of stack hit: "\
				"cannot hit intended element\n";
			break;
		}
	}

	return hit;
}

/**
 * \brief Replaces the flag for the top stack element.
 *
 * Replaces the top stack element with a new one with the same function
 * pointer but different flag.
 *
 * \param flag Flag to replace with.
 * \return Returns false if stack empty.
 */
const bool Game::gsUpdateCurrentFlag(const int flag)
{
	if (sGameStates.empty())
	{
		return false;
	}

	// Get current game state
	std::pair<void (Game::*)(int), int> current = sGameStates.top();
	void (Game:: * currentState)(int) = current.first;

	sGameStates.pop();
	sGameStates.push({ currentState, flag });

	return true;
}
