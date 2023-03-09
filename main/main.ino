#include "configuration.h"
#include "HeatingPress.h"
#include "ForceSensor.h"
#include "HeatingPlate.h"
#include "SettingsController.h"
#include "reportError.h"

// Utworzenie systemu zarządzającego wszystkimi funkcjami urządzenia (4 główne wątki)
HeatingPress device(4);

void setup()
{
  // Utworzenie instancji dla czujnika, każdej grzałki oraz ustawień ogólnych
  device.addThread(new ForceSensor(PIN_FORCE_SENSOR));
  device.addThread(new HeatingPlate(PIN_TEMPERATURE_SENSOR_TOP, PIN_HEAT_SUPPLY_TOP, 1.0, 0.0, "TOP"));    // najpierw 0.96, potem 0.80
  device.addThread(new HeatingPlate(PIN_TEMPERATURE_SENSOR_BOT, PIN_HEAT_SUPPLY_BOT, 1.0, 0.0, "BOT"));    // najpierw 1.00, potem 0.62
  device.addThread(new SettingsController());

  // Konfiguracja wszystkich elementów oraz wyświetlenie informacji o urządzeniu
  device.init();
}

void loop()
{
  device.run();
}

void reportError(String code)
{
  device.reportError(code);
}
