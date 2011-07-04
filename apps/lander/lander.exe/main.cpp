#include <gdemu_emulator.h>
#include <bootstrap.h>

int main(int, char* [])
{
	GDEMU::Emulator.run(setup, loop, 
		GDEMU::EmulatorEnableKeyboard | GDEMU::EmulatorEnableJ1 | GDEMU::EmulatorEnableAudio
		);
	return 0;
}
