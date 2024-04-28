#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/*********************************************
DISPLAY SETUP 
*********************************************/

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     32 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

enum Mode {
  TEMPO,
  LENGTH,
  CHANNEL
};
Mode currentMode = TEMPO;

//prototypes
void newBlackNumber(int val, int size, int x, int y);
void drawParamNames(String name, int size, int x, int y, byte black);

void setupGraphics(){
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  delay(100);
  display.clearDisplay();
}

void drawTempo(int tempo){
  // display.setTextSize(1);             // Normal 1:1 pixel scale
  // display.setTextColor(SSD1306_WHITE);     
  // display.setCursor(0,0);             // Start at top-left corner
  // display.println("Tempo: ");
  newBlackNumber(tempo, 1, 8, 0); //number, size, x,y
  display.display();
}

void drawCycleLength(int num){
  // display.setTextSize(1);             // Normal 1:1 pixel scale
  // display.setTextColor(SSD1306_WHITE);     
  // display.setCursor(0,1*8);             // Start at top-left corner
  // display.println("Length: ");
  newBlackNumber(num, 1, 8, 1); //number, size, x,y
  //display.display();
}

void drawChannel(int num){
  // display.setTextSize(1);             // Normal 1:1 pixel scale
  // display.setTextColor(SSD1306_WHITE);     
  // display.setCursor(0,2*8);             // Start at top-left corner
  // display.println("Chan: ");
  newBlackNumber(num, 1, 8, 2); //number, size, x,y
  //display.display();
}

void newBlackNumber(int val, int size, int x, int y){
  display.fillRect(x*5,y*8, 30*size*2, 8*size, SSD1306_BLACK);
  display.setTextSize(size);             // Normal 1:1 pixel scale
  display.setCursor(x*5,y*8);             // Start at top-left corner
  display.print(String(val));
  display.display();
}

void drawBeat(int beat){
  display.setCursor(64,16);             // Start at top-left corner
  display.setTextSize(3); 
  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
  display.print(String(beat));
  display.display();
}

void updateParamNames(){
  drawParamNames("tempo", 1, 0, 0, currentMode == TEMPO); //name, size, x,y
  drawParamNames("length", 1, 0, 1, currentMode == LENGTH);
  drawParamNames("chan", 1, 0, 2, currentMode == CHANNEL);
  display.display();
}

void drawParamNames(String name, int size, int x, int y, byte black){
  display.setCursor(x*5,y*8);             // Start at top-left corner
  display.setTextSize(size); 
  if( black == 1) display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
  else display.setTextColor(SSD1306_WHITE, SSD1306_BLACK); // Draw 'inverse' text
  display.print(name);
}