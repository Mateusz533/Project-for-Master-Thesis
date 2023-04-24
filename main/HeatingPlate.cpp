#include "HeatingPlate.h"

HeatingPlate::HeatingPlate(short unsigned int pin_temperature_sensor, short unsigned int pin_heat_supply, float tuning_factor, float sensor_offset, const String& name) :
  Heater{ pin_heat_supply },
  temperature_sensor{ pin_temperature_sensor, tuning_factor, sensor_offset },
  DISPLAYED_NAME_{ name }
{
  // przypisanie numerów pinów w mikrosterowniku
}

void HeatingPlate::init()
{
  Heater::init();               // konfiguracja grzałki
  temperature_sensor.init();    // konfiguracja czujnika temperatury
}

void HeatingPlate::executeCommands(const bool buttons[])
{
  if (buttons[UP] && !buttons[DOWN] && set_temperature_ < MAX_TEMPERATURE)
  {
    set_temperature_ += PRESET_TEMPERATURE_RESOLUTION;
    active_regulation_ = false;
    prediction_error_ = 0;
  }
  else if (!buttons[UP] && buttons[DOWN] && set_temperature_ > MIN_TEMPERATURE)
  {
    set_temperature_ -= PRESET_TEMPERATURE_RESOLUTION;
    active_regulation_ = false;
    prediction_error_ = 0;
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
  // Włączenie/wyłączenie grzania - musi następować szybciej niż odświeżanie stanu temperatury
  manageHeating();

  // Pomiar temperatury na płycie
  temperature_sensor.measureTemperature();

  // Ustawienie mocy grzania w zależności od obecnej temperatury
  if (temperature_sensor.isBufferFull())
  {
    float estimated_temperature = ambient_temperature + temperature_sensor.calculateRelativeTemperature();
    // Sprawdzenie poprawności sygnału
    bool start = round(real_temperature_) == ambient_temperature;
    if (ENABLE_ERRORS && !start && abs(real_temperature_ - estimated_temperature) > MAX_TEMPERATURE_GROWTH * TEMPERATURE_ESTIMATION_PERIOD / 1000.0)
      reportError(F("4"));

    real_temperature_ = estimated_temperature;
    temperature_sensor.clearBuffer();
    regulateHeatingPower();
  }
}
