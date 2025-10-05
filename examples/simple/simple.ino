#include <Arduino.h>
#include <Wire.h>

#include "roo_time_ds3231.h"

using namespace roo_time;

Ds3231Clock rtc;

void setup() {
  Serial.begin(9600);
  Wire.begin(SDA, SCL);
}

void loop() {
  // Read current time, and convert to DateTime in UTC.
  DateTime dt(rtc.now(), roo_time::timezone::UTC);

  // Print it formatted.
  Serial.printf("%04d:%02d:%02d %02d:%02d:%02d\n", dt.year(), dt.month(),
                dt.day(), dt.hour(), dt.minute(), dt.second());

  delay(1000);
}
