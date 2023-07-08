#pragma once

#include <Arduino.h>
#include "configuration.h"
#include "Heater.h"
#include "SystemElement.h"
#include "StaticArray.h"
#include "tabularConversion.h"
#include "reportError.h"

// Klasa przechowująca parametry regulatora grzania
class HeatingRegulator : public SystemElement
{
  public:
    HeatingRegulator() = delete;
    HeatingRegulator(const short unsigned int pin_heat_supply);
    ~HeatingRegulator() = default;
    // Konfiguruje porty wejścia/wyjścia dla tego elementu
    void init() override;
    // Włącza/wyłącza grzanie w podczas każdego cyklu zgodnie z zadaną wartością
    void run() override;
    // Ustawia moc grzania potrzebną do osiągnięcia zadanej temperatury lub wyłącza je zależnie od wartości argumentu
    void refreshHeatingPower(const int set_temperature, const float real_temperature, const bool is_heating_set);
    // Zeruje wartość skumulowanej odchyłki temperatury oraz zmienne regulacji
    void reset();

  private:
    // Zwraca obliczoną moc grzania, potrzebną do optymalnej regulacji temperatury w danej chwili
    float calculateRegulatedHeatingPower(const int set_temperature);

    // Instatncja grzałki
    Heater heater_{ 1 };
    // Zmienne regulacji
    bool active_regulation_{ false };
    float temperature_deviation_{ 0 };     // [K]
    float temperature_derivative_{ 0 };    // [K/s]
    float temperature_integral_{ 0 };      // [K*s]
    // Zmienna diagnostyczna
    float prediction_error_{ 0 };
    // Tablice konwersji temperatury na moc w stanie ustalonym
    const StaticArray<const int> TEMPERATURE_VALUES_;
    const StaticArray<const int> HEATING_POWER_VALUES_;
};
