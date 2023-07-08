#include "HeatingPress.h"

HeatingPress::HeatingPress(const short unsigned int allocated_threads_number)
{
  // Zarezerwowanie pamięci na każdy z wątków oraz okien
  threads_ = new Thread[allocated_threads_number];
  windows_ = new DisplayedElement*[allocated_threads_number];
  allocated_threads_number_ = allocated_threads_number;
  allocated_windows_number_ = allocated_threads_number;
}

HeatingPress::~HeatingPress()
{
  reportError(F("?"));
}

void HeatingPress::init()
{
  // Dodanie interfejsów jako kolejnych wątków systemu
  addThread(&input_interface_, BUTTONS_REFRESH_PERIOD);
  addThread(&output_interface_, SCREEN_REFRESH_PERIOD / DISPLAYED_DATA_DIVISOR);

  // Konfiguracja poszczególnych portów
  for (unsigned int i = 0; i < threads_number_; ++i)    // czujniki nacisku, temperatury oraz przekaźnik
    threads_[i].task->init();                           // przyciski, ekran LCD

  TCCR1B = (TCCR1B & B11111000) | B00000101;    // ustawienie częstotliwości PWM pinów 9 i 10 na 30.64 Hz
  cycle_counter_ = millis() + CYCLE_PERIOD;     // ustawienie wartości licznika czasu
}

void HeatingPress::addThread(SystemElement* new_thread, const unsigned int period)
{
  if (threads_number_ == allocated_threads_number_)
  {
    Thread* temp_table = new Thread[allocated_threads_number_ + 1];
    for (unsigned int i = 0; i < allocated_threads_number_; ++i)
      temp_table[i] = threads_[i];
    ++allocated_threads_number_;
    delete[] threads_;
    threads_ = temp_table;
  }
  threads_[threads_number_++] = Thread(new_thread, 0, period / CYCLE_PERIOD);
}

void HeatingPress::addWindow(DisplayedElement* new_window, const unsigned int period)
{
  addThread(new_window, period);
  if (windows_number_ == allocated_windows_number_)
  {
    DisplayedElement** temp_table = new DisplayedElement*[allocated_windows_number_ + 1];
    for (unsigned int i = 0; i < allocated_windows_number_; ++i)
      temp_table[i] = windows_[i];
    ++allocated_windows_number_;
    delete[] windows_;
    windows_ = temp_table;
  }
  windows_[windows_number_++] = new_window;
}

void HeatingPress::readUserCommands()
{
  const bool* const p_buttons_activity = input_interface_.getButtonsActivity();
  if (!p_buttons_activity)
    return;

  if (!input_interface_.isWindowChanged())
  {
    windows_[displayed_window_id_]->executeCommands(p_buttons_activity);
    sendDataToDisplay();
    return;
  }

  if (p_buttons_activity[LEFT])
    displayed_window_id_ += windows_number_ - 1;    // przełączenie wyświetlanego okna
  if (p_buttons_activity[RIGHT])
    ++displayed_window_id_;

  displayed_window_id_ %= windows_number_;
  sendDataToDisplay(true);
}

void HeatingPress::sendDataToDisplay(const bool force_refresh)
{
  String first_line, second_line;
  windows_[displayed_window_id_]->getDataToDisplay(first_line, second_line);
  output_interface_.setDisplayedData(first_line, second_line, force_refresh);
}

void HeatingPress::run()
{
  // Wykonywanie programu z określoną częstotliwością
  const long unsigned int timer = millis();
  if (ENABLE_ERRORS && cycle_counter_ > timer + CYCLE_PERIOD)
    reportError(F("0"));
  if (cycle_counter_ > timer)
    return;

  cycle_counter_ += CYCLE_PERIOD;
  if (ENABLE_ERRORS && cycle_counter_ < timer)
    reportError(F("1"));

  // Wykonanie wszystkich wątków z przypisanymi do nich częstotliwościami
  for (unsigned int i = 0; i < threads_number_; ++i)
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

void HeatingPress::emergencyShutdown(const String& code)
{
  digitalWrite(PIN_HEAT_SUPPLY_TOP, HIGH);                        // awaryjne wyłączenie grzania
  digitalWrite(PIN_HEAT_SUPPLY_BOT, HIGH);                        //
  String first_line = String(F("Error code ")) + code + "   ";    // komunikat
  String second_line = F("Please restart  ");                     //
  output_interface_.setDisplayedData(first_line, second_line);    //
  while (true)                                                    // zatrzymanie wykonywania programu
  {
    output_interface_.run();
    delay(CYCLE_PERIOD);
  };
}
