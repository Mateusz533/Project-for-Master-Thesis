#pragma once

using VectorSize = unsigned short;

// Klasa szblonowa przechowująca tablicę o stałej długości
template<typename T>
class Vector
{
  public:
    Vector() = delete;
    // Konstruktor - alokuje podaną liczbę elementów
    inline Vector(const VectorSize capacity);
    // Konstruktor kopiujący - wykonuje kopię głęboką
    inline Vector(const Vector<T>& right);
    // Operator przypisania - wykonuje kopię głęboką
    inline Vector operator=(const Vector<T>& right);
    // Umożliwienie zapisu i odczytu jak ze zwykłej tablicy
    inline T& operator[](const VectorSize index);
    inline ~Vector();

    // Zwraca długość tablicy
    inline VectorSize length() const;
    // Zwraca ilość zarezerwowanej pamięci
    inline VectorSize capacity() const;
    // Pobiera element o zadanym indeksie
    inline T get(const VectorSize index) const;
    // Dodaje element na koniec oraz w razie potrzeby realokuje tablicę
    void push_back(T new_element);

  private:
    T* value_{ nullptr };
    VectorSize length_{ 0 };
    VectorSize capacity_{ 0 };
};

template<typename T>
inline Vector<T>::Vector(const VectorSize capacity) :
  capacity_{ capacity }
{
  if (capacity != 0)
    value_ = new T[capacity];
}

template<typename T>
inline Vector<T>::Vector(const Vector<T>& right)
{
  if (right.length_ != 0)
  {
    length_ = capacity_ = right.length_;
    value_ = new T[right.length_];
    for (VectorSize i = 0; i < right.length_; ++i)
      value_[i] = right.value_[i];
  }
}

template<typename T>
inline Vector<T> Vector<T>::operator=(const Vector<T>& right)
{
  if (length_ != 0)
    delete[] value_;
  if (right.length_ == 0)
    value_ = nullptr;
  else
  {
    value_ = new T[right.length_];
    for (VectorSize i = 0; i < right.length(); ++i)
      value_[i] = right.value_[i];
  }
  length_ = capacity_ = right.length_;
}

template<typename T>
inline T& Vector<T>::operator[](const VectorSize index)
{
  return value_[index % length_];
}

template<typename T>
inline Vector<T>::~Vector()
{
  delete[] value_;
}

template<typename T>
inline VectorSize Vector<T>::length() const
{
  return length_;
}

template<typename T>
inline VectorSize Vector<T>::capacity() const
{
  return capacity_;
}

template<typename T>
inline T Vector<T>::get(const VectorSize index) const
{
  return value_[index % length_];
}

template<typename T>
void Vector<T>::push_back(T new_element)
{
  if (length_ == capacity_)
  {
    T* temp_table = new T[++capacity_];
    for (VectorSize i = 0; i < length_; ++i)
      temp_table[i] = value_[i];
    delete[] value_;
    value_ = temp_table;
  }
  value_[length_++] = new_element;
}
