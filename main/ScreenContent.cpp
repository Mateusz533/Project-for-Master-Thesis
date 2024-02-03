#include "ScreenContent.h"

ScreenContent::ScreenContent(const char first_line[LCD_COLUMNS_NUMBER], const char second_line[LCD_COLUMNS_NUMBER], const bool progmem)
{
  if (progmem)
    for (BasicSize i = 0; i < LCD_COLUMNS_NUMBER; ++i)
    {
      first_line_[i] = pgm_read_byte(first_line + i);
      second_line_[i] = pgm_read_byte(second_line + i);
    }
  else
    for (BasicSize i = 0; i < LCD_COLUMNS_NUMBER; ++i)
    {
      first_line_[i] = first_line[i];
      second_line_[i] = second_line[i];
    }
}

BasicSize ScreenContent::putNumberFromEnd(const int number, const BasicSize end, const BasicSize line)
{
  char(&selected_line)[LCD_COLUMNS_NUMBER]{ line % LCD_ROWS_NUMBER == 0 ? first_line_ : second_line_ };
  int value{ number };
  BasicSize i{ end % LCD_COLUMNS_NUMBER };
  do
  {
    selected_line[i] = '0' + value % 10;
    value /= 10;
    i = (i + LCD_COLUMNS_NUMBER - 1) % LCD_COLUMNS_NUMBER;
  } while (value != 0);

  if (number < 0)
  {
    selected_line[i] = '-';
    i = (i + LCD_COLUMNS_NUMBER - 1) % LCD_COLUMNS_NUMBER;
  }

  return i;
}

void ScreenContent::putWord(const String& word, const BasicSize begin, const BasicSize line)
{
  char(&selected_line)[LCD_COLUMNS_NUMBER]{ line % LCD_ROWS_NUMBER == 0 ? first_line_ : second_line_ };
  for (BasicSize i = 0; i < word.length(); ++i)
    selected_line[(begin + i) % LCD_COLUMNS_NUMBER] = word[i];
}

void ScreenContent::copySequence(char buffer[], const BasicSize start, const BasicSize length)
{
  for (BasicSize i = 0; i < length; ++i)
    buffer[i] = start + i < LCD_COLUMNS_NUMBER ? first_line_[start + i] : second_line_[start + i - LCD_COLUMNS_NUMBER];
}
