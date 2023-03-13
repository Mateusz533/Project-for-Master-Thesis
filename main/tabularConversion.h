#include "StaticArray.h"
#pragma once

// Funkcja konwertująca podany argument wg. funkcji podanej tabelarycznie z interpolacją liniową
template<typename A, typename V>
V tabularConversion(StaticArray<A>& args_tab, StaticArray<V>& values_tab, const float& arg)
{
  unsigned int CONVERSION_ARRAYS_SIZE_ = min(args_tab.length(), values_tab.length());
  float value = 0;

  for (unsigned int i = 1; i < CONVERSION_ARRAYS_SIZE_; ++i)
  {
    if (arg < args_tab[i])
    {
      value = values_tab[i - 1] + (arg - args_tab[i - 1]) * (values_tab[i] - values_tab[i - 1]) / (args_tab[i] - args_tab[i - 1]);
      break;
    }
    else if (i + 1 == CONVERSION_ARRAYS_SIZE_)
      value = values_tab[i];
  }
  return static_cast<V>(value) == value ? value : round(value);
}
