#pragma once

#include "configuration.h"
#include "Heater.h"
#include "SystemElement.h"
#include "TabularConverter.h"
#include "reportError.h"

// Klasa przechowująca parametry regulatora grzania
class HeatingRegulator : public SystemElement
{
  public:
    HeatingRegulator() = delete;
    HeatingRegulator(const uint8_t pin_heat_supply);
    ~HeatingRegulator() = default;
    // Konfiguruje porty wejścia/wyjścia dla tego elementu
    void init() override;
    // Włącza/wyłącza grzanie w podczas każdego cyklu zgodnie z zadaną wartością
    void run() override;
    // Ustawia moc grzania potrzebną do osiągnięcia zadanej temperatury lub wyłącza je zależnie od wartości argumentu
    void refreshHeatingPower(const DegCelsius target_temperature, const KelvinDiff real_temperature, const bool is_heating_set);
    // Zeruje wartość skumulowanej odchyłki temperatury oraz zmienne regulacji
    void reset();

  private:
    // Zwraca obliczoną moc grzania, potrzebną do optymalnej regulacji temperatury w danej chwili
    inline Watt calculateRegulatedHeatingPower(const DegCelsius target_temperature);

    // Instatncja grzałki
    Heater heater_{ PIN_SAFETY_MOCK_OUTPUT };
    // Zmienne regulacji
    bool active_regulation_{ false };
    KelvinDiff temperature_deviation_{ 0 };
    KelvinPerSec temperature_derivative_{ 0 };
    KelvinSec temperature_integral_{ 0 };
    // Zmienna diagnostyczna
    float prediction_error_{ 0 };
    // Tablice konwersji temperatury na moc w stanie ustalonym
    const TabularConverter<DegCelsius, StorageWatt, TEMPERATURE_TO_POWER_CONVERSION_ARRAY_SIZE> TEMPERATURE_TO_HEATING_POWER_CONVERTER_;
};
