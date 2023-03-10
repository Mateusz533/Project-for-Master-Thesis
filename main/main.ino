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
  device.addThread(new HeatingPlate(PIN_TEMPERATURE_SENSOR_TOP, PIN_HEAT_SUPPLY_TOP, 0.6, 50.0, "TOP"));    //(400/16.432/200*5000/1023, 50)
  device.addThread(new HeatingPlate(PIN_TEMPERATURE_SENSOR_BOT, PIN_HEAT_SUPPLY_BOT, 0.6, 50.0, "BOT"));
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
