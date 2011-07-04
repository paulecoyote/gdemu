#include <UnitTest++.h>
#include <SPI.h>
#include <GD.h>
#include <bootstrap.h>

SUITE(GameBootstrapTests)
{
  TEST(WhenSetupCalled_SS_PIN_IsHIGH)
  {
    // Arrange
    // Act
    setup();

    // Assert
    uint8_t pin = digitalRead(SS_PIN);
    CHECK_EQUAL(HIGH, pin);
  }
}

////TEST(CheckThatThisTestFileIsActuallyBeingUsed){CHECK(false);}