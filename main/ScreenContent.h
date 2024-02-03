#pragma once

#include "configuration.h"

class ScreenContent
{
  public:
    ScreenContent() = default;
    ScreenContent(const char first_line[LCD_COLUMNS_NUMBER], const char second_line[LCD_COLUMNS_NUMBER], const bool progmem = true);
    ~ScreenContent() = default;
    BasicSize putNumberFromEnd(const int number, const BasicSize end, const BasicSize line);
    void putWord(const String& word, const BasicSize begin, const BasicSize line);
    void copySequence(char buffer[], const BasicSize start, const BasicSize length);

  private:
    char first_line_[LCD_COLUMNS_NUMBER];
    char second_line_[LCD_COLUMNS_NUMBER];
};
