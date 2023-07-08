#pragma once

#include <Arduino.h>
#include "configuration.h"
#include "DisplayedElement.h"
#include "StaticArray.h"
#include "Queue.h"
#include "tabularConversion.h"
#include "reportError.h"

// Klasa przechowująca parametry czujnika nacisku
class ForceSensor : public DisplayedElement
{
  public:
    ForceSensor() = delete;
    ForceSensor(const short unsigned int pin_force_sensor);
    ~ForceSensor() = default;
    // Konfiguruje porty wejścia/wyjścia dla tego elementu
    void init() override;
    // Wykonuje pomiar siły nacisku oraz uśrednia go z określoną częstotliwością
    void run() override;
    // Wykonuje pobrane od użytkownika polecania dotyczące wartości punktu zerowego czujnika siły
    void executeCommands(const bool buttons[]) override;
    // Wyświetla dane dotyczące czujnika siły
    void getDataToDisplay(String& first_line, String& second_line) const override;

  private:
    // Zmienne przechowujące aktualne informacje o sile nacisku
    bool is_overloaded_{ false };
    int force_{ 0 };
    int force_offset_{ START_OFFSET };
    // Kolejka przechowująca pomiary siły nacisku
    Queue<int> force_measurements_{ FORCE_ESTIMATION_PERIOD / CYCLE_PERIOD };
    // Numer pinu przypisanego do czujnika
    const short unsigned int PIN_FORCE_SENSOR_{ 0 };
    // Tablice konwersji pomiaru siły w poziomach ADC na Newtony
    const StaticArray<const int> SIGNAL_VALUES_;
    const StaticArray<const int> FORCE_VALUES_;
};
