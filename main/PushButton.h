#pragma once

#include "configuration.h"

// Klasa przechowująca parametry przycisku
class PushButton
{
  public:
    PushButton() = default;
    PushButton(const uint8_t pin);
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
    bool isPressed(const unsigned int min_pressing_time) const;
    // Zeruje wszystkie zapamiętane stany przycisku
    void reset();

  private:
    // Zmienne przechowujące stan przycisku
    bool currently_pressed_{ false };
    bool previously_pressed_{ false };
    // Czas ciągłego wciśnęcia mierzony liczbą cykli
    unsigned int time_of_continuous_pressing_{ 0 };
    // Numer pinu przypisany do przycisku
    uint8_t pin_{ PIN_SAFETY_MOCK_INPUT };
};
