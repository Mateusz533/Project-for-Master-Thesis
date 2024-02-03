#include "System.h"

System::System(const VectorSize allocated_threads_number) :
  threads_{ allocated_threads_number },
  windows_{ allocated_threads_number }
{
  // Zarezerwowanie pamięci na każdy z wątków oraz okien
}

void System::addThread(SystemElement* new_thread, const MilliSec period)
{
  threads_.push_back(Thread(new_thread, 0, period / CYCLE_PERIOD));
}

void System::addWindow(DisplayedElement* new_window, const MilliSec period)
{
  addThread(new_window, period);
  windows_.push_back(new_window);
}
