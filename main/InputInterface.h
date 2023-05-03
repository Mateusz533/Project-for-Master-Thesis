#pragma once

#include "PushButton.h"
#include "SystemElement.h"

// Klasa przechowująca wszystkie przyciski interfejsu wejściowego
class InputInterface : public SystemElement
{
  public:
    InputInterface();
    ~InputInterface();
    // Konfiguruje porty wejścia/wyjścia dla tego elementu
    void init() override;
    // Odczytuje sygnały z portów dla każdego z przycisków
    void run() override;
    // Zwraca tablicę aktywności wciśniętych przycisków
    bool* getButtonsActivity();
    // Zwraca zwraca prawdę, gdy okno zostało przełączone oraz fałsz w przeciwnym wypadku
    bool isWindowChanged() const;

  private:
    // Tablica przechowujaca instancję dla każdego z przycisków
    PushButton* buttons_{ nullptr };
    bool is_ready_{ false };
};
