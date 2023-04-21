#include "configuration.h"
#include "StaticArray.h"
#include "Queue.h"
#include "reportError.h"
#pragma once

// Klasa przechowująca parametry czujnika temperatury
class TemperatureSensor
{
  public:
    TemperatureSensor() = delete;
    // Konstruktor
    TemperatureSensor(short unsigned int pin_temperature_sensor, float tuning_factor, float sensor_offset) :
      PIN_TEMPERATURE_SENSOR_{ pin_temperature_sensor },
      TUNING_FACTOR_{ tuning_factor },
      SENSOR_OFFSET_{ sensor_offset }
    {
      // przypisanie numeru pinu w mikrosterowniku
      // ustawienie wartości czynnika korygującego rzeczywistą wartość wzmocnienia wzmacniacza
    }
    ~TemperatureSensor() = default;
    // Konfiguruje porty wejścia/wyjścia dla tego elementu
    void init();
    // Wykonuje pomiary temperatury i filtruje dolnoprzepustowo z określoną częstotliwością
    void measureTemperature();
    // Zwraca oszacowaną różnicę temperatur pomiędzy płytą a otoczeniem na podstawie zebranych pomiarów
    float calculateRelativeTemperature();
    // Zwraca prawdę, jeśli buffor danych pomiarowych jest pełny
    bool isBufferFull() const;
    // Czyści buffor danych pomiarowych
    void clearBuffer();

  private:
    // Zmienne przechowujące aktualne wartości temperatury
    Queue<unsigned int> long_term_measurements_{ TEMPERATURE_ESTIMATION_PERIOD / TEMPERATURE_AVERAGING_PERIOD };
    Queue<unsigned int> short_term_measurements_{ TEMPERATURE_AVERAGING_PERIOD / CYCLE_PERIOD };
    // Numer pinu przypisanego do czujnika
    const short unsigned int PIN_TEMPERATURE_SENSOR_{ 0 };
    // Czynnik dostrojenia wzmacniacza oraz stałe przesunięcie sygnału
    const float TUNING_FACTOR_{ 1 };
    const float SENSOR_OFFSET_{ 0 };
};

void TemperatureSensor::init()
{
  pinMode(PIN_TEMPERATURE_SENSOR_, INPUT);    // ustawienie czujnika temperatury jako wejście z napięciem zewnętrznym
  digitalWrite(PIN_TEMPERATURE_SENSOR_, LOW);
}

void TemperatureSensor::measureTemperature()
{
  // Zapisanie reprezentacji pomiarów z możliwie jak największą precyzją
  short_term_measurements_.push(8 * analogRead(PIN_TEMPERATURE_SENSOR_));

  if (!short_term_measurements_.isFull())
    return;

  // Cyfrowa filtracja szumów poprzez filtr medianowy
  unsigned int median_value = short_term_measurements_.quantile(0.5);
  short_term_measurements_.clear();

  long_term_measurements_.push(median_value);
}

float TemperatureSensor::calculateRelativeTemperature()
{
  // Sortowanie tablic przez wstawianie
  long_term_measurements_.sort();
  unsigned int lower_index{ 0 };
  unsigned int higher_index = long_term_measurements_.length();
  for (unsigned int i = 0; i < long_term_measurements_.length(); ++i)
  {
    if (long_term_measurements_[i] < 8 * TEMPERATURE_SIGNAL_LOWER_LIMIT)
      lower_index = i + 1;
    if (long_term_measurements_[i] > 8 * TEMPERATURE_SIGNAL_HIGHER_LIMIT)
    {
      higher_index = i;
      break;
    }
  }

  // Cyfrowa filtracja szumów poprzez filtr medianowy
  if (ENABLE_ERRORS && higher_index < lower_index + long_term_measurements_.length() / 2.0)
    reportError(F("3"));

  // Przeskalowanie sygnału do początkowego zakresu
  float signal_value = long_term_measurements_.quantile(0.5, lower_index, higher_index) / 8.0;

  // Korekta ustawionych mechanicznie wzmocnień wzmacniaczy sygnałów temperatury
  return TUNING_FACTOR_ * (signal_value - SENSOR_OFFSET_);
}

bool TemperatureSensor::isBufferFull() const
{
  return long_term_measurements_.isFull();
}

void TemperatureSensor::clearBuffer()
{
  long_term_measurements_.clear();
  short_term_measurements_.clear();
}
