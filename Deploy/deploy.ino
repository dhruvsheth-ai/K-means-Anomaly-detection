#include <test_inference.h>
#include <Arduino_LPS22HB.h>
#include <Arduino_HTS221.h>

#define FREQUENCY_HZ        50
#define INTERVAL_MS         (1000 / (FREQUENCY_HZ + 1))

// to classify 1 frame of data you need EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE values
float features[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE];
// keep track of where we are in the feature array
size_t feature_ix = 0;

static bool debug_nn = false;

float anomalydata;


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
void getWeatherData(WeatherData &data);
void sendWeatherData(const WeatherData &data);

// Global variables
char sendBuf[256];



void setup()
{
    // put your setup code here, to run once:
    Serial.begin(115200);
    Serial.println("Edge Impulse Inferencing Demo");    

    if (!HTS.begin()) {
      Serial.println("Failed to initialize pressure sensor!");
      while (1);
    }
    if (!BARO.begin()) {
      Serial.println("Failed to initialize pressure sensor!");
      while (1);
    }

}


void loop() {
    static unsigned long last_interval_ms = 0;
    
    WeatherData data;
    getWeatherData(data);
    sendWeatherData(data);
    delay(3000);  


    if (millis() > last_interval_ms + INTERVAL_MS) {
        last_interval_ms = millis();
        
        float t = HTS.readTemperature();
        float h = HTS.readHumidity();
        float p = BARO.readPressure();

        // keep filling the features array until it's full
        features[feature_ix++] = t;
        features[feature_ix++] = h;
        features[feature_ix++] = p * 0.3;

        // full frame of data?
        if (feature_ix == EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE) {
            ei_impulse_result_t result;

            // create signal from features frame
            signal_t signal;
            numpy::signal_from_buffer(features, EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, &signal);

            // run classifier
            EI_IMPULSE_ERROR res = run_classifier(&signal, &result, true);
            ei_printf("run_classifier returned: %d\n", res);
            if (res != 0) return;

            // print predictions
            ei_printf("Predictions (DSP: %d ms., Classification: %d ms., Anomaly: %d ms.): \n",
                result.timing.dsp, result.timing.classification, result.timing.anomaly);

            // print the predictions
            for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
                ei_printf("%s:\t%.5f\n", result.classification[ix].label, result.classification[ix].value);
            }
        #if EI_CLASSIFIER_HAS_ANOMALY == 1
            ei_printf("anomaly:\t%.3f\n", result.anomaly);
            anomalydata = (float)result.anomaly;
            
        #endif

            // reset features frame
            feature_ix = 0;
        }
    }
  
}


void getWeatherData(WeatherData &data) {
  ei_impulse_result_t result;
  // This just generates random data for testing
  data.anomaly = anomalydata;
  data.var1 = rand();
  data.var2 = rand();
  data.var3 = rand();
  data.var4 = rand();
  data.var5 = rand();
  data.var6 = rand();
}

void sendWeatherData(const WeatherData &data) {

  snprintf(sendBuf, sizeof(sendBuf), "%d,%d,%d,%d,%d,%d,%d\n",
      data.anomaly, data.var1, data.var2, data.var3, data.var4, data.var5, data.var6);
  Serial.print(sendBuf);

}


void ei_printf(const char *format, ...) {
    static char print_buf[1024] = { 0 };

    va_list args;
    va_start(args, format);
    int r = vsnprintf(print_buf, sizeof(print_buf), format, args);
    va_end(args);

    if (r > 0) {
        Serial.write(print_buf);
    }
}

