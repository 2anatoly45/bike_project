#define MODE 10           // Button pin numbers
#define PAUSE 8
#define MAGNET 7          // Magnet simluated with a button
#define INTERVAL 2000     // Time interval, in milliseconds, which will be used to determine instantaneous velocity
#define DIAMETER 27       // Diamter of the wheel in inches
#define MAGNETS 2         // Number of magnets on the wheel (affects accuracy of measurement)
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
double arc = ((DIAMETER * PI) * IN_TO_M) / MAGNETS ; // Arc length between magnets in meters
double distance = 0;         // Distance travelled
double span = 0;             // Time interval between consecutive velocity calculations
double velocity = 0;         // Instantaneous speed
double av_speed = 0;         // Average speed during trip
double max_speed = 0;        // Max speed attained during trip

int counter = 0;          // Used for switching between modes
int moment = 0;           // Value of millis at which up button is pressed

void setup() {
  Serial.begin(9600);
  pinMode(MODE, INPUT);
  pinMode(PAUSE, INPUT);
  pinMode(MAGNET, INPUT);
}

void convert_time(int mil) { // -----Still need a way to adjust the session time-----
  double sec = mil / 1000; // Convert into seconds first so that the rest of the calculations are easier to read
  
  seconds = sec % 60;
  minutes = sec % 3600 / 60;
  hours = sec / 3600;
}

void loop() { 
  //  Serial.print("millis: "); Serial.println(millis()); // For verifying execution time

  modeState = digitalRead(MODE);
  pauseState = digitalRead(PAUSE);
  magnetState = digitalRead(MAGNET);        // Will be replaced by sensor

  convert_time(millis());       // Convert time into hours, minutes and seconds
  convert_time(riding_time);

  distance = mag * arc / 1000;    // [km]
  av_speed = distance * 3600 / ((millis() - moment) / 1000);   // [km/h]

  if (INTERVAL <= ((millis() - moment) - span)) {
    velocity = (contacts * arc) / INTERVAL * 3600;
    contacts = 0;
    span = millis() - moment;
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

  // Increment ONLY when the magnet has begun to be sensed
  if (magnetState == HIGH && magnetPrev == 0) {
    magnetPrev = 1;
    ++mag;
    ++contacts;
  }

  // Reset previous magnet state once magnet is no longer sensed
  if (magnetState == LOW)
    magnetPrev = 0;

  if (pauseState == HIGH && pausePrev == 0) { // Press the pause button
    moment = millis();
  } 

  // ==== Replace the following with code that will handle pressing pause button again ====
  /* 
   else if (downState == HIGH) {
   int breakStarted = millis();
   
   while(downState != HIGH) {
   // Do nothing (bad solution, spedometer is unusable for this period
   }
   
   int breakOver = millis();
   
   int breakTime = breakOver - breakStarted;
   
   time -= breakTime;
   }  
   */

  printer();
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

