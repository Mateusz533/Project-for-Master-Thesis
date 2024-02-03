#pragma once

#include <LiquidCrystal_I2C.h>    // biblioteka obsługująca ekran z magistralą I2C
#include "SystemElement.h"
#include "ScreenContent.h"

constexpr unsigned short CHAR_SEQUENCE_LENGTH = LCD_ROWS_NUMBER * LCD_COLUMNS_NUMBER / DISPLAYED_DATA_DIVISOR;

// Klasa przechowująca interfejs wyjściowy w postaci ekranu LCD
class OutputInterface : public SystemElement
{
  public:
    OutputInterface() = default;
    ~OutputInterface() = default;
    // Konfiguruje porty wejścia/wyjścia dla tego elementu
    void init() override;
    // Wyświetla przekazane dane z określoną częstotliwością
    void run() override;
    // Zapisuje przekazane dane do wyświetlenia, w przypadku wymuszenia natychmiast wyświetla ustaloną część danych
    void setDisplayedData(const ScreenContent& content, const bool force_refresh = false);

  private:
    // Wyświetla informacje o urządzeniu
    inline void displayIntroduction();

    // Obiekt obsługujący ekran LCD o 2 wierszach i 16 kolumnach
    LiquidCrystal_I2C lcd_{ LCD_ADRESS, LCD_COLUMNS_NUMBER, LCD_ROWS_NUMBER };
    // Zmienne przechowujące dane do wyświetlenia oraz indeks wyświetlanego obecnie znaku
    ScreenContent content_;
    BasicSize index_{ 0 };
};
