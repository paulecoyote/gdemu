// Arduino bootstrapping free functions for game

#include <GD.h>
#include "bootstrap.h"
#include "game.h"

// Setup allows you to prepare the Ardunio and kick off the Gameduinio.
void setup()
{
  // Wait until Gameduino responds properly.
  // Attempt writes until value takes, then return to known default value.
  do
  {
    GD.wr(RAM_SPRIMG, 0xFF);
  } while (GD.rd(RAM_SPRIMG) != 0xFF);

  GD.wr(RAM_SPRIMG, 0x00);

  // Show boot logo for a second
  delay(1000);

  // Prepare Gameduino
  GD.begin();
}

// Main game loop.
void loop()
{
  // Game loop should block until it wants to quit
  Game::Loop();

  // Stop
  GD.end();

  // Restart
  setup();
}