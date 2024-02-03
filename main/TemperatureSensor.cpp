#include "TemperatureSensor.h"

TemperatureSensor::TemperatureSensor(const uint8_t pin_temperature_sensor, const float tuning_factor, const float sensor_offset) :
  PIN_TEMPERATURE_SENSOR_{ pin_temperature_sensor },
  TUNING_FACTOR_{ tuning_factor },
  SENSOR_OFFSET_{ sensor_offset }
{
  // przypisanie numeru pinu w mikrosterowniku
  // ustawienie wartości czynnika korygującego rzeczywistą wartość wzmocnienia wzmacniacza
}

void TemperatureSensor::init()
{
  pinMode(PIN_TEMPERATURE_SENSOR_, INPUT);    // ustawienie czujnika temperatury jako wejście z napięciem zewnętrznym
  digitalWrite(PIN_TEMPERATURE_SENSOR_, LOW);
}

void TemperatureSensor::measureTemperature()
{
  // Zapisanie reprezentacji pomiarów z możliwie jak największą precyzją
  short_term_measurements_.push(STORAGE_MULTIPLIER * analogRead(PIN_TEMPERATURE_SENSOR_));

  if (!short_term_measurements_.isFull())
    return;

  // Cyfrowa filtracja szumów poprzez filtr medianowy
  const StorageSignal median_value{ short_term_measurements_.quantile(0.5) };
  short_term_measurements_.clear();

  long_term_measurements_.push(median_value);
}

KelvinDiff TemperatureSensor::calculateRelativeTemperature()
{
  // Sortowanie tablic przez wstawianie
  long_term_measurements_.sort();
  CollectionSize lower_index{ 0 };
  CollectionSize higher_index{ long_term_measurements_.length() };
  for (CollectionSize i = 0; i < long_term_measurements_.length(); ++i)
  {
    if (long_term_measurements_[i] < STORAGE_MULTIPLIER * TEMPERATURE_SIGNAL_LOWER_LIMIT)
      lower_index = i + 1;
    if (long_term_measurements_[i] > STORAGE_MULTIPLIER * TEMPERATURE_SIGNAL_HIGHER_LIMIT)
    {
      higher_index = i;
      break;
    }
  }

  // Sprawdzenie poziomu zaszumienia sygnału
  if (ENABLE_ERRORS && higher_index < lower_index + MIN_SN_RATIO * long_term_measurements_.length())
    reportError(F("3"));

  // Filtracja medianowa oraz przeskalowanie sygnału do początkowego zakresu
  const float signal_value{ 1.0 * long_term_measurements_.quantile(0.5, lower_index, higher_index) / STORAGE_MULTIPLIER };

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
