#include "SystemElement.h"
#pragma once

// Klasa abstrakcyjna, nadrzędna dla elementów posiadających informacje do wyświetlenia na ekranie
class DisplayedElement : public SystemElement
{
  public:
    // Wykonuje pobrane od użytkownika polecania na podstawie wciśniętych przycisków
    virtual void executeCommands(const bool buttons[]) = 0;
    // Wyświetla dane dotyczące tego elementu
    virtual void getDataToDisplay(String& first_line, String& second_line) const = 0;
};
