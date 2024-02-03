#pragma once

#include "configuration.h"
#include "DisplayedElement.h"
#include "Vector.h"

// Klasa przechowująca parametry prasy grzewczej
class System
{
  public:
    System() = delete;
    System(const VectorSize allocated_threads_number);
    ~System() = default;
    // Dodaje podany element do systemu jako nowy wątek
    void addThread(SystemElement* new_thread, const MilliSec period);
    // Dodaje podany element do systemu jako nowy wątek wraz z oknem interfejsu użytkownika
    void addWindow(DisplayedElement* new_thread, const MilliSec period);
    // Inicjalizuje urządzenie oraz konfigurując wszystkie porty itp.
    virtual void init() = 0;
    // Wykonuje jeden cykl działania systemu
    virtual void run() = 0;

  protected:
    // Struktura przechowująca parametry jednego wątku
    struct Thread
    {
        Thread() = default;
        Thread(SystemElement* task, const unsigned int counter, const unsigned int overflow) :
          task{ task },
          counter{ counter },
          overflow{ overflow }
        {
          // Przypisanie wszystkich pól struktury
        }
        ~Thread() = default;
        // Wskaźnik do instancji obsługiwanej przez dany wątek
        SystemElement* task{ nullptr };
        // Licznik umożliwiający wykonywanie zadania z odpowiednią częstotliwością
        unsigned int counter{ 0 };
        unsigned int overflow{ 1 };
    };

    // Tablica przechowujaca wszystkie wątki
    Vector<Thread> threads_{ 4 };
    // Tablica przechowujaca wskaźniki do każdego z okien
    Vector<DisplayedElement*> windows_{ 4 };
};
