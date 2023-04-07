#include "configuration.h"
#include "InputInterface.h"
#include "OutputInterface.h"
#include "DisplayedElement.h"
#include "reportError.h"
#include "HardwareTester.h"
#pragma once

// Klasa przechowująca parametry prasy grzewczej
class HeatingPress
{
  public:
    // Konstruktor
    HeatingPress(short unsigned int allocated_threads_number = 4)
    {
      // Zarezerwowanie pamięci na każdy z wątków oraz okien
      threads_ = new Thread[allocated_threads_number];
      windows_ = new DisplayedElement*[allocated_threads_number];
      allocated_threads_number_ = allocated_threads_number;
      allocated_windows_number_ = allocated_threads_number;
    }
    // Destruktor
    ~HeatingPress()
    {
      reportError(F("?"));
    }
    // Dodaje podany element do systemu jako nowy wątek
    void addThread(SystemElement* new_thread, unsigned int period);
    // Dodaje podany element do systemu jako nowy wątek wraz oknem interfejsu użytkownika
    void addWindow(DisplayedElement* new_thread, unsigned int period);
    // Inicjalizuje urządzenie konfigurując wszystkie porty, ekran, liczniki oraz wyświetlając wprowadzenie
    void init();
    // Wykonuje jeden cykl działania systemu w przypadku nadejścia odpowiadającego mu czasu
    void run();
    // Wyświetla komunikat o błędzie o zadanym numerze, awaryjnie wyłącza grzałki oraz zatrzymuje program
    void emergencyShutdown(const String& code);

  private:
    // Struktura przechowująca parametry jednego wątku
    struct Thread
    {
        // Konstruktor domyślny
        Thread() = default;
        Thread(SystemElement* task, unsigned int counter, unsigned int overflow) :
          task(task),
          counter(counter),
          overflow(overflow)
        {
          // Przypisanie wszystkich pól struktury
        }
        // Wskaźnik do instancji danego wątku
        SystemElement* task = nullptr;
        // Liczniki umożliwiające wykonywanie zadań z odpowiednimi częstotliwościami
        unsigned int counter = 0;
        unsigned int overflow = 1;
    };

    // Wykonuje polecenia użytkownika na podstawie wciśniętych przycisków
    void readUserCommands();
    // Przesyła dane do wyświetlenia odpowiednie dla aktywnego okna
    void sendDataToDisplay(const bool force_refresh = false);

    // Obiekt obsługujący interfejs wejściowy składający się z 5-ciu przycisków
    InputInterface input_interface_;
    // Tablica przechowujaca instancję każdego z wątków
    Thread* threads_ = nullptr;
    // Tablica przechowujaca wskaźnik do instancji każdego z okien
    DisplayedElement** windows_ = nullptr;
    // Obiekt obsługujący interfejs wyjściowy składający się z ekranu LCD o 2-ch wierszach i 16-stu kolumnach
    OutputInterface output_interface_;
    /* Zmienna przechowująca numer jednego z czterech okien interfejsu:
       0 - informacje o nacisku
       1 - informacje o temperaturze i grzaniu górnej płyty
       2 - informacje o temperaturze i grzaniu dolnej płyty
       3 - informacje o parametrach globalnych
    */
    short int displayed_window_id_ = 0;
    short unsigned int threads_number_ = 0;
    short unsigned int windows_number_ = 0;
    short unsigned int allocated_threads_number_ = 4;
    short unsigned int allocated_windows_number_ = 4;
    // Licznik umożliwiający synchroniczne wykonywanie programu
    long unsigned int cycle_counter_ = 0;
};

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

void HeatingPress::addThread(SystemElement* new_thread, unsigned int period)
{
  if (threads_number_ == allocated_threads_number_)
  {
    Thread* temp_table = new Thread[2 * allocated_threads_number_];
    for (unsigned int i = 0; i < allocated_threads_number_; ++i)
      temp_table[i] = threads_[i];
    allocated_threads_number_ *= 2;
    delete[] threads_;
    threads_ = temp_table;
  }
  threads_[threads_number_++] = Thread(new_thread, 0, period / CYCLE_PERIOD);
}

void HeatingPress::addWindow(DisplayedElement* new_window, unsigned int period)
{
  addThread(new_window, period);
  if (windows_number_ == allocated_windows_number_)
  {
    DisplayedElement** temp_table = new DisplayedElement*[2 * allocated_windows_number_];
    for (unsigned int i = 0; i < allocated_windows_number_; ++i)
      temp_table[i] = windows_[i];
    allocated_windows_number_ *= 2;
    delete[] windows_;
    windows_ = temp_table;
  }
  windows_[windows_number_++] = new_window;
}

void HeatingPress::readUserCommands()
{
  bool* buttons_activity = input_interface_.getButtonsActivity();
  if (!buttons_activity)
    return;

  if (!input_interface_.isWindowChanged())
  {
    windows_[displayed_window_id_]->executeCommands(buttons_activity);
    sendDataToDisplay();
    return;
  }

  if (buttons_activity[LEFT])
    displayed_window_id_ += windows_number_ - 1;    // przełączenie wyświetlanego okna
  if (buttons_activity[RIGHT])
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
  long unsigned int timer = millis();
  if (cycle_counter_ > timer + CYCLE_PERIOD)
    reportError(F("0"));
  if (cycle_counter_ > timer)
    return;

  cycle_counter_ += CYCLE_PERIOD;
  if (cycle_counter_ < timer)
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
