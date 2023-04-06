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
  device.addThread(new ForceSensor(PIN_FORCE_SENSOR));
  device.addThread(new HeatingPlate(PIN_TEMPERATURE_SENSOR_TOP, PIN_HEAT_SUPPLY_TOP, 0.800, 47.0, F("TOP")));
  device.addThread(new HeatingPlate(PIN_TEMPERATURE_SENSOR_BOT, PIN_HEAT_SUPPLY_BOT, 0.692, 42.0, F("BOT")));
  device.addThread(new SettingsController());

  /*INSTANCJA DODANA WYŁĄCZNIE DO TESTÓW*/
  device.addThread(new HardwareTester());

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
