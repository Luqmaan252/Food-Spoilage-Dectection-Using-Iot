/* FOOD SPOILAGE DETECTION SYSTEM */

/**************** BLYNK ****************/
#define BLYNK_TEMPLATE_ID "TMPL2b5EkLEMv"
#define BLYNK_TEMPLATE_NAME "IoT Based Food Spoilage Detection and Temperature"
#define BLYNK_AUTH_TOKEN "7xU3jeXH9-3H5XzsMEon5zykBkxyrS1u"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

/**************** WIFI ****************/
char ssid[] = "Galaxy A1";
char pass[] = "77777777";

/**************** OLED ****************/
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

Adafruit_SH1106G display = Adafruit_SH1106G(128, 64, &Wire, -1);

/**************** DHT ****************/
#include <DHT.h>

#define DHTPIN 4
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

/**************** SENSOR PINS ****************/
#define GAS_PIN      34
#define LIGHT_PIN    27

/**************** OUTPUT PINS ****************/
#define RELAY_PIN    12
#define BUZZER_PIN   26
#define RED_LED      25
#define GREEN_LED    14

/**************** VARIABLES ****************/
float temperature = 0;
float humidity = 0;

int gasValue = 0;
int lightValue = 0;

String foodStatus = "NORMAL AIR";

bool spoilageNotificationSent = false;
bool fireNotificationSent = false;

bool systemEnabled = true;

/**************** BLYNK TIMER ****************/
BlynkTimer timer;

/************************************************************
 BLYNK SWITCH V4
************************************************************/
BLYNK_WRITE(V4)
{
  systemEnabled = param.asInt();

  if (!systemEnabled)
  {
    digitalWrite(RELAY_PIN, HIGH);
    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(RED_LED, LOW);
    digitalWrite(GREEN_LED, LOW);

    display.clearDisplay();
    display.display();
    display.oled_command(SH110X_DISPLAYOFF);

    Serial.println("SYSTEM OFF");
  }
  else
  {
    display.oled_command(SH110X_DISPLAYON);

    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(10, 20);
    display.println("SYSTEM");
    display.println("ON");
    display.display();

    Serial.println("SYSTEM ON");
  }
}

/************************************************************
 SEND DATA TO BLYNK
************************************************************/
void sendToBlynk()
{
  if (!systemEnabled) return;

  Blynk.virtualWrite(V0, temperature);
  Blynk.virtualWrite(V1, humidity);
  Blynk.virtualWrite(V2, gasValue);
  Blynk.virtualWrite(V3, foodStatus);
  Blynk.virtualWrite(V5, lightValue);

  int statusCode = 0;

  if (foodStatus == "NORMAL AIR")
    statusCode = 1;
  else if (foodStatus == "FRESH FOOD")
    statusCode = 2;
  else if (foodStatus == "SPOILAGE FOOD")
    statusCode = 3;
  else
    statusCode = 4;

  Blynk.virtualWrite(V6, statusCode);
}

/************************************************************
 SETUP
************************************************************/
void setup()
{
  Serial.begin(115200);

  dht.begin();

  Wire.begin(21, 22);

  display.begin(0x3C, true);
  display.clearDisplay();
  display.setTextColor(SH110X_WHITE);

  pinMode(LIGHT_PIN, INPUT);

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

  digitalWrite(RELAY_PIN, HIGH);
  digitalWrite(BUZZER_PIN, LOW);

  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, HIGH);

  display.clearDisplay();
  display.setTextSize(2);

  display.setCursor(10, 10);
  display.println("FOOD");

  display.setCursor(10, 35);
  display.println("MONITOR");

  display.display();

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  timer.setInterval(2000L, sendToBlynk);

  Blynk.virtualWrite(V4, 1);

  delay(3000);
}

/************************************************************
 LOOP
************************************************************/
void loop()
{
  Blynk.run();
  timer.run();

  if (!systemEnabled)
  {
    delay(100);
    return;
  }

  /******** READ SENSORS ********/
  gasValue = analogRead(GAS_PIN);

  lightValue = digitalRead(LIGHT_PIN);

  temperature = dht.readTemperature();
  humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity))
  {
    Serial.println("DHT ERROR");
    return;
  }

  /**********************************************************
   GAS STATUS CLASSIFICATION
  **********************************************************/
  if (gasValue < 1200)
  {
    foodStatus = "NORMAL AIR";
  }
  else if (gasValue < 2000)
  {
    foodStatus = "FRESH FOOD";
  }
  else if (gasValue < 3300)
  {
    foodStatus = "SPOILAGE FOOD";
  }
  else
  {
    foodStatus = "GAS & FIRE";
  }

  /**********************************************************
   OUTPUT CONTROL
  **********************************************************/
  if (foodStatus == "NORMAL AIR")
  {
    digitalWrite(RELAY_PIN, HIGH);
    digitalWrite(BUZZER_PIN, LOW);

    digitalWrite(RED_LED, LOW);
    digitalWrite(GREEN_LED, HIGH);

    spoilageNotificationSent = false;
    fireNotificationSent = false;
  }
  else if (foodStatus == "FRESH FOOD")
  {
    digitalWrite(RELAY_PIN, HIGH);
    digitalWrite(BUZZER_PIN, LOW);

    digitalWrite(RED_LED, LOW);
    digitalWrite(GREEN_LED, HIGH);

    spoilageNotificationSent = false;
    fireNotificationSent = false;
  }
  else if (foodStatus == "SPOILAGE FOOD")
  {
    digitalWrite(RELAY_PIN, LOW);

    digitalWrite(BUZZER_PIN, HIGH);

    digitalWrite(RED_LED, HIGH);
    digitalWrite(GREEN_LED, LOW);

    if (!spoilageNotificationSent)
    {
      Blynk.logEvent("spoilage_alert",
                     "Spoilage Food Detected!");

      spoilageNotificationSent = true;
    }

    fireNotificationSent = false;
  }
  else if (foodStatus == "GAS & FIRE")
  {
    digitalWrite(RELAY_PIN, LOW);

    digitalWrite(BUZZER_PIN, HIGH);

    digitalWrite(RED_LED, HIGH);
    digitalWrite(GREEN_LED, LOW);

    if (!fireNotificationSent)
    {
      Blynk.logEvent("gas_fire_alert",
                     "Gas & Fire Alert Detected!");

      fireNotificationSent = true;
    }

    spoilageNotificationSent = false;
  }

  /**********************************************************
   SERIAL MONITOR
  **********************************************************/
  Serial.println("================================");

  Serial.print("Temperature : ");
  Serial.println(temperature);

  Serial.print("Humidity    : ");
  Serial.println(humidity);

  Serial.print("Gas Value   : ");
  Serial.println(gasValue);

  Serial.print("Status      : ");
  Serial.println(foodStatus);

  /**********************************************************
   OLED DISPLAY
  **********************************************************/
  display.clearDisplay();

  display.setTextSize(1);

  display.setCursor(18, 0);
  display.println("FOOD MONITOR");

  display.setCursor(0, 15);
  display.print("TEMP: ");
  display.print(temperature);
  display.println(" C");

  display.setCursor(0, 28);
  display.print("HUM : ");
  display.print(humidity);
  display.println(" %");

  display.setCursor(0, 41);
  display.print("GAS : ");
  display.println(gasValue);

  display.setCursor(0, 54);
  display.print("STATUS:");

  if (foodStatus == "NORMAL AIR")
  {
    display.print(" CLEAN AIR");
  }
  else if (foodStatus == "FRESH FOOD")
  {
    display.print(" FRESH FOOD");
  }
  else if (foodStatus == "SPOILAGE FOOD")
  {
    display.print(" SPOILAGE FOOD");
  }
  else
  {
    display.print(" GAS AND FIRE");
  }

  display.display();

  delay(1000);
}