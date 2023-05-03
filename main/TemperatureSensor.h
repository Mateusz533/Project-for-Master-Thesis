#pragma once

#include <Arduino.h>
#include "configuration.h"
#include "StaticArray.h"
#include "Queue.h"
#include "reportError.h"

// Klasa przechowująca parametry czujnika temperatury
class TemperatureSensor
{
  public:
    TemperatureSensor() = delete;
    TemperatureSensor(short unsigned int pin_temperature_sensor, float tuning_factor, float sensor_offset);
    ~TemperatureSensor() = default;
    // Konfiguruje porty wejścia/wyjścia dla tego elementu
    void init();
    // Wykonuje pomiary temperatury i filtruje dolnoprzepustowo z określoną częstotliwością
    void measureTemperature();
    // Zwraca oszacowaną różnicę temperatur pomiędzy płytą a otoczeniem na podstawie zebranych pomiarów
    float calculateRelativeTemperature();
    // Zwraca prawdę, jeśli buffor danych pomiarowych jest pełny
    bool isBufferFull() const;
    // Czyści buffor danych pomiarowych
    void clearBuffer();

  private:
    // Zmienne przechowujące pomiary sygnału temperatury
    Queue<unsigned int> long_term_measurements_{ TEMPERATURE_ESTIMATION_PERIOD / TEMPERATURE_AVERAGING_PERIOD };
    Queue<unsigned int> short_term_measurements_{ TEMPERATURE_AVERAGING_PERIOD / CYCLE_PERIOD };
    // Numer pinu przypisanego do czujnika
    const short unsigned int PIN_TEMPERATURE_SENSOR_{ 0 };
    // Czynnik dostrojenia wzmacniacza oraz stałe przesunięcie sygnału
    const float TUNING_FACTOR_{ 1 };
    const float SENSOR_OFFSET_{ 0 };
};
