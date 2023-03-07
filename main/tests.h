#include "configuration.h"
#pragma once

// Przekazuje dane o mierzonej temperaturze przez port szeregowy
void testTemperatureSensors();
// Przekazuje dane o mierzonej sile nacisku przez port szeregowy
void testForceSensor();
// Ustawia na wejściu wzmacniacza zadaną wartość napięcia (w poziomach DAC)
void testAmplifier(short unsigned int pin_number, short unsigned int pin_output, short unsigned int signal_value);
// Przekazuje dane o różnicy czasu w danej chwili od podanej wartości
void testTimeSynchronization(long unsigned int timer);

void testTemperatureSensors()
{
  Serial.print(analogRead(PIN_TEMPERATURE_SENSOR_TOP));
  Serial.print(',');
  Serial.print(analogRead(PIN_TEMPERATURE_SENSOR_BOT));
  Serial.println();
}

void testForceSensor()
{
  //// Niefiltrowane
  //Serial.print(analogRead(PIN_FORCE_SENSOR));
  //Serial.println();

  // Filtrowane
  static unsigned int force_measuring_counter = 0;
  static const unsigned int FORCE_MEASUREMENTS_ARRAY_SIZE = 20;
  static unsigned int force_measurements[FORCE_MEASUREMENTS_ARRAY_SIZE];
  force_measurements[force_measuring_counter] = analogRead(PIN_FORCE_SENSOR);

  ++force_measuring_counter;
  if (force_measuring_counter < FORCE_MEASUREMENTS_ARRAY_SIZE)
    return;

  force_measuring_counter = 0;
  float signal_value = 0;
  unsigned int min_value = 0;
  unsigned int max_value = 0;
  for (unsigned int i = 0; i < FORCE_MEASUREMENTS_ARRAY_SIZE; i++)
  {
    signal_value += force_measurements[i];
    if (min_value > force_measurements[i])
      min_value = force_measurements[i];
    if (max_value < force_measurements[i])
      max_value = force_measurements[i];
  }
  signal_value /= FORCE_MEASUREMENTS_ARRAY_SIZE;           // filtrowanie szumów poprzez uśrednianie krótkookresowych odczytów

  unsigned int displayed_value = round(signal_value);
  Serial.print(displayed_value);
  Serial.print(',');
  Serial.print(min_value);
  Serial.print(',');
  Serial.print(max_value);
  Serial.print(',');
  Serial.println();
}

void testAmplifier(short unsigned int pin_input, short unsigned int pin_output, short unsigned int signal_value)
{
  pinMode(pin_input, OUTPUT);         // pin używany tylko do testów

  signal_value = constrain(signal_value, 0, 1023.0 * 300 / 5000);
  analogWrite(pin_input, signal_value);
  delay(20);
  Serial.print(signal_value);
  Serial.print(',');
  Serial.print(analogRead(pin_output));
  Serial.println();
}

void testTimeSynchronization(long unsigned int timer)
{
  Serial.print(millis() - timer);
  Serial.println();
}
