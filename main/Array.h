#pragma once

using CollectionSize = unsigned short;

// Klasa szblonowa przechowująca tablicę o stałej długości
template<typename T, CollectionSize N>
class Array
{
  public:
    inline Array() = default;
    // Konstruktor pobierający tablicę
    inline Array(const T array[]);
    // Konstruktor kopiujący - wykonuje kopię głęboką
    inline Array(const Array<T, N>& right);
    // Umożliwienie zapisu i odczytu jak ze zwykłej tablicy
    inline T& operator[](const CollectionSize index);
    inline ~Array() = default;

    // Zwraca długość tablicy
    inline CollectionSize length() const;
    // Pobiera element o zadanym indeksie
    inline T get(const CollectionSize index) const;

  protected:
    T value_[N];
};

template<typename T, CollectionSize N>
inline Array<T, N>::Array(const T array[])
{
  for (CollectionSize i = 0; i < N; ++i)
    value_[i] = array[i];
}

template<typename T, CollectionSize N>
inline Array<T, N>::Array(const Array<T, N>& right)
{
  for (CollectionSize i = 0; i < N; ++i)
    value_[i] = right.value_[i];
}

template<typename T, CollectionSize N>
inline T& Array<T, N>::operator[](const CollectionSize index)
{
  return value_[index % N];
}

template<typename T, CollectionSize N>
inline CollectionSize Array<T, N>::length() const
{
  return N;
}

template<typename T, CollectionSize N>
inline T Array<T, N>::get(const CollectionSize index) const
{
  return value_[index];
}
