#include "HeatingPlate.h"

HeatingPlate::HeatingPlate(const uint8_t pin_temperature_sensor, const uint8_t pin_heat_supply,
                           const float tuning_factor, const float sensor_offset, const String& name) :
  temperature_sensor_{ pin_temperature_sensor, tuning_factor, sensor_offset },
  regulator_{ pin_heat_supply },
  DISPLAYED_NAME_{ name }
{
  // przypisanie numerów pinów w mikrosterowniku
}

void HeatingPlate::init()
{
  regulator_.init();             // konfiguracja grzałki
  temperature_sensor_.init();    // konfiguracja czujnika temperatury
}

void HeatingPlate::executeCommands(const Array<bool, BUTTONS_NUMBER>& buttons)
{
  if (buttons.get(UP) && !buttons.get(DOWN) && target_temperature_ < MAX_TEMPERATURE)
  {
    target_temperature_ += PRESET_TEMPERATURE_RESOLUTION;
    regulator_.reset();
  }
  else if (!buttons.get(UP) && buttons.get(DOWN) && target_temperature_ > MIN_TEMPERATURE)
  {
    target_temperature_ -= PRESET_TEMPERATURE_RESOLUTION;
    regulator_.reset();
  }
  if (buttons.get(ACTION))
    is_heating_set_ = !is_heating_set_;
}

const char FIRST_LINE[LCD_COLUMNS_NUMBER] PROGMEM = {
  '_', '_', '_', ' ', 't', 'e', 'm', 'p', ':', ' ', ' ', ' ', ' ', DEGREE_SYMBOL_INDEX, 'C', ' '
};
const char SECOND_LINE[LCD_COLUMNS_NUMBER] PROGMEM = { 'S', 'e', 't', ':', ' ', ' ', ' ', ' ', DEGREE_SYMBOL_INDEX,
                                                       'C', ' ', ' ', ' ', ' ', ' ', ' ' };

ScreenContent HeatingPlate::getDataToDisplay() const
{
  constexpr int LAST_DIGIT_IN_FIRST_LINE = 12;
  constexpr int LAST_DIGIT_IN_SECOND_LINE = 7;
  constexpr int STATE_MESSAGE_LENGTH = 3;

  ScreenContent content(FIRST_LINE, SECOND_LINE);
  content.putWord(DISPLAYED_NAME_, 0, 0);
  content.putNumberFromEnd(round(real_temperature_), LAST_DIGIT_IN_FIRST_LINE, 0);
  content.putNumberFromEnd(target_temperature_, LAST_DIGIT_IN_SECOND_LINE, 1);
  content.putWord(is_heating_set_ ? F(" ON") : F("OFF"), LCD_COLUMNS_NUMBER - STATE_MESSAGE_LENGTH, 1);

  return content;
}

void HeatingPlate::run()
{
  // Ustawianie odpowiedniego sygnału grzania
  regulator_.run();

  // Pomiar temperatury na płycie
  temperature_sensor_.measureTemperature();

  // Ustawienie mocy grzania w zależności od obecnej temperatury
  if (temperature_sensor_.isBufferFull())
  {
    const KelvinDiff estimated_temperature{ s_ambient_temperature + temperature_sensor_.calculateRelativeTemperature() };
    // Sprawdzenie poprawności sygnału
    const bool start{ round(real_temperature_) == s_ambient_temperature };
    if (ENABLE_ERRORS && !start
        && abs(real_temperature_ - estimated_temperature) > MAX_TEMPERATURE_GROWTH * TEMPERATURE_ESTIMATION_PERIOD / 1000.0)
      reportError(F("4"));

    real_temperature_ = estimated_temperature;
    temperature_sensor_.clearBuffer();
    regulator_.refreshHeatingPower(target_temperature_, real_temperature_, is_heating_set_);
  }
}
