#pragma once

#include "PushButton.h"
#include "SystemElement.h"
#include "Array.h"

// Klasa przechowująca wszystkie przyciski interfejsu wejściowego
class InputInterface : public SystemElement
{
  public:
    InputInterface();
    ~InputInterface() = default;
    // Konfiguruje porty wejścia/wyjścia dla tego elementu
    void init() override;
    // Odczytuje sygnały z portów dla każdego z przycisków
    void run() override;
    // Zwraca stan aktywności wciśniętych przycisków
    Array<bool, BUTTONS_NUMBER> getButtonsActivity() const;
    // Przedawnia stan przycisków w danym cyklu
    void expireButtons();
    // Zwraca prawdę, gdy okno zostało przełączone oraz fałsz w przeciwnym wypadku
    bool isWindowChanged() const;
    // Zwraca prawdę, jeśli stan przycisków został odświeżony od ostatniego resetu
    bool isReady() const;

  private:
    // Tablica przechowujaca instancję dla każdego z przycisków
    Array<PushButton, BUTTONS_NUMBER> buttons_;
    bool is_ready_{ false };
};
