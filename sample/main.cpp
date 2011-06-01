
#include <gdemu_emulator.h>

void setup();
void loop();

// int __stdcall WinMain(void *, void *, void *, int)
int main(int, char* [])
{
	GDEMU::Emulator.run(setup, loop, true);
	return 0;
}
