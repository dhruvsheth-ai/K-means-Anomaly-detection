// Flash to Argon
#include "Particle.h"

// Constants
const size_t READ_BUF_SIZE = 256;

// Structures
typedef struct {
	int anomaly;
	int var1;
	int var2;
	int var3;
	int var4;
	int var5;
	int var6;
} WeatherData;

// Forward declarations
void processBuffer();
void handleWeatherData(const WeatherData &data);

// Global variables
int counter = 0;
unsigned long lastSend = 0;

char readBuf[READ_BUF_SIZE];
size_t readBufOffset = 0;

void setup() {
	Serial.begin(9600);

	// Serial1 RX is connected to Arduino TX (1)
	// Serial2 TX is connected to Arduino RX (0)
	// Photon GND is connected to Arduino GND
	Serial1.begin(115200);
}

void loop() {
	// Read data from serial
	while(Serial1.available()) {
		if (readBufOffset < READ_BUF_SIZE) {
			char c = Serial1.read();
			if (c != '\n') {
				// Add character to buffer
				readBuf[readBufOffset++] = c;
			}
			else {
				// End of line character found, process line
				readBuf[readBufOffset] = 0;
				processBuffer();
				readBufOffset = 0;
			}
		}
		else {
			Serial.println("readBuf overflow, emptying buffer");
			readBufOffset = 0;
		}
	}

}

void processBuffer() {
	// Serial.printlnf("Received from Arduino: %s", readBuf);
	WeatherData data;

	if (sscanf(readBuf, "%d,%d,%d,%d,%d,%d,%d", &data.anomaly, &data.var1, &data.var2,
			&data.var3, &data.var4, &data.var5, &data.var6) == 7) {

		handleWeatherData(data);
	}
	else {
		Serial.printlnf("invalid data %s", readBuf);
	}
}

void handleWeatherData(const WeatherData &data) {
	Serial.printlnf("got Anomaly=%d v1=%d v2=%d v3=%d v4=%d v5=%d v6=%d",
			data.anomaly, data.var1, data.var2, data.var3, data.var4, data.var5, data.var6);
			
    Particle.publish("Anomaly", String(int(data.anomaly)), PRIVATE);

}



