#include "Initializer.h"

int main()
{
	Initializer initializer;
	const unsigned int mac_address = 0x8bcff4567b2e;

	initializer.onDataRecv(&mac_address, 
		"53444C506\n# Dies ist ein Kommentar\nsay(\"REISE NACH JERUSALEM\")\nwait(5000)\nif (ACTIVE_COUNT > 0) 05 else 15\nplay_music(random(5000, 30000))\nalarm()\nwait_until_pads_occupied(ACTIVE_COUNT)\nsay(\"ALLE MATTEN WURDEN BESETZT\")\wait(5000)\say(\"ALLE VON DEN MATTEN RUNTER\")\wait(5000)\say(\"NÄCHSTE RUNDE\")\deactivate(random(ACTIVE_COUNT))\goto(04)\say(\"Spiel ist zu Ende\")\n", 
		378);
}