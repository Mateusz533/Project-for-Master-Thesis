#include "Heater.h"

Heater::Heater(short unsigned int pin_heat_supply) :
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
  const unsigned int OVERFLOW = TEMPERATURE_ESTIMATION_PERIOD / CYCLE_PERIOD;
  float target_heating_signal = heating_power_ / MAX_HEATING_POWER * MAX_HEAT_SIGNAL;
  short unsigned int lower_heating_signal = floor(target_heating_signal);
  short unsigned int upper_heating_signal = ceil(target_heating_signal);
  unsigned int switching_time = round((target_heating_signal - lower_heating_signal) * OVERFLOW);

  // Przełączenie ustawionej wartości grzania w takim momencie, aby średnia moc odpowiadała jak najdokładniej wartości zadanej
  if (power_switching_counter_ == 0)
    analogWrite(PIN_HEAT_SUPPLY_, MAX_HEAT_SIGNAL - lower_heating_signal);    // przekaźnik wyzwalany stanem niskim
  else if (power_switching_counter_ == switching_time)
    analogWrite(PIN_HEAT_SUPPLY_, MAX_HEAT_SIGNAL - upper_heating_signal);

  ++power_switching_counter_;
  power_switching_counter_ %= OVERFLOW;
}

void Heater::setHeatingPower(const float new_heating_power)
{
  heating_power_ = constrain(new_heating_power, 0, MAX_HEATING_POWER);
}

float Heater::getHeatingPower() const
{
  return heating_power_;
}
