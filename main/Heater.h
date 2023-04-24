#pragma once

#include <Arduino.h>
#include "configuration.h"
#include "SystemElement.h"
#include "StaticArray.h"
#include "tabularConversion.h"
#include "reportError.h"

// Klasa przechowująca parametry grzałki
class Heater : public SystemElement
{
  public:
    Heater() = delete;
    // Konstruktor
    Heater(short unsigned int pin_heat_supply);
    ~Heater() = default;
    // Konfiguruje porty wejścia/wyjścia dla tego elementu
    void init() override;
    // Włącza/wyłącza grzanie w podczas każdego cyklu zgodnie z zadaną wartością
    void manageHeating();
    // Ustawia moc grzania potrzebną do osiągnięcia zadanej temperatury
    void regulateHeatingPower();

  protected:
    // Zwraca obliczoną moc grzania, potrzebną do optymalnej regulacji temperatury w danej chwili
    int calculateRegulatedHeatingPower();
    // Ustawia wprowadzoną moc grzania [W]
    void setHeatingPower(const int new_heating_power);

    // Zmienne przechowujące zadane wartości grzania
    bool is_heating_set_{ false };
    int heating_power_{ 0 };
    int set_temperature_{ 200 };
    float real_temperature_{ ambient_temperature };
    // Zmienne regulacji
    bool active_regulation_{ false };
    float temperature_deviation_{ 0 };     // [K]
    float temperature_derivative_{ 0 };    // [K/s]
    float temperature_integral_{ 0 };      // [K*s]
    // Zmienna diagnostyczna
    float prediction_error_{ 0 };
    // Numer pinu przypisanego do grzałki
    const short unsigned int PIN_HEAT_SUPPLY_{ 1 };
    // Tablice konwersji temperatury na moc w stanie ustalonym
    const StaticArray<const int> TEMPERATURE_VALUES_;
    const StaticArray<const int> HEATING_POWER_VALUES_;
};
