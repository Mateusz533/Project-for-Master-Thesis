#pragma once

#define LCD_ADRESS 0x27                                 // ustawienie adresu układu na 0x27

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
#define CYCLE_PERIOD 50                                 // [ms]
#define SCREEN_REFRESH_PERIOD 500                       // powinny być wielokrotnościami okresu cyklu podstawowego
#define FORCE_ESTIMATION_PERIOD 1000
#define TEMPERATURE_AVERAGING_PERIOD 200
#define TEMPERATURE_ESTIMATION_PERIOD 1000              // powinien być wielokrotnością okresu uśredniania

// Parametry grzania oraz regulacji temperatury
#define MIN_TEMPERATURE 20                              // [*C]
#define MAX_TEMPERATURE 400
#define PRESET_TEMPERATURE_RESOLUTION 5
#define TEMPERATURE_REGULATION_RANGE 30
#define DEFAULT_AMBIENT_TEMPERATURE 20
#define MAX_HEATING_POWER 200                           // [W]
#define MAX_HEAT_SIGNAL 255
#define PROPORCIONAL_REGULATION_COEFFICIENT 3.72        // wg metody Z-N [W/K]
#define DIFFERENTIAL_REGULATION_COEFFICIENT 0.0         // niepotrzebny  [W*s/K]
#define INTEGRAL_REGULATION_COEFFICIENT 0.105           // wg metody Z-N [W/(K*s)]

// Tablica bitów do wyświetlenia symbolu stopnia na ekranie LCD
#define DEGREE_SYMBOL { B00010, B00101, B00010, B00000, B00000, B00000, B00000, B00000 }
#define DEGREE_SYMBOL_INDEX '\1'

// Enumerator dla przycisków
enum Button : short unsigned int { UP = 0, DOWN = 1, LEFT = 2, RIGHT = 3, ACTION = 4 };
#define BUTTONS_NUMBER 5
