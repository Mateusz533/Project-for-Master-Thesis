#include "configuration.h"
#include "DisplayedElement.h"
#include "StaticArray.h"
#include "Queue.h"
#include "tabularConversion.h"
#include "reportError.h"
#pragma once

// Klasa przechowująca parametry czujnika nacisku
class ForceSensor : public DisplayedElement
{
  public:
    ForceSensor() = delete;
    // Konstruktor
    ForceSensor(short unsigned int pin_force_sensor) :
      PIN_FORCE_SENSOR_{ pin_force_sensor },
      SIGNAL_VALUES_{ FORCE_CONVERSION_ARRAY_SIZE, new const int[FORCE_CONVERSION_ARRAY_SIZE] FORCE_SIGNAL_VALUES },
      FORCE_VALUES_{ FORCE_CONVERSION_ARRAY_SIZE, new const int[FORCE_CONVERSION_ARRAY_SIZE] FORCE_VALUES }
    {
      // przypisanie numeru pinu w mikrosterowniku
    }
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
    int force_offset_{ 0 };
    // Kolejka przechowująca pomiary siły nacisku
    Queue<int> force_measurements_{ FORCE_ESTIMATION_PERIOD / CYCLE_PERIOD };
    // Numer pinu przypisanego do czujnika
    const short unsigned int PIN_FORCE_SENSOR_{ 0 };
    // Tablice konwersji pomiaru siły w poziomach ADC na Newtony
    const StaticArray<const int> SIGNAL_VALUES_;
    const StaticArray<const int> FORCE_VALUES_;
};

void ForceSensor::init()
{
  pinMode(PIN_FORCE_SENSOR_, INPUT);    // ustawienie czujnika nacisku jako wejście z napięciem zewnętrznym
  digitalWrite(PIN_FORCE_SENSOR_, LOW);
}

void ForceSensor::executeCommands(const bool buttons[])
{
  if (buttons[ACTION] && !is_overloaded_)
    force_offset_ = force_;
}

void ForceSensor::getDataToDisplay(String& first_line, String& second_line) const
{
  first_line = F("Force:       N  ");
  String str_force(force_ - force_offset_);
  for (unsigned int i = 0; i < str_force.length(); ++i)
    first_line.setCharAt(11 - i, str_force.charAt(str_force.length() - 1 - i));

  if (is_overloaded_)
    second_line = F("OVERLOADED !!!  ");
  else
    second_line = F("Maximum: 200 N  ");
}

void ForceSensor::run()
{
  force_measurements_.push(analogRead(PIN_FORCE_SENSOR_));

  if (!force_measurements_.isFull())
    return;

  float signal_value = force_measurements_.mean_value();    // filtrowanie szumów poprzez uśrednianie krótkookresowych odczytów

  if (signal_value > FORCE_SIGNAL_HIGHER_LIMIT)    // sprawdzenie poprawności sygnału
    reportError(F("2"));

  force_ = tabularConversion<const int, const int>(SIGNAL_VALUES_, FORCE_VALUES_, signal_value);    // konwersja poziomów ADC na Newtony

  is_overloaded_ = (force_ >= FORCE_VALUES_.get(FORCE_VALUES_.length() - 1));    // sprawdzenie przeciążenia czujnika

  force_measurements_.clear();
}
