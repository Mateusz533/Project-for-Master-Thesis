#pragma once

#define LCD_ADRESS 0x27    // ustawienie adresu układu na 0x27

// Oznaczenia pinów w mikrokontrolerze
#define PIN_PB_UP 2
#define PIN_PB_DOWN 3
#define PIN_PB_LEFT 4
#define PIN_PB_RIGHT 5
#define PIN_PB_ACTION 6
#define PIN_HEAT_SUPPLY_TOP 9
#define PIN_HEAT_SUPPLY_BOT 10
#define PIN_FORCE_SENSOR A1
#define PIN_TEMPERATURE_SENSOR_TOP A2
#define PIN_TEMPERATURE_SENSOR_BOT A3

// Okresy poszczególnych części programu
#define CYCLE_PERIOD 10                       // [ms]
#define FORCE_ESTIMATION_PERIOD 1000          // powinny być wielokrotnościami okresu cyklu podstawowego
#define TEMPERATURE_AVERAGING_PERIOD 200      //
#define TEMPERATURE_ESTIMATION_PERIOD 1000    // powinien być dodatkowo wielokrotnością okresu uśredniania
#define DISPLAYED_DATA_DIVISOR 32             // powinien być potęgą liczby 2 z przedziału 2 - 32
#define SCREEN_REFRESH_PERIOD 320             // powinien być dodatkowo wielokrotnością dzielnika powyżej

// Parametry grzania oraz regulacji temperatury
#define MIN_TEMPERATURE 20                 // [*C]
#define MAX_TEMPERATURE 400                //
#define PRESET_TEMPERATURE_RESOLUTION 5    //
#define TEMPERATURE_REGULATION_RANGE 30    //
#define DEFAULT_AMBIENT_TEMPERATURE 20     //
#define MAX_TEMPERATURE_GROWTH 5           // [*C/s]
#define MAX_HEATING_POWER 200              // [W]
#define MAX_HEAT_SIGNAL 255
#define TEMPERATURE_SIGNAL_LOWER_LIMIT 20
#define TEMPERATURE_SIGNAL_HIGHER_LIMIT 720
#define FORCE_SIGNAL_HIGHER_LIMIT 600
#define PROPORTIONAL_REGULATION_COEFFICIENT 3.72    // wg metody Z-N [W/K]
#define INTEGRAL_REGULATION_COEFFICIENT 0.105       // wg metody Z-N [W/(K*s)]
#define DERIVATIVE_REGULATION_COEFFICIENT 0.0       // niepotrzebny  [W*s/K]

// Tabelaryczna charakterystyka czujnika nacisku
#define FORCE_SIGNAL_VALUES \
  { \
    0, 29, 60, 83, 99, 115, 129, 154, 178, 195, 213, 242, 266, 289 \
  }
#define FORCE_VALUES \
  { \
    0, 5, 10, 20, 30, 40, 50, 70, 90, 110, 130, 150, 170, 200 \
  }
#define FORCE_CONVERSION_ARRAY_SIZE 14

// Tabelaryczna zależność mocy grzania od temperatury w stanie ustalonym
#define TEMPERATURE_VALUES \
  { \
    20, 62, 100, 134, 167, 199, 228, 256, 282, 307, 331, 353, 375, 400 \
  }
#define HEATING_POWER_VALUES \
  { \
    0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65 \
  }
#define TEMPERATURE_TO_POWER_CONVERSION_ARRAY_SIZE 14

// Tablica bitów do wyświetlenia symbolu stopnia na ekranie LCD
#define DEGREE_SYMBOL \
  { \
    B00010, B00101, B00010, B00000, B00000, B00000, B00000, B00000 \
  }
#define DEGREE_SYMBOL_INDEX '\1'

// Parametry ekranu LCD
#define LCD_COLUMNS_NUMBER 16
#define LCD_ROWS_NUMBER 2

// Parametry przycisków
#define CONTINUOUS_PRESSING_ACTIVATION_TIME 1000
#define BUTTONS_NUMBER 5

// Enumerator dla przycisków
enum Button : short unsigned int
{
  UP = 0,
  DOWN = 1,
  LEFT = 2,
  RIGHT = 3,
  ACTION = 4
};
