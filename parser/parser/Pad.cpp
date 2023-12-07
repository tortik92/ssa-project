#include "Pad.h"

bool Pad::isPadOccupied() {
	return isOccupied;
}

void Pad::say(std::string text)
{
	throw std::exception("Not implemented");
}

void Pad::play_music(int timeInMs)
{
	throw std::exception("Not implemented");
}

void Pad::alarm()
{
	throw std::exception("Not implemented");
}
