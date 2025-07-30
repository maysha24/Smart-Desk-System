#include <Wire.h>
#include <RTClib.h>
#include <DHT.h>
#include <LiquidCrystal.h>
// Pin Definitions
#define DHTPIN 2            // DHT11 data pin
#define DHTTYPE DHT11       // DHT11 sensor type
#define LIGHT_SENSOR_PIN 3  // Photoresistor module DO pin
#define LED_PIN 6           // Main LED pin for low light
#define BUZZER_PIN 5        // Buzzer pin
#define SWITCH_PIN 30       // Input switch for LED control
#define LED1_PIN 28         // LED 1 pin
#define LED2_PIN 26         // LED 2 pin
#define LED3_PIN 24         // LED 3 pin
#define LED4_PIN 22         // LED 4 pin

// Initialize components
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);
RTC_DS1307 rtc;
DHT dht(DHTPIN, DHTTYPE);

// Global Variables
int count = 0;
int newcount;
bool lowLightNotified = true; // Tracks if low light has been notified
bool hydrateNotified = true;  // Tracks if hydrate reminder has been notified

void setup() {
  // Initialize Serial Monitor
  Serial.begin(9600);

  // Initialize the LCD
  lcd.begin(16, 2);  // Set up the LCD
  lcd.print("Initializing...");
  delay(2000);
  lcd.clear();
  noTone(BUZZER_PIN);

  // Initialize RTC
  if (!rtc.begin()) {
    lcd.print("RTC Not Found");
    while (1);  // Halt if RTC is not found
  }
  if (!rtc.isrunning()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)) + TimeSpan(0, -8, 0, 0));  // Adjust RTC to local time
  }

  // Initialize DHT sensor
  dht.begin();

  // Set pin modes
  pinMode(LIGHT_SENSOR_PIN, INPUT);  // Photoresistor DO pin
  pinMode(LED_PIN, OUTPUT);          // Main LED pin
  pinMode(BUZZER_PIN, OUTPUT);       // Buzzer pin
  pinMode(SWITCH_PIN, INPUT);        // Switch input
  pinMode(LED1_PIN, OUTPUT);         // LED 1
  pinMode(LED2_PIN, OUTPUT);         // LED 2
  pinMode(LED3_PIN, OUTPUT);         // LED 3
  pinMode(LED4_PIN, OUTPUT);         // LED 4
}

void loop() {
  // Get the current time from RTC
  DateTime now = rtc.now();

  // Read temperature and humidity from DHT11
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Read light state from the photoresistor module
  int lightState = digitalRead(LIGHT_SENSOR_PIN);

  // Display time on LCD
  lcd.setCursor(0, 0);
  lcd.print("Time: ");
  lcd.print((now.hour() + 8) % 24);  // Adjust for time zone
  lcd.print(":");
  lcd.print(now.minute());
  lcd.print(":");
  lcd.print(now.second());

  // Display temperature and humidity on LCD
  lcd.setCursor(0, 1);
  if (isnan(temperature) || isnan(humidity)) {
    lcd.print("Temp Error!     ");
  } else {
    lcd.print("T:");
    lcd.print(temperature);
    lcd.print("C ");
    lcd.print("H:");
    lcd.print(humidity);
    lcd.print("% ");
  }

  // Handle low light notification
  if (lightState == HIGH) {  // Low light detected
    digitalWrite(LED_PIN, HIGH);  // Turn on LED
    if (!lowLightNotified) {
      tone(BUZZER_PIN, 1000, 500);  // Beep once for 500ms
      lcd.setCursor(0, 1);
      lcd.print("Low Light!      ");
      lowLightNotified = true;  // Mark low light as notified
    }
  } else {
    digitalWrite(LED_PIN, LOW);  // Turn off LED
    lowLightNotified = false;   // Reset notification when light is sufficient
  }

  // Handle hydrate notification
  if (temperature > 25) {  // High temperature detected
    if (!hydrateNotified) {
      tone(BUZZER_PIN, 1500, 500);  // Beep once for 500ms at a higher tone
      lcd.setCursor(0, 1);
      lcd.print("Hydrate!        ");
      hydrateNotified = true;  // Mark hydrate reminder as notified
    }
  } else {
    hydrateNotified = false;  // Reset notification when temperature is normal
  }

  // Handle switch press for LED control
  if (digitalRead(SWITCH_PIN) == HIGH) {
    newcount = count + 1;  // Increment count on button press
    if (newcount != count) {
      Serial.println(newcount);  // Print new count to Serial Monitor
      switch (newcount) {
        case 1:
          digitalWrite(LED1_PIN, HIGH);  // Turn on LED 1
          break;
        case 2:
          digitalWrite(LED2_PIN, HIGH);  // Turn on LED 2
          break;
        case 3:
          digitalWrite(LED3_PIN, HIGH);  // Turn on LED 3
          break;
        case 4:
          digitalWrite(LED4_PIN, HIGH);  // Turn on LED 4
          break;
        default:
          // Reset LEDs and count
          digitalWrite(LED1_PIN, LOW);
          digitalWrite(LED2_PIN, LOW);
          digitalWrite(LED3_PIN, LOW);
          digitalWrite(LED4_PIN, LOW);
          newcount = 0;
          break;
      }
      count = newcount;  // Update the count
    }
  }

  delay(100);  // Update loop delay
}


