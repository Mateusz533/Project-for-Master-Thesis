#include "configuration.h"
#include "HeatingPress.h"
#include "ForceSensor.h"
#include "HeatingPlate.h"
#include "SettingsController.h"
#include "reportError.h"
#include "StaticArray.h"

// Utworzenie systemu zarządzającego wszystkimi funkcjami urządzenia (4 główne wątki)
HeatingPress device(5);

void setup()
{
  // Utworzenie instancji dla czujnika, każdej grzałki oraz ustawień ogólnych
  device.addWindow(new ForceSensor(PIN_FORCE_SENSOR), FORCE_MEASUREMENT_PERIOD);
  device.addWindow(new HeatingPlate(PIN_TEMPERATURE_SENSOR_TOP, PIN_HEAT_SUPPLY_TOP, 0.800, 47.0, F("TOP")), TEMPERATURE_MEASUREMENT_PERIOD);
  device.addWindow(new HeatingPlate(PIN_TEMPERATURE_SENSOR_BOT, PIN_HEAT_SUPPLY_BOT, 0.692, 42.0, F("BOT")), TEMPERATURE_MEASUREMENT_PERIOD);
  device.addWindow(new SettingsController(), CYCLE_PERIOD);

  /*INSTANCJA DODANA WYŁĄCZNIE DO TESTÓW*/
  device.addThread(new HardwareTester(), CYCLE_PERIOD);

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
