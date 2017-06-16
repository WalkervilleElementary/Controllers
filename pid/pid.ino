#include <phys253.h>          
#include <LiquidCrystal.h>     


  ////////////////////////////////////////////////
// TINAH Demo Program - UBC Engineering Physics 253
// (nakane, 2015 May 7) -  Update for IDE 1.6.4   
// (nakane, 2015 Jan 2)  - Updated for use in Arduino IDE (1.0.6)
/////////////////////////////////////////////////

const int l_motor = 1;
const int r_motor = 0;

const int l_sensor = 2;
const int r_sensor = 0;


void setup()
{  
  #include <phys253setup.txt>
  Serial.begin(9600) ; 
}

const int dt = 10;

int k;

int gain_tot = 7;
int gain_p = 8;
int gain_i = 0;
int gain_d = 8;
int prev_error = 0;
int i_error = 0;

int error = 0;
int l_val;
int r_val;
bool l_on;
bool r_on;
int threshold = 100;

int velocity = 200;

// Positive means going to the right
int getError()
{
  l_val = analogRead(l_sensor);
  r_val = analogRead(r_sensor);

  l_on = l_val > threshold;
  r_on = r_val > threshold;

  if (l_on && r_on) return 0;
  else if(l_on && !r_on) return 1;
  else if(!l_on && r_on) return -1;
  else return (error > 0)? 5 : -5;
}

int computeCommand(int error, int dt)
{
  i_error += error;
  int kp = gain_p * error;
  int ki = gain_i * (i_error > 0)? 5 : ((i_error < -5)? -5 : i_error);
  int kd = gain_d * (error - prev_error)*100/dt;
  prev_error = error;
  return gain_tot*(kp + ki +  kd);
}

void sendMotorCommand(int k)
{
  LCD.clear();  LCD.home() ;
  LCD.setCursor(0,0); LCD.print(velocity - k);  
  LCD.setCursor(5,0); LCD.print(l_val);
  LCD.setCursor(0,1); LCD.print( -(velocity + k));
  LCD.setCursor(5,1); LCD.print(r_val);
  
  motor.speed(l_motor, velocity - k);
  motor.speed(r_motor, -(velocity + k));
}

void update()
{
  motor.speed(l_motor, 0);
  motor.speed(r_motor, 0);
  char state = 0;
  if (stopbutton())
  {
    while (!startbutton())
    {
      if (stopbutton()) state += 1;
      if (state > 5) state = 0;
      int start_val = knob(6);
      delay(100);
      int end_val = knob(6);

      int change = (start_val - end_val)/4 ;
      LCD.clear();  LCD.home() ;
          
      switch (state)
      {
      case 0:
          gain_tot += change;
          LCD.setCursor(0,0); LCD.print("gain_tot");
          LCD.setCursor(0,1); LCD.print(gain_tot);
          break;
      case 1:
          gain_p += change;
          LCD.setCursor(0,0); LCD.print("gain_p");
          LCD.setCursor(0,1); LCD.print(gain_p);
          break;
      case 2:
          gain_i += change;
          LCD.setCursor(0,0); LCD.print("gain_i");
          LCD.setCursor(0,1); LCD.print(gain_i);
          break;
      case 3:
          gain_d += end_val - start_val;
          LCD.setCursor(0,0); LCD.print("gain_d");
          LCD.setCursor(0,1); LCD.print(gain_d);
          break;
      case 4:
          threshold += change;
          LCD.setCursor(0,0); LCD.print("threshold");
          LCD.setCursor(0,1); LCD.print(threshold);
          break;
      case 5:
          velocity += change;
          LCD.setCursor(0,0); LCD.print("velocity");
          LCD.setCursor(0,1); LCD.print(velocity);
      }
      delay(100);
    }
  }
}



void loop()
{
  update();
  error = getError();
  k = computeCommand(error, dt);
  sendMotorCommand(k);
  delay(dt);
}





