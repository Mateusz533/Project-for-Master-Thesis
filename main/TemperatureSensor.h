#pragma once

#include "configuration.h"
#include "Queue.h"
#include "reportError.h"

constexpr unsigned int STORAGE_MULTIPLIER = 8;
constexpr CollectionSize LONG_TERM_MEASURMENT_BUFFER_LENGTH = TEMPERATURE_ESTIMATION_PERIOD / TEMPERATURE_AVERAGING_PERIOD;
constexpr CollectionSize SHORT_TERM_MEASURMENT_BUFFER_LENGTH = TEMPERATURE_AVERAGING_PERIOD / CYCLE_PERIOD;
constexpr float MIN_SN_RATIO = 0.5;
using StorageSignal = unsigned int;

// Klasa przechowująca parametry czujnika temperatury
class TemperatureSensor
{
  public:
    TemperatureSensor() = delete;
    TemperatureSensor(const uint8_t pin_temperature_sensor, const float tuning_factor, const float sensor_offset);
    ~TemperatureSensor() = default;
    // Konfiguruje porty wejścia/wyjścia dla tego elementu
    void init();
    // Wykonuje pomiary temperatury i filtruje dolnoprzepustowo z określoną częstotliwością
    void measureTemperature();
    // Zwraca oszacowaną różnicę temperatur pomiędzy płytą a otoczeniem na podstawie zebranych pomiarów
    KelvinDiff calculateRelativeTemperature();
    // Zwraca prawdę, jeśli buffor danych pomiarowych jest pełny
    bool isBufferFull() const;
    // Czyści buffor danych pomiarowych
    void clearBuffer();

  private:
    // Zmienne przechowujące pomiary sygnału temperatury
    Queue<StorageSignal, LONG_TERM_MEASURMENT_BUFFER_LENGTH> long_term_measurements_;
    Queue<StorageSignal, SHORT_TERM_MEASURMENT_BUFFER_LENGTH> short_term_measurements_;
    // Numer pinu przypisanego do czujnika
    const uint8_t PIN_TEMPERATURE_SENSOR_{ PIN_SAFETY_MOCK_INPUT };
    // Czynnik dostrojenia wzmacniacza oraz stałe przesunięcie sygnału
    const float TUNING_FACTOR_{ 1 };
    const float SENSOR_OFFSET_{ 0 };
};
