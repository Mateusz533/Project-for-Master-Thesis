#include "configuration.h"
#include "SystemElement.h"
#pragma once

// Klasa przechowująca zmieniane przez użytkownika parametry globalne
class SettingsController : public SystemElement
{
  public:
    // Konstruktor
    SettingsController()
    {
      // przypisanie domyślnej temperatury otoczenia
      ambient_temperature_ = DEFAULT_AMBIENT_TEMPERATURE;
    }
    // Nie wykonuje żadnej akcji
    void init();
    // Wykonuje pobrane od użytkownika polecania dotyczące parametrów globalnych
    void executeCommands(const bool buttons[]);
    // Wyświetla dane dotyczące parametrów globalnych
    void getDataToDisplay(String& first_line, String& second_line);
    // Nie wykonuje żadnej akcji
    void run();

  private:
    const int AMBIENT_TEMPERATURE_RESOLUTION_ = 1;
    const int MIN_AMBIENT_TEMPERATURE_ = -50;
    const int MAX_AMBIENT_TEMPERATURE_ = 100;
};

void SettingsController::init()
{
  return;
}

void SettingsController::executeCommands(const bool buttons[])
{
  if (buttons[UP] && !buttons[DOWN] && ambient_temperature_ < MAX_AMBIENT_TEMPERATURE_)
    ambient_temperature_ += AMBIENT_TEMPERATURE_RESOLUTION_;
  else if (!buttons[UP] && buttons[DOWN] && ambient_temperature_ > MIN_AMBIENT_TEMPERATURE_)
    ambient_temperature_ -= AMBIENT_TEMPERATURE_RESOLUTION_;
}

void SettingsController::getDataToDisplay(String& first_line, String& second_line)
{
  first_line = String(F("Ambient:      ")) + DEGREE_SYMBOL_INDEX + F("C");
  String str_ambient_temp(ambient_temperature_);
  for (unsigned int i = 0; i < str_ambient_temp.length(); ++i)
    first_line.setCharAt(13 - i, str_ambient_temp.charAt(str_ambient_temp.length() - 1 - i));

  second_line = F("                ");
}

void SettingsController::run()
{
  return;
}
