#include "configuration.h"
#include "SystemElement.h"
#include "Queue.h"
#pragma once

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
    void testAmplifier(short unsigned int pin_output);
    // Przekazuje przez port szeregowy dane potrzebne do wyznaczenia charakterystyki cieplnej urządzenia
    void testHeaters();
    // Przekazuje dane o różnicy czasu w danej chwili od podanej wartości
    void testTimeSynchronization(long unsigned int timer);

    // Licznik umożliwiający testowanie synchronizacji programu
    long unsigned int cycle_counter_{ 0 };
};

void HardwareTester::init()
{
  // Konfiguracja portu szeregowego
  Serial.begin(9600);
  cycle_counter_ = millis() + CYCLE_PERIOD;    // ustawienie wartości licznika czasu
}

void HardwareTester::run()
{
  cycle_counter_ += CYCLE_PERIOD;

  /*TEST CZUJNIKA NACISKU*/
  testForceSensor();

  /*TEST CZUJNIKÓW TEMPERATTURY*/
  // testTemperatureSensors();

  /*TEST WZMACNIACZA*/
  // testAmplifier(PIN_TEMPERATURE_SENSOR_TOP);

  /*TEST GRZAŁEK*/
  // testHeaters();

  /*TEST SYNCHRONIZACJI CZASU*/
  // testTimeSynchronization(cycle_counter_);
}

void HardwareTester::testForceSensor()
{
  //// Niefiltrowane
  //Serial.print(analogRead(PIN_FORCE_SENSOR));
  //Serial.println();

  // Filtrowane
  static Queue<unsigned int> force_measurements(20);
  force_measurements.push(analogRead(PIN_FORCE_SENSOR));

  if (!force_measurements.isFull())
    return;

  // Filtrowanie szumów poprzez uśrednianie krótkookresowych odczytów
  unsigned int mean_value = force_measurements.mean_value();
  unsigned int min_value = force_measurements.min_value();
  unsigned int max_value = force_measurements.max_value();
  force_measurements.clear();

  Serial.print(mean_value);
  Serial.print(F(","));
  Serial.print(min_value);
  Serial.print(F(","));
  Serial.print(max_value);
  Serial.print(F(","));
  Serial.println();
}

void HardwareTester::testTemperatureSensors()
{
  Serial.print(analogRead(PIN_TEMPERATURE_SENSOR_TOP));
  Serial.print(F(","));
  Serial.print(analogRead(PIN_TEMPERATURE_SENSOR_BOT));
  Serial.println();
}

void HardwareTester::testAmplifier(short unsigned int pin_output)
{
  //// Niefiltrowane
  // Serial.print(analogRead(pin_output));
  // Serial.println();

  // Filtrowane
  static Queue<unsigned int> measurements(100);
  measurements.push(analogRead(pin_output));

  if (!measurements.isFull())
    return;

  // Filtrowanie szumów poprzez uśrednianie krótkookresowych odczytów
  unsigned int mean_value = measurements.mean_value();
  unsigned int min_value = measurements.min_value();
  unsigned int max_value = measurements.max_value();
  unsigned int median_value = measurements.quantile(0.5);
  measurements.clear();

  Serial.print(mean_value);
  Serial.print(F(","));
  Serial.print(min_value);
  Serial.print(F(","));
  Serial.print(max_value);
  Serial.print(F(","));
  Serial.print(median_value);
  Serial.println();
}

void HardwareTester::testHeaters()
{
  // Filtrowane
  static Queue<unsigned int> top_measurements(50);
  top_measurements.push(analogRead(PIN_TEMPERATURE_SENSOR_TOP));
  static Queue<unsigned int> bot_measurements(50);
  bot_measurements.push(analogRead(PIN_TEMPERATURE_SENSOR_BOT));

  if (!top_measurements.isFull())
    return;

  // Filtrowanie szumów poprzez uśrednianie krótkookresowych odczytów
  unsigned int top_value = top_measurements.quantile(0.5);
  unsigned int bot_value = bot_measurements.quantile(0.5);
  top_measurements.clear();
  bot_measurements.clear();

  Serial.print(top_value);
  Serial.print(F(","));
  Serial.print(bot_value);
  Serial.println();
}

void HardwareTester::testTimeSynchronization(long unsigned int timer)
{
  Serial.print(millis() - timer);
  Serial.println();
}
