#include <Arduino.h>
#include <FastLED.h>

namespace {

constexpr uint8_t LED_PIN = 6;
constexpr uint16_t LED_COUNT = 60;
constexpr uint8_t ENCODER_CLK_PIN = 2;
constexpr uint8_t ENCODER_DT_PIN = 3;
constexpr uint8_t ENCODER_SW_PIN = 4;
constexpr int8_t ENCODER_DIRECTION = 1;

constexpr uint8_t HUE_STEP = 4;
constexpr uint8_t BRIGHTNESS_STEP = 8;
constexpr uint16_t BUTTON_DEBOUNCE_MS = 25;
constexpr uint16_t DOUBLE_CLICK_MS = 400;

CRGB leds[LED_COUNT];

uint8_t hue = 0;
uint8_t brightness = 128;
bool stripEnabled = true;

uint8_t previousEncoderState = 0;
int8_t encoderAccumulator = 0;

constexpr int8_t ENCODER_TABLE[16] = {
    0, -1, 1, 0,
    1, 0, 0, -1,
    -1, 0, 0, 1,
    0, 1, -1, 0,
};

bool rawButtonState = HIGH;
bool stableButtonState = HIGH;
uint32_t lastButtonChangeTime = 0;
uint32_t firstClickTime = 0;
uint8_t clickCount = 0;
bool rotatedWhilePressed = false;

void updateStrip() {
  if (stripEnabled) {
    fill_solid(leds, LED_COUNT, CHSV(hue, 255, brightness));
  } else {
    fill_solid(leds, LED_COUNT, CRGB::Black);
  }
  FastLED.show();
}

void toggleStrip() {
  stripEnabled = !stripEnabled;
  updateStrip();
}

void handleEncoderStep(int8_t direction) {
  direction *= ENCODER_DIRECTION;

  if (stableButtonState == LOW) {
    const int16_t nextBrightness =
        static_cast<int16_t>(brightness) + direction * BRIGHTNESS_STEP;
    brightness = static_cast<uint8_t>(constrain(nextBrightness, 1, 255));
    rotatedWhilePressed = true;
  } else {
    hue += direction * HUE_STEP;
  }

  updateStrip();
}

void readEncoder() {
  const uint8_t currentState =
      (digitalRead(ENCODER_CLK_PIN) << 1) | digitalRead(ENCODER_DT_PIN);

  if (currentState == previousEncoderState) {
    return;
  }

  const uint8_t transition = (previousEncoderState << 2) | currentState;
  encoderAccumulator += ENCODER_TABLE[transition];
  previousEncoderState = currentState;

  if (encoderAccumulator >= 4) {
    encoderAccumulator = 0;
    handleEncoderStep(1);
  } else if (encoderAccumulator <= -4) {
    encoderAccumulator = 0;
    handleEncoderStep(-1);
  }
}

void registerClick() {
  const uint32_t now = millis();

  if (clickCount == 0) {
    clickCount = 1;
    firstClickTime = now;
  } else if (now - firstClickTime <= DOUBLE_CLICK_MS) {
    clickCount = 0;
    toggleStrip();
  } else {
    clickCount = 1;
    firstClickTime = now;
  }
}

void readButton() {
  const bool reading = digitalRead(ENCODER_SW_PIN);
  const uint32_t now = millis();

  if (reading != rawButtonState) {
    rawButtonState = reading;
    lastButtonChangeTime = now;
  }

  if (now - lastButtonChangeTime < BUTTON_DEBOUNCE_MS) {
    return;
  }

  if (stableButtonState != rawButtonState) {
    stableButtonState = rawButtonState;

    if (stableButtonState == LOW) {
      rotatedWhilePressed = false;
    } else if (!rotatedWhilePressed) {
      registerClick();
    }
  }

  if (clickCount == 1 && now - firstClickTime > DOUBLE_CLICK_MS) {
    clickCount = 0;
  }
}

}  // namespace

void setup() {
  pinMode(ENCODER_CLK_PIN, INPUT_PULLUP);
  pinMode(ENCODER_DT_PIN, INPUT_PULLUP);
  pinMode(ENCODER_SW_PIN, INPUT_PULLUP);

  previousEncoderState =
      (digitalRead(ENCODER_CLK_PIN) << 1) | digitalRead(ENCODER_DT_PIN);
  rawButtonState = digitalRead(ENCODER_SW_PIN);
  stableButtonState = rawButtonState;

  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, LED_COUNT);
  FastLED.setCorrection(TypicalLEDStrip);
  updateStrip();
}

void loop() {
  readEncoder();
  readButton();
}

