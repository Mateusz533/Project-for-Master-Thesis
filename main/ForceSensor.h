#pragma once

#include "DisplayedElement.h"
#include "Queue.h"
#include "TabularConverter.h"
#include "reportError.h"

constexpr CollectionSize MEASURMENT_BUFFER_LENGTH = FORCE_ESTIMATION_PERIOD / CYCLE_PERIOD;

// Klasa przechowująca parametry czujnika nacisku
class ForceSensor : public DisplayedElement
{
  public:
    ForceSensor() = delete;
    ForceSensor(const uint8_t pin_force_sensor);
    ~ForceSensor() = default;
    // Konfiguruje porty wejścia/wyjścia dla tego elementu
    void init() override;
    // Wykonuje pomiar siły nacisku oraz uśrednia go z określoną częstotliwością
    void run() override;
    // Wykonuje pobrane od użytkownika polecania dotyczące wartości punktu zerowego czujnika siły
    void executeCommands(const Array<bool, BUTTONS_NUMBER>& buttons) override;
    // Wyświetla dane dotyczące czujnika siły
    ScreenContent getDataToDisplay() const override;

  private:
    // Zmienne przechowujące aktualne informacje o sile nacisku
    bool is_overloaded_{ false };
    Newton force_{ 0 };
    Newton force_offset_{ START_OFFSET };
    // Kolejka przechowująca pomiary siły nacisku
    Queue<AnalogSignal, MEASURMENT_BUFFER_LENGTH> force_measurements_;
    // Numer pinu przypisanego do czujnika
    const uint8_t PIN_FORCE_SENSOR_{ PIN_SAFETY_MOCK_INPUT };
    // Tablice konwersji pomiaru siły w poziomach ADC na Newtony
    const TabularConverter<AnalogSignal, Newton, FORCE_CONVERSION_ARRAY_SIZE> SIGNAL_TO_FORCE_CONVERTER_;
};
