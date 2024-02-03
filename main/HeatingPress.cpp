#include "HeatingPress.h"

HeatingPress::HeatingPress(const VectorSize allocated_threads_number) :
  System{ allocated_threads_number }
{
}

HeatingPress::~HeatingPress()
{
  reportError(F("?"));
}

void HeatingPress::init()
{
  // Dodanie interfejsów jako kolejnych wątków systemu
  addThread(&input_interface_, BUTTONS_REFRESH_PERIOD);
  addThread(&output_interface_, CHAR_REFRESH_PERIOD);

  // Konfiguracja poszczególnych portów
  for (VectorSize i = 0; i < threads_.length(); ++i)    // czujniki nacisku, temperatury oraz przekaźnik
    threads_[i].task->init();                           // przyciski, ekran LCD

  TCCR1B = (TCCR1B & B11111000) | B00000101;    // ustawienie częstotliwości PWM pinów 9 i 10 na 30.64 Hz
  cycle_counter_ = millis() + CYCLE_PERIOD;     // ustawienie wartości licznika czasu
}

void HeatingPress::run()
{
  // Wykonywanie programu z określoną częstotliwością
  const unsigned long timer{ millis() };
  if (ENABLE_ERRORS && cycle_counter_ > timer + CYCLE_PERIOD)
    reportError(F("0"));
  if (cycle_counter_ > timer)
    return;

  cycle_counter_ += CYCLE_PERIOD;
  if (ENABLE_ERRORS && cycle_counter_ < timer)
    reportError(F("1"));

  // Wykonanie wszystkich wątków z przypisanymi do nich częstotliwościami
  for (VectorSize i = 0; i < threads_.length(); ++i)
  {
    ++threads_[i].counter;
    if (threads_[i].counter < threads_[i].overflow)
      continue;

    threads_[i].counter = 0;
    threads_[i].task->run();
  }

  // Odczyt interfejsu - działanie przycisków zależy od okna, dla którego zostały aktywowane
  readUserCommands();
}

void HeatingPress::readUserCommands()
{
  if (!input_interface_.isReady())
    return;

  const Array<bool, BUTTONS_NUMBER> buttons_activity{ input_interface_.getButtonsActivity() };
  input_interface_.expireButtons();

  if (!input_interface_.isWindowChanged())
  {
    windows_[displayed_window_id_]->executeCommands(buttons_activity);
    sendDataToDisplay();
    return;
  }

  if (buttons_activity.get(LEFT))
    displayed_window_id_ += windows_.length() - 1;    // przełączenie wyświetlanego okna
  if (buttons_activity.get(RIGHT))
    ++displayed_window_id_;

  displayed_window_id_ %= windows_.length();
  sendDataToDisplay(true);
}

void HeatingPress::sendDataToDisplay(const bool force_refresh)
{
  ScreenContent content{ windows_[displayed_window_id_]->getDataToDisplay() };
  output_interface_.setDisplayedData(content, force_refresh);
}

const char FIRST_LINE[LCD_COLUMNS_NUMBER + 1] PROGMEM = "Error code      ";
const char SECOND_LINE[LCD_COLUMNS_NUMBER + 1] PROGMEM = "Please restart  ";
constexpr int CODE_POSITION = 11;

void HeatingPress::emergencyShutdown(const String& code)
{
  digitalWrite(PIN_HEAT_SUPPLY_TOP, HIGH);           // awaryjne wyłączenie grzania
  digitalWrite(PIN_HEAT_SUPPLY_BOT, HIGH);           //
  ScreenContent content(FIRST_LINE, SECOND_LINE);    // komunikat
  content.putWord(code, CODE_POSITION, 0);           //
  output_interface_.setDisplayedData(content);       //
  while (true)                                       // zatrzymanie wykonywania programu
  {
    output_interface_.run();
    delay(CYCLE_PERIOD);
  };
}
