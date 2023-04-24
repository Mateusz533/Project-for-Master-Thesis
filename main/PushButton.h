#pragma once

#include <Arduino.h>

// Klasa przechowująca parametry przycisku
class PushButton
{
  public:
    PushButton() = delete;
    // Konstruktor
    PushButton(short unsigned int pin);
    ~PushButton() = default;
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
    bool currently_pressed_{ false };
    bool previously_pressed_{ false };
    unsigned int time_of_continuous_pressing_{ 0 };    // czas ciągłego wciśnęcia mierzony liczbą cykli
    // Numer pinu przypisany do przycisku
    const short unsigned int PIN_{ 0 };
};
