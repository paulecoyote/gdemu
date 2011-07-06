#include <SPI.h>
#include <GD.h>

#include "landergame.h"

namespace BootstrapGame 
{
  // Entry point to game.
  void Loop()
  {
    int16_t y = 0;

    while(y<6)
    {
      GD.putstr(20,y++,"Hello World! (sigh)");
      delay(500);
    }

    // Allowing exit from loop resets ends GD, then re-runs setup.
  }
} // end namespace BootstrapGame
