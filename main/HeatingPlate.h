#pragma once

#include <Arduino.h>
#include "configuration.h"
#include "DisplayedElement.h"
#include "TemperatureSensor.h"
#include "Heater.h"
#include "StaticArray.h"
#include "tabularConversion.h"
#include "reportError.h"

// Klasa przechowująca parametry płyty grzewczej
class HeatingPlate : public DisplayedElement, public Heater
{
  public:
    HeatingPlate() = delete;
    // Konstruktor
    HeatingPlate(short unsigned int pin_temperature_sensor, short unsigned int pin_heat_supply, float tuning_factor, float sensor_offset, const String& name);
    ~HeatingPlate() = default;
    // Konfiguruje porty wejścia/wyjścia dla tego elementu
    void init() override;
    // Wykonuje wszystkie funkcje płyty z odpowiednimi dla nich częstotliwościami
    void run() override;
    // Wykonuje pobrane od użytkownika polecania dotyczące zadanej temperatury oraz stanu grzania
    void executeCommands(const bool buttons[]) override;
    // Wyświetla dane dotyczące płyty grzewczej
    void getDataToDisplay(String& first_line, String& second_line) const override;

  private:
    // Instatncja czujnika temperatury
    TemperatureSensor temperature_sensor{ 0, 1.0, 0.0 };
    // Wyświetlana nazwa grzałki
    const String DISPLAYED_NAME_;
};
