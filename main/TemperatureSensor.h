#include "configuration.h"
#include "StaticArray.h"
#include "Queue.h"
#include "reportError.h"
#pragma once

// Klasa przechowująca parametry czujnika temperatury
class TemperatureSensor
{
  public:
    // Konstruktor
    TemperatureSensor(short unsigned int pin_temperature_sensor, float tuning_factor, float sensor_offset) :
      PIN_TEMPERATURE_SENSOR_(pin_temperature_sensor),
      TUNING_FACTOR_(tuning_factor),
      SENSOR_OFFSET_(sensor_offset)
    {
      // przypisanie numerów pinów w mikrosterowniku
      // ustawienie wartości czynnika korygującego rzeczywistą wartość wzmocnienia wzmacniacza
    }
    // Konfiguruje porty wejścia/wyjścia dla tego elementu
    void init();

  protected:
    // Wykonuje pomiary temperatury i filtruje dolnoprzepustowo z określoną częstotliwością
    void measureTemperature();
    // Zwraca oszacowaną różnicę temperatur pomiędzy płytą a otoczeniem na podstawie zebranych pomiarów
    int calculateRelativeTemperature();

    // Zmienne przechowujące aktualne wartości temperatury
    Queue<float> long_temperature_measurements_ = Queue<float>(TEMPERATURE_ESTIMATION_PERIOD / TEMPERATURE_AVERAGING_PERIOD);
    Queue<int> short_temperature_measurements_ = Queue<int>(TEMPERATURE_AVERAGING_PERIOD / CYCLE_PERIOD);

  private:
    // Numer pinu przypisanego do czujnika
    const short unsigned int PIN_TEMPERATURE_SENSOR_ = 0;
    // Czynnik dostrojenia wzmacniacza oraz stałe przesunięcie sygnału
    const float TUNING_FACTOR_ = 1;
    const float SENSOR_OFFSET_ = 0;
};

void TemperatureSensor::init()
{
  pinMode(PIN_TEMPERATURE_SENSOR_, INPUT);    // ustawienie czujnika temperatury jako wejście z napięciem zewnętrznym
  digitalWrite(PIN_TEMPERATURE_SENSOR_, LOW);
}

void TemperatureSensor::measureTemperature()
{
  // Zgrubne ustawienie wzmocnienia sygnału tak, aby wzrost temperatury o 1 *C zwiększał odczyt o 1
  short_temperature_measurements_.push(analogRead(PIN_TEMPERATURE_SENSOR_));

  if (!short_temperature_measurements_.isFull())
    return;

  // Cyfrowa filtracja szumów poprzez filtr medianowy
  float median_temperature = short_temperature_measurements_.quantile(0.5);
  short_temperature_measurements_.clear();

  long_temperature_measurements_.push(median_temperature);
}

int TemperatureSensor::calculateRelativeTemperature()
{
  const unsigned int LOWER_SIGNAL_LIMIT = 20;
  const unsigned int HIGHER_SIGNAL_LIMIT = 720;
  const float QUANTILE = 0.5;

  // Sortowanie tablic przez wstawianie
  long_temperature_measurements_.sort();
  unsigned int lower_index = 0;
  unsigned int higher_index = long_temperature_measurements_.length();
  for (unsigned int i = 0; i < long_temperature_measurements_.length(); ++i)
  {
    if (long_temperature_measurements_[i] < LOWER_SIGNAL_LIMIT)
      lower_index = i + 1;
    if (long_temperature_measurements_[i] > HIGHER_SIGNAL_LIMIT)
    {
      higher_index = i;
      break;
    }
  }

  //  // Cyfrowa filtracja szumów poprzez filtr medianowy
  //  if (higher_index < lower_index + long_temperature_measurements_.length() / 2.0)
  //    reportError(F("2"));

  float signal_value = long_temperature_measurements_.quantile(QUANTILE, lower_index, higher_index);

  // Dodatkowa korekta ustawionych mechanicznie wzmocnień wzmacniaczy sygnałów temperatury
  return round(TUNING_FACTOR_ * (signal_value - SENSOR_OFFSET_));
}
