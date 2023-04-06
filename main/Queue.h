#include "StaticArray.h"
#pragma once

// Klasa szblonowa implementująca kolejkę o ograniczonej długości
template<class T>
class Queue : public StaticArray<T>
{
  public:
    // Konstruktor pobierający długość kolejki
    Queue(unsigned int length) :
      StaticArray<T>(length)
    {
      // inicjalizacja pustej kolejki
    }
    // Dodaje nową wartość na koniec kolejki
    void push(const T new_value);
    // Pobiera wartość z początku kolejki
    T pop();
    // Usuwa wszystkie elementy z kolejki
    void clear();
    // Zwraca prawdę, jeśli kolejka jest pusta
    bool isFull() const;
    // Zwraca fałsz, jeśli kolejka jest pełna
    bool isEmpty() const;

  private:
    unsigned int start_ = 0;
    unsigned int end_ = 0;
    bool is_full_ = 0;
};

template<class T>
void Queue<T>::push(T new_value)
{
  (*this)[end_++] = new_value;
  end_ %= this->length();
  if (is_full_)
    start_ = end_;
  else if (start_ == end_)
    is_full_ = true;
}

template<class T>
T Queue<T>::pop()
{
  T value = this[start_];
  if (isEmpty())
    return value;

  is_full_ = false;
  start_ = ++start_ % this->length();
  return value;
}

template<class T>
void Queue<T>::clear()
{
  start_ = end_ = is_full_ = 0;
}

template<class T>
bool Queue<T>::isFull() const
{
  return is_full_;
}

template<class T>
bool Queue<T>::isEmpty() const
{
  return start_ == end_ && !is_full_;
}
