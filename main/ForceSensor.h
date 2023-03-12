#include "configuration.h"
#include "SystemElement.h"
#include "StaticArray.h"
#pragma once

// Klasa przechowująca parametry czujnika nacisku
class ForceSensor : public SystemElement
{
  public:
    // Konstruktor
    ForceSensor(short unsigned int pin_force_sensor) :
      PIN_FORCE_SENSOR_(pin_force_sensor)
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
    // Tablica z pomiarami siły
    StaticArray<int> force_measurements_ = StaticArray<int>(FORCE_ESTIMATION_PERIOD / CYCLE_PERIOD);
    // Numer pinu przypisanego do czujnika
    const short unsigned int PIN_FORCE_SENSOR_ = 0;
    // Tablice konwersji pomiaru siły w bitach na Newtony
    static const short int CONVERSION_ARRAYS_SIZE_ = 14;
    const int SIGNAL_VALUES_[CONVERSION_ARRAYS_SIZE_] = { 0, 29, 60, 83, 99, 115, 129, 154, 178, 195, 213, 242, 266, 289 };
    const int FORCE_VALUES_[CONVERSION_ARRAYS_SIZE_] = { 0, 5, 10, 20, 30, 40, 50, 70, 90, 110, 130, 150, 170, 200 };
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
  first_line = F("Force:  ");
  if (abs(force_ - force_offset_) < 100)
    first_line += F(" ");
  if (abs(force_ - force_offset_) < 10)
    first_line += F(" ");
  if (force_ - force_offset_ < 0)
    first_line += F("-");
  else
    first_line += F(" ");
  first_line += static_cast<const String>(abs(force_ - force_offset_)) + F(" N  ");

  if (is_overloaded_)
    second_line = F("OVERLOADED !!!  ");
  else
    second_line = F("Maximum: 200 N  ");
}

void ForceSensor::run()
{
  static unsigned int force_measuring_counter = 0;
  force_measurements_[force_measuring_counter] = analogRead(PIN_FORCE_SENSOR_);

  ++force_measuring_counter;
  if (force_measuring_counter < force_measurements_.length())
    return;

  force_measuring_counter = 0;
  float signal_value = force_measurements_.mean_value();    // filtrowanie szumów poprzez uśrednianie krótkookresowych odczytów

  for (unsigned int i = 1; i < CONVERSION_ARRAYS_SIZE_; ++i)    // konwersja bitów na Newtony
  {
    if (signal_value < SIGNAL_VALUES_[i])
    {
      force_ = round(
        FORCE_VALUES_[i - 1] + (signal_value - SIGNAL_VALUES_[i - 1]) * (FORCE_VALUES_[i] - FORCE_VALUES_[i - 1]) / (SIGNAL_VALUES_[i] - SIGNAL_VALUES_[i - 1]));
      is_overloaded_ = false;
      break;
    }
    else if (i == CONVERSION_ARRAYS_SIZE_ - 1)
    {
      force_ = FORCE_VALUES_[i];
      is_overloaded_ = true;
    }
  }
}
