////#define DEBUG_SERIAL_MSGS 
#include <SPI.h>
#include <GD.h>

#include "landergame.h"
#include "podsprite.h"
#include "gdbootstrap.h"

using namespace Sprite;

namespace Lander
{
  bool LanderGame::Play(void)
  {
#ifdef DEBUG_SERIAL_MSGS
    Serial.println("Entered BootstrapGame::Loop");
#endif
    short y = 0;
    GD.copy(PALETTE16A, PodSprite_sprpal, sizeof(PodSprite_sprpal));
    GD.copy(RAM_SPRIMG, PodSprite_sprimg, sizeof(PodSprite_sprimg));

    // For show, randomly scatter the frames on the screen
    GD.__wstartspr(0);
    byte offset_y = 0;
    for (byte anim = 0; anim < PODSPRITE_FRAMES; anim++)
    {
      draw_PodSprite(200, 150 + offset_y, anim, 0);
      offset_y+=16;
    }

    GD.__end();

    GD.putstr(20, y++, "Cluck like a chicken!");
    while(y<6)
    {
      GD.putstr(20, y++, "Bakka!");
      delay(500);
    }

    // Allowing exit from loop resets ends GD, then re-runs setup.
    return false;
  }

  LanderGame game;
} // end namespace BootstrapGame

void BootstrapGame::loop(void)
{
  while (Lander::game.Play());
}
