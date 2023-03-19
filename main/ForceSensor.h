#include "configuration.h"
#include "SystemElement.h"
#include "StaticArray.h"
#include "Queue.h"
#include "tabularConversion.h"
#pragma once

// Klasa przechowująca parametry czujnika nacisku
class ForceSensor : public SystemElement
{
  public:
    // Konstruktor
    ForceSensor(short unsigned int pin_force_sensor) :
      PIN_FORCE_SENSOR_(pin_force_sensor),
      SIGNAL_VALUES_(14, new const int[14]{ 0, 29, 60, 83, 99, 115, 129, 154, 178, 195, 213, 242, 266, 289 }),
      FORCE_VALUES_(14, new const int[14]{ 0, 5, 10, 20, 30, 40, 50, 70, 90, 110, 130, 150, 170, 200 })
    {
      // przypisanie numeru pinu w mikrosterowniku
    }
    // Konfiguruje porty wejścia/wyjścia dla tego elementu
    void init();
    // Wykonuje pobrane od użytkownika polecania dotyczące wartości punktu zerowego czujnika siły
    void executeCommands(const bool buttons[]);
    // Wyświetla dane dotyczące czujnika siły
    void getDataToDisplay(String& first_line, String& second_line);
    // Wykonuje pomiar siły nacisku oraz uśrednia go z określoną częstotliwością
    void run();

  private:
    // Zmienne przechowujące aktualne informacje o sile nacisku
    bool is_overloaded_ = false;
    int force_ = 0;
    int force_offset_ = 0;
    // Kolejka przechowująca pomiary siły nacisku
    Queue<int> force_measurements_ = Queue<int>(FORCE_ESTIMATION_PERIOD / CYCLE_PERIOD);
    // Numer pinu przypisanego do czujnika
    const short unsigned int PIN_FORCE_SENSOR_ = 0;
    // Tablice konwersji pomiaru siły w poziomach ADC na Newtony
    StaticArray<const int> SIGNAL_VALUES_;
    StaticArray<const int> FORCE_VALUES_;
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

void ForceSensor::getDataToDisplay(String& first_line, String& second_line)
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

  force_ = tabularConversion<const int, const int>(SIGNAL_VALUES_, FORCE_VALUES_, signal_value);    // konwersja poziomów ADC na Newtony

  is_overloaded_ = (force_ >= FORCE_VALUES_[FORCE_VALUES_.length() - 1]);    // sprawdzenie przeciążenia czujnika

  force_measurements_.clear();
}
