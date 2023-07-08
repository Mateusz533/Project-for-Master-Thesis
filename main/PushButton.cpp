#include "PushButton.h"

PushButton::PushButton(const short unsigned int pin) :
  PIN_{ pin }
{
  // przypisanie numeru pinu w mikrosterowniku
}

void PushButton::init()
{
  pinMode(PIN_, INPUT);    // ustawienie przycisku jako wejście z rezystorem podciągającym
  digitalWrite(PIN_, HIGH);
}

void PushButton::readSignal()
{
  previously_pressed_ = currently_pressed_;
  currently_pressed_ = !digitalRead(PIN_);    // przycisk wciśnięty przyjmuje stan niski
  if (currently_pressed_)
    ++time_of_continuous_pressing_;
  else
    time_of_continuous_pressing_ = 0;
}

bool PushButton::isClicked() const
{
  return currently_pressed_ && !previously_pressed_;    // stan aktywowany zboczem narastającym
}

bool PushButton::isReleased() const
{
  return !currently_pressed_ && previously_pressed_;    // stan aktywowany zboczem opadającym
}

bool PushButton::isPressed(const unsigned int min_pressing_time) const
{
  return time_of_continuous_pressing_ >= min_pressing_time;    // stan aktywowany trwającym naciśnięciem
}

void PushButton::reset()
{
  currently_pressed_ = false;
  previously_pressed_ = false;
  time_of_continuous_pressing_ = 0;
}
