#include "Blink.h"

//Example on How To Use.

int main() {

	timeBeginPeriod(1);

	while (true) {
		if (GetAsyncKeyState('D') & 0x8000) {
			while (GetAsyncKeyState('D') & 0x8000) std::this_thread::sleep_for(std::chrono::milliseconds(1)); //Used to avoid to run multiple safewalk threads
			runBlink(1, 0, 0);
		}
		else std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}
