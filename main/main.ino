#include "HeatingPress.h"
#include "ForceSensor.h"
#include "HeatingPlate.h"
#include "SettingsController.h"
#include "reportError.h"

// Utworzenie systemu zarządzającego wszystkimi funkcjami urządzenia (4 główne wątki + 2 interfejsy)
HeatingPress device(6);

// Ustawienie domyślnych wartości temperatury otoczenia oraz maksymalnej mocy grzania
DegCelsius SystemElement::s_ambient_temperature = DEFAULT_AMBIENT_TEMPERATURE;
DegCelsius SystemElement::s_max_heating_power = MAX_HEATING_POWER;

// Utworzenie instancji dla czujnika, każdej grzałki oraz ustawień ogólnych
ForceSensor force_sensor{ PIN_FORCE_SENSOR };
HeatingPlate top_heating_plate{ PIN_TEMPERATURE_SENSOR_TOP, PIN_HEAT_SUPPLY_TOP, 0.800, 48.0, "TOP" };
HeatingPlate bot_heating_plate{ PIN_TEMPERATURE_SENSOR_BOT, PIN_HEAT_SUPPLY_BOT, 0.680, 47.0, "BOT" };
SettingsController settings_controller;

void setup()
{
  device.addWindow(static_cast<DisplayedElement*>(&force_sensor), FORCE_MEASUREMENT_PERIOD);
  device.addWindow(static_cast<DisplayedElement*>(&top_heating_plate), TEMPERATURE_MEASUREMENT_PERIOD);
  device.addWindow(static_cast<DisplayedElement*>(&bot_heating_plate), TEMPERATURE_MEASUREMENT_PERIOD);
  device.addWindow(static_cast<DisplayedElement*>(&settings_controller), CYCLE_PERIOD);

  /*INSTANCJA DODANA WYŁĄCZNIE DO TESTÓW*/
  // device.addThread(new HardwareTester(), CYCLE_PERIOD);

  // Konfiguracja wszystkich elementów oraz wyświetlenie informacji o urządzeniu
  device.init();
}

void loop()
{
  device.run();
}

void reportError(const String& code)
{
  device.emergencyShutdown(code);
}
