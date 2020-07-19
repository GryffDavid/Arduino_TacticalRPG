#include <Adafruit_GFX.h>
#include <TFTScreen.h>
#include <TouchScreen.h>
#include <Fonts/Anims1.h>

TFTScreen tft;

uint8_t YP = A1;
uint8_t XM = A2;
uint8_t YM = 7;
uint8_t XP = 6;

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
TSPoint tp;

uint8_t SwapXY = 0;

uint16_t TS_LEFT = 880;
uint16_t TS_RT  = 170;
uint16_t TS_TOP = 950;
uint16_t TS_BOT = 180;



#define MINPRESSURE 20
#define MAXPRESSURE 1000

#define SWAP(a, b) {uint16_t tmp = a; a = b; b = tmp;}

int16_t BOXSIZE;
int16_t PENRADIUS = 3;
uint16_t identifier, oldcolor, currentcolor;
uint8_t Orientation = 1;

//Assign human-readable names to some common 16-bit color values:
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define GREY 4359


class cBtn
{
public:
	cBtn(byte pin) { buttonPin = pin; }

	byte buttonPin;
	bool buttonState;
	bool lastBtnState;
	unsigned long lastDebounceTime = 0;
	unsigned long debounceDelay = 50;
};

//Physical buttons
cBtn topBtn(28);

TFTButton UpButton;


class Player
{
  public:
	  //Position
	  int x, y;
	  int prevX, prevY;
  
	  //Stats
	  int STR, DEX, CON, INT, WIS, CHA;

	  int money;

	  int MaxHP, CurHP;

	  int Level;
};

//Enemy for the player to fight
class Enemy
{
	public:
		int x, y;
		int STR, DEX;
};

//Non-enemy character for the player to interact with. e.g. Shopkeeper, quest giver etc.
class NPC
{
	public:
		int x, y;
};

//Items for the player to pick up
class Item
{
	public:
		int x, y;
};

class Tile
{
  public:
    uint8_t x;
    byte y;
    char style; //The character used to draw the tile from the font
    uint16_t color; //The colour this tile should be when drawn on screen
};

char screen [15][26];
Tile tScreen [15][26];

Player player;

enum GameState { World, CharacterScreen, Combat };
GameState CurrentGameState;


void setup(void)
{
    CurrentGameState = World;
    uint16_t tmp;
    tft.begin(9600);    
    Serial.begin(9600);
    tft.reset();
    identifier = tft.readID();

    pinMode(topBtn.buttonPin, INPUT);
    
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

    player.x = 5;
    player.y = 5;

    String str1 =  "bbbbbbbbbbbbbbbbbbbbbbbbb";
    String str2 =  "bddddddddddbddddddddddddb";
    String str3 =  "bddddddddddbddddddddddddb";
    String str4 =  "bddddddddddbddddddddddddb";
    String str5 =  "bddddddcdddbddddcdddddddb";
    String str6 =  "bddddddddddbddddddddddddb";
    String str7 =  "bddddddddddddcddddddddddb";
    String str8 =  "bdddddddcdddddddddddddddb";
    String str9 =  "bdddddddcdddddddddddddddb";
    String str10 = "bdddddddddddddddddddddddb";
    String str11 = "bdddddddddddddddddddddddb";
    String str12 = "bddddddddeddddddddddddddb";        
    String str13 = "bdddddddddddddddddddddddb";
    String str14 = "bdddddddddddddddddddddddb";
    String str15 = "bbbbbbbbbbbbbbbbbbbbbbbbb";
    
    str1.toCharArray(screen[0], 26);
    str2.toCharArray(screen[1], 26);
    str3.toCharArray(screen[2], 26);
    str4.toCharArray(screen[3], 26);
    str5.toCharArray(screen[4], 26);
    str6.toCharArray(screen[5], 26);
    str7.toCharArray(screen[6], 26);
    str8.toCharArray(screen[7], 26);
    str9.toCharArray(screen[8], 26);
    str10.toCharArray(screen[9], 26);
    str11.toCharArray(screen[10], 26);
    str12.toCharArray(screen[11], 26);    
    str13.toCharArray(screen[12], 26);
    str14.toCharArray(screen[13], 26);
    str15.toCharArray(screen[14], 26);
    
    String foo = "";

    screen[5][5] = 'c';
    
    for (int y = 0; y < 15; y++)
    {
      for (int x = 0; x < 26; x++)
      { 
        foo.concat(screen[y][x]);
        Tile newTile;
        newTile.x = x;
        newTile.y = y;
        newTile.style = screen[y][x];
        
        if (newTile.style == 'b')
            newTile.color = GetColor(100, 100, 100);
        else if (newTile.style == 'e')
            newTile.color = WHITE;
          else
            newTile.color = GREEN;
        
        tScreen[y][x] = newTile;
      }
    }

  for (int y = 0; y < 15; y++)
  {
    for (int x = 0; x < 26; x++)
    {
      tft.setTextColor(tScreen[y][x].color);
      tft.print(tScreen[y][x].style);
    }
  }

	//tft.print(foo);

	tft.setCursor(player.x * 16, player.y * 16 + 16);
	tft.print('a');

    UpButton.InitButton(&tft, 0, 0, 64, 64, WHITE, BLACK, BLACK, "");
    
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

    switch (CurrentGameState)
    {
        case World:

          if (UpdatePhysicalBtn(topBtn) == true)
          {
              player.prevX = player.x;
              player.prevY = player.y;

              //TODO: Should check to see if the player can/has moved before drawing things.              
              if (screen[player.y-1][player.x] != 'b')
              {
                //Move the player
                player.y -= 1;
        
                //Draw a black square over where the player WAS
                tft.fillRect(player.prevX * 16, player.prevY * 16, 16, 16, BLACK);
                tft.setTextColor(RED);
                //Draw the player sprite at the new location
                tft.setCursor(player.x * 16, player.y * 16 + 16);
                tft.print('a');
        
                //Redraw the tile where the player was standing (e.g. Grass), so that there isn't a black space left
                tft.setTextColor(tScreen[player.prevY][player.prevX].color);        
                tft.setCursor(player.prevX * 16, player.prevY * 16 + 16);
                tft.print(screen[player.prevY][player.prevX]);
              }
          }
        
        
          if (UpdateTFTBtn(UpButton) == true)
          {
              player.prevX = player.x;
              player.prevY = player.y;

              //TODO: Should check to see if the player can/has moved before drawing things.              
              if (screen[player.y+1][player.x] != 'b')
              {
                //Move the player
                player.y += 1;
        
                //Draw a black square over where the player WAS
                tft.fillRect(player.prevX * 16, player.prevY * 16, 16, 16, BLACK);
                tft.setTextColor(RED);
                //Draw the player sprite at the new location
                tft.setCursor(player.x * 16, player.y * 16 + 16);
                tft.print('a');
        
                //Redraw the tile where the player was standing (e.g. Grass), so that there isn't a black space left
                tft.setTextColor(tScreen[player.prevY][player.prevX].color);  
                tft.setCursor(player.prevX * 16, player.prevY * 16 + 16);
                tft.print(screen[player.prevY][player.prevX]);
              }
          }
          break;
    }

	//Player touched the screen
    if (tp.z > MINPRESSURE && tp.z < MAXPRESSURE) 
    {      
      if (SwapXY != (Orientation & 1)) 
          SWAP(tp.x, tp.y);
            
      xpos = map(tp.x, TS_LEFT, TS_RT, 0, tft.width());
      ypos = map(tp.y, TS_TOP, TS_BOT, 0, tft.height());

      switch (CurrentGameState)
      {
          case World:
            UpButton.CheckButton(xpos, ypos);
          break;
      }      
    }
    else
    {
      switch (CurrentGameState)
      {
          case World:
            UpButton.CheckButton(-1, -1);
          break;
      }
    }
}

//Update touch button
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

bool UpdatePhysicalBtn(cBtn &button)
{
	int reading = digitalRead(button.buttonPin);
	bool result = false;

	if (reading != button.lastBtnState)
	{
		button.lastDebounceTime = millis();
	}

	if ((millis() - button.lastDebounceTime) > button.debounceDelay && reading != button.buttonState)
	{
		button.buttonState = reading;
		if (button.buttonState == LOW)
		{
			result = true;
		}
	}

	button.lastBtnState = reading;
	return result;
}

uint16_t GetColor(uint8_t r, uint8_t g, uint8_t b) 
{
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}


