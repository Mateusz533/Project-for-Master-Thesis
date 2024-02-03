#pragma once

#include "SystemElement.h"
#include "ScreenContent.h"
#include "Array.h"

// Klasa abstrakcyjna, nadrzędna dla elementów posiadających informacje do wyświetlenia na ekranie
class DisplayedElement : public SystemElement
{
  public:
    // Wykonuje pobrane od użytkownika polecania na podstawie wciśniętych przycisków
    virtual void executeCommands(const Array<bool, BUTTONS_NUMBER>& buttons) = 0;
    // Zwraca dane do wyświetlenia dotyczące tego elementu
    virtual ScreenContent getDataToDisplay() const = 0;
};
