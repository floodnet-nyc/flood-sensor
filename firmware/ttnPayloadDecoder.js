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
    /*
      CFG update uplink Format:
      | Error Flag  |   sensor_sleep   |    sensor_agg     |   sensor_meas_delta     | sensor_reading_count   |    sensor_state   |    fw_ver       |
      |    255 (FF) |     2 bytes      |      1 byte       |          2 bytes        |          1 byte        |        1 byte     |    6 bytes      |
                 
      Sensor State:
      |  Start  |   Stop  |  Reset  |
      |   's'   |   'x'   |   'r'   |

    */

    // Duty cycle
    var sensor_sleep = (b[2] << 8) | b[1];
    decoded.sensor_sleep = sensor_sleep;

    // Sensor Mode
    var sensor_agg = b[3];
    decoded.sensor_agg = sensor_agg;

    // Sensor Sampling Rate
    var sensor_meas_delta = (b[5] << 8) | b[4];
    decoded.sensor_meas_delta = sensor_meas_delta;

    // Sensor number of readings per measurement
    var sensor_reading_count = b[6];
    decoded.sensor_reading_count = sensor_reading_count;

    // Sensor State
    var sensor_state = b[7].toString();
    if (sensor_state == "115") {
      sensor_state = "Sensing";
    } else if (sensor_state == "120") {
      sensor_state = "CFG Update";
    } else if (sensor_state == "114") {
      sensor_state = "Reset";
    }
    decoded.sensor_state = sensor_state;

    // Firmware Version
    let major = b[8].toString();
    let minor = b[9].toString();
    let patch = b[10].toString();
    let v = "v";
    let dot = ".";
    let fw_ver = v.concat(major, dot, minor, dot, patch);
    decoded.fw_ver = fw_ver;


  } else {
    // Regular Payload
    var sdError, battery, distance;
    // Converting Error Flag bits
    sdError = errorFlag % 2;
    decoded.sdError = sdError;

    // battery
    battery = (b[2] << 8) | b[1]; // battery in centi Volts
    battery = battery / 1000; // Convert to Volts
    decoded.battery = battery;

    // distance
    distance = (b[4] << 8) | b[3];
    decoded.distance = distance;

    if (b[5] !== null) {
      // temperature
      var temperature = (b[6] & 0x80 ? 0xFFFF << 16 : 0) | b[6] << 8 | b[5];
      temperature = temperature / 100;
      decoded.temperature = temperature;

      // pressure
      var pressure = (((b[10] << 24) | (b[9] << 16)) | (b[8] << 8)) | b[7];
      pressure = pressure / 100;
      decoded.pressure = pressure;

      // altitude
      var altitude = (b[12] << 8) | b[11];
      altitude = altitude / 100;
      decoded.altitude = altitude;

      // humidity
      var humidity = (b[14] << 8) | b[13];
      humidity = humidity / 100;
      decoded.humidity = humidity;
    }
  }

  return decoded;
}