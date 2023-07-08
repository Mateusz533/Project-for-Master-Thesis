#include "HardwareTester.h"

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
  // testForceSensor();

  /*TEST CZUJNIKÓW TEMPERATTURY*/
  // testTemperatureSensors();

  /*TEST WZMACNIACZA*/
  // testAmplifier(PIN_TEMPERATURE_SENSOR_TOP);

  /*TEST GRZAŁEK*/
  // testHeaters();

  /*TEST REAKCJI CZUJNIKA NACISKU NA TEMPERATURĘ*/
  // testForceSensorThermalEffect();

  /*TEST SYNCHRONIZACJI CZASU*/
  // testTimeSynchronization(cycle_counter_);

  /*TEST WSZYSTKIEGO*/
  testAll();
}

void HardwareTester::testForceSensor()
{
  //// Niefiltrowane
  //Serial.print(analogRead(PIN_FORCE_SENSOR));
  //Serial.println();

  // Filtrowane
  static bool s_in_use = false;
  static Queue<unsigned int>* p_force_measurements{ nullptr };
  if (!s_in_use)
  {
    s_in_use = true;
    p_force_measurements = new Queue<unsigned int>(20);
  }
  p_force_measurements->push(analogRead(PIN_FORCE_SENSOR));

  if (!p_force_measurements->isFull())
    return;

  // Filtrowanie szumów poprzez uśrednianie krótkookresowych odczytów
  const unsigned int mean_value = p_force_measurements->mean_value();
  const unsigned int min_value = p_force_measurements->min_value();
  const unsigned int max_value = p_force_measurements->max_value();
  p_force_measurements->clear();

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

void HardwareTester::testAmplifier(const short unsigned int pin_output)
{
  //// Niefiltrowane
  // Serial.print(analogRead(pin_output));
  // Serial.println();

  // Filtrowane
  static bool s_in_use = false;
  static Queue<unsigned int>* p_measurements{ nullptr };
  if (!s_in_use)
  {
    s_in_use = true;
    p_measurements = new Queue<unsigned int>(100);
  }
  p_measurements->push(analogRead(pin_output));

  if (!p_measurements->isFull())
    return;

  // Filtrowanie szumów poprzez uśrednianie krótkookresowych odczytów
  const unsigned int mean_value = p_measurements->mean_value();
  const unsigned int min_value = p_measurements->min_value();
  const unsigned int max_value = p_measurements->max_value();
  const unsigned int median_value = p_measurements->quantile(0.5);
  p_measurements->clear();

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
  static bool s_in_use = false;
  static Queue<unsigned int>* p_top_measurements{ nullptr };
  static Queue<unsigned int>* p_bot_measurements{ nullptr };
  if (!s_in_use)
  {
    s_in_use = true;
    p_top_measurements = new Queue<unsigned int>(50);
    p_bot_measurements = new Queue<unsigned int>(50);
  }
  p_top_measurements->push(analogRead(PIN_TEMPERATURE_SENSOR_TOP));
  p_bot_measurements->push(analogRead(PIN_TEMPERATURE_SENSOR_BOT));

  if (!p_top_measurements->isFull())
    return;

  // Filtrowanie szumów poprzez uśrednianie krótkookresowych odczytów
  const unsigned int top_value = p_top_measurements->quantile(0.5);
  const unsigned int bot_value = p_bot_measurements->quantile(0.5);
  p_top_measurements->clear();
  p_bot_measurements->clear();

  Serial.print(top_value);
  Serial.print(F(","));
  Serial.print(bot_value);
  Serial.println();
}

void HardwareTester::testForceSensorThermalEffect()
{
  // Filtrowane
  static bool s_in_use = false;
  static Queue<unsigned int>* p_force_measurements{ nullptr };
  static Queue<unsigned int>* p_top_measurements{ nullptr };
  if (!s_in_use)
  {
    s_in_use = true;
    p_force_measurements = new Queue<unsigned int>(50);
    p_top_measurements = new Queue<unsigned int>(50);
  }
  p_force_measurements->push(analogRead(PIN_FORCE_SENSOR));
  p_top_measurements->push(analogRead(PIN_TEMPERATURE_SENSOR_TOP));

  if (!p_force_measurements->isFull())
    return;

  // Filtrowanie szumów poprzez uśrednianie krótkookresowych odczytów
  const unsigned int force_value = p_force_measurements->quantile(0.5);
  const unsigned int top_value = p_top_measurements->quantile(0.5);
  p_force_measurements->clear();
  p_top_measurements->clear();

  Serial.print(force_value);
  Serial.print(F(","));
  Serial.print(top_value);
  Serial.println();
}

void HardwareTester::testTimeSynchronization(const long unsigned int timer)
{
  Serial.print(millis() - timer);
  Serial.println();
}

void HardwareTester::testAll()
{
  // Filtrowane
  static bool s_in_use = false;
  static Queue<unsigned int>* p_force_measurements{ nullptr };
  static Queue<unsigned int>* p_top_measurements{ nullptr };
  static Queue<unsigned int>* p_bot_measurements{ nullptr };
  if (!s_in_use)
  {
    s_in_use = true;
    p_force_measurements = new Queue<unsigned int>(30);
    p_top_measurements = new Queue<unsigned int>(30);
    p_bot_measurements = new Queue<unsigned int>(30);
  }
  p_force_measurements->push(analogRead(PIN_FORCE_SENSOR));
  p_top_measurements->push(analogRead(PIN_TEMPERATURE_SENSOR_TOP));
  p_bot_measurements->push(analogRead(PIN_TEMPERATURE_SENSOR_BOT));

  if (!p_force_measurements->isFull())
    return;

  // Filtrowanie szumów poprzez uśrednianie krótkookresowych odczytów
  const unsigned int force_value = p_force_measurements->quantile(0.5);
  const unsigned int top_value = p_top_measurements->quantile(0.5);
  const unsigned int bot_value = p_bot_measurements->quantile(0.5);
  p_force_measurements->clear();
  p_top_measurements->clear();
  p_bot_measurements->clear();

  Serial.print(force_value);
  Serial.print(F(","));
  Serial.print(top_value);
  Serial.print(F(","));
  Serial.print(bot_value);
  Serial.println();
}
