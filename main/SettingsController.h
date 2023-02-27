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
    // Pobiera od użytkownika polecania dotyczące parametrów globalnych
    void getCommands(const bool buttons[]);
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

void SettingsController::getCommands(const bool buttons[])
{
  if (buttons[UP] && !buttons[DOWN] && ambient_temperature_ < MAX_AMBIENT_TEMPERATURE_)
    ambient_temperature_ += AMBIENT_TEMPERATURE_RESOLUTION_;
  else if (!buttons[UP] && buttons[DOWN] && ambient_temperature_ > MIN_AMBIENT_TEMPERATURE_)
    ambient_temperature_ -= AMBIENT_TEMPERATURE_RESOLUTION_;
}

void SettingsController::getDataToDisplay(String& first_line, String& second_line)
{
  first_line = "Ambient:  ";
  if (abs(ambient_temperature_) < 100)
    first_line += " ";
  if (abs(ambient_temperature_) < 10)
    first_line += " ";
  if (ambient_temperature_ < 0)
    first_line += "-";
  else
    first_line += " ";
  first_line += static_cast<String>(abs(ambient_temperature_)) + DEGREE_SYMBOL_INDEX + "C";

  second_line = "                ";
}

void SettingsController::run()
{
  return;
}
