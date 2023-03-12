#pragma once

// Klasa szblonowa przechowująca tablicę o stałej długości wraz z metodami umożliwiającymi efektywne zarządzanie
template<class T>
class StaticArray
{
  public:
    // Konstruktor pobierający długość tablicy
    StaticArray(unsigned int length) :
      LENGTH_(length),
      value_(new T[length])
    {
      // ustawienie stałej wielkości tablicy i stałego jej adresu
    }
    // Konstruktor kopiujący tablicę
    StaticArray(unsigned int length, T array[]) :
      LENGTH_(length),
      value_(new T[length])
    {
      for (unsigned int i = 0; i < LENGTH_; ++i)
        value_[i] = array[i];
    }
    // Umożliwienie zapisu i odczytu jak ze zwykłej tablicy
    T& operator[](unsigned int index)
    {
      return value_[index % LENGTH_];
    }
    // Destruktor
    ~StaticArray()
    {
      delete[] value_;
    }

    // Zwraca długość tablicy
    unsigned int length();
    // Sortowanie tablic przez wstawianie
    void sort();
    // Zwraca minimalną wartość z podanego przedziału tablicy
    T min_value(unsigned int start = 0, unsigned int end = -1);
    // Zwraca maksymalną wartość z podanego przedziału tablicy
    T max_value(unsigned int start = 0, unsigned int end = -1);
    // Zwraca średnią z podanego przedziału wartości tablicy
    T mean_value(unsigned int start = 0, unsigned int end = -1);
    // Zwraca kwantyl o określonym poziomie z podanego przedziału wartości z tablicy
    T quantile(float quantile_level, unsigned int start = 0, unsigned int end = -1);

  private:
    T* const value_ = nullptr;
    const unsigned int LENGTH_ = 0;
};

template<class T>
unsigned int StaticArray<T>::length()
{
  return LENGTH_;
}

template<class T>
void StaticArray<T>::sort()
{
  for (unsigned int i = 0; i < LENGTH_; ++i)
  {
    for (unsigned int j = i; j > 0; --j)
    {
      if (value_[j - 1] <= value_[j])
        break;

      unsigned int aux_var = value_[j];
      value_[j] = value_[j - 1];
      value_[j - 1] = aux_var;
    }
  }
  return;
}

template<class T>
T StaticArray<T>::min_value(unsigned int start, unsigned int end)
{
  end = end < LENGTH_ ? end : LENGTH_;
  unsigned int min_index = value_[start];
  for (unsigned int i = start + 1; i < end; ++i)
    if (value_[i] < value_[min_index])
      min_index = i;

  return value_[min_index];
}

template<class T>
T StaticArray<T>::max_value(unsigned int start, unsigned int end)
{
  end = end < LENGTH_ ? end : LENGTH_;
  unsigned int max_index = value_[start];
  for (unsigned int i = start + 1; i < end; ++i)
    if (value_[i] > value_[max_index])
      max_index = i;

  return value_[max_index];
}

template<class T>
T StaticArray<T>::mean_value(unsigned int start, unsigned int end)
{
  end = end < LENGTH_ ? end : LENGTH_;
  float sum = value_[start];
  for (unsigned int i = start + 1; i < end; ++i)
    sum += value_[i];

  return static_cast<T>(sum) == sum ? sum / (end - start) : round(sum / (end - start));
}

template<class T>
T StaticArray<T>::quantile(float quantile_level, unsigned int start, unsigned int end)
{
  end = end < LENGTH_ ? end : LENGTH_;
  sort();
  float quantile_position = quantile_level * (end - start - 1);
  unsigned int lower_index = start + floor(quantile_position);
  unsigned int higher_index = start + ceil(quantile_position);
  float quantile = 0;
  if (lower_index == higher_index)
    quantile = value_[lower_index];
  else
    quantile = (higher_index - quantile_position) * value_[lower_index] + (quantile_position - lower_index) * value_[higher_index];

  return static_cast<T>(quantile) == quantile ? quantile : round(quantile);
}
