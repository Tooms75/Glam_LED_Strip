#include <SPI.h>
#include <Adafruit_DotStar.h>

#define NUMPIXELS 119
//#define DATAPIN    0
//#define CLOCKPIN   1

// Create strip
Adafruit_DotStar strip = Adafruit_DotStar(NUMPIXELS, DOTSTAR_BGR) ;

#include <IRremote.h>

int RECV_PIN = 7;

IRrecv irrecv(RECV_PIN);

decode_results results;



void setup() {
  // put your setup code here, to run once:

  //--------------------------------------------
  //Initialise the Serial monitor so that we can
  //see what's going on with the remote input.
  //--------------------------------------------
  
  //Serial.begin(9600);
  // In case the interrupt driver crashes on setup, give a clue
  // to the user what's going on.
  //Serial.println("Enabling IRin");
  irrecv.enableIRIn(); // Start the receiver
  //Serial.println("Enabled IRin");

  strip.begin() ;
  strip.show() ;
}


uint32_t color = 0x000000;      // Initial color (off)
uint8_t index = 0;              // Color Wheel Index
uint8_t bright = 5;             // Initial brightness
bool Auto = false;              // Auto Cycle Colors
uint8_t Mode = 0;               // Auto Cycle Mode

void loop() {
  // put your main code here, to run repeatedly:

  // Read the IR code received from the remote
  if (irrecv.decode(&results)) {
    switch(results.value)  // formatted to be less 'long'
      {
      // For each colour button, we check if we're already close to the target
      // colour wheel index.  If we're far away, we jump to the desired colour.
      // If we're close, we increment toward it.
      case 0x1FE40BF: //GREEN button press
        if (abs(index - 85) > 45 || abs(index - 85) < 10) {
          index = 85;
        }
        else if (index > 85) {
          index -= 10;
        }
        else {
          index += 10;
        }
        color = ColorWheel(index);
        Auto = false;
        SingleColor(color);
        break;
      case 0x1FE807F: //RED
        if (index > 245 || index < 10 || (index > 45 && index < 210)) {
          index = 0;
        }
        else if (index >= 10 && index <= 45) {
          index -= 10;
        }
        else {
          index += 10;
        }
        color = ColorWheel(index);
        Auto = false;
        SingleColor(color);
        break;
      case 0x1FEC03F: //BLUE
        if (abs(index - 170) > 45 || abs(index - 170) < 10) {
          index = 170;
        }
        else if (index > 170) {
          index -= 10;
        }
        else {
          index += 10;
        }
        color = ColorWheel(index);
        Auto = false;
        SingleColor(color);
        break;
      case 0x1FEE01F: //YELLOW
        if (abs(index - 43) > 43 || abs(index - 43) < 10) {
          index = 43;
        }
        else if (index > 43) {
          index -= 10;
        }
        else {
          
          index += 10;
        }
        color = ColorWheel(index);
        Auto = false;
        SingleColor(color);
        break;
      case 0x1FE906F: //PURPLE
        if (abs(index - 212) > 45 || abs(index - 212) < 10) {
          index = 212;
        }
        else if (index > 212) {
          index -= 10;
        }
        else {
          index += 10;
        }
        color = ColorWheel(index);
        Auto = false;
        SingleColor(color);
        break;
      case 0x1FE10EF: //CYAN
        if (abs(index - 127) > 45 || abs(index - 127) < 10) {
          index = 127;
        }
        else if (index > 127) {
          index -= 10;
        }
        else {
          index += 10;
        }
        color = ColorWheel(index);
        Auto = false;
        SingleColor(color);
        break;
      case 0x1FEB04F: //WHITE
        Auto = false;
        color = 0xA0A050; // Slightly warm white (less blue)
        SingleColor(color);
        break;
      case 0x1FE7887: //ON
        SingleColor(color);
        break;
      case 0x1FE48B7: //OFF
        Auto = false;
        SingleColor(0x000000);
        break;
      case 0x1FEF807: //BRIGHTER
        bright = bright + 1;
        if (bright > 7) bright = 7;
        if (!Auto) SingleColor(color);
        break;
      case 0x1FE50AF: //DIMMER
        bright = bright - 1;
        if (bright < 1) bright = 1;
        if (!Auto) SingleColor(color);
        break;
      case 0x1FE30CF: //AUTO button
        // If we weren't in auto mode already, set it with mode 0
        if (!Auto) {
          Auto = true;
          Mode = 0;
        }
        // If we're already in auto, increment the mode value
        //   Mode 0 = Cycle through the colour wheel with all pixels
        //            set to the same colour.
        //   Mode 1 = Colour Wheel chasing effect
        else {
          Mode++;
          if (Mode > 1) Mode = 0;
        }
        break;
      case 0x1FE708F: //MANUAL
        Auto = false;
        SingleColor(color);
        break;
      case 0xFFFFFFFF: //REPEAT
        break;
      default: //Everything Else
        break;
      } 
    //uint32_t rcvcode = results.value ;
    irrecv.resume(); // Receive the next value
    Serial.println(results.value, HEX);

  }
  if (Auto) {
    index++;
    if (index > 255) index = 0;
    switch(Mode) {
      case 1:
      // Color wheel moving down the LED strip
        for (int i=0; i<NUMPIXELS; i++) {
          uint8_t tempindex = index + i;
          if (tempindex > 255) tempindex -= 255;
          uint32_t tempColor = ColorWheel(tempindex);
          uint8_t SetBlue = tempColor;
          uint8_t SetGreen = tempColor>>8;
          uint8_t SetRed = tempColor>>16;
          strip.setPixelColor(i, map(bright, 0, 10, 0, SetRed), map(bright, 0, 10, 0, SetGreen), map(bright, 0, 10, 0, SetBlue));
        }
        strip.show();
        break;
      default:
        color = ColorWheel(index);
        SingleColor(color);
        break;
    }
  }
  delay (100);
}

void SingleColor(uint32_t SetColor) {
  uint8_t SetBlue = SetColor;
  uint8_t SetGreen = SetColor>>8;
  uint8_t SetRed = SetColor>>16;
  
  for (int i=0; i<NUMPIXELS; i++) {
      strip.setPixelColor(i, map(bright, 0, 10, 0, SetRed), map(bright, 0, 10, 0, SetGreen), map(bright, 0, 10, 0, SetBlue));
       // 'On' pixel at head
  }
  strip.show();
}
uint32_t ColorWheel(uint8_t WheelPos) {
  uint32_t ReturnColor = 0;
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    ReturnColor = (255 - WheelPos * 3);
    ReturnColor = ReturnColor<<16;
    ReturnColor = ReturnColor + (WheelPos * 3);
    return ReturnColor;
    //return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    ReturnColor = (WheelPos * 3);
    ReturnColor = ReturnColor<<8;
    ReturnColor = ReturnColor + (255 - WheelPos * 3);
    return ReturnColor;
    //return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  ReturnColor = (WheelPos * 3);
  ReturnColor = ReturnColor<<8;
  ReturnColor = ReturnColor + (255 - WheelPos * 3);
  ReturnColor = ReturnColor<<8;
  return ReturnColor;
  //return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
