#include "Pad.h"

bool Pad::isPadOccupied() {
	return isOccupied;
}

void Pad::say(std::string text)
{
	std::cout << text << "\n";
}

void Pad::play_music(int timeInMs)
{
	std::cout << "Playing music for " << timeInMs << " ms\n";
}

void Pad::alarm()
{
	std::cout << "alarm" << "\n";
}
