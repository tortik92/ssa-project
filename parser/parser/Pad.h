#pragma once

#include <iostream>
#include <string>

class Pad
{
private:
	bool isOccupied;
public:
	Pad() {
		isOccupied = false;
	}
	bool isPadOccupied();
	void say(char* text);
	void play_music(int timeInMs);
	void alarm();
};

