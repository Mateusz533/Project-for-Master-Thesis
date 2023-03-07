#include "configuration.h"
#include "reportError.h"
#pragma once

// Klasa przechowująca parametry czujnika temperatury
class TemperatureSensor
{
  public:
    // Konstruktor
    TemperatureSensor(short unsigned int pin_temperature_sensor, float tuning_factor, float sensor_offset):
      PIN_TEMPERATURE_SENSOR_ (pin_temperature_sensor),
      TUNING_FACTOR_ (tuning_factor),
      SENSOR_OFFSET_ (sensor_offset)
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
    static const short int SHORT_MEASUREMENTS_ARRAY_SIZE_ = TEMPERATURE_AVERAGING_PERIOD / CYCLE_PERIOD;
    static const short int LONG_MEASUREMENTS_ARRAY_SIZE_ = TEMPERATURE_ESTIMATION_PERIOD / TEMPERATURE_AVERAGING_PERIOD;
    float long_temperature_measurements_[LONG_MEASUREMENTS_ARRAY_SIZE_];
    int short_temperature_measurements_[SHORT_MEASUREMENTS_ARRAY_SIZE_];
    // Liczniki wyznaczające częstotliwość pomiaru
    unsigned int short_measuring_counter_ = 0;
    unsigned int long_measuring_counter_ = 0;
    // Numer pinu przypisanego do czujnika
    const short unsigned int PIN_TEMPERATURE_SENSOR_;
    // Czynnik dostrojenia wzmacniacza oraz stałe przesunięcie sygnału
    const float TUNING_FACTOR_;
    const float SENSOR_OFFSET_;
};

void TemperatureSensor::init()
{
  pinMode(PIN_TEMPERATURE_SENSOR_, INPUT);          // ustawienie czujnika temperatury jako wejście z napięciem zewnętrznym
  digitalWrite(PIN_TEMPERATURE_SENSOR_, LOW);
}

void TemperatureSensor::measureTemperature()
{
  // Zgrubne ustawienie wzmocnienia sygnału tak, aby wzrost temperatury o 1 *C zwiększał odczyt o 1
  short_temperature_measurements_[short_measuring_counter_] = analogRead(PIN_TEMPERATURE_SENSOR_);

  ++short_measuring_counter_;
  if (short_measuring_counter_ < SHORT_MEASUREMENTS_ARRAY_SIZE_)
    return;

  short_measuring_counter_ = 0;

  // Sortowanie tablic przez wstawianie
  for (unsigned int i = 0; i < SHORT_MEASUREMENTS_ARRAY_SIZE_; i++)
  {
    for (unsigned int j = i; j > 0; j--)
    {
      if (short_temperature_measurements_[j - 1] <= short_temperature_measurements_[j])
        break;

      unsigned int aux_var = short_temperature_measurements_[j];
      short_temperature_measurements_[j] = short_temperature_measurements_[j - 1];
      short_temperature_measurements_[j - 1] = aux_var;
    }
  }

  // Cyfrowa filtracja szumów poprzez filtr medianowy
  float median_position = 0.5 * (SHORT_MEASUREMENTS_ARRAY_SIZE_ - 1);
  unsigned int lower_index = floor(median_position);
  unsigned int higher_index = ceil(median_position);
  float median_temperature = 0;
  if (lower_index == higher_index)
    median_temperature = short_temperature_measurements_[lower_index];
  else
    median_temperature = (higher_index - median_position) * short_temperature_measurements_[lower_index]
                         + (median_position - lower_index) * short_temperature_measurements_[higher_index];

  long_temperature_measurements_[long_measuring_counter_] = median_temperature;
  ++long_measuring_counter_;
}

int TemperatureSensor::calculateRelativeTemperature()
{
  const unsigned int LOWER_SIGNAL_LIMIT = 10;
  const unsigned int HIGHER_SIGNAL_LIMIT = 500;
  const float QUANTILE = 0.5;

  // Sortowanie tablic przez wstawianie
  unsigned int aux_var = 0;
  unsigned int table_size = 0;
  for (unsigned int i = 0; i < LONG_MEASUREMENTS_ARRAY_SIZE_; i++)
  {
    aux_var = long_temperature_measurements_[i];
    if (aux_var < LOWER_SIGNAL_LIMIT || aux_var > HIGHER_SIGNAL_LIMIT)
      continue;

    long_temperature_measurements_[table_size] = aux_var;
    for (unsigned int j = table_size; j > 0; j--)
    {
      if (long_temperature_measurements_[j - 1] <= aux_var)
        break;

      long_temperature_measurements_[j] = long_temperature_measurements_[j - 1];
      long_temperature_measurements_[j - 1] = aux_var;
    }
    ++table_size;
  }

  //  // Cyfrowa filtracja szumów poprzez filtr medianowy
  //  if (table_size < LONG_MEASUREMENTS_ARRAY_SIZE_ / 2.0)
  //    reportError("2");

  float quantile_position = QUANTILE * (table_size - 1);
  unsigned int lower_index = floor(quantile_position);
  unsigned int higher_index = ceil(quantile_position);
  float signal_value = 0;
  if (lower_index == higher_index)
    signal_value = long_temperature_measurements_[lower_index];
  else
    signal_value = (higher_index - quantile_position) * long_temperature_measurements_[lower_index]
                   + (quantile_position - lower_index) * long_temperature_measurements_[higher_index];

  // Dodatkowa korekta ustawionych mechanicznie wzmocnień wzmacniaczy sygnałów temperatury
  return round(TUNING_FACTOR_ * ( signal_value - SENSOR_OFFSET_));
}
