#pragma once

#include "System.h"
#include "InputInterface.h"
#include "OutputInterface.h"
#include "reportError.h"

constexpr int CHAR_REFRESH_PERIOD = SCREEN_REFRESH_PERIOD / DISPLAYED_DATA_DIVISOR;

// Klasa przechowująca parametry prasy grzewczej
class HeatingPress : public System
{
  public:
    HeatingPress() = delete;
    HeatingPress(const VectorSize allocated_threads_number = 6);
    ~HeatingPress();
    // Inicjalizuje urządzenie konfigurując wszystkie porty, ekran, liczniki oraz wyświetlając wprowadzenie
    void init() override;
    // Wykonuje jeden cykl działania systemu w przypadku nadejścia odpowiadającego mu czasu
    void run() override;
    // Wyświetla komunikat o błędzie o zadanym numerze, awaryjnie wyłącza grzałki oraz zatrzymuje program
    void emergencyShutdown(const String& code);

  private:
    // Wykonuje polecenia użytkownika na podstawie wciśniętych przycisków
    void readUserCommands();
    // Przesyła dane do wyświetlenia odpowiednie dla aktywnego okna
    void sendDataToDisplay(const bool force_refresh = false);

    // Obiekt obsługujący interfejs wejściowy składający się z 5-ciu przycisków
    InputInterface input_interface_;
    // Obiekt obsługujący interfejs wyjściowy składający się z ekranu LCD o 2-ch wierszach i 16-stu kolumnach
    OutputInterface output_interface_;
    /* Zmienna przechowująca numer jednego z czterech okien interfejsu:
       0 - informacje o nacisku
       1 - informacje o temperaturze i grzaniu górnej płyty
       2 - informacje o temperaturze i grzaniu dolnej płyty
       3 - informacje o parametrach globalnych
    */
    short displayed_window_id_{ 0 };
    // Licznik umożliwiający synchroniczne wykonywanie programu
    unsigned long cycle_counter_{ 0 };
};
