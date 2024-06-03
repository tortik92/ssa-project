#pragma once

#include <iostream>
#include <string>

class Pad
{
private:
	bool occupied;
public:
	Pad() {
		occupied = false;
	}

	bool isOccupied();
	void say(char* text);
	void play_music(int timeInMs);
	void alarm();
};

