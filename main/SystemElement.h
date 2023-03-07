#include "configuration.h"
#pragma once

// Klasa abstrakcyjna, nadrzędna dla czujnika nacisku oraz płyt grzewczych
class SystemElement
{
  public:
    // Konfiguruje porty wejścia/wyjścia dla tego elementu
    virtual void init() = 0;
    // Wykonuje pobrane od użytkownika polecania na podstawie wciśniętych przycisków
    virtual void executeCommands(const bool buttons[]) = 0;
    // Wyświetla dane dotyczące tego elementu
    virtual void getDataToDisplay(String& first_line, String& second_line) = 0;
    // Wykonuje wszystkie funkcje elementu z odpowiednimi częstotliwościami
    virtual void run() = 0;

  protected:
    static int ambient_temperature_;   // temperatura otoczenia [*C]
};

// Ustawienie domyślnej wartości temperatury otoczenia
int SystemElement::ambient_temperature_ = DEFAULT_AMBIENT_TEMPERATURE;
