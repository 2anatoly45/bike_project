#define MODE 10           // Pin numbers for each button
#define DOWN 5
#define UP 9
#define MAGNET 8          // Magnet simluated with a button

#define DEBOUNCE 50
#define INCH 0.0254

boolean modeState = 0;    // 3 buttons + simulated magnet sensor
boolean downState = 0;
boolean upState = 0;
boolean magnetState = 0;

int mag = 0;              // Times magnet passes by sensor
int time = 0;             // Time of trip
int seconds = 0;
int minutes = 0;
int hours = 0;
double distance = 0;         // Distance travelled
double velocity = 0;         // Instantaneous speed
double av_speed = 0;         // Average speed during trip
double diameter = 27;
double circum = 0;           // Circumference of wheel

int counter = 0;          // Used for switching between modes
int moment = 0;           // Value of millis at which button is pressed

boolean modePrev = 0;

void setup() {
  Serial.begin(9600);
  pinMode(MODE, INPUT);
  pinMode(DOWN, INPUT);
  pinMode(UP, INPUT);
  pinMode(MAGNET, INPUT);
}

void loop() {
  
  modeState = digitalRead(MODE);
  downState = digitalRead(DOWN);
  upState = digitalRead(UP);
  magnetState = digitalRead(MAGNET);
  
  seconds = (millis() - moment)/1000 % 60;
  minutes = ((millis() - moment)/1000 % 3600) / 60;
  hours = (millis() - moment)/1000 / 3600;
  
  distance = mag * circum;
  av_speed = distance * 3600 / (millis() - moment)/1000;
  //velocity = 
  
  if (modeState == HIGH) {
    debounce();   
    
    if (counter == 0)
      counter = 1;
  
    else if (counter == 1)
      counter = 2;
  
    else if (counter == 2)
      counter = 0;
  }
 
  if (magnetState == HIGH && counter == 0) {
    debounce();
    ++mag;
  }

    
  if (counter == 0) {
    if (upState == HIGH)
      moment = millis();
  }
  
  if (counter == 1) {
  }
 
  if (counter == 2) {
    if (downState == HIGH)
      --diameter;
    else if (upState == HIGH)
      ++diameter;
      
    circumference(diameter);
  } 
  
  printer();
}

void debounce() { delay(DEBOUNCE); }

void circumference(double diameter) {

  circum = ((diameter) * 0.0254) * 3.141593;
}

void printer() {             // Note that printer is limited to a 16x2 display
  
  if (counter == 0) {
    Serial.print("S: ");
    Serial.print(velocity);
    Serial.println(" km/h");
    
    Serial.print("T: ");
    Serial.print(hours);
    Serial.print(':');
    Serial.print(minutes);
    Serial.print(':');
    Serial.println(seconds);
  }
  
  if (counter == 1) {
    Serial.print("D: ");
    Serial.print(distance);
    Serial.print(" Av.Sp.: ");
    Serial.println(av_speed);
    //Serial.print();            // Total time of trip
  }
  
  if (counter == 2) { // Print wheel diameter in inches
    Serial.print("Size: ");
    Serial.println(diameter);
  }
}
