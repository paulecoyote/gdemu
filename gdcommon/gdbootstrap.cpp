// Arduino bootstrapping free functions for game

#include <SPI.h>
#include <GD.h>
#include "gdbootstrap.h"

namespace BootstrapGame
{
  // Decalre game blocking game loop, exiting restarts.
  void Loop(void);
}

// Setup allows you to prepare the Ardunio and kick off the Gameduinio.
void setup()
{
  Serial.begin(115200);

  // Wait until Gameduino responds properly.
  // Attempt writes until value takes, then return to known default value.
  do
  {
    GD.wr(RAM_SPRIMG, 0xFF);
  } while (GD.rd(RAM_SPRIMG) != 0xFF);

  GD.wr(RAM_SPRIMG, 0x00);

  // Prepare Gameduino
  GD.begin();

  // Show boot logo for a second
  delay(1000);
}

// Main game loop.
void loop()
{
  // Game loop should block until it wants to quit
  BootstrapGame::Loop();

  // Stop
  GD.end();
  Serial.end();

  // Restart
  setup();
}
