#include "SettingsController.h"

SettingsController::SettingsController()
{
  // przypisanie domyślnych wartości temperatury otoczenia oraz maksymalnej mocy grznia
  ambient_temperature = DEFAULT_AMBIENT_TEMPERATURE;
  max_heating_power = MAX_HEATING_POWER / 2;
}

void SettingsController::init()
{
  return;
}

void SettingsController::executeCommands(const bool buttons[])
{
  if (current_line_ == 0)
  {
    if (buttons[UP] && !buttons[DOWN] && ambient_temperature < MAX_AMBIENT_TEMPERATURE_)
      ambient_temperature += AMBIENT_TEMPERATURE_RESOLUTION_;
    else if (!buttons[UP] && buttons[DOWN] && ambient_temperature > MIN_AMBIENT_TEMPERATURE_)
      ambient_temperature -= AMBIENT_TEMPERATURE_RESOLUTION_;
  }
  else if (current_line_ == 1)
  {
    if (buttons[UP] && !buttons[DOWN] && max_heating_power < MAX_HEATING_POWER)
      max_heating_power += MAX_HEATING_POWER_RESOLUTION_;
    else if (!buttons[UP] && buttons[DOWN] && max_heating_power > MAX_HEATING_POWER_RESOLUTION_)
      max_heating_power -= MAX_HEATING_POWER_RESOLUTION_;
  }
  if (buttons[ACTION])
    current_line_ = !current_line_;
}

void SettingsController::getDataToDisplay(String& first_line, String& second_line) const
{
  first_line = String(F("Ambient:      ")) + DEGREE_SYMBOL_INDEX + F("C");
  String str_ambient_temp(ambient_temperature);
  for (unsigned int i = 0; i < str_ambient_temp.length(); ++i)
    first_line.setCharAt(13 - i, str_ambient_temp.charAt(str_ambient_temp.length() - 1 - i));

  second_line = String(F("Max power:     W"));
  String str_max_power(max_heating_power);
  for (unsigned int i = 0; i < str_max_power.length(); ++i)
    second_line.setCharAt(14 - i, str_max_power.charAt(str_max_power.length() - 1 - i));
}

void SettingsController::run()
{
  return;
}
