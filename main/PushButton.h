#pragma once

// Klasa przechowująca parametry przycisku
class PushButton
{
  public:
    // Konstruktor
    PushButton(short unsigned int pin):
      PIN_(pin)
    {
      // przypisanie numeru pinu w mikrosterowniku
    }
    // Konfiguruje porty wejścia/wyjścia dla tego elementu
    void init();
    // Odczytuje sygnał z portu, uznając stan niski za wciśnięty przycisk
    void readSignal();
    // Zwraca prawdę dla zbocza narastającego oraz fałsz w przypadku jego braku
    bool isClicked();
    // Zwraca prawdę dla zbocza opadającego oraz fałsz w przypadku jego braku
    bool isReleased();
    // Zwraca prawdę dla wciśniętego przycisku przez żądaną liczbę cykli oraz fałsz w przeciwnym wypadku
    bool isPressed(int min_pressing_time);
    // Zeruje wszystkie zapamiętane stany przycisku
    void reset();

  private:
    // Zmienne przechowujące ostatnie stany przycisku
    bool currently_pressed_ = false;
    bool previously_pressed_ = false;
    int time_of_continuous_pressing_ = 0;                   // czas ciągłego wciśnęcia mierzony liczbą cykli
    // Numer pinu przypisany do przycisku
    const short unsigned int PIN_;
};

void PushButton::init()
{
  pinMode(PIN_, INPUT);                                     // ustawienie przycisku jako wejście z rezystorem podciągającym
  digitalWrite(PIN_, HIGH);
}

void PushButton::readSignal()
{
  previously_pressed_ = currently_pressed_;
  currently_pressed_ = !digitalRead(PIN_);                  // przycisk wciśnięty przyjmuje stan niski
  if (currently_pressed_)
    ++time_of_continuous_pressing_;
  else
    time_of_continuous_pressing_ = 0;
}

bool PushButton::isClicked()
{
  if (currently_pressed_ && !previously_pressed_)           // stan aktywowany zboczem narastającym
    return true;
  else
    return false;
}

bool PushButton::isReleased()
{
  if (!currently_pressed_ && previously_pressed_)           // stan aktywowany zboczem opadającym
    return true;
  else
    return false;
}

bool PushButton::isPressed(int min_pressing_time = 1)
{
  if (time_of_continuous_pressing_ >= min_pressing_time)    // stan aktywowany trwającym naciśnięciem
    return true;
  else
    return false;
}

void PushButton::reset()
{
  currently_pressed_ = false;
  previously_pressed_ = false;
  time_of_continuous_pressing_ = 0;
}
