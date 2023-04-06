#include "configuration.h"
#include "SystemElement.h"
#pragma once

// Klasa przechowująca parametry grzałki
class Heater : public SystemElement
{
  public:
    // Konstruktor
    Heater(short unsigned int pin_heat_supply) :
      PIN_HEAT_SUPPLY_(pin_heat_supply),
      TEMPERATURE_VALUES_(TEMPERATURE_TO_POWER_CONVERSION_ARRAY_SIZE, new const int[TEMPERATURE_TO_POWER_CONVERSION_ARRAY_SIZE] TEMPERATURE_VALUES),
      HEATING_POWER_VALUES_(TEMPERATURE_TO_POWER_CONVERSION_ARRAY_SIZE, new const int[TEMPERATURE_TO_POWER_CONVERSION_ARRAY_SIZE] HEATING_POWER_VALUES)
    {
      // przypisanie numeru pinu w mikrosterowniku
    }
    // Konfiguruje porty wejścia/wyjścia dla tego elementu
    void init();
    // Włącza/wyłącza grzanie w podczas każdego cyklu zgodnie z zadaną wartością
    void manageHeating();
    // Ustawia moc grzania potrzebną do osiągnięcia zadanej temperatury
    void regulateHeatingPower();

  protected:
    // Zwraca obliczoną moc grzania, potrzebną do optymalnej regulacji temperatury w danej chwili
    int calculateRegulatedHeatingPower();
    // Ustawia wprowadzoną moc grzania [W]
    void setHeatingPower(int new_heating_power);

    // Zmienne przechowujące zadane wartości grzania
    bool is_heating_set_ = false;
    int heating_power_ = 0;
    int set_temperature_ = 200;
    int real_temperature_ = 20;
    // Zmienne regulacji
    bool active_regulation_ = false;
    int temperature_deviation_ = 0;     // [K]
    int temperature_derivative_ = 0;    // [K/s]
    int temperature_integral_ = 0;      // [K*s]
    // Numer pinu przypisanego do grzałki
    const short unsigned int PIN_HEAT_SUPPLY_ = 1;
    // Tablice konwersji temperatury na moc w stanie ustalonym
    const StaticArray<const int> TEMPERATURE_VALUES_;
    const StaticArray<const int> HEATING_POWER_VALUES_;
};

void Heater::init()
{
  pinMode(PIN_HEAT_SUPPLY_, OUTPUT);       // ustawienie kanału przekaźnika jako wyjście
  digitalWrite(PIN_HEAT_SUPPLY_, HIGH);    // początkowy stan wysoki odpowiada wyłączonemu kanałowi przekaźnika
}

void Heater::manageHeating()
{
  if (!is_heating_set_)
    setHeatingPower(0);
  else if (real_temperature_ - set_temperature_ > TEMPERATURE_REGULATION_RANGE)
    setHeatingPower(0);
  else if (real_temperature_ - set_temperature_ < -TEMPERATURE_REGULATION_RANGE)
    setHeatingPower(max_heating_power);
}

void Heater::regulateHeatingPower()
{
  temperature_derivative_ = 1.0 * (real_temperature_ - set_temperature_ - temperature_deviation_) * 1000 / TEMPERATURE_ESTIMATION_PERIOD;    // zamiana ms w s
  temperature_deviation_ = real_temperature_ - set_temperature_;

  if (!is_heating_set_)
    return;

  temperature_integral_ += 1.0 * temperature_deviation_ * TEMPERATURE_ESTIMATION_PERIOD / 1000;
  // temperature_integral_ = constrain(temperature_integral_, -max_heating_power / INTEGRAL_REGULATION_COEFFICIENT, max_heating_power / INTEGRAL_REGULATION_COEFFICIENT);

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
  // float predicted_temperature_change = 5e-6 * (heat_supply - tabularConversion(TEMPERATURE_VALUES_, HEATING_POWER_VALUES_, real_temperature_)) * TEMPERATURE_ESTIMATION_PERIOD;
  // float real_temperature_change = (temperature_derivative_ * TEMPERATURE_ESTIMATION_PERIOD / 1000) % PRESET_TEMPERATURE_RESOLUTION;
  // if (2 * abs(real_temperature_change) > PRESET_TEMPERATURE_RESOLUTION)
  //   real_temperature_change -= PRESET_TEMPERATURE_RESOLUTION * abs(real_temperature_change) / real_temperature_change;
  // static float prediction_error = 0;
  // float current_error = real_temperature_change - predicted_temperature_change;
  // current_error = current_error == 0 ? 0 : current_error / max(abs(real_temperature_change), abs(predicted_temperature_change));
  // prediction_error = 0.9 * prediction_error + 0.1 * current_error;
  // if (abs(prediction_error) > 0.9)
  //   reportError(F("5"));
}

int Heater::calculateRegulatedHeatingPower()
{
  int power_deviation = round(-DERIVATIVE_REGULATION_COEFFICIENT * temperature_derivative_ - PROPORTIONAL_REGULATION_COEFFICIENT * temperature_deviation_
                              - INTEGRAL_REGULATION_COEFFICIENT * temperature_integral_);
  int new_heating_power = constrain(heating_power_ + power_deviation, 0, max_heating_power);
  if (!active_regulation_)
  {
    active_regulation_ = true;
    new_heating_power = tabularConversion<const int, const int>(TEMPERATURE_VALUES_, HEATING_POWER_VALUES_, set_temperature_);
  }
  return new_heating_power;
}

void Heater::setHeatingPower(int new_heating_power)
{
  heating_power_ = constrain(new_heating_power, 0, MAX_HEATING_POWER);
  short unsigned int heating_signal = round(1.0 * heating_power_ / MAX_HEATING_POWER * MAX_HEAT_SIGNAL);
  analogWrite(PIN_HEAT_SUPPLY_, MAX_HEAT_SIGNAL - heating_signal);    // przekaźnik wyzwalany stanem niskim
}
