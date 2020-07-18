#include <TFTScreen.h>
#include <Fonts/Anims1.h>

#define DARK_BLUE 627
#define LIGHT_BLUE 1599
#define win10Col 986
#define BLACK 0x0000
#define WHITE 0xFFFF
#define RED 63488
#define VS_PURPLE 22898
#define CRM_YEL 65248

#define MINPRESSURE 20
#define MAXPRESSURE 1000

TFTScreen myScreen;

void setup(void)
{
  Serial.begin(9600);
  myScreen.begin();
  myScreen.reset();

  myScreen.begin();
  myScreen.setRotation(1);
  myScreen.fillScreen(BLACK);
  myScreen.setFont(&Anims1);
  myScreen.setTextSize(1);
  myScreen.setTextColor(RED);
  myScreen.setCursor(0, 16);
  myScreen.println("bbbbbbbbbbbbbbbbbbbbbbbbb");
  myScreen.println("bcccccccccccccccccccccccb");
  myScreen.println("bcccccccccccccccccccccccb");
  myScreen.println("bcccccccccccccccccccccccb");
  myScreen.println("bcccccccccccccccccccccccb");
  myScreen.println("bcccccccccccccccccccccccb");
  myScreen.println("bcccccccccccccccccccccccb");
  myScreen.println("bcccccccccccccccccccccccb");
  myScreen.println("bcccccccccccccccccccccccb");
  myScreen.println("bcccccccccccccccccccccccb");
  myScreen.println("bcccccccccccccccccccccccb");
  myScreen.println("bcccccccccccccccccccccccb");
  myScreen.println("bcccccccccccccccccccccccb");
  myScreen.println("bcccccccccccccccccccccccb");
  myScreen.println("bbbbbbbbbbbbbbbbbbbbbbbbb");

   myScreen.setTextColor(LIGHT_BLUE);
  myScreen.setCursor(48, 48);
  myScreen.println("a");

//  myScreen.setTextSize(2);
//  myScreen.setTextColor(LIGHT_BLUE);
//  myScreen.setCursor(48, 48);
//  myScreen.println("a");

  pinMode(12, OUTPUT); //Buzzer pin

  delay(500);
}


void loop()
{
  
}



