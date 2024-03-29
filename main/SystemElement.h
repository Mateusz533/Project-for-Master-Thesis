#pragma once

#include "configuration.h"

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
    static DegCelsius s_ambient_temperature;    // temperatura otoczenia [*C]
    static StorageWatt s_max_heating_power;        // maksymalna moc grzania jednej płyty [W]
};
