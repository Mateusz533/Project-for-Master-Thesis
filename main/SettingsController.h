#pragma once

#include <Arduino.h>
#include "configuration.h"
#include "DisplayedElement.h"

// Klasa przechowująca zmieniane przez użytkownika parametry globalne
class SettingsController : public DisplayedElement
{
  public:
    // Konstruktor
    SettingsController();
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
