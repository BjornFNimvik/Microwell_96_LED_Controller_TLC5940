
//For controlling 6xTLC5940 LED drivers using custom build Arduino Leonardo board "Microwell 94 LED Controller"
//TLC5940 product page https://www.ti.com/product/TLC5940
//Created by Unknown
//Edited 2024-05-30 by bjorn.nimvik@niva.no 

#define SIN     16    //TLC5940 Serial data inpu 
#define SCLK    15    //TLC5940 Serial data shift clock
#define XLAT    9     //TLC5940 Level triggered latch signal. When XLAT = high, the TLC5940 writes data from the input shift register to GS register. When XLAT = low, the data in GS or DC register is held constant.
#define BLANK   10    //TLC5940 Blank all outputs. When BLANK = H, all OUTn outputs are forced OFF.G S counter is also reset. When BLANK = L, OUTn are controlled by grayscale PWM control.
#define GSCLK   5     //TLC5940 Reference clock for grayscale PWM control

int LED[96]; //Array for storing individual LED light level value

void setup() 
{
  //Defining Arduino pins as output
  pinMode(SIN,   OUTPUT);    
  pinMode(SCLK,  OUTPUT); 
  pinMode(XLAT,  OUTPUT);    
  pinMode(BLANK, OUTPUT);     
  pinMode(GSCLK, OUTPUT); 
  while (!digitalRead(A0));
  

  for (int i=0; i<96; i++) 
  { 
    LED[i] = 0;
  }  

//---------------Program LED here----------------------
//Functions accept integer input 0 to 4095. Funtion sets brightness % = input/4095

  //setAllLED(0);

  //setSingleLED(2000, 0 , 0);

  setColumnLED(500, 1, 4);




//-----------------------------------------------------
  // Tell TLC5940 we're done
  digitalWrite(XLAT, HIGH);
  digitalWrite(XLAT, LOW); 
}

void loop()
{
run_pwm_cycle();
}

void setColumnLED(int brightnessvalue, int col_start, int col_end ){
  int LEDno = 0;
  for (col_start; col_start <= col_end; col_start++){
    //iterate through columns
    for (int LEDinCol = 0; LEDinCol < 8; LEDinCol++){
      //iterate through LED in column
      LEDno = (8 * col_start) + LEDinCol;
      LED[LEDno] = brightnessvalue;
    }
  }

  // Send levels to TLC5940 in backwards order. 
  for (int i = 95; i >= 0; i--) {
    // Shift out bits in reverse order    
    for (int j = 0; j < 12; j++) {
      //Takes int value in LED[i], 
      int x = (LED[i] << j) & 0x800; // hex 800 = bit 12
      // Write bit to TLC5940
      digitalWrite(SIN, x > 0);  
      digitalWrite(SCLK, HIGH);
      digitalWrite(SCLK, LOW);
    }
  } 

}


void setAllLED( int brightnessvalue )
{
  //int brightnessvalue = ((brightnessvalue_pcnt/100) * 4095);

  //int LED[96];
  //for (int i=0; i<96; i++) 
  //{ 
  // LED[i] = 0;
  //}

  int LEDno=0;
  //int brightnessValue accepts values from 0 - 4095. Lightlevel is calculated by equation "Brightness in % = (brightnessValue/4095) * 100"
  //Iterating through each LED bank
  for (int bank = 0; bank < 6; bank++)
   {
    //Assigning value to each 16 LED on a bank
   for (int led = 0; led < 16; led++)
     {
       LEDno = (16 * bank) + led;
       LED[LEDno] = brightnessvalue;
       
     }
     
    // incremental light level per LEDbank 
    //brightnessvalue = brightnessvalue + 680;
   }

  // Send levels to TLC5940 in backwards order. 
  for (int i = 95; i >= 0; i--) {
    // Shift out bits in reverse order    
    for (int j = 0; j < 12; j++) {
      //Takes int value in LED[i], converts it to 12bit int? 
      int x = (LED[i] << j) & 0x800; // hex 800 = bit 12
      // Write bit to TLC5940
      digitalWrite(SIN, x > 0);  
      digitalWrite(SCLK, HIGH);
      digitalWrite(SCLK, LOW);
    }
  }  
}

void setSingleLED(int brightnessvalue, int led_bank_pos, int led_pos){

  //Sets a single diode value to brightnessvalue. "Brightness in % = (brightnessValue/4095) * 100"
  //Mapping all LED on panel in a vector


  int LEDno=0;
  for (int led_bank = 0; led_bank < 6; led_bank++)
   {
    //Assigning value to each 16 LED on a bank
   for (int led = 0; led < 16; led++)
     { 
      if ((led_bank_pos == led_bank) && (led_pos == led)){
        LEDno = (16 * led_bank) + led;
        LED[LEDno] = brightnessvalue;
      }   
     }
  }
     
  // Send levels to TLC5940 in backwards order. 
  for (int i = 95; i >= 0; i--) {
    // Shift out bits in reverse order    
    for (int j = 0; j < 12; j++) {
      //Takes int value in LED[i], 
      int x = (LED[i] << j) & 0x800; // hex 800 = bit 12
      // Write bit to TLC5940
      digitalWrite(SIN, x > 0);  
      digitalWrite(SCLK, HIGH);
      digitalWrite(SCLK, LOW);
    }
  } 
}

void run_pwm_cycle(){
  //The grayscale PWM cycle starts with the falling edge of BLANK.
  digitalWrite(BLANK, HIGH);    
  digitalWrite(BLANK, LOW);

  //Counts a full PWM cycle. A BLANK=H signal after 4096 GSCLK pulses resets the grayscale counter to zero and completes the grayscale PWM cycle 
  for (int i=0; i<4096; i++) 
    {
      //PORTC maps to Arduino analog pins 0 to 5
      PORTC = B01000000;
      PORTC = B00000000;
    }
  delay(1);
  }
