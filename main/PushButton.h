#pragma once

// Klasa przechowująca parametry przycisku
class PushButton
{
  public:
    // Konstruktor
    PushButton(short unsigned int pin) :
      PIN_(pin)
    {
      // przypisanie numeru pinu w mikrosterowniku
    }
    // Konfiguruje porty wejścia/wyjścia dla tego elementu
    void init();
    // Odczytuje sygnał z portu, uznając stan niski za wciśnięty przycisk
    void readSignal();
    // Zwraca prawdę dla zbocza narastającego oraz fałsz w przypadku jego braku
    bool isClicked() const;
    // Zwraca prawdę dla zbocza opadającego oraz fałsz w przypadku jego braku
    bool isReleased() const;
    // Zwraca prawdę dla wciśniętego przycisku przez żądaną liczbę cykli oraz fałsz w przeciwnym wypadku
    bool isPressed(unsigned int min_pressing_time) const;
    // Zeruje wszystkie zapamiętane stany przycisku
    void reset();

  private:
    // Zmienne przechowujące ostatnie stany przycisku
    bool currently_pressed_ = false;
    bool previously_pressed_ = false;
    unsigned int time_of_continuous_pressing_ = 0;    // czas ciągłego wciśnęcia mierzony liczbą cykli
    // Numer pinu przypisany do przycisku
    const short unsigned int PIN_ = 0;
};

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

bool PushButton::isPressed(unsigned int min_pressing_time = 1) const
{
  return time_of_continuous_pressing_ >= min_pressing_time;    // stan aktywowany trwającym naciśnięciem
}

void PushButton::reset()
{
  currently_pressed_ = false;
  previously_pressed_ = false;
  time_of_continuous_pressing_ = 0;
}
