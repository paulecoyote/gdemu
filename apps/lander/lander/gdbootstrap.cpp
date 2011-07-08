// Arduino bootstrapping free functions for game
#define DEBUG_SERIAL_MSGS 

#include <SPI.h>
#include <GD.h>
#include "gdbootstrap.h"
#include "reload_code.h"

namespace BootstrapGame
{
  // Decalre game blocking game loop, exiting restarts.
  void Loop(void);
}

// Setup allows you to prepare the Ardunio and kick off the Gameduinio.
void setup()
{
  Serial.begin(115200);
  
  // Prepare Gameduino (SPI, clearing ram, etc)
  GD.begin();
  
  // Wait until Gameduino responds properly.
  // Attempt writes until value takes, then return to known default value.
#ifdef DEBUG_SERIAL_MSGS
  Serial.println("Before wait for Gameduino loop");
#endif
  do
  {
    GD.wr(RAM_SPRIMG, 0xFF);
  } while (GD.rd(RAM_SPRIMG) != 0xFF);

  GD.wr(RAM_SPRIMG, 0x00);

#ifdef DEBUG_SERIAL_MSGS
  Serial.println("After wait for Gameduino loop");
#endif

  // Controlled show of logo
  GD.microcode(reload_code, sizeof(reload_code));
  delay(5000);
  // Clear down again, this time no need to wait.
  // (Gameduino is already initialised)
  GD.begin();
  GD.ascii();
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
