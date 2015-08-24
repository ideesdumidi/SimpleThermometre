#include <LiquidCrystal.h>

LiquidCrystal lcd(9, 8, 5, 4, 3, 2);

// Example testing sketch for various DHT humidity/temperature sensors
// Written by ladyada, public domain
double dewPoint(double celsius, double humidity) {
  // (1) Saturation Vapor Pressure = ESGG(T)
  double RATIO = 373.15 / (273.15 + celsius);
  double RHS = -7.90298 * (RATIO - 1);
  RHS += 5.02808 * log10(RATIO);
  RHS += -1.3816e-7 * (pow(10, (11.344 * (1 - 1 / RATIO ))) - 1) ;
  RHS += 8.1328e-3 * (pow(10, (-3.49149 * (RATIO - 1))) - 1) ;
  RHS += log10(1013.246);   // factor -3 is to adjust units - Vapor Pressure SVP * humidity 
  double VP = pow(10, RHS - 3) * humidity;   // (2) DEWPOINT = F(Vapor Pressure) 
  double T = log(VP / 0.61078); // temp var 
  return (241.88 * T) / (17.558 - T);
}

byte termometre[8] = //icon for termometer
{
    B00100,
    B01010,
    B01010,
    B01110,
    B01110,
    B11111,
    B11111,
    B01110
};

byte goutte[8] = //icon for water droplet
{
    B00100,
    B00100,
    B01010,
    B01010,
    B10001,
    B10001,
    B10001,
    B01110,
};

byte feel[8] = {
  0b00000,
  0b01110,
  0b10001,
  0b10001,
  0b10001,
  0b01110,
  0b10001,
  0b00000
};

byte minmax[8] = {
  0b00100,
  0b01110,
  0b10101,
  0b00100,
  0b00100,
  0b10101,
  0b01110,
  0b00100
};


#include "DHT.h"

#define DHTPIN A0     // what pin we're connected to
#define LEDSCREENPIN 10
// Uncomment whatever type you're using!
#define DHTTYPE DHT11   // DHT 11




// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.
DHT dht(DHTPIN, DHTTYPE);
float tempMin;
float tempMax;

void setup() {
  Serial.begin(9600);
  Serial.println("DHTxx test!");

  dht.begin();
  tempMin = tempMax = dht.readTemperature();

  analogWrite(LEDSCREENPIN, 128);
  
  lcd.begin(16,2);         // initialize the lcd for 16 chars 2 lines, turn on backlight
  lcd.clear();
  lcd.createChar(1,termometre);
  lcd.createChar(2,goutte);
  lcd.createChar(3,feel);
  lcd.createChar(4,minmax);
}

float lastTemp, lastHumidity;

void loop() {
  // Wait a few seconds between measurements.
  delay(2000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();

if(t>tempMax)
tempMax = t;

if(t<tempMin)
tempMin = t;

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  
  if(h == lastHumidity && t == lastTemp)
    return;

    lastHumidity = h;
    lastTemp = t;

  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);
  
  
  lcd.setCursor(2, 0);
  lcd.write(1);
  lcd.setCursor(4, 0);
  lcd.print(t, 0);
  lcd.setCursor(6, 0);
  lcd.print((char)223); //degree sign

   
  lcd.setCursor(9, 0);
  lcd.write(2);
  lcd.setCursor(11, 0);
  lcd.print(h, 0);
  lcd.print("%");

  lcd.setCursor(0, 1);
  lcd.write(3);
  lcd.setCursor(2, 1);
  lcd.print(hic, 0);
  lcd.setCursor(4, 1);
  lcd.print((char)223); //degree sign

  lcd.setCursor(7, 1);
  lcd.write(4);
  lcd.setCursor(9, 1);
  lcd.print(tempMin, 0);
  lcd.setCursor(11, 1);
  lcd.print((char)223);
  
  lcd.setCursor(12, 1);
  lcd.print("/");
  lcd.print(tempMax, 0);
  lcd.setCursor(15, 1);
  lcd.print((char)223);

  
  Serial.print("Humidite: ");
  Serial.print(h);
  Serial.print(" %\t");
  
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.write('°');
  Serial.print("\t");
  
  Serial.print("Ressenti: ");
  Serial.print(hic);
  Serial.write('°');
  Serial.print("\t");
  
  Serial.print("Td: ");
  Serial.print(dewPoint(t, h));
  Serial.write('°');
  
  Serial.println("");
}