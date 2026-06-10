#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x3F, 16, 2);  // LCD address 0x3F, 16 chars, 2 lines

int htimei, ltimei, htimeo, ltimeo;
float Ttimei, freqi, Ttimeo, freqo;

void setup() {
  lcd.init();
  lcd.backlight();
  Serial.begin(9600);
  pinMode(8, INPUT); // Input frequency
  pinMode(7, INPUT); // Output frequency
}

void loop() {
  // Measure input frequency
  htimei = pulseIn(8, HIGH);
  ltimei = pulseIn(8, LOW);

  if (htimei == 0 || ltimei == 0) {
    freqi = 0; // No valid frequency detected
  } else {
    Ttimei = htimei + ltimei;
    freqi = 1000000 / Ttimei;
  }

  // Measure output frequency
  htimeo = pulseIn(7, HIGH);
  ltimeo = pulseIn(7, LOW);

  if (htimeo == 0 || ltimeo == 0) {
    freqo = 0; // No valid frequency detected
  } else {
    Ttimeo = htimeo + ltimeo;
    freqo = 1000000 / Ttimeo;
  }

  // Update LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("In freq: ");
  lcd.print(freqi, 2); // Show 2 decimal places
  lcd.print("Hz");

  lcd.setCursor(0, 1);
  lcd.print("Out freq: ");
  lcd.print(freqo, 2); // Show 2 decimal places
  lcd.print("Hz");

  // Print to Serial Monitor
  Serial.print("Input Frequency: ");
  Serial.println(freqi);
  Serial.print("Output Frequency: ");
  Serial.println(freqo);

  delay(2000); // Adjust delay as needed
}
