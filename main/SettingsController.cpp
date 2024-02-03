#include "SettingsController.h"

SettingsController::SettingsController()
{
  // Przypisanie domyślnych wartości temperatury otoczenia oraz maksymalnej mocy grznia
  s_ambient_temperature = DEFAULT_AMBIENT_TEMPERATURE;
  s_max_heating_power = MAX_HEATING_POWER / 2;
}

void SettingsController::init()
{
  return;
}

void SettingsController::executeCommands(const Array<bool, BUTTONS_NUMBER>& buttons)
{
  if (current_line_ == 0)
  {
    if (buttons.get(UP) && !buttons.get(DOWN) && s_ambient_temperature < MAX_AMBIENT_TEMPERATURE_)
      s_ambient_temperature += AMBIENT_TEMPERATURE_RESOLUTION_;
    else if (!buttons.get(UP) && buttons.get(DOWN) && s_ambient_temperature > MIN_AMBIENT_TEMPERATURE_)
      s_ambient_temperature -= AMBIENT_TEMPERATURE_RESOLUTION_;
  }
  else if (current_line_ == 1)
  {
    if (buttons.get(UP) && !buttons.get(DOWN) && s_max_heating_power < MAX_HEATING_POWER)
      s_max_heating_power += MAX_HEATING_POWER_RESOLUTION_;
    else if (!buttons.get(UP) && buttons.get(DOWN) && s_max_heating_power > MAX_HEATING_POWER_RESOLUTION_)
      s_max_heating_power -= MAX_HEATING_POWER_RESOLUTION_;
  }
  if (buttons.get(ACTION))
    current_line_ = (current_line_ + 1) % LCD_ROWS_NUMBER;
}

const char FIRST_LINE[LCD_COLUMNS_NUMBER] PROGMEM = {
  'A', 'm', 'b', 'i', 'e', 'n', 't', ':', ' ', ' ', ' ', ' ', ' ', ' ', DEGREE_SYMBOL_INDEX, 'C'
};
const char SECOND_LINE[LCD_COLUMNS_NUMBER + 1] PROGMEM = "Max power:     W";

ScreenContent SettingsController::getDataToDisplay() const
{
  constexpr int LAST_DIGIT_IN_FIRST_LINE = 13;
  constexpr int LAST_DIGIT_IN_SECOND_LINE = 14;

  ScreenContent content(FIRST_LINE, SECOND_LINE);
  content.putNumberFromEnd(s_ambient_temperature, LAST_DIGIT_IN_FIRST_LINE, 0);
  content.putNumberFromEnd(s_max_heating_power, LAST_DIGIT_IN_SECOND_LINE, 1);

  return content;
}

void SettingsController::run()
{
  return;
}
