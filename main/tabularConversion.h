#pragma once

#include "StaticArray.h"

// Funkcja konwertująca podany argument wg. funkcji podanej tabelarycznie z interpolacją liniową
template<typename A, typename V>
V tabularConversion(const StaticArray<A>& args_tab, const StaticArray<V>& values_tab, const float& arg);

template<typename A, typename V>
V tabularConversion(const StaticArray<A>& args_tab, const StaticArray<V>& values_tab, const float& arg)
{
  unsigned int CONVERSION_ARRAYS_SIZE_ = min(args_tab.length(), values_tab.length());
  float value{ 0 };

  for (unsigned int i = 1; i < CONVERSION_ARRAYS_SIZE_; ++i)
  {
    if (arg < args_tab.get(i))
    {
      value = values_tab.get(i - 1) + (arg - args_tab.get(i - 1)) * (values_tab.get(i) - values_tab.get(i - 1)) / (args_tab.get(i) - args_tab.get(i - 1));
      break;
    }
    else if (i + 1 == CONVERSION_ARRAYS_SIZE_)
      value = values_tab.get(i);
  }
  return static_cast<V>(value) == value ? value : round(value);
}
