#include "Heater.h"

Heater::Heater(const uint8_t pin_heat_supply) :
  PIN_HEAT_SUPPLY_{ pin_heat_supply }
{
  // przypisanie numeru pinu w mikrosterowniku
}

void Heater::init()
{
  pinMode(PIN_HEAT_SUPPLY_, OUTPUT);       // ustawienie kanału przekaźnika jako wyjście
  digitalWrite(PIN_HEAT_SUPPLY_, HIGH);    // początkowy stan wysoki odpowiada wyłączonemu kanałowi przekaźnika
}

void Heater::run()
{
  constexpr unsigned int OVERFLOW = TEMPERATURE_ESTIMATION_PERIOD / CYCLE_PERIOD;
  const float target_heating_signal{ heating_power_ / MAX_HEATING_POWER * MAX_HEAT_SIGNAL };
  const SignlalPWM lower_heating_signal{ static_cast<SignlalPWM>(floor(target_heating_signal)) };
  const SignlalPWM upper_heating_signal{ static_cast<SignlalPWM>(ceil(target_heating_signal)) };
  const unsigned int switching_time = round((target_heating_signal - lower_heating_signal) * OVERFLOW);

  // Przełączenie ustawionej wartości grzania w takim momencie, aby średnia moc odpowiadała jak najdokładniej wartości zadanej
  if (power_switching_counter_ == 0)
    setPowerModulation(lower_heating_signal);
  else if (power_switching_counter_ == switching_time)
    setPowerModulation(upper_heating_signal);

  ++power_switching_counter_;
  power_switching_counter_ %= OVERFLOW;
}

void Heater::setHeatingPower(const Watt new_heating_power)
{
  heating_power_ = constrain(new_heating_power, 0, MAX_HEATING_POWER);
}

Watt Heater::getHeatingPower() const
{
  return heating_power_;
}

inline void Heater::setPowerModulation(const SignlalPWM heating_signal)
{
  analogWrite(PIN_HEAT_SUPPLY_, MAX_HEAT_SIGNAL - heating_signal);    // przekaźnik wyzwalany stanem niskim
}
