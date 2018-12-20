/*
   This sketch shows the absolute basic usage of the INA260 library.
   Most likely you will not want to use it exactly like this, see the advanced
   example for making better use of the library.
*/
#include <INA260.h>

// define the INA260 instance, giving the I2C address (4-bits), in this case 0
static INA260 ina260(0);
// alternatively you can use two constants representing how the address pins
// on the device are wired (see "Serial Bus Address" in the datasheet).
//static INA260 ina260(INA260::ADDR_GND, INA260::ADDR_GND);

void setup() {
  // for this demo, the readings will be printed over serial, so the Serial
  // library is initialized here.
  Serial.begin(9600);

  // Call the begin() function to initialize the instance. This will also
  // initialize the Wire/I2C library.
  ina260.begin();
}

void loop() {
  double value = 0.0;

  // get the current reading from the device, checking for errors. This will
  // use I2C to talk to the device, no values are cached in the library.
  if (ina260.readCurrentRegisterInAmps(value)) {
    Serial.print("Current: ");
    Serial.println(value);
  }

  // get the bus voltage reading from the device, leaving out error checking
  // for brevity.
  ina260.readBusVoltageRegisterInVolts(value);
  Serial.print("Voltage: ");
  Serial.println(value);

  // get the power reading from the device
  ina260.readPowerRegisterInWatts(value);
  Serial.print("Power: ");
  Serial.println(value);

  // for the demo, wait a second between taking readings
  delay(1000);
}
