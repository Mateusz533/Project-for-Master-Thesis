#include "configuration.h"
#include "DisplayedElement.h"
#include "TemperatureSensor.h"
#include "Heater.h"
#include "StaticArray.h"
#include "tabularConversion.h"
#include "reportError.h"
#pragma once

// Klasa przechowująca parametry płyty grzewczej
class HeatingPlate : public DisplayedElement, public Heater
{
  public:
    // Konstruktor
    HeatingPlate(short unsigned int pin_temperature_sensor, short unsigned int pin_heat_supply, float tuning_factor, float sensor_offset, const String& name) :
      Heater(pin_heat_supply),
      temperature_sensor(pin_temperature_sensor, tuning_factor, sensor_offset),
      DISPLAYED_NAME_(name)
    {
      // przypisanie numerów pinów w mikrosterowniku
    }
    // Konfiguruje porty wejścia/wyjścia dla tego elementu
    void init();
    // Wykonuje pobrane od użytkownika polecania dotyczące zadanej temperatury oraz stanu grzania
    void executeCommands(const bool buttons[]);
    // Wyświetla dane dotyczące płyty grzewczej
    void getDataToDisplay(String& first_line, String& second_line) const;
    // Wykonuje wszystkie funkcje płyty z odpowiednimi dla nich częstotliwościami
    void run();

  private:
    // Instatncja czujnika temperatury
    TemperatureSensor temperature_sensor = TemperatureSensor(0, 1.0, 0.0);
    // Wyświetlana nazwa grzałki
    const String DISPLAYED_NAME_;
};

void HeatingPlate::init()
{
  Heater::init();               // konfiguracja grzałki
  temperature_sensor.init();    // konfiguracja czujnika temperatury
}

void HeatingPlate::executeCommands(const bool buttons[])
{
  if (buttons[UP] && !buttons[DOWN] && set_temperature_ < MAX_TEMPERATURE)
  {
    set_temperature_ += PRESET_TEMPERATURE_RESOLUTION;
    active_regulation_ = false;
  }
  else if (!buttons[UP] && buttons[DOWN] && set_temperature_ > MIN_TEMPERATURE)
  {
    set_temperature_ -= PRESET_TEMPERATURE_RESOLUTION;
    active_regulation_ = false;
  }
  if (buttons[ACTION])
    is_heating_set_ = !is_heating_set_;
}

void HeatingPlate::getDataToDisplay(String& first_line, String& second_line) const
{
  first_line = DISPLAYED_NAME_ + F(" temp:    ") + DEGREE_SYMBOL_INDEX + F("C ");
  String str_real_temp(real_temperature_);
  for (unsigned int i = 0; i < str_real_temp.length(); ++i)
    first_line.setCharAt(12 - i, str_real_temp.charAt(str_real_temp.length() - 1 - i));

  second_line = String(F("Set:    ")) + DEGREE_SYMBOL_INDEX + F("C   ");
  String str_set_temp(set_temperature_);
  for (unsigned int i = 0; i < str_set_temp.length(); ++i)
    second_line.setCharAt(7 - i, str_set_temp.charAt(str_set_temp.length() - 1 - i));
  second_line += is_heating_set_ ? F(" ON") : F("OFF");
}

void HeatingPlate::run()
{
  // Włączenie/wyłączenie grzania - musi następować szybciej niż odświeżanie stanu temperatury
  manageHeating();

  // Pomiar temperatury na płycie
  temperature_sensor.measureTemperature();

  // Ustawienie mocy grzania w zależności od obecnej temperatury
  if (temperature_sensor.isBufferFull())
  {
    int estimated_temperature = ambient_temperature + temperature_sensor.calculateRelativeTemperature();
    // if (abs(real_temperature_ - estimated_temperature) > MAX_TEMPERATURE_GROWTH * TEMPERATURE_ESTIMATION_PERIOD / 1000)    // sprawdzenie poprawności sygnału
    //   reportError(F("4"));

    real_temperature_ = estimated_temperature;
    temperature_sensor.clearBuffer();
    regulateHeatingPower();
  }
}
