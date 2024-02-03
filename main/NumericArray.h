#pragma once

#include "Array.h"

// Klasa szblonowa rozszerzająca tablicę o stałej długości o metody oparte o porównywanie elementów
template<typename T, CollectionSize N>
class NumericArray : public Array<T, N>
{
  public:
    inline NumericArray() = default;
    // Konstruktor pobierający tablicę
    inline NumericArray(const T array[]);
    // Konstruktor kopiujący - wykonuje kopię głęboką
    inline NumericArray(const NumericArray<T, N>& right);
    // Umożliwienie zapisu i odczytu jak ze zwykłej tablicy
    inline T& operator[](const CollectionSize index);
    inline ~NumericArray() = default;

    // Sortuje tablicę przez wstawianie
    inline void sort();
    // Zwraca minimalną wartość z podanego przedziału tablicy
    inline T min_value(CollectionSize start = 0, CollectionSize end = -1) const;
    // Zwraca maksymalną wartość z podanego przedziału tablicy
    inline T max_value(CollectionSize start = 0, CollectionSize end = -1) const;
    // Zwraca średnią z podanego przedziału wartości tablicy
    inline T mean_value(CollectionSize start = 0, CollectionSize end = -1) const;
    // Zwraca kwantyl o określonym poziomie z podanego przedziału wartości z tablicy
    inline T quantile(const float quantile_level, CollectionSize start = 0, CollectionSize end = -1);
};

template<typename T, CollectionSize N>
inline NumericArray<T, N>::NumericArray(const T array[]) :
  Array<T, N>{ array }
{
}

template<typename T, CollectionSize N>
inline NumericArray<T, N>::NumericArray(const NumericArray<T, N>& right) :
  Array<T, N>{ static_cast<Array<T, N>>(right) }
{
}

template<typename T, CollectionSize N>
inline T& NumericArray<T, N>::operator[](const CollectionSize index)
{
  return Array<T, N>::operator[](index);
}

template<typename T, CollectionSize N>
inline void NumericArray<T, N>::sort()
{
  for (CollectionSize i = 0; i < N; ++i)
  {
    for (CollectionSize j = i; j > 0; --j)
    {
      if (this->value_[j - 1] <= this->value_[j])
        break;

      const T aux_var{ this->value_[j] };
      this->value_[j] = this->value_[j - 1];
      this->value_[j - 1] = aux_var;
    }
  }
}

template<typename T, CollectionSize N>
inline T NumericArray<T, N>::min_value(CollectionSize start, CollectionSize end) const
{
  end = end < N ? end : N;
  CollectionSize min_index{ start };
  for (CollectionSize i = start + 1; i < end; ++i)
    if (this->value_[i] < this->value_[min_index])
      min_index = i;

  return this->value_[min_index];
}

template<typename T, CollectionSize N>
inline T NumericArray<T, N>::max_value(CollectionSize start, CollectionSize end) const
{
  end = end < N ? end : N;
  CollectionSize max_index{ start };
  for (CollectionSize i = start + 1; i < end; ++i)
    if (this->value_[i] > this->value_[max_index])
      max_index = i;

  return this->value_[max_index];
}

template<typename T, CollectionSize N>
inline T NumericArray<T, N>::mean_value(CollectionSize start, CollectionSize end) const
{
  end = end < N ? end : N;
  float sum{ static_cast<float>(this->value_[start]) };
  for (CollectionSize i = start + 1; i < end; ++i)
    sum += this->value_[i];

  return static_cast<T>(sum) == sum ? sum / (end - start) : round(sum / (end - start));
}

template<typename T, CollectionSize N>
inline T NumericArray<T, N>::quantile(const float quantile_level, CollectionSize start, CollectionSize end)
{
  end = end < N ? end : N;
  sort();
  const float quantile_position{ quantile_level * (end - start - 1) };
  const CollectionSize lower_index{ start + static_cast<CollectionSize>(floor(quantile_position)) };
  const CollectionSize higher_index{ start + static_cast<CollectionSize>(ceil(quantile_position)) };
  float quantile{ 0 };
  if (lower_index == higher_index)
    quantile = this->value_[lower_index];
  else
    quantile = (higher_index - quantile_position) * this->value_[lower_index]
               + (quantile_position - lower_index) * this->value_[higher_index];

  return static_cast<T>(quantile) == quantile ? quantile : round(quantile);
}
