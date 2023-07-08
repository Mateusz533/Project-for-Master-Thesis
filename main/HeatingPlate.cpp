#include "HeatingPlate.h"

HeatingPlate::HeatingPlate(const short unsigned int pin_temperature_sensor, const short unsigned int pin_heat_supply,
                           const float tuning_factor, const float sensor_offset, const String& name) :
  temperature_sensor_{ pin_temperature_sensor, tuning_factor, sensor_offset },
  regulator_{ pin_heat_supply },
  DISPLAYED_NAME_{ name }
{
  // przypisanie numerów pinów w mikrosterowniku
}

void HeatingPlate::init()
{
  regulator_.init();             // konfiguracja grzałki
  temperature_sensor_.init();    // konfiguracja czujnika temperatury
}

void HeatingPlate::executeCommands(const bool buttons[])
{
  if (buttons[UP] && !buttons[DOWN] && set_temperature_ < MAX_TEMPERATURE)
  {
    set_temperature_ += PRESET_TEMPERATURE_RESOLUTION;
    regulator_.reset();
  }
  else if (!buttons[UP] && buttons[DOWN] && set_temperature_ > MIN_TEMPERATURE)
  {
    set_temperature_ -= PRESET_TEMPERATURE_RESOLUTION;
    regulator_.reset();
  }
  if (buttons[ACTION])
    is_heating_set_ = !is_heating_set_;
}

void HeatingPlate::getDataToDisplay(String& first_line, String& second_line) const
{
  first_line = DISPLAYED_NAME_ + F(" temp:    ") + DEGREE_SYMBOL_INDEX + F("C ");
  String str_real_temp(round(real_temperature_));
  for (unsigned int i = 0; i < str_real_temp.length(); ++i)
    first_line.setCharAt(12 - i, str_real_temp.charAt(str_real_temp.length() - 1 - i));

  second_line = String(F("Set:    ")) + DEGREE_SYMBOL_INDEX + F("C   ");
  String str_set_temp(set_temperature_);
  for (unsigned int i = 0; i < str_set_temp.length(); ++i)
    second_line.setCharAt(7 - i, str_set_temp.charAt(str_set_temp.length() - 1 - i));
  second_line += is_heating_set_ ? F(" ON") : F("OFF");
}

void HeatingPlate::run()
{
  // Ustawianie odpowiedniego sygnału grzania
  regulator_.run();

  // Pomiar temperatury na płycie
  temperature_sensor_.measureTemperature();

  // Ustawienie mocy grzania w zależności od obecnej temperatury
  if (temperature_sensor_.isBufferFull())
  {
    const float estimated_temperature = s_ambient_temperature + temperature_sensor_.calculateRelativeTemperature();
    // Sprawdzenie poprawności sygnału
    const bool start = round(real_temperature_) == s_ambient_temperature;
    if (ENABLE_ERRORS && !start
        && abs(real_temperature_ - estimated_temperature) > MAX_TEMPERATURE_GROWTH * TEMPERATURE_ESTIMATION_PERIOD / 1000.0)
      reportError(F("4"));

    real_temperature_ = estimated_temperature;
    temperature_sensor_.clearBuffer();
    regulator_.refreshHeatingPower(set_temperature_, real_temperature_, is_heating_set_);
  }
}
