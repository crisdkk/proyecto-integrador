#include <arduinoFFT.h>
#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "red wifi a usar";
const char* password = "contraseña wifi";
const char* serverName = "servidor donde mandar los datos";

const int portPin = 21; // lectura del voltaje en el pin
int potValor = 0;    
float voltaje;   // variables para la lectura del voltaje
float voltaje100;

const uint16_t samples = 128; // debe ser una potencia de 2
const double samplingFrequency = 1000; // Hz, debe ser mayor que 2 veces la frecuencia máxima esperada

unsigned long anteriorTIME; // timer para sustituir el delay
int periodo = 500; // Cambiado a 500ms (medio segundo)

double vReal[samples];
double vImag[samples];

ArduinoFFT FFT = ArduinoFFT(vReal, vImag, samples, samplingFrequency);

void setup() {
  Serial.begin(115200);
  // Inicia la conexión WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  if((millis() - anteriorTIME) >= periodo){
    anteriorTIME = millis();
    
    // Collect samples
    for (uint16_t i = 0; i < samples; i++) {
      potValor = analogRead(portPin);
      voltaje = (potValor / 1170.0);  // formula para tranformarlo de bits a voltaje
      vReal[i] = voltaje;
      vImag[i] = 0;
      delayMicroseconds(1000000 / samplingFrequency);
    }

    // Compute FFT
    FFT.windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.compute(FFT_FORWARD);
    FFT.complexToMagnitude();

    // Fundamental frequency (1st harmonic)
    double fundamentalFreq = FFT.majorPeak();
    double fundamentalMag = vReal[1];
    double fundamentalPhase = atan2(vImag[1], vReal[1]);
    
    // 3rd harmonic
    double thirdHarmonicFreq = fundamentalFreq * 3;
    double thirdHarmonicMag = vReal[3];
    double thirdHarmonicPhase = atan2(vImag[3], vReal[3]);
    
    // 5th harmonic
    double fifthHarmonicFreq = fundamentalFreq * 5;
    double fifthHarmonicMag = vReal[5];
    double fifthHarmonicPhase = atan2(vImag[5], vReal[5]);
    
    // 7th harmonic
    double seventhHarmonicFreq = fundamentalFreq * 7;
    double seventhHarmonicMag = vReal[7];
    double seventhHarmonicPhase = atan2(vImag[7], vReal[7]);
    
    // 9th harmonic
    double ninthHarmonicFreq = fundamentalFreq * 9;
    double ninthHarmonicMag = vReal[9];
    double ninthHarmonicPhase = atan2(vImag[9], vReal[9]);

    // Print results
    imprimirV(voltaje100);
   
    // Calculate and print sine waves
    Serial.println("Sine waves:");
    for (double t = 0; t < 0.1; t += 0.1) { // Increment t in steps of 0.5 seconds
      double fundamentalSine = fundamentalMag * sin(2 * PI * fundamentalFreq * t + fundamentalPhase);
      double thirdHarmonicSine = thirdHarmonicMag * sin(2 * PI * thirdHarmonicFreq * t + thirdHarmonicPhase);
      double fifthHarmonicSine = fifthHarmonicMag * sin(2 * PI * fifthHarmonicFreq * t + fifthHarmonicPhase);
      double seventhHarmonicSine = seventhHarmonicMag * sin(2 * PI * seventhHarmonicFreq * t + seventhHarmonicPhase);
      double ninthHarmonicSine = ninthHarmonicMag * sin(2 * PI * ninthHarmonicFreq * t + ninthHarmonicPhase);

      // Envía los datos al servidor mediante una petición GET
      sendGET(fundamentalSine, thirdHarmonicSine, fifthHarmonicSine, seventhHarmonicSine, ninthHarmonicSine);
    }
  }
}

void imprimirV(float voltaje100){
  Serial.print("voltaje => ");  // impresión de la lectura en monitor serial
  Serial.print(voltaje100);
  Serial.println(" V");
}

void sendGET(double fundamentalSine, double thirdHarmonicSine, double fifthHarmonicSine, double seventhHarmonicSine, double ninthHarmonicSine) {
  if ((WiFi.status() == WL_CONNECTED)) { // Check the current connection status
    HTTPClient http;

    // Construye la URL con los parámetros
    String url = serverName;
    url += "?fundamental=" + String(fundamentalSine);
    url += "&thirdHarmonic=" + String(thirdHarmonicSine);
    url += "&fifthHarmonic=" + String(fifthHarmonicSine);
    url += "&seventhHarmonic=" + String(seventhHarmonicSine);
    url += "&ninthHarmonic=" + String(ninthHarmonicSine);

    Serial.println("Sending data to server: " + url);

    http.begin(url); // Specify the URL
    int httpResponseCode = http.GET(); // Send the request

    if (httpResponseCode > 0) { // Check for the response code
      String response = http.getString(); // Get the response payload
      Serial.println("HTTP Response code: " + String(httpResponseCode));
      Serial.println(response); // Print the response payload
    } else {
      Serial.println("Error in HTTP request");
    }

    http.end(); // Free resources
  } else {
    Serial.println("WiFi Disconnected");
  }
}
