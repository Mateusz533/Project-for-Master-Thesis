#include "PushButton.h"
#pragma once

// Klasa przechowująca wszystkie przyciski interfejsu wejściowego
class InputInterface
{
  public:
    // Konstruktor
    InputInterface()
    {
      // Utworzenie instancji dla każdego z przycisków
      buttons_ = new PushButton[BUTTONS_NUMBER]{
        PushButton(PIN_PB_UP), PushButton(PIN_PB_DOWN), PushButton(PIN_PB_LEFT), PushButton(PIN_PB_RIGHT), PushButton(PIN_PB_ACTION),
      };
    }
    // Konfiguruje porty wejścia/wyjścia dla tego elementu
    void init();
    // Odczytuje sygnały z portów dla każdego z przycisków
    void readSignals();
    // Zwraca tablicę aktywności wciśniętych przycisków
    bool* getButtonsActivity();
    // Zwraca zwraca prawdę, gdy okno zostało przełączone oraz fałsz w przeciwnym wypadku
    bool isWindowChanged() const;

  private:
    // Tablica przechowujaca instancję dla każdego z przycisków
    PushButton* buttons_ = nullptr;
};

void InputInterface::init()
{
  // Konfiguracja poszczególnych portów
  for (unsigned int i = 0; i < BUTTONS_NUMBER; ++i)
    buttons_[i].init();
}

void InputInterface::readSignals()
{
  // Pobranie stanu wejść, gdy stan niski oznacza wciśnięty przycisk
  for (unsigned int i = 0; i < BUTTONS_NUMBER; ++i)
    buttons_[i].readSignal();
}

bool* InputInterface::getButtonsActivity()
{
  bool buttons_activity[BUTTONS_NUMBER];
  buttons_activity[UP] = buttons_[UP].isClicked() || buttons_[UP].isPressed(CONTINUOUS_PRESSING_ACTIVATION_TIME / CYCLE_PERIOD);
  buttons_activity[DOWN] = buttons_[DOWN].isClicked() || buttons_[DOWN].isPressed(CONTINUOUS_PRESSING_ACTIVATION_TIME / CYCLE_PERIOD);
  buttons_activity[LEFT] = buttons_[LEFT].isClicked();
  buttons_activity[RIGHT] = buttons_[RIGHT].isClicked();
  buttons_activity[ACTION] = buttons_[ACTION].isClicked();

  if (isWindowChanged())
  {
    buttons_[UP].reset();    // zresetowanie stanów przycisków przy zmianie okna
    buttons_[DOWN].reset();
    buttons_[ACTION].reset();
  }

  bool* ptr = buttons_activity;
  return ptr;
}

bool InputInterface::isWindowChanged() const
{
  return buttons_[LEFT].isClicked() != buttons_[RIGHT].isClicked();
}
