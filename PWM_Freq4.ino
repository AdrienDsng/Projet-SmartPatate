#include <PololuLedStrip.h>

//****************************************************************************************
// PWM Square Wave Generator
// VLO 2013-2014
//****************************************************************************************


const int outPin = 9; // output pin
const float maxFreq = 16000000; //max freq supported by Arduino (16MHz)

PololuLedStrip<12> led_strip;
rgb_color colors[60];

//set frequency (Hz)
//min frequency: 0.24 Hz
//max frequency: 8MHz
void setFrequency(float frequency)
{
 
  if(frequency >=0.24 || frequency <=8000000)
  {
    
    TCCR1A=0b10000010;        // Set up frequency generator, mode 14 (Fast PWM)
    //WGM11 = 1, COM1A1 = 1
    //Fast PWM: TOP=ICR1, update of OCR1x=BOTTOM, TOV1 Flag Set on TOP
    
  
    unsigned int v=0;
    int indscale=0;
    float prescale[] = {1.0,8.0,64.0,256.0,1024.0};
    float period=1/frequency;
    
    while(v==0)
    {
      float curfreq=maxFreq/prescale[indscale];
      float tickperiod=1/curfreq;
      float nbtick=period/tickperiod;
     
      if(nbtick>65535)
      {
        indscale=indscale+1;
      }
      else
      {
        v=nbtick;
      }

    }
    
    int c=prescale[indscale];
    
     switch (c) {
      
         case 1: TCCR1B=0b00011001; break;
         case 8: TCCR1B=0b00011010; break;
         case 64: TCCR1B=0b00011011; break;
         case 256: TCCR1B=0b00011100; break;
         case 1024: TCCR1B=0b00011101; break;
         default: TCCR1B=0b00011000;
     }
    
    //WGM12 = 1, WGM13 = 1
    
    //three last bit of TCCR1B:    CS12   CS11   CS10
    // 0: no clock (timer stopped)  0      0      0
    // clk/1: no prescaling         0      0      1 
    // clk/8                        0      1      0
    // clk/64                       0      1      1
    // clk/256                      1      0      0
    // clk/1024                     1      0      1

    
    ICR1=v; //pulse duration = ICR1 value x time per counter tick
    
    //for 16Mhz (chip frequency)
    //Prescale Time per counter tick
    //1         0.0625 uS
    //8         0.5 uS
    //64  4 uS
    //256 16 uS
    //1024  64uS
    
    OCR1A=v/2; //Output Compare Register //low state

  }
  
  
}

rgb_color hsvToRgb(uint16_t h, uint8_t s, uint8_t v)
{
    uint8_t f = (h % 60) * 255 / 60;
    uint8_t p = (255 - s) * (uint16_t)v / 255;
    uint8_t q = (255 - f * (uint16_t)s / 255) * (uint16_t)v / 255;
    uint8_t t = (255 - (255 - f) * (uint16_t)s / 255) * (uint16_t)v / 255;
    uint8_t r = 0, g = 0, b = 0;
    switch((h / 60) % 6){
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        case 5: r = v; g = p; b = q; break;
    }
    return (rgb_color){r, g, b};
}

void setup()
{

  Serial.begin(115200); //for sending A0 pin values
  
  pinMode(outPin,OUTPUT);        //Signal generator pin
  
  setFrequency(16000000); //generate a square wave (in this case at 1000Hz)
  
  //for test
  //Serial.println();
  //Serial.println(ICR1);
  //Serial.println(TCCR1B);

  for(int i = 0; i < 0; i++) {
    colors[i] = 0;
  }
}

void loop()
{          
  for(int i = 0; i < 60; i++) {
    colors[i] = {0, 0, 0};
  }
                               
  //Serial.write( 0xff );                                                         
  //Serial.write( (val >> 8) & 0xff );                                            
  //Serial.write( val & 0xff );

  float val = analogRead(0);     
  
  float value = val * 5.0 / 1023.0;
  int val_led = val * 60 / 1023;
  int val_led2 = 60 - val_led;
  val_led2 = val_led2 * 100 / 20 * 60 / 100;
  
  digitalWrite(2, 0);
  digitalWrite(3, 0);
  digitalWrite(4, 0);
  
  if(value >= 4 && value < 4.9) {
    digitalWrite(2, 1);
  }
  else if (value >= 3.7 && value < 4) {
    digitalWrite(3, 1);
  }
  else if (value >= 3.1 && value < 3.7) {
    digitalWrite(4, 1);
  }

  for(int i = 0; i < val_led2; i++) {
    colors[i] = hsvToRgb(i * 6, 255, 255);
  }

  led_strip.write(colors, 60);
}
