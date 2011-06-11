
#include <gdemu_emulator.h>

void setup();
void loop();

// int __stdcall WinMain(void *, void *, void *, int)
int main(int, char* [])
{
	GDEMU::Emulator.run(setup, loop, 
		GDEMU::EmulatorEnableKeyboard | GDEMU::EmulatorEnableJ1 | GDEMU::EmulatorEnableAudio
		);
	return 0;
}
