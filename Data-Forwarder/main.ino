#include <Arduino_LPS22HB.h>
#include <Arduino_HTS221.h>

#define FREQUENCY_HZ        50
#define INTERVAL_MS         (1000 / (FREQUENCY_HZ + 1))

void setup() {
  Serial.begin(115200);
  Serial.println("Started");

  if (!HTS.begin()) {
    Serial.println("Failed to initialize pressure sensor!");
    while (1);
  }
}

void loop() {
    static unsigned long last_interval_ms = 0;

    if (millis() > last_interval_ms + INTERVAL_MS) {
        last_interval_ms = millis();
        
        float t = HTS.readTemperature();
        float h = HTS.readHumidity();
        float p = BARO.readPressure();
        
        Serial.print(t);
        Serial.print('\t');
        Serial.print(h);
        Serial.print('\t');
        Serial.println(p * 0.3);        
    
    }
}
 
  
  
