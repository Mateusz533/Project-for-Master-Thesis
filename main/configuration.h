#pragma once

#include <Arduino.h>

using BasicSize = unsigned short;
using AnalogSignal = int;
using SignlalPWM = unsigned short;
using MilliSec = int;
using Newton = int;
using DegCelsius = int;
using StorageWatt = int;

using KelvinDiff = float;
using Second = float;
using KelvinPerSec = float;
using KelvinSec = float;
using Watt = float;

// Oznaczenia pinów w mikrokontrolerze
constexpr uint8_t PIN_PB_UP = 2;
constexpr uint8_t PIN_PB_DOWN = 3;
constexpr uint8_t PIN_PB_LEFT = 4;
constexpr uint8_t PIN_PB_RIGHT = 5;
constexpr uint8_t PIN_PB_ACTION = 6;
constexpr uint8_t PIN_HEAT_SUPPLY_TOP = 9;
constexpr uint8_t PIN_HEAT_SUPPLY_BOT = 10;
constexpr uint8_t PIN_FORCE_SENSOR = A1;
constexpr uint8_t PIN_TEMPERATURE_SENSOR_TOP = A2;
constexpr uint8_t PIN_TEMPERATURE_SENSOR_BOT = A3;
constexpr uint8_t PIN_SAFETY_MOCK_INPUT = 0;
constexpr uint8_t PIN_SAFETY_MOCK_OUTPUT = 1;

// Okresy poszczególnych części programu - powinny być wielokrotnościami okresu cyklu podstawowego
constexpr MilliSec CYCLE_PERIOD = 9;
constexpr MilliSec BUTTONS_REFRESH_PERIOD = 45;
constexpr MilliSec FORCE_MEASUREMENT_PERIOD = 9;
constexpr MilliSec FORCE_ESTIMATION_PERIOD = 180;
constexpr MilliSec TEMPERATURE_MEASUREMENT_PERIOD = 9;
constexpr MilliSec TEMPERATURE_AVERAGING_PERIOD = 90;
constexpr MilliSec TEMPERATURE_ESTIMATION_PERIOD = 900;    // powinien być dodatkowo wielokrotnością okresu uśredniania
constexpr MilliSec SCREEN_REFRESH_PERIOD = 288;          // powinien być dodatkowo wielokrotnością dzielnika poniżej
constexpr unsigned short DISPLAYED_DATA_DIVISOR = 32;    // powinien być potęgą liczby 2 z przedziału 2 - 32

// Parametry diagnostyki
constexpr bool ENABLE_ERRORS = true;    // zezwolenie na przerwanie działania urządzenia w przypadku błędu
constexpr AnalogSignal FORCE_SIGNAL_HIGHER_LIMIT = 600;
constexpr AnalogSignal TEMPERATURE_SIGNAL_LOWER_LIMIT = 20;
constexpr AnalogSignal TEMPERATURE_SIGNAL_HIGHER_LIMIT = 720;
constexpr KelvinPerSec MAX_TEMPERATURE_GROWTH = 15.0;

// Parametry czujnika nacisku
constexpr Newton TARE = 5;
constexpr Newton START_OFFSET = 5;

// Parametry grzania oraz regulacji temperatury
constexpr DegCelsius MIN_TEMPERATURE = 20;
constexpr DegCelsius MAX_TEMPERATURE = 400;
constexpr DegCelsius PRESET_TEMPERATURE_RESOLUTION = 5;
constexpr DegCelsius TEMPERATURE_REGULATION_RANGE = 20;
constexpr DegCelsius DEFAULT_AMBIENT_TEMPERATURE = 20;
constexpr StorageWatt MAX_HEATING_POWER = 400;
constexpr SignlalPWM MAX_HEAT_SIGNAL = 255;
constexpr float PROPORTIONAL_REGULATION_COEFFICIENT = 6.5;    // [W/K] dobrane z wykorzystaniem symulacji
constexpr float INTEGRAL_REGULATION_COEFFICIENT = 0.075;      // [W/(K*s)]
constexpr float DERIVATIVE_REGULATION_COEFFICIENT = 0.0;      // [W*s/K] niepotrzebny

// Tabelaryczna charakterystyka czujnika nacisku
constexpr BasicSize FORCE_CONVERSION_ARRAY_SIZE = 21;
constexpr AnalogSignal FORCE_SIGNAL_VALUES[FORCE_CONVERSION_ARRAY_SIZE] = {
  0, 16, 36, 50, 61, 70, 78, 85, 92, 97, 103, 108, 113, 117, 121, 125, 129, 133, 137, 140, 144
};
constexpr Newton FORCE_VALUES[FORCE_CONVERSION_ARRAY_SIZE] = {
  0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 160, 170, 180, 190, 200
};

// Tabelaryczna zależność mocy grzania od temperatury w stanie ustalonym
constexpr BasicSize TEMPERATURE_TO_POWER_CONVERSION_ARRAY_SIZE = 14;
constexpr DegCelsius TEMPERATURE_VALUES[TEMPERATURE_TO_POWER_CONVERSION_ARRAY_SIZE] = {
  20, 62, 100, 134, 167, 199, 228, 256, 282, 307, 331, 353, 375, 400
};
constexpr StorageWatt HEATING_POWER_VALUES[TEMPERATURE_TO_POWER_CONVERSION_ARRAY_SIZE] = {
  0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65
};

// Tablica bitów do wyświetlenia symbolu stopnia na ekranie LCD
#define DEGREE_SYMBOL \
  { \
    B00010, B00101, B00010, B00000, B00000, B00000, B00000, B00000 \
  }
constexpr uint8_t DEGREE_SYMBOL_INDEX = '\1';

// Parametry ekranu LCD
constexpr uint8_t LCD_ADRESS = 0x27;
constexpr BasicSize LCD_COLUMNS_NUMBER = 16;
constexpr BasicSize LCD_ROWS_NUMBER = 2;

// Parametry przycisków
constexpr MilliSec CONTINUOUS_PRESSING_ACTIVATION_TIME = 900;    // powinien być wielokrotnością okresu odświeżania przycisków
constexpr BasicSize BUTTONS_NUMBER = 5;

// Enumerator dla przycisków
enum Button : BasicSize
{
  UP = 0,
  DOWN = 1,
  LEFT = 2,
  RIGHT = 3,
  ACTION = 4
};
