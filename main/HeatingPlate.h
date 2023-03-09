#include "configuration.h"
#include "SystemElement.h"
#include "TemperatureSensor.h"
#pragma once

// Klasa przechowująca parametry płyty grzewczej
class HeatingPlate : public SystemElement, public TemperatureSensor
{
  public:
    // Konstruktor
    HeatingPlate(short unsigned int pin_temperature_sensor, short unsigned int pin_heat_supply, float tuning_factor, float sensor_offset, String name) :
      TemperatureSensor(pin_temperature_sensor, tuning_factor, sensor_offset),
      PIN_HEAT_SUPPLY_(pin_heat_supply),
      DISPLAYED_NAME_(name)
    {
      // przypisanie numerów pinów w mikrosterowniku
    }
    // Konfiguruje porty wejścia/wyjścia dla tego elementu
    void init();
    // Wykonuje pobrane od użytkownika polecania dotyczące zadanej temperatury oraz stanu grzania
    void executeCommands(const bool buttons[]);
    // Wyświetla dane dotyczące płyty grzewczej
    void getDataToDisplay(String& first_line, String& second_line);
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
    const short unsigned int PIN_HEAT_SUPPLY_ = 0;
    // Wyświetlana nazwa grzałki
    const String DISPLAYED_NAME_ = "HEATER";
    // Tablice konwersji temperatury na moc w stanie ustalonym
    static const short int CONVERSION_ARRAYS_SIZE_ = 14;
    const int TEMPERATURE_VALUES_[CONVERSION_ARRAYS_SIZE_] = { 20, 62, 100, 134, 167, 199, 228, 256, 282, 307, 331, 353, 375, 400 };
    const int HEATING_POWER_VALUES_[CONVERSION_ARRAYS_SIZE_] = { 0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65 };
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
  if (buttons[ACTION]) is_heating_set_ = !is_heating_set_;
}

void HeatingPlate::getDataToDisplay(String& first_line, String& second_line)
{
  first_line = DISPLAYED_NAME_ + " temp: ";
  if (real_temperature_ < 100) first_line += " ";
  if (real_temperature_ < 10) first_line += " ";
  first_line += static_cast<String>(real_temperature_) + DEGREE_SYMBOL_INDEX + "C ";

  second_line = "Set: ";
  if (set_temperature_ < 100) second_line += " ";
  if (set_temperature_ < 10) second_line += " ";
  second_line += static_cast<String>(set_temperature_) + DEGREE_SYMBOL_INDEX + "C   ";
  if (is_heating_set_) second_line += " ON";
  else second_line += "OFF";
}

void HeatingPlate::run()
{
  // Włączenie/wyłączenie grzania - musi następować szybciej niż odświeżanie stanu temperatury
  switchHeating();

  // Pomiar temperatury na płycie
  measureTemperature();

  // Ustawienie mocy grzania w zależności od obecnej temperatury
  if (long_measuring_counter_ == LONG_MEASUREMENTS_ARRAY_SIZE_)
  {
    long_measuring_counter_ = 0;
    real_temperature_ = ambient_temperature_ + calculateRelativeTemperature();
    regulateHeatingPower();
  }
}

void HeatingPlate::switchHeating()
{
  if (!is_heating_set_) setHeatingPower(0);
  else if (real_temperature_ - set_temperature_ > TEMPERATURE_REGULATION_RANGE) setHeatingPower(0);
  else if (real_temperature_ - set_temperature_ < -TEMPERATURE_REGULATION_RANGE) setHeatingPower(MAX_HEATING_POWER);
}

void HeatingPlate::regulateHeatingPower()
{
  if (!is_heating_set_) return;

  temperature_derivative_ = 1.0 * (real_temperature_ - set_temperature_ - temperature_deviation_) * 1000 / TEMPERATURE_ESTIMATION_PERIOD;    // zamiana ms w s
  temperature_deviation_ = real_temperature_ - set_temperature_;
  temperature_integral_ += 1.0 * temperature_deviation_ * TEMPERATURE_ESTIMATION_PERIOD / 1000;

  int heat_supply = 0;
  if (temperature_deviation_ < -TEMPERATURE_REGULATION_RANGE)    // procedura regulacji
  {
    active_regulation_ = false;
    heat_supply = MAX_HEATING_POWER;
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
}

int HeatingPlate::calculateRegulatedHeatingPower()
{
  static int heating_power = 0;
  if (!active_regulation_)
  {
    active_regulation_ = true;
    for (unsigned int i = 1; i < CONVERSION_ARRAYS_SIZE_; ++i)
    {
      if (set_temperature_ < TEMPERATURE_VALUES_[i])
      {
        heating_power = round(HEATING_POWER_VALUES_[i - 1]
                              + 1.0 * (set_temperature_ - TEMPERATURE_VALUES_[i - 1]) * (HEATING_POWER_VALUES_[i] - HEATING_POWER_VALUES_[i - 1])
                                  / (TEMPERATURE_VALUES_[i] - TEMPERATURE_VALUES_[i - 1]));
        break;
      }
      else heating_power = HEATING_POWER_VALUES_[i];
    }
  }
  int power_deviation = round(-DERIVATIVE_REGULATION_COEFFICIENT * temperature_derivative_ - PROPORTIONAL_REGULATION_COEFFICIENT * temperature_deviation_
                              - INTEGRAL_REGULATION_COEFFICIENT * temperature_integral_);
  heating_power = constrain(heating_power + power_deviation, 0, MAX_HEATING_POWER);
  return heating_power;
}

void HeatingPlate::setHeatingPower(int heating_power)
{
  heating_power = constrain(heating_power, 0, MAX_HEATING_POWER);
  heating_power = map(heating_power, 0, MAX_HEATING_POWER, 0, MAX_HEAT_SIGNAL);
  analogWrite(PIN_HEAT_SUPPLY_, MAX_HEAT_SIGNAL - heating_power);    // przekaźnik wyzwalany stanem niskim
}
