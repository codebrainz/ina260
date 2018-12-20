/*
   This sketch shows more advanced usage of the INA260 library.
   Use it with the Arduino IDE's Serial Plotter for best effect.
*/
#include <INA260.h>

// this define is used to inidicate which pin the INA260's ALERT pin
// is wired to, change it based on how you've wired your INA260.
#define INA260_ALERT_PIN 2

// this is the pin that this example will turn on when an ALERT is
// received. For example the LED builtin to an Arduino.
#define INA260_LED_PIN LED_BUILTIN

// in this example, we'll have the INA260 alert when the current
// reading is over this amount. See the setup() function for where
// this is used.
#define INA260_OVER_CURRENT_LIMIT 0.12 // amps

// define the INA260 instance, giving the I2C address (4-bits), in this case 0
static INA260 ina260(0);
// alternatively you can use two constants representing how the address pins
// on the device are wired (see "Serial Bus Address" in the datasheet).
//static INA260 ina260(INA260::ADDR_GND, INA260::ADDR_GND);

// this variable is used as a flag to signal when an interrupt is
// received, indicating that an alert has been triggered by the INA260
static bool alertPending = false;

// interrupt service routine called when the INA260_ALERT_PIN goes low,
// indicating that the INA260 has an alert.
static void onAlertPinLow(void) {
  // to avoid doing I2C/serial communications inside the interrupt service
  // routine, we'll just flag the alert and process it in the main
  // loop() function.
  alertPending = true;
}

void setup() {
  // for this demo, the readings will be printed over serial, so the Serial
  // library is initialized here.
  Serial.begin(9600);

  // ---
  // Call the begin() function to initialize the instance. This will also
  // initialize the Wire/I2C library.
  ina260.begin();

  // ---
  // this part will configure the INA260 averaging mode and ADC sampling
  // rate. Changing the averaging mode and sampling rate will control
  // how stable the readings are as well as how long it will take to
  // acquire new samples. You most likely will want to tweak these for
  // your specific application. The INA260 datasheet provides lots of
  // information on how this works.

  // create an instance of the register structure. note the ={0} used
  // to zero-initialize the instance. You could also use memset() or
  // manually initialize all of the fields to zero. All of the fields
  // are written to the config. register so you want to make sure you
  // know what they're all set to.
  INA260::ConfigurationRegister configReg = {0};
  // average 64 samples for each reading
  configReg.avg = INA260::AVG_16;
  // allow 2.116ms for the ADC sampling of the bus voltage
  configReg.vbusct = INA260::VBUSCT_1_1MS;
  // allow 2.116ms also for the ADC sampling of the current
  configReg.ishct = INA260::ISHCT_1_1MS;
  // continuously sample voltage and current (this is default)
  configReg.mode = INA260::MODE_ISH_VBUS_CONTINUOUS;
  // write the configuration register to the INA260 over I2C
  ina260.writeConfigurationRegister(configReg);

  // ---
  // this part will tell the INA260 to notify, using its ALERT pin,
  // when a new ADC conversion is ready, as well as when the measured
  // current is over INA260_OVER_CURRENT_LIMIT (defined at top of file).

  // create an instance of the register structure. As with the config.
  // register, you want to make sure to initialize unused fields to a
  // known value, for example zero.
  INA260::MaskEnableRegister maskEnReg = {0};
  // set the OCL flag so it notifies when the alert limit is reached
  maskEnReg.ocl = 1;
  // set the CNVR flag so it notifies when a sample is ready
  maskEnReg.cnvr = 1;
  // set the alert latch enable flag so the alert will be triggered
  // until we reset it by reading the mask/enable register.
  maskEnReg.len = 1;
  // write the mask/enable register to the INA260 over I2C
  ina260.writeMaskEnableRegister(maskEnReg);

  // ---
  // this tells the INA260 what value triggers the ALERT notification.
  // The INA260 interprets the value depending on which alerts are
  // requested using the mask/enable register (above). In this case we
  // want it to trip when an upper-limit current value is reached.
  ina260.writeAlertLimitRegisterInAmps(INA260_OVER_CURRENT_LIMIT);

  // ---
  // config the alert LED pin as a digital output and set it low
  pinMode(INA260_LED_PIN, OUTPUT);
  digitalWrite(INA260_LED_PIN, LOW);

  // ---
  // configure the arduino to call the onAlertPinLow() interrupt service
  // routine whenever the specified pin is FALLING from high to low
  // voltage. This is the default for the INA260, but the polarity can
  // be inverted by setting the APOL field of the mask/enable register.
  attachInterrupt(digitalPinToInterrupt(INA260_ALERT_PIN), onAlertPinLow, FALLING);

  // just to get the initial readings, set the alertPending flag
  alertPending = true;
}

void loop() {

  // don't do anything until the INA260 has triggered an alert. Normally
  // you'd do other stuff here as required by your application.
  if (! alertPending)
    return;

  // when the alertPending flag is true, we know the the INA260 has
  // sent us an alert, which is processed here.

  // first we'll read the current measurements to save them. note that
  // for brevity, error handling is omitted. The functions used here
  // do the conversion from the raw values to more useful units, but
  // the raw values can also be read using the appropriate equivalent
  // member functions.
  double amps = 0.0, volts = 0.0, watts = 0.0;
  ina260.readCurrentRegisterInAmps(amps);
  ina260.readBusVoltageRegisterInVolts(volts);
  ina260.readPowerRegisterInWatts(watts);

  // reset the alertPending flag for next time
  alertPending = false;

  // read the mask/enable register to see what happened. this has the
  // side effect of clearing the alert, which is why we saved the
  // measurements above first. The clearAlert() function is also
  // available as a shortcut if you don't need to look at the read-in
  // mask/enable register value.
  INA260::MaskEnableRegister maskEnReg = {0};
  ina260.readMaskEnableRegister(maskEnReg);

  // if the alert was as a result of the overcurrent condition we
  // configured above, then we'll turn on the LED.
  if (maskEnReg.aff) {
    digitalWrite(INA260_LED_PIN, HIGH);
  } else {
    digitalWrite(INA260_LED_PIN, LOW);
  }

  // print the readings so they can be used by the Serial Plotter.
  Serial.print(amps);
  Serial.print("\t");
  Serial.print(volts);
  Serial.print("\t");
  Serial.println(watts);

}
