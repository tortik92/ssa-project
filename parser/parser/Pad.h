#pragma once

#include <string>

class Pad
{
private:
	float frequency; // at which to play alarm sound (to identify mat)
public:
	void say(std::string text);
	void play_music(int timeInMs);
	void alarm();
};

