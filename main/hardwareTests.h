#include "configuration.h"
#include "Queue.h"
#pragma once

// Przekazuje dane o mierzonej sile nacisku przez port szeregowy
void testForceSensor();
// Przekazuje dane o mierzonej temperaturze przez port szeregowy
void testTemperatureSensors();
// Ustawia na wejściu wzmacniacza zadaną wartość napięcia (w poziomach DAC)
void testAmplifier(short unsigned int pin_number, short unsigned int pin_output, short unsigned int signal_value);
// Przekazuje dane o różnicy czasu w danej chwili od podanej wartości
void testTimeSynchronization(long unsigned int timer);

void testForceSensor()
{
  //// Niefiltrowane
  //Serial.print(analogRead(PIN_FORCE_SENSOR));
  //Serial.println();

  // Filtrowane
  static Queue<unsigned int> force_measurements(20);
  force_measurements.push(analogRead(PIN_FORCE_SENSOR));

  if (!force_measurements.isFull())
    return;

  // Filtrowanie szumów poprzez uśrednianie krótkookresowych odczytów
  unsigned int mean_value = force_measurements.mean_value();
  unsigned int min_value = force_measurements.min_value();
  unsigned int max_value = force_measurements.max_value();
  force_measurements.clear();

  Serial.print(mean_value);
  Serial.print(F(","));
  Serial.print(min_value);
  Serial.print(F(","));
  Serial.print(max_value);
  Serial.print(F(","));
  Serial.println();
}

void testTemperatureSensors()
{
  Serial.print(analogRead(PIN_TEMPERATURE_SENSOR_TOP));
  Serial.print(F(","));
  Serial.print(analogRead(PIN_TEMPERATURE_SENSOR_BOT));
  Serial.println();
}

void testAmplifier(short unsigned int pin_output)
{
  //// Niefiltrowane
  // Serial.print(analogRead(pin_output));
  // Serial.println();

  // Filtrowane
  static Queue<unsigned int> measurements(100);
  measurements.push(analogRead(pin_output));

  if (!measurements.isFull())
    return;

  // Filtrowanie szumów poprzez uśrednianie krótkookresowych odczytów
  unsigned int mean_value = measurements.mean_value();
  unsigned int min_value = measurements.min_value();
  unsigned int max_value = measurements.max_value();
  unsigned int median_value = measurements.quantile(0.5);
  measurements.clear();

  Serial.print(mean_value);
  Serial.print(F(","));
  Serial.print(min_value);
  Serial.print(F(","));
  Serial.print(max_value);
  Serial.print(F(","));
  Serial.print(median_value);
  Serial.println();
}

void testHeaters()
{
  // Filtrowane
  static Queue<unsigned int> top_measurements(50);
  top_measurements.push(analogRead(PIN_TEMPERATURE_SENSOR_TOP));
  static Queue<unsigned int> bot_measurements(50);
  bot_measurements.push(analogRead(PIN_TEMPERATURE_SENSOR_BOT));

  if (!top_measurements.isFull())
    return;

  // Filtrowanie szumów poprzez uśrednianie krótkookresowych odczytów
  unsigned int top_value = top_measurements.quantile(0.5);
  unsigned int bot_value = bot_measurements.quantile(0.5);
  top_measurements.clear();
  bot_measurements.clear();

  Serial.print(top_value);
  Serial.print(F(","));
  Serial.print(bot_value);
  Serial.println();
}

void testTimeSynchronization(long unsigned int timer)
{
  Serial.print(millis() - timer);
  Serial.println();
}
