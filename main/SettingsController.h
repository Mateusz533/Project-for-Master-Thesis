#include "configuration.h"
#include "DisplayedElement.h"
#pragma once

// Klasa przechowująca zmieniane przez użytkownika parametry globalne
class SettingsController : public DisplayedElement
{
  public:
    // Konstruktor
    SettingsController()
    {
      // przypisanie domyślnych wartości temperatury otoczenia oraz maksymalnej mocy grznia
      ambient_temperature = DEFAULT_AMBIENT_TEMPERATURE;
      max_heating_power = MAX_HEATING_POWER / 2;
    }
    ~SettingsController() = default;
    // Nie wykonuje żadnej akcji
    void init() override;
    // Nie wykonuje żadnej akcji
    void run() override;
    // Wykonuje pobrane od użytkownika polecania dotyczące parametrów globalnych
    void executeCommands(const bool buttons[]) override;
    // Wyświetla dane dotyczące parametrów globalnych
    void getDataToDisplay(String& first_line, String& second_line) const override;

  private:
    const int AMBIENT_TEMPERATURE_RESOLUTION_{ 1 };
    const int MIN_AMBIENT_TEMPERATURE_{ -50 };
    const int MAX_AMBIENT_TEMPERATURE_{ 100 };
    const int MAX_HEATING_POWER_RESOLUTION_{ 5 };
    short unsigned int current_line_{ 0 };
};

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
