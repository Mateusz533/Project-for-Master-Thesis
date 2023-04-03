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
    // Konstruktor pobierający tablicę wraz z jej długością
    StaticArray(unsigned int length, T* array) :
      LENGTH_(length),
      value_(array)
    {
      // przypisanie długości oraz wskaźnika - płytka kopia tablicy
    }
    // Konstruktor kopiujący - wykonuje kopię głęboką
    StaticArray(const StaticArray<T>& right) :
      LENGTH_(right.LENGTH_),
      value_(new T[right.LENGTH_])
    {
      for (unsigned int i = 0; i < LENGTH_; ++i)
        value_[i] = right.value_[i];
    }
    // Umożliwienie zapisu i odczytu jak ze zwykłej tablicy
    T& operator[](unsigned int index)
    {
      return value_[index % LENGTH_];
    }
    // Destruktor
    virtual ~StaticArray()
    {
      delete[] value_;
    }

    // Zwraca długość tablicy
    unsigned int length() const;
    // Pobiera element o zadanym indeksie
    T get(unsigned int index) const;
    // Sortowanie tablic przez wstawianie
    void sort();
    // Zwraca minimalną wartość z podanego przedziału tablicy
    T min_value(unsigned int start = 0, unsigned int end = -1) const;
    // Zwraca maksymalną wartość z podanego przedziału tablicy
    T max_value(unsigned int start = 0, unsigned int end = -1) const;
    // Zwraca średnią z podanego przedziału wartości tablicy
    T mean_value(unsigned int start = 0, unsigned int end = -1) const;
    // Zwraca kwantyl o określonym poziomie z podanego przedziału wartości z tablicy
    T quantile(float quantile_level, unsigned int start = 0, unsigned int end = -1);

  private:
    const unsigned int LENGTH_ = 0;
    T* const value_ = nullptr;
};

template<class T>
unsigned int StaticArray<T>::length() const
{
  return LENGTH_;
}

template<class T>
T StaticArray<T>::get(unsigned int index) const
{
  return value_[index];
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
T StaticArray<T>::min_value(unsigned int start, unsigned int end) const
{
  end = end < LENGTH_ ? end : LENGTH_;
  unsigned int min_index = start;
  for (unsigned int i = start + 1; i < end; ++i)
    if (value_[i] < value_[min_index])
      min_index = i;

  return value_[min_index];
}

template<class T>
T StaticArray<T>::max_value(unsigned int start, unsigned int end) const
{
  end = end < LENGTH_ ? end : LENGTH_;
  unsigned int max_index = start;
  for (unsigned int i = start + 1; i < end; ++i)
    if (value_[i] > value_[max_index])
      max_index = i;

  return value_[max_index];
}

template<class T>
T StaticArray<T>::mean_value(unsigned int start, unsigned int end) const
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
