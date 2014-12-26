#define MODE 10           // Button pin numbers
#define DOWN 9
#define UP 8
#define MAGNET 7          // Magnet simluated with a button
#define INTERVAL 2000     // Interval for velocity

#define DEBOUNCE 50
#define INCH 0.0254       // Meters/inch

boolean modeState = 0;    // 3 buttons + simulated magnet sensor
boolean downState = 0;
boolean upState = 0;
boolean magnetState = 0;
boolean magnetPrev = 0;

int mag = 0;              // Times magnet passes by sensor
int contacts = 0;         // For velocity, runs parallel to mag
int time = 0;             // Time of trip
int seconds = 0;
int minutes = 0;
int hours = 0;
double span = 0;
double diameter = 27;        // Wheel diameter in inches
double distance = 0;         // Distance travelled
double velocity = 0;         // Instantaneous speed
double av_speed = 0;         // Average speed during trip
double max_speed = 0;        // Max speed attained during trip

int counter = 0;          // Used for switching between modes
int moment = 0;           // Value of millis at which button is pressed

void setup() {
  Serial.begin(9600);
  pinMode(MODE, INPUT);
  pinMode(DOWN, INPUT);
  pinMode(UP, INPUT);
  pinMode(MAGNET, INPUT);
}

void loop() {      

  Serial.print("millis: "); 
  Serial.println(millis()); // For verifying execution time

  modeState = digitalRead(MODE);
  downState = digitalRead(DOWN);
  upState = digitalRead(UP);
  magnetState = digitalRead(MAGNET);        // Will be replaced by sensor

  seconds = (millis() - moment)/1000 % 60;
  minutes = ((millis() - moment)/1000 % 3600) / 60;
  hours = (millis() - moment)/1000 / 3600;

  distance = mag * ((diameter * 0.0254) * 3.141593) / 1000;  // In kilometers
  av_speed = distance * 3600 / ((millis() - moment) / 1000);   // In km/h

  if (INTERVAL <= ((millis() - moment) - span)) {
    velocity = (contacts * (diameter * 0.0254) * 3.141593) / INTERVAL * 3600;
    contacts = 0;
    span = millis() - moment;
  }

  if (max_speed < velocity) 
    max_speed = velocity;


  if (modeState == HIGH) {
    debounce();   

    if (counter == 0)
      counter = 1;
    else if (counter == 1)
      counter = 2;
    else if (counter == 2)
      counter = 0;
  }

  // Increment ONLY when the magnet has begun to be sensed
  if (magnetState == HIGH && magnetPrev == 0) {
    debounce();
    magnetPrev = 1;
    ++mag;
    ++contacts;
  }

  // Reset the magnet state once the magnet is no longer being sensed
  // This makes sure that magnet contacts isn't being counted forever while in contact with sensor
  if (magnetState == LOW && magnetPrev == 1)
    magnetPrev = 0;

  if (counter != 2) {
    if (upState == HIGH) {
      debounce();
      moment = millis();
    } 
    else if (downState == HIGH) {
      int breakStarted = millis();

      while(downState != HIGH) {
        // Do nothing (bad solution, spedometer is unusable for this period
      }

      int breakOver = millis();

      int breakTime = breakOver - breakStarted;

      time -= breakTime;
    }      
  }
  
/*
  if (counter == 2) {
    if (downState == HIGH)
      --diameter;
    else if (upState == HIGH)
      ++diameter;
  } 
*/

  printer();
}

/*
double calculateSpeed() {
 int start = getTime(); // obtain initial time
 
 int initialCount = mag; // initial number of contacts
 
 int finalCount = 0; 
 int contacts = 0;
 int intervalSpeed = 0;
 
 // determine how many magnet contacts have been made within the specified interval
 if(millis() >= start + INTERVAL) {
 contacts = mag - initialCount;
 intervalSpeed = contacts * ((diameter * 0.0254) * 3.141593) / 1000;
 return intervalSpeed;
 }
 
 return 0; 
 }
 */


void debounce() { 
  delay(DEBOUNCE); 
}

void printer() {             // Note that printer is limited to a 16x2 display  
  if (counter == 0) {
    Serial.print("S ");         // 2
    if (velocity < 10) Serial.print('0');    
    Serial.print(velocity, 1);  // 4
    Serial.println(" km/h");    // 5
                                // Total = 11
    Serial.print("MAX S ");     // 6
    if (max_speed < 10) Serial.print('0');
    Serial.print(max_speed, 1); // 4
    Serial.print(" km/h");      // 5
                                // Total = 15
  }

  if (counter == 1) {
    Serial.print(" D ");        // 3
    if (distance < 10) Serial.print('0');    
    Serial.print(distance, 1);  // 4
    Serial.println(" km");      // 3
                                // Total = 10
    Serial.print("T ");         // 2
    if (hours < 10) Serial.print('0');
    Serial.print(hours);        // 2
    Serial.print(':');          // 1
    if (minutes < 10) Serial.print('0');
    Serial.print(minutes);      // 2
    Serial.print(':');          // 1
    if (seconds < 10) Serial.print('0');
    Serial.print(seconds);      // 2
                                // Total = 10
  }
  
  if (counter == 2) {
    Serial.print("AVG S ");     // 6
    if (av_speed < 10) Serial.print('0');    
    Serial.print(av_speed, 1);  // 4
    Serial.println(" km/h");    // 5
                                // Total = 15
    //Serial.print(time);            // Total time of trip
  }
  
/*
  if (counter == 2) { // Print wheel diameter in inches
    Serial.print("ADJ SIZE: ");
    Serial.print(diameter);
  }
*/
}
