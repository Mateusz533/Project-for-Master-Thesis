#pragma once

#include <Arduino.h>
#include "configuration.h"

// Klasa przechowująca parametry grzałki
class Heater
{
  public:
    Heater() = delete;
    // Konstruktor
    Heater(short unsigned int pin_heat_supply);
    ~Heater() = default;
    // Konfiguruje porty wejścia/wyjścia dla tego elementu
    void init();
    // Zmienia ustawioną wartość mocy grzania podczas każdego cyklu umożliwiając jej sprzętową interpolację do wartości zadanej
    void run();
    // Ustawia wprowadzoną moc grzania [W]
    void setHeatingPower(const float new_heating_power);
    // Zwraca aktualną moc grzania [W]
    float getHeatingPower() const;

  private:
    // Zmienna przechowująca zadaną wartość grzania
    float heating_power_{ 0 };
    unsigned int power_switching_counter_{ 0 };
    // Numer pinu przypisanego do grzałki
    const short unsigned int PIN_HEAT_SUPPLY_{ 1 };
};
