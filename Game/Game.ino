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

/* some RGB color definitions                                                 */
#define Navy            0x000F      /*   0,   0, 128 */
#define DarkGreen       0x03E0      /*   0, 128,   0 */
#define DarkCyan        0x03EF      /*   0, 128, 128 */
#define Maroon          0x7800      /* 128,   0,   0 */
#define Purple          0x780F      /* 128,   0, 128 */
#define Olive           0x7BE0      /* 128, 128,   0 */
#define LightGrey       0xC618      /* 192, 192, 192 */
#define DarkGrey        0x7BEF      /* 128, 128, 128 */
#define Orange          0xFD20      /* 255, 165,   0 */
#define GreenYellow     0xAFE5      /* 173, 255,  47 */
#define Pink            0xF81F


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
cBtn leftBtn(26);
cBtn rightBtn(30);
cBtn downBtn(32);

TFTButton UpButton;


class Player
{
  public:
	  //Position
	  uint16_t x, prevX;
	  byte y, prevY;
  
	  //Stats
	  byte STR, DEX, CON, INT, WIS, CHA;

	  uint16_t money;

	  byte MaxHP, CurHP;

	  byte Level;
};

//Enemy for the player to fight
class Enemy
{
	public:
		uint8_t x;
    byte y;
		byte STR, DEX;
};

//Non-enemy character for the player to interact with. e.g. Shopkeeper, quest giver etc.
class NPC
{
	public:
		uint8_t x;
    byte y;
};

//Items for the player to pick up
class Item
{
	public:
		uint8_t x;
    byte y;
};

class Tile
{
  public:
    uint8_t x;
    byte y;
    char style; //The character used to draw the tile from the font
    uint16_t color; //The colour this tile should be when drawn on screen
};

Tile tScreen [15][26];
Enemy enemies[10];
Item items[10];
NPC npcs[10];

Player player;

enum GameState { World, CharacterScreen, Combat };
GameState CurrentGameState;

uint16_t tmp;


void setup(void)
{
    CurrentGameState = World;
    tft.begin(9600);    
    Serial.begin(9600);
    tft.reset();
    identifier = tft.readID();

    pinMode(53, OUTPUT);
    digitalWrite(53, HIGH);

    pinMode(topBtn.buttonPin, INPUT);
    pinMode(leftBtn.buttonPin, INPUT);
    pinMode(rightBtn.buttonPin, INPUT);
    pinMode(downBtn.buttonPin, INPUT);
    
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

    player.prevX = 5;
    player.prevY = 5;

    String str1 =  
    "bbbbbbbbbbbbggggggggggggg"
    "bffffffffffbggggggggggggg"
    "bddddddddddbgggggggggcggg"
    "bddddddddddbgggcggggggggg"
    "bddddddddddbgggcgggggggcc"
    "bddddddddddbcggggggggcccc"
    "bddddddddddbgggggggggcccc"
    "bddddddddddbbbbbbbgbbbbbb"
    "bbbbdbbbdddfffffffdfffffb"
    "bfffdffbddddddddddddddddb"
    "bddddddbddddddhdddddddddb"
    "bddddddbdedddddiddddddddb"
    "bddddddbddddhddhddddddddb"
    "bddddddbddddddddddddddddb"
    "bbbbbbbbbbbbbbbbbbbbbbbbb";
    
    for (int y = 0; y < 15; y++)
    {
      for (int x = 0; x < 25; x++)
      {
        Tile newTile;
        newTile.x = x;
        newTile.y = y;

        newTile.style = str1[(25*y)+x];

        switch (newTile.style)
        {
            default:
              newTile.color = GREEN;
            break;
          
            case 'b':
            case 'f':
              newTile.color = YELLOW;
              break;
            
            case 'd':
              newTile.color = Pink;
              break;
              
            case 'e':
              newTile.color = WHITE;
              break;

            case 'g':
              newTile.color = DarkGreen;
              break;

            case 'h':
              newTile.color = GetColor(150, 150, 150);
              break;
        }
        
        tScreen[y][x] = newTile;

        tft.setTextColor(newTile.color);
        tft.print(newTile.style);
      }
    }

  	tft.setCursor(player.x * 16, player.y * 16 + 16);
  	tft.print('a');

    UpButton.InitButton(&tft, 0, 0, 64, 64, WHITE, BLACK, BLACK, "");

    tft.fillRect(240, 0, 160, 240, BLACK);
    
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
        {
          //MOVE UP
          if (UpdatePhysicalBtn(topBtn) == true)
          {
              if (tScreen[player.y-1][player.x].style != 'b')
              {
                player.prevX = player.x;
                player.prevY = player.y;
                
                player.y -= 1;
        
                tft.fillRect(player.prevX * 16, player.prevY * 16, 16, 16, BLACK);
                
                tft.setTextColor(RED);
                tft.setCursor(player.x * 16, player.y * 16 + 16);
                tft.print('a');
        
                tft.setTextColor(tScreen[player.prevY][player.prevX].color);        
                tft.setCursor(player.prevX * 16, player.prevY * 16 + 16);
                tft.print(tScreen[player.prevY][player.prevX].style);
              }
          }

          //MOVE DOWN
          if (UpdatePhysicalBtn(downBtn) == true)
          {              
              if (tScreen[player.y+1][player.x].style != 'b')
              {
                player.prevX = player.x;
                player.prevY = player.y;
                                
                player.y += 1;
        
                tft.fillRect(player.prevX * 16, player.prevY * 16, 16, 16, BLACK);
                
                tft.setTextColor(RED);
                tft.setCursor(player.x * 16, player.y * 16 + 16);
                tft.print('a');
        
                tft.setTextColor(tScreen[player.prevY][player.prevX].color);        
                tft.setCursor(player.prevX * 16, player.prevY * 16 + 16);
                tft.print(tScreen[player.prevY][player.prevX].style);
              }
          }

          //MOVE LEFT
          if (UpdatePhysicalBtn(leftBtn) == true)
          {            
              if (tScreen[player.y][player.x-1].style != 'b')
              {
                player.prevX = player.x;
                player.prevY = player.y;
                
                player.x -= 1;
        
                tft.fillRect(player.prevX * 16, player.prevY * 16, 16, 16, BLACK);
                
                tft.setTextColor(RED);                
                tft.setCursor(player.x * 16, player.y * 16 + 16);
                tft.print('a');
        
                tft.setTextColor(tScreen[player.prevY][player.prevX].color);        
                tft.setCursor(player.prevX * 16, player.prevY * 16 + 16);
                tft.print(tScreen[player.prevY][player.prevX].style);
              }
          }

          //MOVE RIGHT
          if (UpdatePhysicalBtn(rightBtn) == true)
          {
              if (tScreen[player.y][player.x+1].style != 'b')
              {
                player.prevX = player.x;
                player.prevY = player.y;
                
                player.x += 1;
        
                tft.fillRect(player.prevX * 16, player.prevY * 16, 16, 16, BLACK);
                
                tft.setTextColor(RED);
                tft.setCursor(player.x * 16, player.y * 16 + 16);
                tft.print('a');
        
                tft.setTextColor(tScreen[player.prevY][player.prevX].color);        
                tft.setCursor(player.prevX * 16, player.prevY * 16 + 16);
                tft.print(tScreen[player.prevY][player.prevX].style);
              }
          }
        
//          if (UpdateTFTBtn(UpButton) == true)
//          {
//          }
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

//This function will need to redraw the game screen again after the player has opened a menu and then returned to the game
//or has transitioned out of combat back to the main game
void RedrawGame()
{
  
}


