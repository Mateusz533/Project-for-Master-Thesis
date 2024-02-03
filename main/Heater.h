#pragma once

#include "configuration.h"

// Klasa przechowująca parametry grzałki
class Heater
{
  public:
    Heater() = delete;
    Heater(const uint8_t pin_heat_supply);
    ~Heater() = default;
    // Konfiguruje porty wejścia/wyjścia dla tego elementu
    void init();
    // Zmienia ustawiony sygnał mocy grzania podczas każdego cyklu
    // umożliwiając jej sprzętową interpolację do wartości zadanej
    void run();
    // Ustawia wprowadzoną moc grzania [W]
    void setHeatingPower(const Watt new_heating_power);
    // Zwraca aktualną moc grzania [W]
    Watt getHeatingPower() const;

  private:
    // Ustawia zmodulowany synał mocy z podanym wypełnieniem impulsu
    inline void setPowerModulation(const SignlalPWM heating_signal);

    // Zadana wartość grzania
    Watt heating_power_{ 0 };
    unsigned int power_switching_counter_{ 0 };
    // Numer pinu przypisanego do grzałki
    const uint8_t PIN_HEAT_SUPPLY_{ PIN_SAFETY_MOCK_OUTPUT };
};
