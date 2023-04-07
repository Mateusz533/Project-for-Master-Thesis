#include "configuration.h"
#pragma once

// Klasa abstrakcyjna, nadrzędna dla każdego elementu działającego jako osobny wątek
class SystemElement
{
  public:
    virtual ~SystemElement() = default;
    // Konfiguruje porty wejścia/wyjścia dla tego elementu
    virtual void init() = 0;
    // Wykonuje wszystkie funkcje elementu z odpowiednimi częstotliwościami
    virtual void run() = 0;

  protected:
    static int ambient_temperature;    // temperatura otoczenia [*C]
    static int max_heating_power;      // maksymalna moc grzania jednej płyty [W]
};

// Ustawienie domyślnych wartości temperatury otoczenia oraz maksymalnej mocy grznia
int SystemElement::ambient_temperature = DEFAULT_AMBIENT_TEMPERATURE;
int SystemElement::max_heating_power = MAX_HEATING_POWER;
