#pragma once

#include "configuration.h"
#include "InputInterface.h"
#include "OutputInterface.h"
#include "DisplayedElement.h"
#include "reportError.h"
#include "HardwareTester.h"

// Klasa przechowująca parametry prasy grzewczej
class HeatingPress
{
  public:
    HeatingPress() = delete;
    HeatingPress(short unsigned int allocated_threads_number = 4);
    ~HeatingPress();
    // Dodaje podany element do systemu jako nowy wątek
    void addThread(SystemElement* new_thread, unsigned int period);
    // Dodaje podany element do systemu jako nowy wątek wraz z oknem interfejsu użytkownika
    void addWindow(DisplayedElement* new_thread, unsigned int period);
    // Inicjalizuje urządzenie konfigurując wszystkie porty, ekran, liczniki oraz wyświetlając wprowadzenie
    void init();
    // Wykonuje jeden cykl działania systemu w przypadku nadejścia odpowiadającego mu czasu
    void run();
    // Wyświetla komunikat o błędzie o zadanym numerze, awaryjnie wyłącza grzałki oraz zatrzymuje program
    void emergencyShutdown(const String& code);

  private:
    // Struktura przechowująca parametry jednego wątku
    struct Thread
    {
        Thread() = default;
        Thread(SystemElement* task, unsigned int counter, unsigned int overflow) :
          task{ task },
          counter{ counter },
          overflow{ overflow }
        {
          // Przypisanie wszystkich pól struktury
        }
        ~Thread() = default;
        // Wskaźnik do instancji obsługiwanej przez dany wątku
        SystemElement* task{ nullptr };
        // Licznik umożliwiający wykonywanie zadania z odpowiednią częstotliwością
        unsigned int counter{ 0 };
        unsigned int overflow{ 1 };
    };

    // Wykonuje polecenia użytkownika na podstawie wciśniętych przycisków
    void readUserCommands();
    // Przesyła dane do wyświetlenia odpowiednie dla aktywnego okna
    void sendDataToDisplay(const bool force_refresh = false);

    // Obiekt obsługujący interfejs wejściowy składający się z 5-ciu przycisków
    InputInterface input_interface_;
    // Tablica przechowujaca wszystkie wątki
    Thread* threads_{ nullptr };
    // Tablica przechowujaca wskaźnik do każdego z okien
    DisplayedElement** windows_{ nullptr };
    // Obiekt obsługujący interfejs wyjściowy składający się z ekranu LCD o 2-ch wierszach i 16-stu kolumnach
    OutputInterface output_interface_;
    /* Zmienna przechowująca numer jednego z czterech okien interfejsu:
       0 - informacje o nacisku
       1 - informacje o temperaturze i grzaniu górnej płyty
       2 - informacje o temperaturze i grzaniu dolnej płyty
       3 - informacje o parametrach globalnych
    */
    short int displayed_window_id_{ 0 };
    short unsigned int threads_number_{ 0 };
    short unsigned int windows_number_{ 0 };
    short unsigned int allocated_threads_number_{ 4 };
    short unsigned int allocated_windows_number_{ 4 };
    // Licznik umożliwiający synchroniczne wykonywanie programu
    long unsigned int cycle_counter_{ 0 };
};
