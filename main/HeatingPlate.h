#include "configuration.h"
#include "SystemElement.h"
#include "TemperatureSensor.h"
#include "StaticArray.h"
#include "tabularConversion.h"
#include "reportError.h"
#pragma once

// Klasa przechowująca parametry płyty grzewczej
class HeatingPlate : public SystemElement, public TemperatureSensor
{
  public:
    // Konstruktor
    HeatingPlate(short unsigned int pin_temperature_sensor, short unsigned int pin_heat_supply, float tuning_factor, float sensor_offset, const String& name) :
      TemperatureSensor(pin_temperature_sensor, tuning_factor, sensor_offset),
      PIN_HEAT_SUPPLY_(pin_heat_supply),
      DISPLAYED_NAME_(name),
      TEMPERATURE_VALUES_(TEMPERATURE_TO_POWER_CONVERSION_ARRAY_SIZE, new const int[TEMPERATURE_TO_POWER_CONVERSION_ARRAY_SIZE] TEMPERATURE_VALUES),
      HEATING_POWER_VALUES_(TEMPERATURE_TO_POWER_CONVERSION_ARRAY_SIZE, new const int[TEMPERATURE_TO_POWER_CONVERSION_ARRAY_SIZE] HEATING_POWER_VALUES)

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
    // Włącza/wyłącza grzanie w podczas każdego cyklu zgodnie z zadaną wartością
    void switchHeating();
    // Zwraca obliczoną moc grzania, potrzebną do optymalnej regulacji temperatury w danej chwili
    int calculateRegulatedHeatingPower();
    // Ustawia moc grzania potrzebną do osiągnięcia zadanej temperatury
    void regulateHeatingPower();
    // Ustawia wprowadzoną moc grzania [W]
    void setHeatingPower(int heating_power);

    // Zmienne przechowujące zadane wartości grzania
    bool is_heating_set_ = false;
    int set_temperature_ = 200;
    int real_temperature_ = 20;
    // Zmienne regulacji
    bool active_regulation_ = false;
    int temperature_deviation_ = 0;     // [K]
    int temperature_derivative_ = 0;    // [K/s]
    int temperature_integral_ = 0;      // [K*s]
    // Numer pinu przypisanego do grzałki
    const short unsigned int PIN_HEAT_SUPPLY_ = 1;
    // Wyświetlana nazwa grzałki
    const String DISPLAYED_NAME_;
    // Tablice konwersji temperatury na moc w stanie ustalonym
    StaticArray<const int> TEMPERATURE_VALUES_;
    StaticArray<const int> HEATING_POWER_VALUES_;
};

void HeatingPlate::init()
{
  TemperatureSensor::init();               // konfiguracja czujnika temperatury
  pinMode(PIN_HEAT_SUPPLY_, OUTPUT);       // ustawienie kanału przekaźnika jako wyjście
  digitalWrite(PIN_HEAT_SUPPLY_, HIGH);    // początkowy stan wysoki odpowiada wyłączonemu kanałowi przekaźnika
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
  switchHeating();

  // Pomiar temperatury na płycie
  measureTemperature();

  // Ustawienie mocy grzania w zależności od obecnej temperatury
  if (long_temperature_measurements_.isFull())
  {
    int estimated_temperature = ambient_temperature + calculateRelativeTemperature();
    // if (abs(real_temperature_ - estimated_temperature) > MAX_TEMPERATURE_GROWTH * TEMPERATURE_ESTIMATION_PERIOD / 1000)    // sprawdzenie poprawności sygnału
    //   reportError(F("4"));

    real_temperature_ = estimated_temperature;
    long_temperature_measurements_.clear();
    regulateHeatingPower();
  }
}

void HeatingPlate::switchHeating()
{
  if (!is_heating_set_)
    setHeatingPower(0);
  else if (real_temperature_ - set_temperature_ > TEMPERATURE_REGULATION_RANGE)
    setHeatingPower(0);
  else if (real_temperature_ - set_temperature_ < -TEMPERATURE_REGULATION_RANGE)
    setHeatingPower(max_heating_power);
}

void HeatingPlate::regulateHeatingPower()
{
  temperature_derivative_ = 1.0 * (real_temperature_ - set_temperature_ - temperature_deviation_) * 1000 / TEMPERATURE_ESTIMATION_PERIOD;    // zamiana ms w s
  temperature_deviation_ = real_temperature_ - set_temperature_;

  if (!is_heating_set_)
    return;

  temperature_integral_ += 1.0 * temperature_deviation_ * TEMPERATURE_ESTIMATION_PERIOD / 1000;

  int heat_supply = 0;
  if (temperature_deviation_ < -TEMPERATURE_REGULATION_RANGE)    // procedura regulacji
  {
    active_regulation_ = false;
    heat_supply = max_heating_power;
    temperature_integral_ = 0;
  }
  else if (temperature_deviation_ > TEMPERATURE_REGULATION_RANGE)
  {
    active_regulation_ = false;
    heat_supply = 0;
    temperature_integral_ = 0;
  }
  else    // regulator PID
  {
    heat_supply = calculateRegulatedHeatingPower();
  }
  setHeatingPower(heat_supply);

  // Sprawdzenie poprawności grzania
  float predicted_temperature_change = 5e-6 * (heat_supply - tabularConversion(TEMPERATURE_VALUES_, HEATING_POWER_VALUES_, real_temperature_)) * TEMPERATURE_ESTIMATION_PERIOD;
  float real_temperature_change = (temperature_derivative_ * TEMPERATURE_ESTIMATION_PERIOD / 1000) % PRESET_TEMPERATURE_RESOLUTION;
  if (2 * abs(real_temperature_change) > PRESET_TEMPERATURE_RESOLUTION)
    real_temperature_change -= PRESET_TEMPERATURE_RESOLUTION * abs(real_temperature_change) / real_temperature_change;
  static float prediction_error = 0;
  float current_error = real_temperature_change - predicted_temperature_change;
  current_error = current_error == 0 ? 0 : current_error / max(abs(real_temperature_change), abs(predicted_temperature_change));
  prediction_error = 0.9 * prediction_error + 0.1 * current_error;
  if (abs(prediction_error) > 0.9)
    reportError(F("5"));
}

int HeatingPlate::calculateRegulatedHeatingPower()
{
  static int heating_power = 0;
  if (!active_regulation_)
  {
    active_regulation_ = true;
    heating_power = tabularConversion<const int, const int>(TEMPERATURE_VALUES_, HEATING_POWER_VALUES_, set_temperature_);
  }
  int power_deviation = round(-DERIVATIVE_REGULATION_COEFFICIENT * temperature_derivative_ - PROPORTIONAL_REGULATION_COEFFICIENT * temperature_deviation_
                              - INTEGRAL_REGULATION_COEFFICIENT * temperature_integral_);
  heating_power = constrain(heating_power + power_deviation, 0, max_heating_power);
  return heating_power;
}

void HeatingPlate::setHeatingPower(int heating_power)
{
  heating_power = constrain(heating_power, 0, max_heating_power);
  heating_power = map(heating_power, 0, max_heating_power, 0, MAX_HEAT_SIGNAL);
  analogWrite(PIN_HEAT_SUPPLY_, MAX_HEAT_SIGNAL - heating_power);    // przekaźnik wyzwalany stanem niskim
}
