#include <UnitTest++.h>
#include <SPI.h>
#include <GD.h>
#include <gdbootstrap.h>

const byte EXPECTED_SPI_CLOCK = SPI_CLOCK_DIV2;
const int16_t EXPECTED_SPRPAL_LENGTH = 2048;
const int16_t EXPECTED_SPR_LENGTH = 512 * 4;
const int16_t EXPECTED_SPRITES = 512;
const int16_t EXPECTED_VOICES_LENGTH = 256;
const int16_t EXPECTED_PALETTE16A_LENGTH = 128;
const uint16_t SPRITE_HIDDEN_VAL = 400U;
const uint16_t SPRITE_HIDDEN_LOW_BYTE = lowByte(400U);
const uint16_t SPRITE_HIDDEN_HIGH_BYTE = highByte(400U); 

// NOT tested because couldn't see it exposed:
// * ClockDivider speed
// * BitOrder
// * DataMode
// * If Serial is initialised

struct SetupFixture
{
  SetupFixture() 
  {
    originalEnableResetLogoDelay = BootstrapGame::enableResetLogoDelay;

    // Arrange
    BootstrapGame::enableResetLogoDelay = false;
     
    // Act
    setup();
  };

  ~SetupFixture()
  {
    // Restore
    BootstrapGame::enableResetLogoDelay = originalEnableResetLogoDelay;
  }

  private: 
    bool originalEnableResetLogoDelay;
};


SUITE(GameBootstrapTests)
{
  TEST_FIXTURE(SetupFixture, WhenSetupCalled_BG_COLOR_IsZero)
  {
    // Assert
    uint16_t read = GD.rd16(BG_COLOR);
    CHECK_EQUAL((int)0, (int)read);
  }

  TEST_FIXTURE(SetupFixture, WhenSetupCalled_IOMODE_IsZero)
  {
    // Assert
    byte read = GD.rd(IOMODE);
    CHECK_EQUAL((int)0, (int)read);
  }

  TEST_FIXTURE(SetupFixture, WhenSetupCalled_JK_MODE_IsZero)
  {
    // Assert
    byte read = GD.rd(JK_MODE);
    CHECK_EQUAL((int)0, (int)read);
  }

  TEST_FIXTURE(SetupFixture, WhenSetupCalled_MODULATED_IsZero)
  {
    // Assert
    byte read = GD.rd(MODULATED);
    CHECK_EQUAL((int)0, (int)read);
  }

  TEST_FIXTURE(SetupFixture, WhenSetupCalled_Palette16aIsBlack)
  {
    // Arrange
    int16_t i=0;
    byte transferred = 0;

    // Assert
    GD.__start(PALETTE16A);
    for (i=0; i < EXPECTED_PALETTE16A_LENGTH; ++i) {
      transferred = SPI.transfer(0);
      if (transferred != 0) {
        break;
      }
    }
    GD.__end();

    CHECK_EQUAL((int)EXPECTED_PALETTE16A_LENGTH, (int)i);
  }

  TEST_FIXTURE(SetupFixture, WhenSetupCalled_SAMPLE_L_IsZero)
  {
    // Assert
    uint16_t read = GD.rd16(SAMPLE_L);
    CHECK_EQUAL((int)0, (int)read);
  }

  TEST_FIXTURE(SetupFixture, WhenSetupCalled_SAMPLE_R_IsZero)
  {
    // Assert
    uint16_t read = GD.rd16(SAMPLE_R);
    CHECK_EQUAL((int)0, (int)read);
  }

  TEST_FIXTURE(SetupFixture, WhenSetupCalled_SCREENSHOT_Y_IsZero)
  {
    // Assert
    uint16_t read = GD.rd16(SCREENSHOT_Y);
    CHECK_EQUAL((int)0, (int)read);
  }

  TEST_FIXTURE(SetupFixture, WhenSetupCalled_SCROLL_X_IsZero)
  {
    // Assert
    uint16_t read = GD.rd16(SCROLL_X);
    CHECK_EQUAL((int)0, (int)read);
  }
  
  TEST_FIXTURE(SetupFixture, WhenSetupCalled_SCROLL_Y_IsZero)
  {
    // Assert
    uint16_t read = GD.rd16(SCROLL_Y);
    CHECK_EQUAL((int)0, (int)read);
  }

  TEST_FIXTURE(SetupFixture, WhenSetupCalled_SPR_DISABLE_IsZero)
  {
    // Assert
    byte read = GD.rd(SPR_DISABLE);
    CHECK_EQUAL((int)0, (int)read);
  }

  TEST_FIXTURE(SetupFixture, WhenSetupCalled_SPR_PAGE_IsZero)
  {
    // Assert
    byte read = GD.rd(SPR_PAGE);
    CHECK_EQUAL((int)0, (int)read);
  }

  TEST_FIXTURE(SetupFixture, WhenSetupCalled_SpriteDataIsHidden)
  {
    // Arrange
    int16_t sprite;
    uint16_t transferred;
    uint16_t low_byte, high_byte;
    
    // Assert
    for (sprite=0; sprite < 512; ++sprite)  {
      // For every sprite, check that it has been hidden
      // Sprite mem start + offset for each sprite boundary
      GD.__start(RAM_SPR + (sprite << 2));
      transferred = SPI.transfer(0);
      GD.__end();
      low_byte = lowByte(transferred);
      high_byte = highByte(transferred);
      if (SPRITE_HIDDEN_LOW_BYTE != low_byte && SPRITE_HIDDEN_HIGH_BYTE != high_byte) {
        break;
      }
    }

    CHECK_EQUAL((int)EXPECTED_SPRITES, (int)sprite);
  }

  TEST_FIXTURE(SetupFixture, WhenSetupCalled_SpritePalletesAreBlack)
  {
    int i;

    // Assert
    GD.__start(RAM_SPRPAL);
    for (i=0; i < EXPECTED_SPRPAL_LENGTH; ++i) {
      if (SPI.transfer(0) != 0) {
        break;
      }
    }
    GD.__end();

    CHECK_EQUAL((int)EXPECTED_SPRPAL_LENGTH, (int)i);
  }

  TEST_FIXTURE(SetupFixture, WhenSetupCalled_SS_PIN_IsHIGH)
  {
    // Assert
    uint8_t pin = digitalRead(SS_PIN);
    CHECK_EQUAL((int)HIGH, (int)pin);
  }

  TEST_FIXTURE(SetupFixture, WhenSetupCalled_UNMODULATED_IsZero)
  {
    // Assert
    byte read = GD.rd(UNMODULATED);
    CHECK_EQUAL((int)0, (int)read);
  }

  TEST_FIXTURE(SetupFixture, WhenSetupCalled_VoicesAreSilent)
  {
    // Arrange
    int16_t i=0;
    byte transferred = 0;

    // Assert
    GD.__start(VOICES);
    for (i=0; i < EXPECTED_VOICES_LENGTH; ++i) {
      transferred = SPI.transfer(0);
      if (transferred != 0) {
        break;
      }
    }
    GD.__end();

    CHECK_EQUAL((int)EXPECTED_VOICES_LENGTH, (int)i);
  }
}

////TEST(CheckThatThisTestFileIsActuallyBeingUsed){CHECK(false);}