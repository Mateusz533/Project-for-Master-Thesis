#include "configuration.h"
#include "InputInterface.h"
#include "OutputInterface.h"
#include "SystemElement.h"
#include "reportError.h"
#include "hardwareTests.h"
#pragma once

// Klasa przechowująca parametry prasy grzewczej
class HeatingPress
{
  public:
    // Konstruktor
    HeatingPress(short unsigned int max_threads_number) :
      MAX_THREADS_NUMBER_(max_threads_number)
    {
      // Zarezerwowanie pamięci na każdy z wątków
      threads_ = new SystemElement*[MAX_THREADS_NUMBER_];
    }
    // Destruktor
    ~HeatingPress()
    {
      reportError(F("?"));
    }
    // Dodaje podany element do systemu jako nowy wątek
    void addThread(SystemElement* new_thread);
    // Inicjalizuje urządzenie konfigurując wszystkie porty, ekran, liczniki oraz wyświetlając wprowadzenie
    void init();
    // Wykonuje jeden cykl działania systemu w przypadku nadejścia odpowiadającego mu czasu
    void run();
    // Wyświetla komunikat o błędzie o zadanym numerze, awaryjnie wyłącza grzałki oraz zatrzymuje program
    void emergencyShutdown(const String& code);

  private:
    // Wykonuje polecenia użytkownika na podstawie wciśniętych przycisków
    void readUserCommands();
    // Wyświetla dane odpowiednie dla aktywnego okna z określoną częstotliwością
    void sendDataToDisplay();

    // Obiekt obsługujący interfejs wejściowy składający się z 5-ciu przycisków
    InputInterface buttons_;
    // Tablica przechowujaca instancję dla każdego z wątków
    SystemElement** threads_ = NULL;
    // Obiekt obsługujący interfejs wyjściowy składający się z ekranu LCD o 2 wierszach i 16 kolumnach
    OutputInterface lcd_;
    /* Zmienna przechowująca numer jednego z czterech okien interfejsu:
       0 - informacje o nacisku
       1 - informacje o temperaturze i grzaniu górnej płyty
       2 - informacje o temperaturze i grzaniu dolnej płyty
       3 - informacje o parametrach globalnych
    */
    short int displayed_thread_id_ = 0;
    short unsigned int threads_number_ = 0;
    const short unsigned int MAX_THREADS_NUMBER_ = 4;
    // Licznik umożliwiający synchroniczne wykonywanie programu
    long unsigned int cycle_counter_ = 0;
};

void HeatingPress::init()
{
  // Konfiguracja poszczególnych portów
  buttons_.init();                                      // przyciski
  for (unsigned int i = 0; i < threads_number_; ++i)    // czujniki nacisku, temperatury oraz przekaźnik
    threads_[i]->init();

  // Konfiguracja ekranu LCD
  lcd_.init();

  TCCR1B = (TCCR1B & B11111000) | B00000101;    // ustawienie częstotliwości PWM pinów 9 i 10 na 30.64 Hz
  cycle_counter_ = millis() + CYCLE_PERIOD;     // ustawienie wartości licznika czasu

  /*KONFIGURACJA PORTU SZEREGOWEGO - WYŁĄCZNIE DO TESTÓW*/
  Serial.begin(9600);
}

void HeatingPress::addThread(SystemElement* new_thread)
{
  if (threads_number_ < MAX_THREADS_NUMBER_)
    threads_[threads_number_++] = new_thread;
}

void HeatingPress::readUserCommands()
{
  bool* buttons_activity = buttons_.getButtonsActivity();

  if (!buttons_.isWindowChanged())
  {
    threads_[displayed_thread_id_]->executeCommands(buttons_activity);
    return;
  }

  if (buttons_activity[LEFT])
    displayed_thread_id_ += threads_number_ - 1;    // przełączenie wyświetlanego okna
  if (buttons_activity[RIGHT])
    ++displayed_thread_id_;

  displayed_thread_id_ %= threads_number_;
}

void HeatingPress::sendDataToDisplay()
{
  String first_line, second_line;
  threads_[displayed_thread_id_]->getDataToDisplay(first_line, second_line);
  lcd_.displayData(first_line, second_line, buttons_.isWindowChanged());
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

  // Pobranie stanu wejść każdego z przycisków
  buttons_.readSignals();

  // Odczyt interfejsu - działanie przycisków zależy od okna, dla którego zostały aktywowane
  readUserCommands();

  /*TEST CZUJNIKA NACISKU*/
  // testForceSensor();

  /*TEST CZUJNIKÓW TEMPERATTURY*/
  // testTemperatureSensors();

  /*TEST WZMACNIACZA*/
  // testAmplifier(PIN_TEMPERATURE_SENSOR_BOT);

  // Pomiar nacisku oraz wykonanie wszystkich okresowych działań obu płyt grzewczych
  for (unsigned int i = 0; i < threads_number_; ++i)
    threads_[i]->run();

  // Wyświetlenie danych z określoną częstotliwością - w przypadku przełączenia okna następuje niezwłocznie
  sendDataToDisplay();

  /*TEST SYNCHRONIZACJI CZASU*/
  // testTimeSynchronization(timer);
}

void HeatingPress::emergencyShutdown(const String& code)
{
  digitalWrite(PIN_HEAT_SUPPLY_TOP, HIGH);                        // awaryjne wyłączenie grzania
  digitalWrite(PIN_HEAT_SUPPLY_BOT, HIGH);                        //
  String first_line = String(F("Error code ")) + code + "   ";    // komunikat
  String second_line = F("Please restart  ");                     //
  while (true)                                                    // zatrzymanie wykonywania programu
  {
    lcd_.displayData(first_line, second_line);
    delay(CYCLE_PERIOD);
  };
}
