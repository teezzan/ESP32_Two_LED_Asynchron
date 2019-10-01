/*
   This blinks two LEDs independently and not synchronized. Both have other blink frequencies.
   The blink sketches run in two tasks and on two cores.
*/
#include "MegunoLink.h"
#include "Filter.h"

#define LED1 13
#define LED2 14
#define DAC1 25
#define DAC2 26

TaskHandle_t Task1, Task2;


int counter = 0;
ExponentialFilter<long> ADCFilter(10, 0);

void codeForTask1( void * parameter )
{
  for (;;) {
    int RawValue = analogRead(34);
    ADCFilter.Filter(RawValue);
    TimePlot Plot;
    // xSemaphoreTake( baton, portMAX_DELAY );
    //    blink(LED1, 1000);

    //Plot.SendData("Raw", RawValue);
    //Plot.SendData("Filtered", ADCFilter.Current());
    //  xSemaphoreGive( baton );
    int value = (int(ADCFilter.Current()) * 255) / 4096; // 4096;
    dacWrite(DAC1, value);
    delay(50);
    Serial.println("Task 1: " + (String)value);
  }
}

void codeForTask2( void * parameter )
{ int ramp = 0;
  int  val[10];
  int sum = 0;
  for (;;) {
//    for (int i = 0; i < 10; i++)
//      sum += analogRead(32);
//    sum = sum / 10;
//    dacWrite(DAC2, sum);
//    sum=0;
        if (ramp != 255) {
          ramp++;
        }
        else {
          ramp = 0;
        }
    
    
        dacWrite(DAC2,ramp + random(-30,30));
        delay(100);
        Serial.println(ramp + random(-30,30));
        Serial.println("             Task 2: ");
  }
}

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(115200);
  // initialize digital pin LED_BUILTIN as an output.

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);

  xTaskCreatePinnedToCore(
    codeForTask1,
    "led1Task",
    1000,
    NULL,
    1,
    &Task1,
    0);
  delay(500);  // needed to start-up task1

  xTaskCreatePinnedToCore(
    codeForTask2,
    "led2Task",
    1000,
    NULL,
    1,
    &Task2,
    1);

}

void loop() {
  delay(1000);
}

double ReadVoltage(byte pin) {
  double reading = analogRead(pin); // Reference voltage is 3v3 so maximum reading is 3v3 = 4095 in range 0 to 4095
  if (reading < 1 || reading > 4095) return 0;
  // return -0.000000000009824 * pow(reading,3) + 0.000000016557283 * pow(reading,2) + 0.000854596860691 * reading + 0.065440348345433;
  return -0.000000000000016 * pow(reading, 4) + 0.000000000118171 * pow(reading, 3) - 0.000000301211691 * pow(reading, 2) + 0.001109019271794 * reading + 0.034143524634089;
} // Added an improved polynomial, use either, comment out as required
