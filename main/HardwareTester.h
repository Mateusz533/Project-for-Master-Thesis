#pragma once

#include "configuration.h"
#include "SystemElement.h"
#include "Queue.h"

// Klasa przeznaczona wyłącznie do testów sprzętu
class HardwareTester : public SystemElement
{
  public:
    HardwareTester() = default;
    ~HardwareTester() = default;
    // Konfiguruje porty wejścia/wyjścia dla tego elementu
    void init() override;
    // Wykonuje wszystkie funkcje elementu z odpowiednimi częstotliwościami
    void run() override;

  private:
    // Przekazuje przez port szeregowy dane o mierzonej sile nacisku
    void testForceSensor();
    // Przekazuje przez port szeregowy surowe dane o mierzonym sygnale temperatury
    void testTemperatureSensors();
    // Odczytuje sygnał z wyjścia wzmacniacza napięcia (w poziomach ADC)
    void testAmplifier(const uint8_t pin_output);
    // Przekazuje przez port szeregowy dane potrzebne do wyznaczenia charakterystyki cieplnej urządzenia
    void testHeaters();
    // Przekazuje przez port szeregowy dane o mierzonej sile nacisku w zależności od temperatury
    void testForceSensorThermalEffect();
    // Przekazuje dane o różnicy czasu w danej chwili od podanej wartości
    void testTimeSynchronization(const unsigned long timer);
    // Przekazuje informacje zarówno o pomiarze temperatury jak i siły nacisku
    void testAll();

    // Licznik umożliwiający testowanie synchronizacji programu
    unsigned long cycle_counter_{ 0 };
};
