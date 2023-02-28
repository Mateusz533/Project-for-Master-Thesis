#include <LiquidCrystal_I2C.h>                          // biblioteka obsługująca ekran z magistralą I2C
#include "configuration.h"
#include "PushButton.h"
#include "SystemElement.h"
#include "tests.h"
#pragma once

// Klasa przechowująca parametry prasy grzewczej
class HeatingPress
{
  public:
    // Konstruktor
    HeatingPress(short unsigned int max_threads_number):
      MAX_THREADS_NUMBER_ (max_threads_number)
    {
      // Utworzenie instancji dla każdego z przycisków
      buttons_ = new PushButton[BUTTONS_NUMBER] {
        PushButton(PIN_PB_UP),
        PushButton(PIN_PB_DOWN),
        PushButton(PIN_PB_LEFT),
        PushButton(PIN_PB_RIGHT),
        PushButton(PIN_PB_ACTION),
      };

      // Zarezerwowanie pamięci na każdy z wątków
      threads_ = new SystemElement*[MAX_THREADS_NUMBER_];
    }
    // Destruktor
    ~HeatingPress()
    {
      delete[] buttons_;
      for (int i = 0; i < threads_number_; i++)
        delete threads_[i];
      delete[] threads_;
      reportError("?");
    }
    // Dodaje podany element do systemu jako nowy wątek
    void addThread(SystemElement* new_thread);
    // Inicjalizuje urządzenie konfigurując wszystkie porty, ekran, liczniki oraz wyświetlając wprowadzenie
    void init();
    // Wykonuje jeden cykl działania systemu w przypadku nadejścia odpowiadającego mu czasu
    void run();
    // Wyświetla komunikat o błędzie o zadanym numerze, awaryjnie wyłącza grzałki oraz zatrzymuje program
    void reportError(String code);

  private:
    // Wyświetla informacje o urządzeniu
    void displayIntroduction();
    // Wykonuje polecenia użytkownika na podstawie wciśniętych przycisków
    void readUserCommands();
    // Wyświetla dane odpowiednie dla aktywnego okna z określoną częstotliwością
    void displayData();

    // Tablica przechowujaca instancję dla każdego z przycisków
    PushButton* buttons_ = NULL;
    // Tablica przechowujaca instancję dla każdego z wątków
    SystemElement** threads_ = NULL;
    // Obiektu obsługujący ekran LCD o 2 wierszach i 16 kolumnach
    LiquidCrystal_I2C lcd_ = LiquidCrystal_I2C(LCD_ADRESS, 16, 2);
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

void HeatingPress::displayIntroduction()
{
  lcd_.setCursor(0, 0);                       // ustawienie kursora w pozycji 0,0 (pierwsza kolumna, pierwszy wiersz)
  lcd_.print("Heating press");
  lcd_.setCursor(0, 1);                       // ustawienie kursora w pozycji 0,1 (pierwsza kolumna, drugi wiersz)
  lcd_.print("0-200N, 20-400");
  lcd_.write(DEGREE_SYMBOL_INDEX);            // wyświetlenie symbolu stopnia
  lcd_.print("C");
  delay(1000);
  lcd_.clear();
  lcd_.setCursor(0, 0);
  lcd_.print("Made in WUT by");
  lcd_.setCursor(0, 1);
  lcd_.print("M.Sc. M.Frejlich");
  delay(1000);
  lcd_.clear();
}

void HeatingPress::init()
{
  // Konfiguracja poszczególnych portów
  for (int i = 0; i < BUTTONS_NUMBER; i++)          // przyciski
    buttons_[i].init();
  for (int i = 0; i < threads_number_; i++)         // czujniki nacisku, temperatury oraz przekaźnik
    threads_[i]->init();

  // Konfiguracja ekranu
  lcd_.begin();                                     // inicjalizacja LCD
  lcd_.backlight();                                 // załączenie podświetlenia
  lcd_.createChar(DEGREE_SYMBOL_INDEX, DEGREE);     // utworzenie symbolu stopnia

  displayIntroduction();                            // wyświetlenie informacji o urządzeniu

  TCCR1B = (TCCR1B & B11111000) | B00000101;        // ustawienie częstotliwości PWM pinów 9 i 10 na 30.64 Hz
  cycle_counter_ = millis() + CYCLE_PERIOD;         // ustawienie wartości licznika czasu

  /*KONFIGURACJA PORTU SZEREGOWEGO - WYŁĄCZNIE DO TESTÓW*/
  Serial.begin(9600);
}

void HeatingPress::addThread(SystemElement* new_thread)
{
  if (threads_number_ < MAX_THREADS_NUMBER_)
    threads_[++threads_number_] = new_thread;
}

void HeatingPress::readUserCommands()
{
  if (buttons_[LEFT].isClicked() == buttons_[RIGHT].isClicked())
  {
    bool buttons_activity[BUTTONS_NUMBER];
    buttons_activity[UP] = buttons_[UP].isClicked() || buttons_[UP].isPressed(20);
    buttons_activity[DOWN] = buttons_[DOWN].isClicked() || buttons_[DOWN].isPressed(20);
    buttons_activity[LEFT] = buttons_[LEFT].isClicked();
    buttons_activity[RIGHT] = buttons_[RIGHT].isClicked();
    buttons_activity[ACTION] = buttons_[ACTION].isClicked();
    threads_[displayed_thread_id_]->getCommands(buttons_activity);  //executeCommands?
    return;
  }

  if (buttons_[LEFT].isClicked())                   // przełączenie wyświetlanego okna
    displayed_thread_id_ += threads_number_ - 1;
  if (buttons_[RIGHT].isClicked())
    ++displayed_thread_id_;

  displayed_thread_id_ %= threads_number_;

  for (int i = 0; i < BUTTONS_NUMBER; i++)          // zresetowanie stanów przycisków przy zmianie okna
    buttons_[i].reset();
}

void HeatingPress::displayData()
{
  static const unsigned int REFRESH_COUNTER_OVERFLOW = SCREEN_REFRESH_PERIOD / CYCLE_PERIOD;
  static unsigned int refresh_counter = 0;
  ++refresh_counter;
  if (refresh_counter < REFRESH_COUNTER_OVERFLOW && (buttons_[LEFT].isClicked() == buttons_[RIGHT].isClicked()))
    return;

  refresh_counter = 0;
  if (buttons_[LEFT].isClicked() != buttons_[RIGHT].isClicked())
    lcd_.clear();
  String first_line, second_line;
  threads_[displayed_thread_id_]->getDataToDisplay(first_line, second_line);
  lcd_.setCursor(0, 0);
  lcd_.print(first_line);
  lcd_.setCursor(0, 1);
  lcd_.print(second_line);
}

void HeatingPress::run()
{
  // Wykonywanie programu z określoną częstotliwością
  long unsigned int timer = millis();
  if (cycle_counter_ > timer + CYCLE_PERIOD)
    reportError("0");
  if (cycle_counter_ > timer)
    return;

  cycle_counter_ += CYCLE_PERIOD;
  if (cycle_counter_ < timer)
    reportError("1");

  // Pobranie stanu wejść, gdy stan niski oznacza wciśnięty przycisk
  for (int i = 0; i < BUTTONS_NUMBER; i++)
    buttons_[i].readSignal();

  // Odczyt interfejsu - działanie przycisków zależy od okna, dla którego zostały aktywowane
  readUserCommands();

  /*TEST CZUJNIKA NACISKU*/
  //testForceSensor();

  /*TEST CZUJNIKÓW TEMPERATTURY*/
  //testTemperatureSensors();

  /*TEST WZMACNIACZA*/
  //testAmplifier(A0, PIN_TEMPERATURE_SENSOR_TOP, 30);

  // Pomiar nacisku oraz wykonanie wszystkich okresowych działań obu płyt grzewczych
  for (int i = 0; i < threads_number_; i++)
    threads_[i]->run();

  // Wyświetlenie danych z określoną częstotliwością - w przypadku przełączenia okna następuje niezwłocznie
  displayData();

  /*TEST SYNCHRONIZACJI CZASU*/
  //testTimeSynchronization(timer);
}

void HeatingPress::reportError(String code)
{
  digitalWrite(PIN_HEAT_SUPPLY_TOP, HIGH);      // awaryjne wyłączenie grzania
  digitalWrite(PIN_HEAT_SUPPLY_BOT, HIGH);
  lcd_.clear();                                 // komunikat
  lcd_.setCursor(0, 0);
  lcd_.print("Error code " + code);
  lcd_.setCursor(0, 1);
  lcd_.print("Please restart");
  while (true);                                 // zatrzymanie wykonywania programu
}
