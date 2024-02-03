#pragma once

#include "configuration.h"
#include "DisplayedElement.h"

// Klasa przechowująca zmieniane przez użytkownika parametry globalne
class SettingsController : public DisplayedElement
{
  public:
    SettingsController();
    ~SettingsController() = default;
    // Nie wykonuje żadnej akcji
    void init() override;
    // Nie wykonuje żadnej akcji
    void run() override;
    // Wykonuje pobrane od użytkownika polecania dotyczące parametrów globalnych
    void executeCommands(const Array<bool, BUTTONS_NUMBER>& buttons) override;
    // Wyświetla dane dotyczące parametrów globalnych
    ScreenContent getDataToDisplay() const override;

  private:
    // Położenie obecnie ustawianego parametru
    BasicSize current_line_{ 0 };
    // Ograniczenia ustawianych parametrów
    static constexpr DegCelsius AMBIENT_TEMPERATURE_RESOLUTION_{ 1 };
    static constexpr DegCelsius MIN_AMBIENT_TEMPERATURE_{ -50 };
    static constexpr DegCelsius MAX_AMBIENT_TEMPERATURE_{ 100 };
    static constexpr StorageWatt MAX_HEATING_POWER_RESOLUTION_{ 5 };
};
