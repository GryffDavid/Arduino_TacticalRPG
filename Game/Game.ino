#include <Adafruit_GFX.h>
#include <TFTScreen.h>
TFTScreen tft;
#include <TouchScreen.h>
#include <Fonts/Anims1.h>

uint8_t YP = A1;
uint8_t XM = A2;
uint8_t YM = 7;
uint8_t XP = 6;
uint8_t SwapXY = 0;

uint16_t TS_LEFT = 880;
uint16_t TS_RT  = 170;
uint16_t TS_TOP = 950;
uint16_t TS_BOT = 180;

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
TSPoint tp;

#define MINPRESSURE 20
#define MAXPRESSURE 1000

#define SWAP(a, b) {uint16_t tmp = a; a = b; b = tmp;}

int16_t BOXSIZE;
int16_t PENRADIUS = 3;
uint16_t identifier, oldcolor, currentcolor;
uint8_t Orientation = 1;

// Assign human-readable names to some common 16-bit color values:
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

char arx [15][26];
int pX, pY; //Player X and Y
int pvX, pvY; //Previous X and Y

TFTButton UpButton;

void setup(void)
{
    uint16_t tmp;
    tft.begin(9600);    
    Serial.begin(9600);
    tft.reset();
    identifier = tft.readID();

    pinMode(28, INPUT);
    pinMode(30, INPUT);
    pinMode(32, INPUT);
    
     switch (Orientation) 
    {
        case 0:  
        break;
          
        case 1:   
            tmp = TS_LEFT, 
            TS_LEFT = TS_BOT, 
            TS_BOT = TS_RT, 
            TS_RT = TS_TOP, 
            TS_TOP = tmp;  
        break;
          
        case 2:   
          SWAP(TS_LEFT, TS_RT); 
          SWAP(TS_TOP, TS_BOT); 
        break;
        
        case 3:   
          tmp = TS_LEFT, 
          TS_LEFT = TS_TOP, 
          TS_TOP = TS_RT, 
          TS_RT = TS_BOT, 
          TS_BOT = tmp;  
        break;
    }

    ts = TouchScreen(XP, YP, XM, YM, 300);     //call the constructor AGAIN with new values.
    tft.begin(identifier);
    tft.setRotation(Orientation);
    tft.fillScreen(BLACK);    
    tft.setCursor(0, 10);
    tft.setFont(&Anims1);
    
    pX = 5;
    pY = 5;

    String str1 = "bbbbbbbbbbbbbbbbbbbbbbbbb";
    String str2 = "bdddddddddddddddddddddddb";
    String str3 = "bdddddddddddddddddddddddb";
    String str4 = "bdddddddddddddddddddddddb";
    String str5 = "bddddddcddddddddcdddddddb";
    String str6 = "bdddddddddddddddddddddddb";
    String str7 = "bddddddddddddcddddddddddb";
    String str8 = "bdddddddcdddddddddddddddb";
    String str9 = "bdddddddcdddddddddddddddb";
    String str10 = "bdddddddddddddddddddddddb";
    String str11 = "bdddddddddddddddddddddddb";
    String str12 = "bdddddddddddddddddddddddb";        
    String str13 = "bdddddddddddddddddddddddb";
    String str14 = "bdddddddddddddddddddddddb";
    String str15 = "bbbbbbbbbbbbbbbbbbbbbbbbb";
    
    str1.toCharArray(arx[0], 26);
    str2.toCharArray(arx[1], 26);
    str3.toCharArray(arx[2], 26);
    str4.toCharArray(arx[3], 26);
    str5.toCharArray(arx[4], 26);
    str6.toCharArray(arx[5], 26);
    str7.toCharArray(arx[6], 26);
    str8.toCharArray(arx[7], 26);
    str9.toCharArray(arx[8], 26);
    str10.toCharArray(arx[9], 26);
    str11.toCharArray(arx[10], 26);
    str12.toCharArray(arx[11], 26);    
    str13.toCharArray(arx[12], 26);
    str14.toCharArray(arx[13], 26);
    str15.toCharArray(arx[14], 26);
    
    for (int y = 0; y < 14; y++)
    {
      Serial.println(arx[y]);
    }

    String foo = "";
    
    for (int y = 0; y < 15; y++)
    {
      for (int x = 0; x < 26; x++)
      { 
        foo.concat(arx[y][x]);     
      }
    }

    UpButton.InitButton(&tft, 0, 0, 64, 64, WHITE, BLACK, BLACK, "");
    //UpButton.DrawButton();
    
    tft.print(foo);
    
    delay(1000);
}



void loop()
{
    uint16_t xpos, ypos;
    tp = ts.getPoint();

    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
    pinMode(XP, OUTPUT);
    pinMode(YM, OUTPUT);

    pvX = pX;
    pvY = pY;

    if (UpdateTFTBtn(UpButton) == true)
    {
        
        pX += 1;
        tft.fillRect((pvX*16), (pvY*16)-16, 16, 16, BLACK);
        tft.setCursor(pX*16, pY*16);
        tft.setTextColor(RED);
        tft.print('a');

        tft.setCursor((pvX*16), (pvY*16));
        tft.setTextColor(RED);
        tft.print(arx[pvX][pvY]);
    }

    if (tp.z > MINPRESSURE && tp.z < MAXPRESSURE) 
    {      
      if (SwapXY != (Orientation & 1)) 
          SWAP(tp.x, tp.y);
            
      xpos = map(tp.x, TS_LEFT, TS_RT, 0, tft.width());
      ypos = map(tp.y, TS_TOP, TS_BOT, 0, tft.height());

      //tft.fillCircle(xpos, ypos, 3, RED);

      UpButton.CheckButton(xpos, ypos);
    }
    else
    {
      UpButton.CheckButton(-1, -1);
    }
}


bool UpdateTFTBtn(TFTButton &tftBtn)
{
  int reading = tftBtn.pressed;
  bool result = false;

  if (reading != tftBtn.lastButtonState) 
  {
    tftBtn.lastDebounceTime = millis();
  }

  if ((millis() - tftBtn.lastDebounceTime) > tftBtn.debounceDelay && reading != tftBtn.buttonState)
  {  
    tftBtn.buttonState = reading;
    
    if (tftBtn.buttonState == LOW) 
    {
      result = true;
    }
  }
  
  tftBtn.lastButtonState = reading;
  return result;
}

