#pragma once

#include "NumericArray.h"

// Klasa szblonowa implementująca kolejkę o ograniczonej długości
template<typename T, CollectionSize N>
class Queue : public NumericArray<T, N>
{
  public:
    inline Queue() = default;
    Queue(const Queue<T, N>& right) = delete;
    inline ~Queue() = default;
    // Dodaje nową wartość na koniec kolejki
    inline void push(const T new_value);
    // Pobiera wartość z początku kolejki
    inline T pop();
    // Usuwa wszystkie elementy z kolejki
    inline void clear();
    // Zwraca prawdę, jeśli kolejka jest pusta
    inline bool isFull() const;
    // Zwraca fałsz, jeśli kolejka jest pełna
    inline bool isEmpty() const;

  private:
    CollectionSize start_{ 0 };
    CollectionSize end_{ 0 };
    bool is_full_{ 0 };
};

template<typename T, CollectionSize N>
inline void Queue<T, N>::push(const T new_value)
{
  (*this)[end_++] = new_value;
  end_ %= N;
  if (is_full_)
    start_ = end_;
  else if (start_ == end_)
    is_full_ = true;
}

template<typename T, CollectionSize N>
inline T Queue<T, N>::pop()
{
  const T value{ this[start_] };
  if (isEmpty())
    return value;

  is_full_ = false;
  start_ = ++start_ % N;
  return value;
}

template<typename T, CollectionSize N>
inline void Queue<T, N>::clear()
{
  start_ = end_ = is_full_ = 0;
}

template<typename T, CollectionSize N>
inline bool Queue<T, N>::isFull() const
{
  return is_full_;
}

template<typename T, CollectionSize N>
inline bool Queue<T, N>::isEmpty() const
{
  return start_ == end_ && !is_full_;
}
