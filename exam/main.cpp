#include "Game.h"

int main(int argc, char** argv)
{
	Game game;

	if (game.init() != 0)
	{
		std::cerr << "Failed to initialize application" << std::endl;
		return 1;
	}

	// Run the main loop
	unsigned result = game.run();

	// Shutdown is called automatically in destructor
	std::cout << "Application finished with code: " << result << std::endl;

	return result;
}