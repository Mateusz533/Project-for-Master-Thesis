#include <LiquidCrystal_I2C.h>                          // biblioteka obsługująca ekran z magistralą I2C
#include "configuration.h"
#include "PushButton.h"
#include "SystemElement.h"
#include "ForceSensor.h"
#include "HeatingPlate.h"
#include "SettingsController.h"
#include "reportError.h"
#include "tests.h"

// Utworzenie obiektu obsługującego ekran LCD o 2 wierszach i 16 kolumnach
LiquidCrystal_I2C lcd(LCD_ADRESS, 16, 2);

// Utworzenie instancji dla każdego z przycisków
PushButton buttons[BUTTONS_NUMBER] = {
  PushButton(PIN_PB_UP),
  PushButton(PIN_PB_DOWN),
  PushButton(PIN_PB_LEFT),
  PushButton(PIN_PB_RIGHT),
  PushButton(PIN_PB_ACTION)
};

/* Zmienna przechowująca numer jednego z trzech okien interfejsu:
   0 - informacje o nacisku
   1 - informacje o temperaturze i grzaniu górnej płyty
   2 - informacje o temperaturze i grzaniu dolnej płyty
   3 - informacje o parametrach globalnych
*/
const short unsigned int THREADS_NUMBER = 4;
short int displayed_thread_id = 0;

// Utworzenie instancji dla czujnika oraz każdej grzałki
SystemElement* threads[THREADS_NUMBER] = {
  new ForceSensor(PIN_FORCE_SENSOR),
  new HeatingPlate(PIN_TEMPERATURE_SENSOR_TOP, PIN_HEAT_SUPPLY_TOP, 1.0, 0.0, "TOP"),   // najpierw 0.96, potem 0.80
  new HeatingPlate(PIN_TEMPERATURE_SENSOR_BOT, PIN_HEAT_SUPPLY_BOT, 1.0, 0.0, "BOT"),   // najpierw 1.00, potem 0.62
  new SettingsController()
};

// Licznik umożliwiający synchroniczne wykonywanie programu
long unsigned int cycle_counter = 0;

/*Funkcje wykonujące kolejne operacje*/

// Wyświetla informacje o urządzeniu
void displayIntroduction();
// Wykonuje polecenia użytkownika na podstawie wciśniętych przycisków
void readUserCommands();
// Wyświetla dane odpowiednie dla aktywnego okna z określoną częstotliwością
void displayData();

/*
  HeatingPress device(<many arguments>);
  void setup()
  {
  device.init();
  device.printIntroduction();
  }
  void loop()
  {
  device.run();
  }
*/

void setup()
{
  // Konfiguracja poszczególnych portów
  for (int i = 0; i < BUTTONS_NUMBER; i++)          // przyciski
    buttons[i].init();
  for (int i = 0; i < THREADS_NUMBER; i++)          // czujniki nacisku, temperatury oraz przekaźnik
    threads[i]->init();

  // Konfiguracja ekranu
  lcd.begin();                                      // inicjalizacja LCD
  lcd.backlight();                                  // załączenie podświetlenia
  lcd.createChar(DEGREE_SYMBOL_INDEX, DEGREE);      // utworzenie symbolu stopnia

  displayIntroduction();                            // wyświetlenie informacji o urządzeniu

  TCCR1B = (TCCR1B & B11111000) | B00000101;        // ustawienie częstotliwości PWM pinów 9 i 10 na 30.64 Hz
  cycle_counter = millis() + CYCLE_PERIOD;          // ustawienie wartości licznika czasu

  /*KONFIGURACJA PORTU SZEREGOWEGO - WYŁĄCZNIE DO TESTÓW*/
  Serial.begin(9600);
}

void loop()
{
  // Wykonywanie programu z określoną częstotliwością
  long unsigned int timer = millis();
  if (cycle_counter > timer + CYCLE_PERIOD)
    reportError("0");
  if (cycle_counter > timer)
    return;

  cycle_counter += CYCLE_PERIOD;
  if (cycle_counter < timer)
    reportError("1");

  // Pobranie stanu wejść, gdy stan niski oznacza wciśnięty przycisk
  for (int i = 0; i < BUTTONS_NUMBER; i++)
    buttons[i].readSignal();

  // Odczyt interfejsu - działanie przycisków zależy od okna, dla którego zostały aktywowane
  readUserCommands();

  /*TEST CZUJNIKA NACISKU*/
  //testForceSensor();

  /*TEST CZUJNIKÓW TEMPERATTURY*/
  //testTemperatureSensors();

  /*TEST WZMACNIACZA*/
  //testAmplifier(A0, PIN_TEMPERATURE_SENSOR_TOP, 30);

  // Pomiar nacisku oraz wykonanie wszystkich okresowych działań obu płyt grzewczych
  for (int i = 0; i < THREADS_NUMBER; i++)
    threads[i]->run();

  // Wyświetlenie danych z określoną częstotliwością - w przypadku przełączenia okna następuje niezwłocznie
  displayData();

  /*TEST SYNCHRONIZACJI CZASU*/
  //testTimeSynchronization(timer);
}

void displayIntroduction()
{
  lcd.setCursor(0, 0);                        // ustawienie kursora w pozycji 0,0 (pierwsza kolumna, pierwszy wiersz)
  lcd.print("Heating press");
  lcd.setCursor(0, 1);                        // ustawienie kursora w pozycji 0,1 (pierwsza kolumna, drugi wiersz)
  lcd.print("0-200N, 20-400");
  lcd.write(DEGREE_SYMBOL_INDEX);             // wyświetlenie symbolu stopnia
  lcd.print("C");
  delay(1000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Made in WUT by");
  lcd.setCursor(0, 1);
  lcd.print("M.Sc. M.Frejlich");
  delay(1000);
  lcd.clear();
}

void readUserCommands()
{
  if (buttons[LEFT].isClicked() == buttons[RIGHT].isClicked())
  {
    bool buttons_activity[BUTTONS_NUMBER];
    buttons_activity[UP] = buttons[UP].isClicked() || buttons[UP].isPressed(20);
    buttons_activity[DOWN] = buttons[DOWN].isClicked() || buttons[DOWN].isPressed(20);
    buttons_activity[LEFT] = buttons[LEFT].isClicked();
    buttons_activity[RIGHT] = buttons[RIGHT].isClicked();
    buttons_activity[ACTION] = buttons[ACTION].isClicked();
    threads[displayed_thread_id]->getCommands(buttons_activity);
    return;
  }

  if (buttons[LEFT].isClicked())                // przełączenie wyświetlanego okna
    displayed_thread_id += THREADS_NUMBER - 1;
  if (buttons[RIGHT].isClicked())
    ++displayed_thread_id;

  displayed_thread_id %= THREADS_NUMBER;

  for (int i = 0; i < BUTTONS_NUMBER; i++)      // zresetowanie stanów przycisków przy zmianie okna
    buttons[i].reset();
}

void displayData()
{
  static const unsigned int REFRESH_COUNTER_OVERFLOW = SCREEN_REFRESH_PERIOD / CYCLE_PERIOD;
  static unsigned int refresh_counter = 0;
  ++refresh_counter;
  if (refresh_counter < REFRESH_COUNTER_OVERFLOW && (buttons[LEFT].isClicked() == buttons[RIGHT].isClicked()))
    return;

  refresh_counter = 0;
  if (buttons[LEFT].isClicked() != buttons[RIGHT].isClicked())
    lcd.clear();
  String first_line, second_line;
  threads[displayed_thread_id]->getDataToDisplay(first_line, second_line);
  lcd.setCursor(0, 0);
  lcd.print(first_line);
  lcd.setCursor(0, 1);
  lcd.print(second_line);
}

void reportError(String code)
{
  digitalWrite(PIN_HEAT_SUPPLY_TOP, HIGH);      // awaryjne wyłączenie grzania
  digitalWrite(PIN_HEAT_SUPPLY_BOT, HIGH);
  lcd.clear();                                  // komunikat
  lcd.setCursor(0, 0);
  lcd.print("Error code " + code);
  lcd.setCursor(0, 1);
  lcd.print("Please restart");
  while (true);                                 // zatrzymanie wykonywania programu
}
