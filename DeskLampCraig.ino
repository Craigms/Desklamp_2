/* Alternative firmware for the Mijia Xiaomi Desk Lamp 2 
 *  Uses ESP32 micro - single core version
 *  
 *  See https://github.com/lbernstone/arduino-esp32-solo to setup ESP32 single core libraries
 */

#include <ESP32Encoder.h>

#define ENCODER_PIN_A 26
#define ENCODER_PIN_B 27
#define ENCODER_BUTTON_PIN 33
#define PWM_FREQ 30000
#define ENABLE_PWM_FREQ 20000
#define PWM_RESOLUTION 9 // max value 512 (9)
#define WARM_LED_CHANNEL 0
#define COLD_LED_CHANNEL 1
#define ENABLE_LED_CHANNEL 2

// 10kHz
// 0.287 - 3.3V
// 8.8% to 100%
// min current 2.45mA?
// max current 300mA
// 10 steps (first step has no effect)

// testing with custom code:
// 30mA @ 100%
// goes continuous at 5mA (34/255 = 13%)
// discontinuous under that
// minimum value (4/255= 1.5% = 0.5mA


#define WARMWHITE_PIN 4
#define LED_ENABLE_PIN 12
// voltage range 0.245-2.43V
// 10kHz
// 7.52% - 74% duty
#define COLDWHITE_PIN 2

#define TOTAL_DIM_STEPS 17
#define TOTAL_MIX_STEPS 10

uint32_t warmLookup[] = {8, 9, 10, 12, 15, 25, 35,/*factory values follow*/
                         45, 58, 80, 109, 145, 188, 238, 296, 361, 433, 512
                        };
uint32_t coldLookup[] = {10, 12, 15, 17, 20, 25, 30,/*factory values follow*/
                         38, 48, 64,  85, 112, 144, 181, 213, 271, 323, 382
                        };

int32_t currentWarmColdTenPercent = 10;  // 10 = (100%) warm, 0 = cold
int32_t currentBrightness = 8;  // 0 = min, 14 = max
uint32_t buttonlastPressedTime = 0;
uint32_t powerState = 0; // 0 = off
ESP32Encoder encoder;

void IRAM_ATTR buttonInterrupt() {
  
  // if button just been pressed, start timer
  if(!digitalRead(ENCODER_BUTTON_PIN))
  { 
     if( buttonlastPressedTime == 0)
     {
        buttonlastPressedTime = millis();
     }
  }

  // button released, check if it was a quick press, also debounce 
  else
  { 
    if(digitalRead(ENCODER_BUTTON_PIN))
    {
      if(((millis() - buttonlastPressedTime) < 1000) && ((millis() - buttonlastPressedTime) > 50))
      {
        // this was a 'click'
        powerState = !powerState;
        digitalWrite(LED_ENABLE_PIN, powerState);
      }
      buttonlastPressedTime = 0;  // reset timer ready for next press
    }
  }
}


void setup() {
  Serial.begin(115200);
  Serial.println("hello");
  
  // LED PWM setup
  ledcSetup(WARM_LED_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
  ledcSetup(COLD_LED_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(WARMWHITE_PIN, WARM_LED_CHANNEL);
  ledcAttachPin(COLDWHITE_PIN, COLD_LED_CHANNEL);

  // LED enable pin setup
  pinMode(LED_ENABLE_PIN, OUTPUT);
  digitalWrite(LED_ENABLE_PIN, LOW);

  // Encoder setup: each 'click' = 1 in count change
  ESP32Encoder::useInternalWeakPullResistors = UP;
  encoder.attachHalfQuad(ENCODER_PIN_A, ENCODER_PIN_B);
  encoder.setCount(0);

  // encoder button
  pinMode(ENCODER_BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(ENCODER_BUTTON_PIN, buttonInterrupt, CHANGE);
}

void loop() {
  int32_t countDiff;
  countDiff = encoder.getCount();

  if (!digitalRead(ENCODER_BUTTON_PIN))
  {
    // adjust warm cold ratio
    currentWarmColdTenPercent -= countDiff;
    if (currentWarmColdTenPercent > TOTAL_MIX_STEPS)
    {
      currentWarmColdTenPercent = TOTAL_MIX_STEPS;
    }
    if (currentWarmColdTenPercent < 0)
    {
      currentWarmColdTenPercent = 0;
    }
  }
  else
  { // adjust brightness
    currentBrightness += countDiff;
    if (currentBrightness > TOTAL_DIM_STEPS)
    {
      currentBrightness = TOTAL_DIM_STEPS;
    }
    if (currentBrightness < 0)
    {
      currentBrightness = 0;
    }
  }
  ledcWrite(WARM_LED_CHANNEL, currentWarmColdTenPercent*warmLookup[currentBrightness]/10); 
  ledcWrite(COLD_LED_CHANNEL, (10-currentWarmColdTenPercent)*coldLookup[currentBrightness]/10);

  encoder.setCount(0); // reset the count each time
  
  delay(100);
}
