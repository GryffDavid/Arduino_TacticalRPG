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

enum GameState { Menu, Creator, World, CharacterScreen, VATS };
enum PlayerState { Normal, SelectMode, Combat };

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

class Game;

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
		char Style; //The character to represent this weapon in menus etc
};

class Selector
{
	public:
		Selector();
		byte _x, _y;
		bool _active = false;
		void GetGame(Game *game) { _game = game; };
		void GetScreen(TFTScreen *screen) { _screen = screen; };
		void Init(byte xPos, byte yPos, char uChar, uint16_t uColor);
		void Update(unsigned long elapsed);
		void Draw();
		void Undraw();
		void Move(int xpos, int ypos, char uChar, uint16_t uColor);

	private:
		Game * _game;
		TFTScreen * _screen;	
		uint16_t _curTime, _maxTime;
		uint16_t _color, _uColor;
		char _uChar;	
		bool _flash = false; //Whether the selector is drawn or the character underneath
};

class Explosion
{
	public:
		Explosion();
		void GetGame(Game *game) { _game = game; };
		void GetScreen(TFTScreen *screen) {	_screen = screen; };
		void Init(byte xPos, byte yPos, char uChar, uint16_t uColor);		
		void Update(unsigned long elapsed);
		void Draw();
		void Undraw();		

	private:
		Game * _game;
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

class Enemy
{
	public:
		Enemy(void);
		void EndTurn();
		void StartTurn();
		byte x, y;
		byte STR, DEX;
		byte MaxHP, CurHP;
		byte MaxAP, CurAP;
		char Style;
		String Name;
		uint16_t Color;
		bool Active, Targeted;
		bool MyTurn = false;
		bool HadTurn = false;
};

class Player
{
	public:
		Player(void);

		void EndTurn();
		void StartTurn();
		void SelectEnemy(Enemy *enemy) { _selectedEnemy = enemy; };
		void GetGame(Game *game) { _game = game; };

		PlayerState PlayerState = Normal;

		Gun CurrentGun;
		MeleeWeapon CurrentMelee;

		//Position
		byte x, prevX;
		byte y, prevY;

		//Strength, Perception, Endurance, Charisma, Intelligence, Agility, Luck
		byte STR, PER, END, CHA, INT, AGI, LUC;

		bool HasTarget;

		//http://fallout.wikia.com/wiki/Fallout_2_derived_statistics
		byte Armor, CritChance, DResist, HealRate, MeleeD, PerkRate, RadResist, Sequence, SkillRate;

		uint16_t money;

		byte MaxHP, CurHP;
		byte MaxAP, CurAP; //Action points
		byte Level;

		bool MyTurn = false;
		bool HadTurn = false;

	private:
		Enemy * _selectedEnemy;
		Game * _game;
};



class NPC
{
	public:
		NPC(void);
		byte x, y;
};

class Tile
{
	public:
		Tile(void);
		byte x, y;
		char style; //The character used to draw the tile from the font
		uint16_t color; //The colour this tile should be when drawn on screen
};

class ActivityFeed
{
	public:
		ActivityFeed();
		void GetGame(Game *game) { _game = game; };
		void GetScreen(TFTScreen *screen) { _screen = screen; };
		void Draw();
		void Update(String activity);
		String Lines[4];

	private:
		TFTScreen * _screen;
		Game * _game;

};

class Game
{
	public:
		Game();

		void Init();
		void LoadPlayer(); //Load the save data
		void Loop();
		void Input();
		void MovePlayer();
		void LoadChunk(uint16_t xStart, uint16_t yStart);
		void LoadEnemies(uint16_t xStart, uint16_t yStart);
		void CopyFile(String sourceFile, String destFile);
		void DrawScreen();
		void  DrawUI();
		void UpdateEnemies();
		void DrawEnemies();
		void CheckTouchScreen();

		unsigned long Time;
		unsigned long ElaspedTime;

		int data;

		TFTScreen tft;
		TouchScreen ts = TouchScreen(6, A1, A2, 7, 300);
		TSPoint tp;

		String sScreen, sEnemies;

		File myFile;

		byte xChunk, yChunk;

		GameState CurrentGameState;
		Player player;

		Tile tScreen[yLength][xLength];
		Enemy enemies[10];
		Item items[10];
		NPC npcs[10];
		Explosion explosion;
		Selector selector;
		ActivityFeed activityFeed;

		cBtn topBtn = cBtn(28);
		cBtn leftBtn = cBtn(26);
		cBtn rightBtn = cBtn(30);
		cBtn downBtn = cBtn(32);

		cBtn aButton = cBtn(34);
		cBtn bButton = cBtn(36);		
	private:
};



#endif