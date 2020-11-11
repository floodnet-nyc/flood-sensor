// decoder variable contains Error Flags followed by Battery and sensor data.
function Decoder(b, port) {
  var decoded = {};

  // Error Flags
  /*
  Format:
    |--------------------------------------------- Error Flags  -----------------------------------------------------------------------------------------------------------|
    |     bit 7                                                    |     bit 6   |     bit 5    |     bit 4    |     bit 3    |     bit 2    |     bit 1    |     bit 0    |
    |     Used only for CFG update (all other bits are high)       |             |              |              |              |              |              | SD error flag|
  255 or FF is a CFG update
  */
  var errorFlag = b[0];
  if (errorFlag == 255) {
    // Payload is Sensor cfg update

    // Duty cycle
    var dutyCycle = (b[2]<< 8) | b[1];
    decoded.dutyCycle = dutyCycle;

    // Sensor Mode
    var sensorMode = b[3];
    decoded.sensorMode = sensorMode;

    // Sensor Sampling Rate
    var sensorSamplingRate = (b[5] << 8) | b[4];
    decoded.sensorSamplingRate = sensorSamplingRate;

    // Sensor number of readings per measurement
    var sensorNumberOfReadings = b[6];
    decoded.sensorNumberOfReadings = sensorNumberOfReadings;

  } else {
    // Regular Payload

    // Converting Error Flag bits
    var sdError = errorFlag % 2;
    decoded.sdError = sdError;

    // battery
    var battery = (b[2] << 8) | b[1]; // battery in centi Volts
    battery = battery / 1000; // Convert to Volts
    decoded.battery = battery;

    // distance
    var distance = (b[4] << 8) | b[3];
    decoded.distance = distance;
  }

  return decoded;
}
