#pragma once

#include "DisplayedElement.h"
#include "TemperatureSensor.h"
#include "HeatingRegulator.h"
#include "reportError.h"

constexpr int DEFAULT_START_TEMPERATURE = 200;

// Klasa przechowująca parametry płyty grzewczej
class HeatingPlate : public DisplayedElement
{
  public:
    HeatingPlate() = delete;
    HeatingPlate(const uint8_t pin_temperature_sensor, const uint8_t pin_heat_supply, const float tuning_factor,
                 const float sensor_offset, const String& name);
    ~HeatingPlate() = default;
    // Konfiguruje porty wejścia/wyjścia dla tego elementu
    void init() override;
    // Wykonuje wszystkie funkcje płyty z odpowiednimi dla nich częstotliwościami
    void run() override;
    // Wykonuje pobrane od użytkownika polecania dotyczące zadanej temperatury oraz stanu grzania
    void executeCommands(const Array<bool, BUTTONS_NUMBER>& buttons) override;
    // Wyświetla dane dotyczące płyty grzewczej
    ScreenContent getDataToDisplay() const override;

  private:
    // Instatncja czujnika temperatury
    TemperatureSensor temperature_sensor_{ PIN_SAFETY_MOCK_INPUT, 1.0, 0.0 };
    // Instatncja regulatora grzania
    HeatingRegulator regulator_{ PIN_SAFETY_MOCK_OUTPUT };
    // Zmienne przechowujące wyświetlane parametry grzania
    bool is_heating_set_{ false };
    DegCelsius target_temperature_{ DEFAULT_START_TEMPERATURE };
    KelvinDiff real_temperature_{ static_cast<KelvinDiff>(s_ambient_temperature) };
    // Wyświetlana nazwa grzałki
    const String DISPLAYED_NAME_;
};
