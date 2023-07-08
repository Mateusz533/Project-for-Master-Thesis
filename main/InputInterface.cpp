#include "InputInterface.h"

InputInterface::InputInterface()
{
  // Utworzenie instancji dla każdego z przycisków
  p_buttons_ = new PushButton[BUTTONS_NUMBER]{
    PushButton(PIN_PB_UP),    PushButton(PIN_PB_DOWN),   PushButton(PIN_PB_LEFT),
    PushButton(PIN_PB_RIGHT), PushButton(PIN_PB_ACTION),
  };
}

InputInterface::~InputInterface()
{
  delete[] p_buttons_;
}

void InputInterface::init()
{
  // Konfiguracja poszczególnych portów
  for (unsigned int i = 0; i < BUTTONS_NUMBER; ++i)
    p_buttons_[i].init();
}

void InputInterface::run()
{
  // Pobranie stanu wejść, gdy stan niski oznacza wciśnięty przycisk
  for (unsigned int i = 0; i < BUTTONS_NUMBER; ++i)
    p_buttons_[i].readSignal();
  is_ready_ = true;
}

bool* InputInterface::getButtonsActivity()
{
  if (!is_ready_)
    return nullptr;

  is_ready_ = false;
  static bool buttons_activity[BUTTONS_NUMBER];
  buttons_activity[UP] =
    p_buttons_[UP].isClicked() || p_buttons_[UP].isPressed(CONTINUOUS_PRESSING_ACTIVATION_TIME / BUTTONS_REFRESH_PERIOD);
  buttons_activity[DOWN] =
    p_buttons_[DOWN].isClicked() || p_buttons_[DOWN].isPressed(CONTINUOUS_PRESSING_ACTIVATION_TIME / BUTTONS_REFRESH_PERIOD);
  buttons_activity[LEFT] = p_buttons_[LEFT].isClicked();
  buttons_activity[RIGHT] = p_buttons_[RIGHT].isClicked();
  buttons_activity[ACTION] = p_buttons_[ACTION].isClicked();

  if (isWindowChanged())
  {
    p_buttons_[UP].reset();    // zresetowanie stanów przycisków przy zmianie okna
    p_buttons_[DOWN].reset();
    p_buttons_[ACTION].reset();
  }

  return buttons_activity;
}

bool InputInterface::isWindowChanged() const
{
  return p_buttons_[LEFT].isClicked() != p_buttons_[RIGHT].isClicked();
}
