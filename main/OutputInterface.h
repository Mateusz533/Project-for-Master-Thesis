#include <LiquidCrystal_I2C.h>    // biblioteka obsługująca ekran z magistralą I2C
#pragma once

// Klasa przechowująca interfejs wyjściowy w postaci ekranu LCD
class OutputInterface
{
  public:
    // Konfiguruje porty wejścia/wyjścia dla tego elementu
    void init();
    // Wyświetla przekazane dane z określoną częstotliwością, w przypadku wymuszenia natychmiast wyświetla ustaloną część danych
    void displayData(String& first_line, String& second_line, const bool force_refresh = false);

  private:
    // Wyświetla informacje o urządzeniu
    void displayIntroduction();

    // Obiekt obsługujący ekran LCD o 2 wierszach i 16 kolumnach
    LiquidCrystal_I2C lcd_ = LiquidCrystal_I2C(LCD_ADRESS, LCD_COLUMNS_NUMBER, LCD_ROWS_NUMBER);
};

void OutputInterface::init()
{
  // Konfiguracja ekranu
  lcd_.begin();                                    // inicjalizacja LCD
  lcd_.backlight();                                // załączenie podświetlenia
  uint8_t degree[8] = DEGREE_SYMBOL;               //
  lcd_.createChar(DEGREE_SYMBOL_INDEX, degree);    // utworzenie symbolu stopnia
                                                   //
  displayIntroduction();                           // wyświetlenie informacji o urządzeniu
}

void OutputInterface::displayData(String& first_line, String& second_line, const bool force_refresh)
{
  static const unsigned int REFRESH_COUNTER_OVERFLOW = SCREEN_REFRESH_PERIOD / CYCLE_PERIOD / DISPLAYED_DATA_DIVISOR;
  static const unsigned int CHARS_NUMBER = LCD_ROWS_NUMBER * LCD_COLUMNS_NUMBER / DISPLAYED_DATA_DIVISOR;
  static unsigned int refresh_counter = 0;
  static unsigned int index = 0;

  ++refresh_counter;
  if (refresh_counter < REFRESH_COUNTER_OVERFLOW && !force_refresh)
    return;

  refresh_counter = 0;
  if (force_refresh)
    index = 0;

  if (index < LCD_COLUMNS_NUMBER)
  {
    lcd_.setCursor(index, 0);
    lcd_.print(first_line.substring(index, index + CHARS_NUMBER));
  }
  else
  {
    lcd_.setCursor(index - LCD_COLUMNS_NUMBER, 1);
    lcd_.print(second_line.substring(index - LCD_COLUMNS_NUMBER, index + CHARS_NUMBER - LCD_COLUMNS_NUMBER));
  }
  index += CHARS_NUMBER;
  index %= LCD_ROWS_NUMBER * LCD_COLUMNS_NUMBER;
}

void OutputInterface::displayIntroduction()
{
  lcd_.setCursor(0, 0);               // ustawienie kursora w pozycji 0,0 (pierwsza kolumna, pierwszy wiersz)
  lcd_.print(F("Heating press"));     //
  lcd_.setCursor(0, 1);               // ustawienie kursora w pozycji 0,1 (pierwsza kolumna, drugi wiersz)
  lcd_.print(F("0-200N, 20-400"));    //
  lcd_.write(DEGREE_SYMBOL_INDEX);    // wyświetlenie symbolu stopnia
  lcd_.print(F("C"));
  delay(1000);
  lcd_.clear();
  lcd_.setCursor(0, 0);
  lcd_.print(F("Made in WUT by"));
  lcd_.setCursor(0, 1);
  lcd_.print(F("M.Sc. M.Frejlich"));
  delay(1000);
  lcd_.clear();
}
