#include "HeatingRegulator.h"

HeatingRegulator::HeatingRegulator(const uint8_t pin_heat_supply) :
  heater_{ pin_heat_supply },
  TEMPERATURE_TO_HEATING_POWER_CONVERTER_{ NumericArray<DegCelsius, TEMPERATURE_TO_POWER_CONVERSION_ARRAY_SIZE>(TEMPERATURE_VALUES),
                                           NumericArray<StorageWatt, TEMPERATURE_TO_POWER_CONVERSION_ARRAY_SIZE>(HEATING_POWER_VALUES) }
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

void HeatingRegulator::refreshHeatingPower(const DegCelsius target_temperature, const KelvinDiff real_temperature, const bool is_heating_set)
{
  temperature_derivative_ =
    (real_temperature - target_temperature - temperature_deviation_) * 1000 / TEMPERATURE_ESTIMATION_PERIOD;    // zamiana ms w s
  temperature_deviation_ = real_temperature - target_temperature;

  if (!is_heating_set)
  {
    heater_.setHeatingPower(0);
    prediction_error_ = 0;
    return;
  }

  temperature_integral_ += temperature_deviation_ * TEMPERATURE_ESTIMATION_PERIOD / 1000;
  const KelvinSec max_scope{ s_max_heating_power / INTEGRAL_REGULATION_COEFFICIENT };
  temperature_integral_ = constrain(temperature_integral_, -max_scope, max_scope);

  Watt heating_power{ 0 };
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
    heating_power = calculateRegulatedHeatingPower(target_temperature);
  }
  heater_.setHeatingPower(heating_power);

  constexpr float THERMAL_REACTANCE{ 0.0042 };
  constexpr float MAX_NOISE{ 3.0 };
  constexpr float CONST_COEFF{ 0.08 };
  constexpr float INPUT_POWER_COEFF{ 0.5 / MAX_HEATING_POWER };
  constexpr float OUTPUT_POWER_COEFF{ 0.3 / MAX_HEATING_POWER };
  constexpr float ERROR_SUSCEPTANCE{ 0.01 };

  // Sprawdzenie poprawności grzania w jego początkowym etapie
  const Watt balance_power = TEMPERATURE_TO_HEATING_POWER_CONVERTER_.convert(real_temperature);
  const KelvinPerSec predicted_temperature_growth{ THERMAL_REACTANCE * (heating_power - balance_power) };
  const KelvinPerSec real_temperature_growth{ constrain(temperature_derivative_, -MAX_NOISE, MAX_NOISE) };
  const float current_error{ real_temperature_growth - predicted_temperature_growth };
  prediction_error_ = (1.0 - ERROR_SUSCEPTANCE) * prediction_error_ + ERROR_SUSCEPTANCE * current_error;

  if (ENABLE_ERRORS && abs(prediction_error_) > CONST_COEFF + INPUT_POWER_COEFF * heating_power + OUTPUT_POWER_COEFF * balance_power)
    reportError(F("5"));
}

void HeatingRegulator::reset()
{
  active_regulation_ = false;
  temperature_integral_ = 0;
  prediction_error_ = 0;
}

inline Watt HeatingRegulator::calculateRegulatedHeatingPower(const DegCelsius target_temperature)
{
  active_regulation_ = true;
  const Watt P{ -PROPORTIONAL_REGULATION_COEFFICIENT * temperature_deviation_ };
  const Watt I{ -INTEGRAL_REGULATION_COEFFICIENT * temperature_integral_ };
  const Watt D{ -DERIVATIVE_REGULATION_COEFFICIENT * temperature_derivative_ };
  const Watt power_correction{ P + I + D };
  const Watt base_heating_power = TEMPERATURE_TO_HEATING_POWER_CONVERTER_.convert(target_temperature);
  const Watt new_heating_power{ constrain(base_heating_power + power_correction, 0, s_max_heating_power) };
  return new_heating_power;
}
