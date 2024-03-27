#include <string>

#include "Initializer.h"

int main()
{
	Initializer initializer;
	Initializer::msg_struct msg;

	const char* signature = "SDLP\0";
	strncpy_s(msg.signature, SIGNATURE_LENGTH + 1, signature, SIGNATURE_LENGTH);
	msg.padsCount = 6;
	strcpy_s(msg.code, CODE_BUFFER_LENGTH, "# Dies ist ein Kommentar\nsay(REISE NACH JERUSALEM)\nwait(5000)\nif(ACTIVE_COUNT!>0)5else15\nplay_music(random(5000, 30000))\nalarm()\nwait_until_pads_occupied(ACTIVE_COUNT)\nsay(ALLE MATTEN WURDEN BESETZT)\nwait(5000)\nsay(ALLE VON DEN MATTEN RUNTER)\nwait(5000)\nsay(NÄCHSTE RUNDE)\ndeactivate(random(ACTIVE_COUNT))\ngoto(4)\nsay(Spiel ist zu Ende)\n\0");
	const unsigned int mac_address = 0x8bcff4567b2e;

	try {
		initializer.onDataRecv(&mac_address, msg.code, sizeof(msg.code));
	}
	catch (const std::exception &e) {
		std::cout << e.what() << "\n";
	}
}