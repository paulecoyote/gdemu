// Arduino bootstrapping free functions for game
#define DEBUG_SERIAL_MSGS 
// #define ENABLE_LOGO_DELAY // Slows down compile time if unit tests being run against setup.

#include <SPI.h>
#include <GD.h>
#include "gdbootstrap.h"
#include "reload_code.h"

void reset();

using namespace Forth;

const uint32_t LOGO_DELAY_LENGTH = 4000;
const long SERIAL_BAUD_RATE = 115200;

// Main game loop.
void loop()
{
  // Game loop should block until it wants to quit
  BootstrapGame::Loop();

  // Stop
  GD.end();
  Serial.end();

  // Restart
  reset();
}

void reset()
{
  // Controlled show of logo
  GD.microcode(reload_code, sizeof(reload_code));
#ifdef ENABLE_LOGO_DELAY
  delay(LOGO_DELAY_LENGTH);
#endif
  // Clear down again, this time no need to wait.
  // (Gameduino is already initialised)
  GD.begin();
  //GD.ascii();
}

// Setup allows you to prepare the Ardunio and kick off the Gameduinio.
void setup()
{
  Serial.begin(SERIAL_BAUD_RATE);
  
  // Prepare Gameduino (SPI, clearing ram, etc)
  // Initializes the SPI bus, setting SCK, MOSI, and SS to outputs, pulling SCK and MOSI low and SS high.
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

  reset();
}
