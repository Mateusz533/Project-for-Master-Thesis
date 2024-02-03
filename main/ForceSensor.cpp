#include "ForceSensor.h"

ForceSensor::ForceSensor(const uint8_t pin_force_sensor) :
  PIN_FORCE_SENSOR_{ pin_force_sensor },
  SIGNAL_TO_FORCE_CONVERTER_{ NumericArray<AnalogSignal, FORCE_CONVERSION_ARRAY_SIZE>(FORCE_SIGNAL_VALUES),
                              NumericArray<Newton, FORCE_CONVERSION_ARRAY_SIZE>(FORCE_VALUES) }
{
  // przypisanie numeru pinu w mikrosterowniku
}

void ForceSensor::init()
{
  pinMode(PIN_FORCE_SENSOR_, INPUT);    // ustawienie czujnika nacisku jako wejście z napięciem zewnętrznym
  digitalWrite(PIN_FORCE_SENSOR_, LOW);
}

void ForceSensor::executeCommands(const Array<bool, BUTTONS_NUMBER>& buttons)
{
  if (buttons.get(ACTION) && !is_overloaded_)
    force_offset_ = force_;
}

const char FIRST_LINE[LCD_COLUMNS_NUMBER + 1] PROGMEM = "Force:       N  ";
const char SECOND_LINE_OK[LCD_COLUMNS_NUMBER + 1] PROGMEM = "Maximum: 200 N  ";
const char SECOND_LINE_OVER[LCD_COLUMNS_NUMBER + 1] PROGMEM = "OVERLOADED !!!  ";

ScreenContent ForceSensor::getDataToDisplay() const
{
  constexpr int LAST_DIGIT_POSITION = 11;

  ScreenContent content(FIRST_LINE, is_overloaded_ ? SECOND_LINE_OVER : SECOND_LINE_OK);
  BasicSize position{ content.putNumberFromEnd(max(force_, force_offset_) + TARE, LAST_DIGIT_POSITION, 0) };
  if (force_ <= force_offset_)
    content.putWord(F("<"), position, 0);

  return content;
}

void ForceSensor::run()
{
  force_measurements_.push(analogRead(PIN_FORCE_SENSOR_));

  if (!force_measurements_.isFull())
    return;

  const AnalogSignal signal_value{ force_measurements_.mean_value() };    // filtrowanie szumów poprzez uśrednianie krótkookresowych odczytów

  if (ENABLE_ERRORS && signal_value > FORCE_SIGNAL_HIGHER_LIMIT)    // sprawdzenie poprawności sygnału
    reportError(F("2"));

  force_ = SIGNAL_TO_FORCE_CONVERTER_.convert(signal_value);    // konwersja poziomów ADC na Newtony

  is_overloaded_ = (force_ >= SIGNAL_TO_FORCE_CONVERTER_.maxValue());    // sprawdzenie przeciążenia czujnika

  force_measurements_.clear();
}
