import processing.net.*;
import controlP5.*;

//set up wifi
Client myClient = new Client(this,"192.168.1.5",56676);

int currentTime, prevTime;

int data = 0;
Textarea myTextarea;
Slider mySlider;
Println console;
int prev_data = 0; // initialise arbitrary character
int input;

//stops buttons from being automatically pressed when GUI is turned on
int i = 1;

//create new sketch
void setup(){
  //set colour and size
  size(860,1000);
  background(0);
  
  ControlP5 cp5 = new ControlP5(this);
  
  //create GO and STOP buttons
  cp5.addButton("GO").setValue(0).setPosition(450,300).setSize(150,150)
  .setFont(createFont("arial",20));
  cp5.addButton("STOP").setValue(0).setPosition(650,300).setSize(150,150)
  .setFont(createFont("arial",20));
  
  //create slider to display distance from object
  mySlider = cp5.addSlider("Distance (0-15)").setPosition(450,150).setWidth(300)
  .setFont(createFont("arial",12)).setRange(0,15);
  
  //create textbox for feedback from arduino
  myTextarea = cp5.addTextarea("txt").setPosition(10, 10).setSize(400, 890)
                                            .setFont(createFont("arial",18));
  
  //Show the PID co-efficients
  cp5.addButton("Kp = 9.5     Ki = 6     Kd = 2")
  .setValue(0).setPosition(450,500).setSize(350,50)
  .setFont(createFont("arial",15));
  
  console = cp5.addConsole(myTextarea);
  myClient.clear();
  data = 0;
}

void draw(){
  
  //read distance from US sensor and display on slider 
  get_distance();
  

  //handle input 
  input = myClient.read();
  
  if((input == 50 || input == 40 || input == 60) && input != prev_data){
    //ensures data is valid and we dont get spam
    data = input;
    if (data == 40) {
      println("Stopping for obstacle");
    }
    else if (data == 50) {
       println("Obstacle detected");
    }
    else if (data == 60) {
       println("No obstacle detected");
    }
  }
 
  prev_data = data;
  
  //remeber what we just said so we don't continuosly spam response

  
  //clear cache to speed things up
  myClient.clear();
}

public void mySlider(){
   
}

public void GO(){
  if(myClient.active() && i == 0){
    myClient.write('g');
    println("Starting");
    myClient.clear();
  }
}

public void STOP(){
  if(myClient.active() && i == 0){
    myClient.write('s');
    println("Stopping");
    myClient.clear();
  }
  i = 0;
}

void get_distance(){
  int dist = myClient.read(); 
  if(dist <= 20 && dist >= 0 ){
    mySlider.setValue(dist);
  }
}
