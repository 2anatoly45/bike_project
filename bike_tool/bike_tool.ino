#include <LiquidCrystal.h>

#define MODE 7            // Button pin numbers
#define PAUSE 8
#define SENSOR 9          // Hall Effect Sensor (latching)
#define INTERVAL 2000     // Time interval, in milliseconds, for determining instantaneous velocity
#define DIAMETER 27       // Diamter of the wheel in inches
#define MAGNETS 2         // Number of magnets (affects accuracy of measurement)
#define IN_TO_M 0.0254    // Conversion rate from inches to meters (1 inch = 0.0254 meters)
#define PI 3.141593

boolean modeState = 0;    // 3 buttons + simulated magnet sensor
boolean modePrev = 0;
boolean pauseState = 0;
boolean pausePrev = 0;
boolean magnetState = 0;
boolean magnetPrev = 0;

int mag = 0;              // Times magnet passes by sensor
int contacts = 0;         // For velocity, runs parallel to mag
int riding_time = 0;      // Time of trip in milliseconds
int seconds = 0;
int minutes = 0;
int hours = 0;
int riding_seconds = 0;
int riding_minutes = 0;
int riding_hours = 0;
double arc = ((DIAMETER * PI) * IN_TO_M) / MAGNETS ; // Arc length between magnets [m]
double distance = 0;         // Distance travelled
double span = 0;             // Time interval between consecutive velocity calculations
double velocity = 0;         // Instantaneous speed
double av_speed = 0;         // Average speed during trip
double max_speed = 0;        // Max speed attained during trip

int counter = 0;          // Used for switching between modes
int moment = 0;           // Value of millis at which up button is pressed

LiquidCrystal lcd(2, 3, 13, 12, 11, 10);

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  pinMode(MODE, INPUT);
  pinMode(PAUSE, INPUT);
  pinMode(SENSOR, INPUT);
}

void loop() { 
  //  Serial.print("millis: "); Serial.println(millis()); // For verifying execution time

  modeState = digitalRead(MODE);
  pauseState = digitalRead(PAUSE);
  magnetState = digitalRead(SENSOR);

  convert_time(millis());         // Convert time into hours, minutes and seconds
  convert_time(riding_time);

  distance = mag * arc / 1000;                         // [km]
  av_speed = distance * 3600 / (riding_time / 1000);   // [km/h]

  if (INTERVAL <= (riding_time - span)) {
    velocity = (contacts * arc) * 3600 / INTERVAL;
    contacts = 0;
    span = riding_time;
  }

  if (max_speed < velocity) 
    max_speed = velocity;

  if (modeState == HIGH && modePrev == 0) {
    modePrev = 1;

    if (counter == 0)
      counter = 1;
    else if (counter == 1)
      counter = 2;
    else if (counter == 2)
      counter = 0;
  }

  if (modeState == LOW)
    modePrev = 0;

  if (magnetState == HIGH && magnetPrev == 0) {
    magnetPrev = 1;
    ++mag;
    ++contacts;
  }

  if (magnetState == LOW)
    magnetPrev = 0;

  if (pauseState == HIGH && pausePrev == 0) { // Press the pause button
    moment = millis();
  } 

  printer();
  lcd_printer();
}

void convert_time(int mil) { // -----Still need a way to adjust the session time-----  
  seconds = (mil / 1000) % 60;
  minutes = (mil / 1000) % 3600 / 60;
  hours = (mil / 1000) / 3600;
}

void printer() {   //   Serial.print(mag); Serial.print("   ");       // Note: printer is limited to a 16x2 display  
  if (counter == 0) {
    Serial.print("S ");            // 2
    if (velocity < 10) Serial.print('0');    
    Serial.print(velocity, 1);     // 4
    Serial.print("km/h");          // 4   Total = 10

    Serial.print(" ");
    Serial.print(" MAX S ");       // 6
    if (max_speed < 10) Serial.print('0');
    Serial.print(max_speed, 1);    // 4
    Serial.println("km/h");        // 4   Total = 14
    // 
  }
  else if (counter == 1) {
    Serial.print("D ");            // 2
    if (distance < 10) Serial.print('0');    
    Serial.print(distance, 1);     // 4
    Serial.print("km");            // 2   Total = 8

    Serial.print(" ");
    Serial.print("T ");            // 2
    if (hours < 10) Serial.print('0');
    Serial.print(hours);           // 2
    Serial.print(':');             // 1
    if (minutes < 10) Serial.print('0');
    Serial.print(minutes);         // 2
    Serial.print(':');             // 1
    if (seconds < 10) Serial.print('0');
    Serial.println(seconds);       // 2   Total = 10

  }
  else if (counter == 2) {
    Serial.print("AVG S ");        // 6
    if (av_speed < 10) Serial.print('0');    
    Serial.print(av_speed, 1);     // 4
    Serial.println("km/h");        // 4   Total = 14

    //Serial.print(time);          // Total time of trip
  }
}

void lcd_printer() {               // Note: printer is limited to a 16x2 display
  lcd.clear();

  if (counter == 0) {
    lcd.setCursor(0, 0);
    lcd.print("S ");            // 2
    if (velocity < 10) lcd.print('0');    
    lcd.print(velocity, 1);     // 4
    lcd.print("km/h");          // 4   Total = 10
    
    lcd.setCursor(0, 1);
    lcd.print("MAX S ");       // 6
    if (max_speed < 10) lcd.print('0');
    lcd.print(max_speed, 1);    // 4
    lcd.print("km/h");        // 4   Total = 14
    // 
  }
  else if (counter == 1) {
    lcd.setCursor(0, 0);
    lcd.print("D ");            // 2
    if (distance < 10) lcd.print('0');    
    lcd.print(distance, 1);     // 4
    lcd.print("km");            // 2   Total = 8

    lcd.setCursor(0, 1);
    lcd.print("T ");            // 2
    if (hours < 10) lcd.print('0');
    lcd.print(hours);           // 2
    lcd.print(':');             // 1
    if (minutes < 10) lcd.print('0');
    lcd.print(minutes);         // 2
    lcd.print(':');             // 1
    if (seconds < 10) lcd.print('0');
    lcd.print(seconds);         // 2   Total = 10
    
    lcd.setCursor(0, 1);
    lcd.print("AVG S ");        // 6
    if (av_speed < 10) lcd.print('0');    
    lcd.print(av_speed, 1);     // 4
    lcd.print("km/h");          // 4   Total = 14
  }
  else if (counter == 2) {
    lcd.setCursor(0, 0);
    lcd.print("TOT T ");         // 6
    if (riding_hours < 10) lcd.print('0');
    lcd.print(riding_hours);     // 2
    lcd.print(':');              // 1
    if (riding_minutes < 10) lcd.print('0');
    lcd.print(riding_minutes);   // 2
    lcd.print(':');              // 1
    if (riding_seconds < 10) lcd.print('0');
    lcd.print(riding_seconds);   // 2   Total = 10
  }
}

