#ifndef Game_h
#define Game_h

#include <Arduino.h>
#include <TFTScreen.h>
#include <TouchScreen.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_GFX.h>
#include <Fonts/Anims1.h>

#define MINPRESSURE 20
#define MAXPRESSURE 1000
#define SWAP(a, b) {uint16_t tmp = a; a = b; b = tmp;}

#define TS_LEFT 180
#define TS_BOT 170
#define TS_RT  950
#define TS_TOP 880

#define xLength 21
#define yLength 14
#define SD_CS 5

//Assign human-readable names to some common 16-bit color values:
#define BLACK			0
#define BLUE			31
#define RED				63488
#define GREEN			2016
#define CYAN			2047
#define MAGENTA			63519
#define YELLOW			65504
#define WHITE			65535
#define NAVY            15
#define DARKGREEN       992
#define DARKCYAN        1007
#define MAROON          30720
#define PURPLE          30735
#define OLIVE           31712
#define LIGHTGREY       50712
#define DARKGREY        31727
#define ORANGE          64800
#define GREENYELLOW     45029
#define PINK            63519

#define BROKEN 64512

enum GameState { Menu, Creator, World, CharacterScreen, Combat };

class cBtn
{
	public:
		cBtn(byte pin);
		bool UpdatePhysicalBtn();
		byte buttonPin;
		bool buttonState;
		bool lastBtnState;
		unsigned long lastDebounceTime = 0;
		unsigned long debounceDelay = 50;
};

class Explosion
{
	public:
		Explosion();
		void Init(TFTScreen *screen, byte xPos, byte yPos, char uChar, uint16_t uColor);		
		void Update(unsigned long elapsed);
		void Draw();
		void Undraw();

	private:
		TFTScreen * _screen;
		bool _active = false;

		char _curChar;

		//Character underneath the explosion
		char _uChar;
		//Color of the tile underneath the explosion
		uint16_t _uColor;

		uint16_t _curTime, _maxTime;
		byte _x, _y;
};

class Player
{
	public:
		Player(void);

		//Position
		byte x, prevX;
		byte y, prevY;

		//Strength, Perception, Endurance, Charisma, Intelligence, Agility, Luck
		byte STR, PER, END, CHA, INT, AGI, LUC;

		//http://fallout.wikia.com/wiki/Fallout_2_derived_statistics
		byte Armor, CritChance, DResist, HealRate, MeleeD, PerkRate, RadResist, Sequence, SkillRate;

		uint16_t money;

		byte MaxHP, CurHP;
		byte MaxAP, CurAP; //Action points
		byte Level;
};

class Enemy
{
	public:
		Enemy(void);
		byte x, y;
		byte STR, DEX;
		byte MaxHP, CurHP;
		byte MaxAP, CurAP;
		char Style;
		uint16_t Color;
		bool Active, Targeted;
};

class NPC
{
	public:
		NPC(void);
		byte x, y;
};

class Item
{
	public:
		Item(void);
		byte x, y;
};

class MeleeWeapon
{
	public:
		byte Damage;
};

class Gun
{
	public:
		byte ReloadAP, Range, Damage, APCost, Magazine;
};

class Tile
{
	public:
		Tile(void);
		byte x, y;
		char style; //The character used to draw the tile from the font
		uint16_t color; //The colour this tile should be when drawn on screen
};

class Game
{
	public:
		Game();

		void Init();
		void Loop();
		void MovePlayer();
		void LoadChunk(uint16_t xStart, uint16_t yStart);
		void LoadEnemies(uint16_t xStart, uint16_t yStart);
		void CopyFile(String sourceFile, String destFile);
		void DrawScreen();
		void DrawEnemies();
		void CheckTouchScreen();

		unsigned long Time;
		unsigned long ElaspedTime;

		int data;

		TFTScreen tft;
		TouchScreen ts = TouchScreen(6, A1, A2, 7, 300);
		TSPoint tp;

		String sScreen;

		File myFile;

		byte xChunk, yChunk;

		GameState CurrentGameState;
		Player player;

		Tile tScreen[15][xLength];
		Enemy enemies[10];
		Item items[10];
		NPC npcs[10];
		Explosion explosion;

		cBtn topBtn = cBtn(28);
		cBtn leftBtn = cBtn(26);
		cBtn rightBtn = cBtn(30);
		cBtn downBtn = cBtn(32);

		cBtn aButton = cBtn(34);
		cBtn bButton = cBtn(36);
	private:
};

#endif