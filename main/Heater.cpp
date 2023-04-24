#include "Heater.h"

Heater::Heater(short unsigned int pin_heat_supply) :
  PIN_HEAT_SUPPLY_{ pin_heat_supply },
  TEMPERATURE_VALUES_{ TEMPERATURE_TO_POWER_CONVERSION_ARRAY_SIZE, new const int[TEMPERATURE_TO_POWER_CONVERSION_ARRAY_SIZE] TEMPERATURE_VALUES },
  HEATING_POWER_VALUES_{ TEMPERATURE_TO_POWER_CONVERSION_ARRAY_SIZE, new const int[TEMPERATURE_TO_POWER_CONVERSION_ARRAY_SIZE] HEATING_POWER_VALUES }
{
  // przypisanie numeru pinu w mikrosterowniku
}

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
  temperature_derivative_ = (real_temperature_ - set_temperature_ - temperature_deviation_) * 1000 / TEMPERATURE_ESTIMATION_PERIOD;    // zamiana ms w s
  temperature_deviation_ = real_temperature_ - set_temperature_;

  if (!is_heating_set_)
  {
    prediction_error_ = 0;
    return;
  }

  temperature_integral_ += temperature_deviation_ * TEMPERATURE_ESTIMATION_PERIOD / 1000;
  temperature_integral_ = constrain(temperature_integral_, -max_heating_power / INTEGRAL_REGULATION_COEFFICIENT, max_heating_power / INTEGRAL_REGULATION_COEFFICIENT);

  int heat_supply{ 0 };
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

  // Sprawdzenie poprawności grzania w jego początkowym etapie
  const float WEIGHT{ 0.01 };
  float balance_power = tabularConversion(TEMPERATURE_VALUES_, HEATING_POWER_VALUES_, real_temperature_);
  float predicted_temperature_growth = 0.0042 * (heating_power_ - balance_power);
  float real_temperature_growth = constrain(temperature_derivative_, -3.0, 3.0);
  float current_error = real_temperature_growth - predicted_temperature_growth;
  prediction_error_ = (1.0 - WEIGHT) * prediction_error_ + WEIGHT * current_error;

  if (ENABLE_ERRORS && abs(prediction_error_) > 0.1 + 1.5 * max(heating_power_, balance_power) / MAX_HEATING_POWER)
    reportError(F("5"));
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

void Heater::setHeatingPower(const int new_heating_power)
{
  heating_power_ = constrain(new_heating_power, 0, MAX_HEATING_POWER);
  short unsigned int heating_signal = round(1.0 * heating_power_ / MAX_HEATING_POWER * MAX_HEAT_SIGNAL);
  analogWrite(PIN_HEAT_SUPPLY_, MAX_HEAT_SIGNAL - heating_signal);    // przekaźnik wyzwalany stanem niskim
}
