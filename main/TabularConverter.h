#pragma once

#include "NumericArray.h"

// Klasa przechowująca funkcję konvertującą w postaci tabelarycznie
template<typename A, typename V, CollectionSize N>
class TabularConverter
{
  public:
    TabularConverter() = delete;
    TabularConverter(const TabularConverter<A, V, N>& right) = delete;
    inline TabularConverter(const NumericArray<A, N>& args_tab, const NumericArray<V, N>& values_tab);
    ~TabularConverter() = default;
    // Konwertuje podany argument tabelarycznie z interpolacją liniową
    V convert(const float& arg) const;
    inline A minArgument() const;
    inline A maxArgument() const;
    inline V minValue() const;
    inline V maxValue() const;

  private:
    const NumericArray<A, N> ARGS_TAB_;
    const NumericArray<V, N> VALUES_TAB_;
};

template<typename A, typename V, CollectionSize N>
inline TabularConverter<A, V, N>::TabularConverter(const NumericArray<A, N>& args_tab, const NumericArray<V, N>& values_tab) :
  ARGS_TAB_{ args_tab },
  VALUES_TAB_{ values_tab }
{
}

template<typename A, typename V, CollectionSize N>
V TabularConverter<A, V, N>::convert(const float& arg) const
{
  float value{ 0 };

  for (CollectionSize i = 1; i < N; ++i)
  {
    if (arg < ARGS_TAB_.get(i))
    {
      value = VALUES_TAB_.get(i - 1)
              + (arg - ARGS_TAB_.get(i - 1)) * (VALUES_TAB_.get(i) - VALUES_TAB_.get(i - 1))
                  / (ARGS_TAB_.get(i) - ARGS_TAB_.get(i - 1));
      break;
    }
    else if (i + 1 == N)
      value = VALUES_TAB_.get(i);
  }
  return static_cast<V>(value) == value ? value : round(value);
}

template<typename A, typename V, CollectionSize N>
inline A TabularConverter<A, V, N>::minArgument() const
{
  return ARGS_TAB_.get(0);
}

template<typename A, typename V, CollectionSize N>
inline A TabularConverter<A, V, N>::maxArgument() const
{
  return ARGS_TAB_.get(N - 1);
}

template<typename A, typename V, CollectionSize N>
inline V TabularConverter<A, V, N>::minValue() const
{
  return VALUES_TAB_.get(0);
}

template<typename A, typename V, CollectionSize N>
inline V TabularConverter<A, V, N>::maxValue() const
{
  return VALUES_TAB_.get(N - 1);
}
