#include <Adafruit_GFX.h>
#include <TFTScreen.h>
#include <TouchScreen.h>
#include <Fonts/Anims1.h>
#include <Game.h>
#include <SPI.h>
#include <SD.h>

TFTScreen tft;

TouchScreen ts = TouchScreen(6, A1, A2, 7, 300);
TSPoint tp;

uint16_t TS_LEFT = 180;
uint16_t TS_BOT = 170;
uint16_t TS_RT = 950;
uint16_t TS_TOP = 880;

#define MINPRESSURE 20
#define MAXPRESSURE 1000

#define SWAP(a, b) {uint16_t tmp = a; a = b; b = tmp;}

uint16_t identifier;
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


//Physical buttons
//cBtn topBtn(28);
//cBtn leftBtn(26);
//cBtn rightBtn(30);
//cBtn downBtn(32);

//TFTButton UpButton;

#define SD_CS 5
#define xLength 16

String sScreen;

Game Game;

uint16_t xChunk = 0;
uint16_t yChunk = 0;

Tile tScreen [15][xLength];
Enemy enemies[10];
Item items[10];
NPC npcs[10];

File myFile;

void setup(void)
{
    Serial.begin(9600);
    randomSeed(analogRead(6));
    
    identifier = tft.readID();
    tft.begin(identifier);
    tft.setRotation(Orientation);
    tft.fillScreen(BLACK);  

    Game.CurrentGameState = World;
    
    bool SDCard = SD.begin(SD_CS);

    if (!SDCard)
    {
        tft.println("SD failed to initialise");
        Serial.println("SD failed to initialise");
        while(1);
    }
    
    tft.fillRect(240, 0, 160, 240, BLACK);
    tft.setCursor(16*15+16, 16);
    tft.println("HP: 10/10");

    LoadChunk(xChunk, yChunk);
    DrawScreen();
    
    //This pin is used to power the buttons circuit
    pinMode(53, OUTPUT);
    digitalWrite(53, HIGH);


  	tft.setCursor(16, 32);
  	tft.print('a');

    //UpButton.InitButton(&tft, 0, 0, 64, 64, WHITE, BLACK, BLACK, "");
    
    delay(1000);
}

void loop()
{
    uint16_t xpos, ypos;
    tp = ts.getPoint();

    pinMode(A2, OUTPUT);
    pinMode(A1, OUTPUT);
    pinMode(6, OUTPUT);
    pinMode(7, OUTPUT);

    Game.Loop();

    switch (Game.CurrentGameState)
    {
        case World:
        {
            MovePlayer();
        
//          if (UpdateTFTBtn(UpButton) == true)
//          {
//          }
        }
        break;
    }

	  //Player touched the screen
    if (tp.z > MINPRESSURE && tp.z < MAXPRESSURE) 
    {
      SWAP(tp.x, tp.y);
            
      xpos = map(tp.x, TS_LEFT, TS_RT, 0, tft.width());
      ypos = map(tp.y, TS_TOP, TS_BOT, 0, tft.height());

      switch (Game.CurrentGameState)
      {
          case World:
            //UpButton.CheckButton(xpos, ypos);
          break;
      }      
    }
    else
    {
      switch (Game.CurrentGameState)
      {
          case World:
            //UpButton.CheckButton(-1, -1);
          break;
      }
    }
}


void MovePlayer()
{
  //MOVE UP
  if (Game.topBtn.UpdatePhysicalBtn() == true)
  {            
      if (player.y == 0)
      {
          player.y = 14;
          yChunk -= 1;
          LoadChunk(xChunk, yChunk);
          DrawScreen();

          tft.setTextColor(RED);
          tft.setCursor(player.x * 16, player.y * 16 + 16);
          tft.print('a');
          return;
      }
    
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
  if (Game.downBtn.UpdatePhysicalBtn() == true)
  {
      if (player.y >= 14)
      {
          player.y = 0;
          yChunk += 1;
          LoadChunk(xChunk, yChunk);
          DrawScreen();

          tft.setTextColor(RED);
          tft.setCursor(player.x * 16, player.y * 16 + 16);
          tft.print('a');
          return;
      }
    
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
  if (Game.leftBtn.UpdatePhysicalBtn() == true)
  {            
      if (player.x == 0)
      {
          player.x = 14;
          xChunk -= 1;
          LoadChunk(xChunk, yChunk);
          DrawScreen();      

          tft.setTextColor(RED);
          tft.setCursor(player.x * 16, player.y * 16 + 16);
          tft.print('a');
          return;
      }
      
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
  if (Game.rightBtn.UpdatePhysicalBtn() == true)
  {
      if (player.x >= 14)
      {
          player.x = 0;
          xChunk += 1;
          LoadChunk(xChunk, yChunk);
          DrawScreen();  

          tft.setTextColor(RED);
          tft.setCursor(player.x * 16, player.y * 16 + 16);
          tft.print('a');
          return;
      }
      
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
}

void LoadChunk(uint16_t xStart, uint16_t yStart)
{
    xStart *= 15;
    yStart *= 15;
    
    sScreen = "";
        
    myFile = SD.open("Level.txt");
    if (myFile) 
    {        
        if (myFile.available()) 
        {
          for (int y = yStart; y < yStart+15; y++)
          {
            char buf[16];
            myFile.seek(((151*y+y)) + xStart);        
            myFile.readBytes(buf, 15);
            sScreen += buf;
          }
        }
        
        sScreen.replace('', '\n');        
        myFile.close();
    } 
    else 
    {      
      Serial.println("error opening Level.txt");
    }
    
    tft.fillRect(0, 0, 16*15, 16*15, BLACK);
}

void DrawScreen()
{
    tft.setFont(&Anims1);
    tft.setCursor(0, 16);
   
    String str1 = sScreen;
  
    for (int y = 0; y < 15; y++)
    {
      for (int x = 0; x < xLength; x++)
      {
        Tile newTile;
        newTile.x = x;
        newTile.y = y;

        newTile.style = str1[(xLength*y)+x];

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
              newTile.color = tft.GetColor(150, 150, 150);
              break;
        }
        
        tScreen[y][x] = newTile;

        tft.setTextColor(newTile.color);
        tft.print(newTile.style);        
      }
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

//bool UpdatePhysicalBtn(cBtn &button)
//{
//	int reading = digitalRead(button.buttonPin);
//	bool result = false;
//
//	if (reading != button.lastBtnState)
//	{
//		button.lastDebounceTime = millis();
//	}
//
//	if ((millis() - button.lastDebounceTime) > button.debounceDelay && reading != button.buttonState)
//	{
//		button.buttonState = reading;
//		if (button.buttonState == HIGH)
//		{
//			result = true;
//		}
//	}
//
//	button.lastBtnState = reading;
//	return result;
//}

