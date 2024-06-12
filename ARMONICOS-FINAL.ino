#include <arduinoFFT.h>
#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "Crisdkk";
const char* password = "alberto22";
const char* serverName = "http://cygnus.uniajc.edu.co/proyecto1/embebidos/ARMONICOS-CRISDKK/brokersub.php";

const int analogPin = 34; // Cambiado a GPIO 32
float voltaje;   // variables para la lectura del voltaje
float voltaje100;
const uint16_t samples = 128; // debe ser una potencia de 2
const double samplingFrequency = 60; // Hz, debe ser mayor que 2 veces la frecuencia máxima esperada

unsigned long anteriorTIME; // timer para sustituir el delay
int periodo = 500; // Cambiado a 100ms

double vReal[samples];
double vImag[samples];

ArduinoFFT FFT = ArduinoFFT(vReal, vImag, samples, samplingFrequency);

void setup() {
  Serial.begin(9600);
  // Inicia la conexión WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  if ((millis() - anteriorTIME) >= periodo) {
    anteriorTIME = millis();
    
    // Collect samples
    for (uint16_t i = 0; i < samples; i++) {
      int potValor = analogRead(analogPin);
      voltaje = (potValor / 4095.0) * 2.5;
      voltaje100 = voltaje*100;
      vReal[i] = voltaje100;
      vImag[i] = 0;
      delayMicroseconds(60000 / samplingFrequency);
    }

    // Compute FFT
    FFT.windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.compute(FFT_FORWARD);
    FFT.complexToMagnitude();

    // Fundamental frequency (1st harmonic)
    double fundamentalMag = vReal[1];
    double fundamentalPhase = atan2(vImag[1], vReal[1]);
    
    // 3rd harmonic
    double thirdHarmonicMag = vReal[3];
    double thirdHarmonicPhase = atan2(vImag[3], vReal[3]);
    
    // 5th harmonic
    double fifthHarmonicMag = vReal[5];
    double fifthHarmonicPhase = atan2(vImag[5], vReal[5]);
    
    // 7th harmonic
    double seventhHarmonicMag = vReal[7];
    double seventhHarmonicPhase = atan2(vImag[7], vReal[7]);
    
    // 9th harmonic
    double ninthHarmonicMag = vReal[9];
    double ninthHarmonicPhase = atan2(vImag[9], vReal[9]);

    // Imprimir el voltaje en el monitor serie
    imprimirV(voltaje100);
   
    // Calculate and send sine waves
    Serial.println("Sine waves:");
    for (double t = 0; t < 1.0; t += 0.1) { // Aseguramos enviar un ciclo completo
      double fundamentalSine = fundamentalMag * sin(2 * PI * 1 * t + fundamentalPhase);
      double thirdHarmonicSine = thirdHarmonicMag * sin(2 * PI * 3 * t + thirdHarmonicPhase);
      double fifthHarmonicSine = fifthHarmonicMag * sin(2 * PI * 5 * t + fifthHarmonicPhase);
      double seventhHarmonicSine = seventhHarmonicMag * sin(2 * PI * 7 * t + seventhHarmonicPhase);
      double ninthHarmonicSine = ninthHarmonicMag * sin(2 * PI * 9 * t + ninthHarmonicPhase);

      // Enviar los datos al servidor mediante una petición GET
      sendGET(fundamentalSine, thirdHarmonicSine, fifthHarmonicSine, seventhHarmonicSine, ninthHarmonicSine);
      delay(10); // Agregar un pequeño retraso entre cada solicitud para evitar saturar el servidor
    }
  }
}

void imprimirV(float voltaje100) {
  Serial.print("Voltaje => ");  // Impresión de la lectura en monitor serial
  Serial.print(voltaje100);
  Serial.println(" V");
}

void sendGET(double fundamentalSine, double thirdHarmonicSine, double fifthHarmonicSine, double seventhHarmonicSine, double ninthHarmonicSine) {
  if ((WiFi.status() == WL_CONNECTED)) { // Check the current connection status
    HTTPClient http;

    // Construir la URL con los parámetros
    String url = serverName;
    url += "?fundamental=" + String(fundamentalSine);
    url += "&thirdHarmonic=" + String(thirdHarmonicSine);
    url += "&fifthHarmonic=" + String(fifthHarmonicSine);
    url += "&seventhHarmonic=" + String(seventhHarmonicSine);
    url += "&ninthHarmonic=" + String(ninthHarmonicSine);

   // Serial.println("Sending data to server: " + url);

    http.begin(url); // Especificar la URL
    int httpResponseCode = http.GET(); // Enviar la solicitud

    if (httpResponseCode > 0) { // Check for the response code
      String response = http.getString(); // Obtener la respuesta
      Serial.println("HTTP Response code: " + String(httpResponseCode));
      Serial.println(response); // Imprimir la respuesta
    } else {
      Serial.println("Error in HTTP request");
    }

    http.end(); // Free resources
  } else {
    Serial.println("WiFi Disconnected");
  }
}
