#include "OutputInterface.h"

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

void OutputInterface::run()
{
  char text[CHAR_SEQUENCE_LENGTH + 1];
  text[CHAR_SEQUENCE_LENGTH] = '\0';
  content_.copySequence(text, index_, CHAR_SEQUENCE_LENGTH);
  lcd_.setCursor(index_ % LCD_COLUMNS_NUMBER, index_ >= LCD_COLUMNS_NUMBER);
  lcd_.print(text);
  index_ += CHAR_SEQUENCE_LENGTH;
  index_ %= LCD_ROWS_NUMBER * LCD_COLUMNS_NUMBER;
}

void OutputInterface::setDisplayedData(const ScreenContent& content, const bool force_refresh)
{
  content_ = content;
  if (force_refresh)
    index_ = 0;
}

inline void OutputInterface::displayIntroduction()
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
