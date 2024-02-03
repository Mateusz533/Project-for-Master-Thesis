#include "InputInterface.h"

InputInterface::InputInterface()
{
  // Utworzenie instancji dla każdego z przycisków
  const PushButton temp_buttons[BUTTONS_NUMBER]{ PushButton(PIN_PB_UP), PushButton(PIN_PB_DOWN), PushButton(PIN_PB_LEFT),
                                           PushButton(PIN_PB_RIGHT), PushButton(PIN_PB_ACTION) };
  this->buttons_ = Array<PushButton, BUTTONS_NUMBER>(temp_buttons);
}

void InputInterface::init()
{
  // Konfiguracja poszczególnych portów
  for (CollectionSize i = 0; i < BUTTONS_NUMBER; ++i)
    this->buttons_[i].init();
}

void InputInterface::run()
{
  // Pobranie stanu wejść, gdy stan niski oznacza wciśnięty przycisk
  for (CollectionSize i = 0; i < BUTTONS_NUMBER; ++i)
    this->buttons_[i].readSignal();
  is_ready_ = true;
}

Array<bool, BUTTONS_NUMBER> InputInterface::getButtonsActivity() const
{
  constexpr int PRESSING_PERIODS_TO_ACTIVATION = CONTINUOUS_PRESSING_ACTIVATION_TIME / BUTTONS_REFRESH_PERIOD;
  Array<bool, BUTTONS_NUMBER> buttons_activity;

  for (CollectionSize i = 0; i < BUTTONS_NUMBER; ++i)
    buttons_activity[i] = this->buttons_.get(i).isClicked();

  buttons_activity[UP] |= this->buttons_.get(UP).isPressed(PRESSING_PERIODS_TO_ACTIVATION);
  buttons_activity[DOWN] |= this->buttons_.get(DOWN).isPressed(PRESSING_PERIODS_TO_ACTIVATION);

  return buttons_activity;
}

void InputInterface::expireButtons()
{
  is_ready_ = false;
  if (isWindowChanged())
  {
    this->buttons_[UP].reset();    // zresetowanie stanów przycisków przy zmianie okna
    this->buttons_[DOWN].reset();
    this->buttons_[ACTION].reset();
  }
}

bool InputInterface::isWindowChanged() const
{
  return this->buttons_.get(LEFT).isClicked() != this->buttons_.get(RIGHT).isClicked();
}

bool InputInterface::isReady() const
{
  return is_ready_;
}
