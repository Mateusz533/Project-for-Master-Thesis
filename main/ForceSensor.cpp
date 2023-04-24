#include "ForceSensor.h"

ForceSensor::ForceSensor(short unsigned int pin_force_sensor) :
  PIN_FORCE_SENSOR_{ pin_force_sensor },
  SIGNAL_VALUES_{ FORCE_CONVERSION_ARRAY_SIZE, new const int[FORCE_CONVERSION_ARRAY_SIZE] FORCE_SIGNAL_VALUES },
  FORCE_VALUES_{ FORCE_CONVERSION_ARRAY_SIZE, new const int[FORCE_CONVERSION_ARRAY_SIZE] FORCE_VALUES }
{
  // przypisanie numeru pinu w mikrosterowniku
}

void ForceSensor::init()
{
  pinMode(PIN_FORCE_SENSOR_, INPUT);    // ustawienie czujnika nacisku jako wejście z napięciem zewnętrznym
  digitalWrite(PIN_FORCE_SENSOR_, LOW);
}

void ForceSensor::executeCommands(const bool buttons[])
{
  if (buttons[ACTION] && !is_overloaded_)
    force_offset_ = force_;
}

void ForceSensor::getDataToDisplay(String& first_line, String& second_line) const
{
  first_line = F("Force:       N  ");
  String str_force = force_ < 10 + START_OFFSET ? F("<10") : String(force_ - force_offset_);
  for (unsigned int i = 0; i < str_force.length(); ++i)
    first_line.setCharAt(11 - i, str_force.charAt(str_force.length() - 1 - i));

  if (is_overloaded_)
    second_line = F("OVERLOADED !!!  ");
  else
    second_line = F("Maximum: 200 N  ");
}

void ForceSensor::run()
{
  force_measurements_.push(analogRead(PIN_FORCE_SENSOR_));

  if (!force_measurements_.isFull())
    return;

  float signal_value = force_measurements_.mean_value();    // filtrowanie szumów poprzez uśrednianie krótkookresowych odczytów

  if (ENABLE_ERRORS && signal_value > FORCE_SIGNAL_HIGHER_LIMIT)    // sprawdzenie poprawności sygnału
    reportError(F("2"));

  force_ = tabularConversion<const int, const int>(SIGNAL_VALUES_, FORCE_VALUES_, signal_value);    // konwersja poziomów ADC na Newtony

  is_overloaded_ = (force_ >= FORCE_VALUES_.get(FORCE_VALUES_.length() - 1));    // sprawdzenie przeciążenia czujnika

  force_measurements_.clear();
}
