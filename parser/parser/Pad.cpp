#include "Pad.h"

bool Pad::isPadOccupied() {
	return isOccupied;
}

void Pad::say(std::string text)
{
	std::cout << text;
}

void Pad::play_music(float timeInMs)
{
	std::cout << "Playing music for " << timeInMs << " ms";
}

void Pad::alarm()
{
	std::cout << "alarm";
}
