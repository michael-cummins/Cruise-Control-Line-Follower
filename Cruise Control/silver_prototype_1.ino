
#include <WiFiNINA.h>
//wifi details
char ssid[] = "eir37842095-2.4G";
char pass[] = "ezmns3bw";
//set up server with port number
WiFiServer server(56676);

//PID co-efficients
double kp = 9.5;
double ki = 6;
double kd = 2;

//used in compute_pid
unsigned long currentTime, previousTime;
double elapsedTime;
double error;
double lastError;
double input,setPoint;
double output = 0;
double cumError, rateError;
int range;

const int hard_limit = 255;

bool moving = false;
bool forward;
bool stopped = false;

//declare pins
const int LEYE = 21; //left eye
const int REYE = 20; //right eye
const int echo = 15; //US sensor
const int l_backward = 12;
const int trig = 14; //US sensor
const int l_forward = 19;
const int r_forward = 16;
const int r_backward = 17;

int current_speed;

//function declerations
double distance();
void move_forward(int x);
void reverse(int x);
void move_right(int x);
void move_left(int x);
void stop_();
void coast();
double computePID(double inp);
int hash(double PID);

void setup() {
  Serial.begin(9600);
  //setup server
  WiFi.begin(ssid, pass);
  server.begin();
  delay(2000);
  
  //declare safe distance
  setPoint = 10;
  
  //set up pins
  pinMode( r_forward, OUTPUT );
  pinMode( r_backward, OUTPUT );
  pinMode( l_backward, OUTPUT );
  pinMode( l_forward, OUTPUT );
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  pinMode( LEYE, INPUT );
  pinMode( REYE, INPUT );

  
  
  stop_();
}

bool stopped = false;

void loop() {
    //set up wifi
    WiFiClient client = server.available();
    
    //handle input from buttons
    //g = GO
    //s = STOP
    char input = client.read();
    //moving or not moving
    if(input == 'g'){
      moving = true;
    }
    else if(input == 's'){
      moving = false;
    }
    
    // compute the current speed based off of the range
    // moving forward or backwards?
    range = distance();
    if(range <= 15){
      int dist = (int)(range);
      server.write(dist);
      current_speed = computePID(range);
      //int index = hash(output);  
      if(moving && current_speed<50){
        //stoppping for obstacle
          server.write(40);
          stopped = true;
          stop_();         
      }
      else{
        //current_speed = Speed[index];
        if(current_speed>0 && moving){
          forward = true;
          stopped = false;
          //obstacle detected
          server.write(50);
        }
        else if(current_speed < 0 && moving){
          forward = false;
          //obstacle detected
           server.write(50);
           stopped = false;
          }
        }
     }
     else if (range > 15 && moving){
      forward = true;
      stopped = false;
      current_speed = hard_limit;
      //no obstacle detected
      server.write(60);
    }

    for(int i=0; i<10; i++){
    //movement algorithm
    //check if we hit the GO or STOP button
    //3 different conditions, not moving, moving forward and moving backward
      if(!stopped){
        if(!moving){
            stop_();
          }
        if(forward && moving){
          if(digitalRead( REYE ) != HIGH){
             move_right(current_speed);  
          }
          else if(digitalRead( LEYE ) != HIGH){
            move_left(current_speed);
          }
          else{
           move_forward(current_speed);
          }
        }
        if(moving && !forward){
          reverse(current_speed);
        }
      }
      else{
        stop_();
      }
    }
}

double computePID(double inp){
  currentTime = millis();
  elapsedTime = (double)(currentTime - previousTime);

  error = inp - setPoint;
  cumError += error*elapsedTime;
  rateError = (error - lastError)/elapsedTime;

  double out = kp*error + ki*cumError + kd*rateError;

  previousTime = currentTime;
  lastError = error;

  if(previousTime >= 800){
    cumError = 0;
  }

  return out;
}

void move_forward(int x)
{
   if(x <= 255){
    analogWrite(r_forward, x);
    analogWrite(r_backward, 0);  
    analogWrite(l_forward, x);
    analogWrite(l_backward, 0);  
  }
}

void reverse(int x){
   if(x <= 255){
    analogWrite(r_forward, 0);
    analogWrite(r_backward, x);  
    analogWrite(l_forward, 0);
    analogWrite(l_backward, x);  
  }
}

void move_right(int x)
{
  if(x <= 255){
    int y =(int)(x/4);
    analogWrite(r_forward, 0);
    analogWrite(r_backward, y);  
    analogWrite(l_forward, x);
    analogWrite(l_backward, 0);  
  }
}

void move_left(int x)
{
   if(x <= 255){
    int y =(int)(x/4);
    analogWrite(r_forward, x);
    analogWrite(r_backward, 0);  
    analogWrite(l_forward, 0);
    analogWrite(l_backward, y);  
  }
}

void stop_(){
  analogWrite(r_forward, 255);
  analogWrite(r_backward, 255);  
  analogWrite(l_forward, 255);
  analogWrite(l_backward, 255);
}

void coast(){
  digitalWrite(r_forward, LOW);
  digitalWrite(r_backward, LOW);  
  digitalWrite(l_forward, LOW);
  digitalWrite(l_backward, LOW);
}

double distance(){
  double Range;
  digitalWrite( trig, LOW );
  delay(1);
  digitalWrite( trig, HIGH );
  delay(1);
  digitalWrite( trig, LOW );
  Range = 0.017*(pulseIn(echo, HIGH));
  return Range;
}
