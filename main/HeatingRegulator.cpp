#include "HeatingRegulator.h"

HeatingRegulator::HeatingRegulator(const short unsigned int pin_heat_supply) :
  heater_{ pin_heat_supply },
  TEMPERATURE_VALUES_{ TEMPERATURE_TO_POWER_CONVERSION_ARRAY_SIZE,
                       new const int[TEMPERATURE_TO_POWER_CONVERSION_ARRAY_SIZE] TEMPERATURE_VALUES },
  HEATING_POWER_VALUES_{ TEMPERATURE_TO_POWER_CONVERSION_ARRAY_SIZE, new const int[TEMPERATURE_TO_POWER_CONVERSION_ARRAY_SIZE] HEATING_POWER_VALUES }
{
  // przypisanie numeru pinu w mikrosterowniku
}

void HeatingRegulator::init()
{
  heater_.init();
}

void HeatingRegulator::run()
{
  heater_.run();
}

void HeatingRegulator::refreshHeatingPower(const int set_temperature, const float real_temperature, const bool is_heating_set)
{
  temperature_derivative_ =
    (real_temperature - set_temperature - temperature_deviation_) * 1000 / TEMPERATURE_ESTIMATION_PERIOD;    // zamiana ms w s
  temperature_deviation_ = real_temperature - set_temperature;

  if (!is_heating_set)
  {
    heater_.setHeatingPower(0);
    prediction_error_ = 0;
    return;
  }

  temperature_integral_ += temperature_deviation_ * TEMPERATURE_ESTIMATION_PERIOD / 1000;
  const float max_scope{ s_max_heating_power / INTEGRAL_REGULATION_COEFFICIENT };
  temperature_integral_ = constrain(temperature_integral_, -max_scope, max_scope);

  float heating_power{ 0 };
  if (temperature_deviation_ < -TEMPERATURE_REGULATION_RANGE)    // procedura regulacji
  {
    active_regulation_ = false;
    heating_power = s_max_heating_power;
    temperature_integral_ = 0;
  }
  else if (temperature_deviation_ > TEMPERATURE_REGULATION_RANGE)
  {
    active_regulation_ = false;
    heating_power = 0;
    temperature_integral_ = 0;
  }
  else    // regulator PID
  {
    heating_power = calculateRegulatedHeatingPower(set_temperature);
  }
  heater_.setHeatingPower(heating_power);

  // Sprawdzenie poprawności grzania w jego początkowym etapie
  const float WEIGHT{ 0.01 };
  const float balance_power = tabularConversion(TEMPERATURE_VALUES_, HEATING_POWER_VALUES_, real_temperature);
  const float predicted_temperature_growth = 0.0042 * (heating_power - balance_power);
  const float real_temperature_growth = constrain(temperature_derivative_, -3.0, 3.0);
  const float current_error = real_temperature_growth - predicted_temperature_growth;
  prediction_error_ = (1.0 - WEIGHT) * prediction_error_ + WEIGHT * current_error;

  if (ENABLE_ERRORS && abs(prediction_error_) > 0.08 + (0.5 * heating_power + 0.3 * balance_power) / MAX_HEATING_POWER)
    reportError(F("5"));
}

void HeatingRegulator::reset()
{
  active_regulation_ = false;
  temperature_integral_ = 0;
  prediction_error_ = 0;
}

float HeatingRegulator::calculateRegulatedHeatingPower(const int set_temperature)
{
  active_regulation_ = true;
  const float P = -PROPORTIONAL_REGULATION_COEFFICIENT * temperature_deviation_;
  const float I = -INTEGRAL_REGULATION_COEFFICIENT * temperature_integral_;
  const float D = -DERIVATIVE_REGULATION_COEFFICIENT * temperature_derivative_;
  const float power_correction = P + I + D;
  const float base_heating_power =
    tabularConversion<const int, const int>(TEMPERATURE_VALUES_, HEATING_POWER_VALUES_, set_temperature);
  const float new_heating_power = constrain(base_heating_power + power_correction, 0, s_max_heating_power);
  return new_heating_power;
}
