## gdemu - Gameduino Emulator
An emulator for the retro-gaming [Gameduino][0] hardware.

Intended to make development and debugging easier. Just compile your game's code together with the emulator library, and run and debug directly on your PC.

Get the original source here: http://code.google.com/p/gdemu/source/checkout

First home on github here (case important in url): http://github.com/PaulECoyote/gdemu

## Setup
You need to have the [DirectX SDK][DX_SDK] installed.  
Make sure that the VC++ directories for includes and libraries includes "$(DXSDK_DIR)include" and "$(DXSDK_DIR)lib\x86".  
In Visual Studio 2010 these directories should be set in Microsoft.Cpp.Win32.user property page.

## Links
### Gamedunio 
* http://excamera.com/sphinx/gameduino/
* http://twitter.com/#!/gameduino
* http://answers.gameduino.com/questions/

### Gameduino Tutorials
* http://www.artlum.com/gameduino/gameduino.html

## Credit

James Bowman - [Creator of the Gameduino][0]

Jan Boon - Original author - http://www.kaetemi.be/ + http://twitter.com/#!/kaetemi

### Contributors
* Paul Evans - http://paulecoyote.com + http://twitter.com/#!/paulecoyote

License
-------
* Core emulator libraries [GNU GPL v2][10]
* Original author [gave][7] [permission][7b] for games to be included but use a different licence MIT/BSD/etc where GPL is incompatible.

Changelog
---------
Versions only really make sense for this fork.

* V0.1.0 Imported in to github from google code, added UnitTest++ as submodule.  Solution compiles with no warnings.
* V0.1.1 Provided Visual Studio 2010 solutions.
* V0.1.2 Fixed Visual Studio 2010 solutions and added Arduino projects.
* V0.1.3 Added lander sprite, display the different ships.  Made logo appear on purpose for 3 secs (not emu).

[0]: http://excamera.com/sphinx/gameduino/
[1]: http://code.google.com/p/gdemu/source/checkout
[2]: http://www.kaetemi.be/
[3]: http://twitter.com/#!/kaetemi
[4]: http://twitter.com/#!/paulecoyote
[5]: http://paulecoyote.com
[6]: http://twitter.com/#!/gameduino
[7]: http://twitter.com/#!/kaetemi/status/84115376404836352
[7b]: http://twitter.com/#!/paulecoyote/status/84028090866601984
[8]: http://answers.gameduino.com/questions/
[9]: http://www.artlum.com/gameduino/gameduino.html
[10]: http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
[11]: https://github.com/PaulECoyote/gdemu
[DX_SDK]: http://www.microsoft.com/download/en/details.aspx?displaylang=en&id=6812