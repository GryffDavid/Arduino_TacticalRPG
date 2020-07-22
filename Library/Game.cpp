#include "Game.h"

Player::Player(void)
{
	x = 1;
	y = 1;
	prevX = 1;
	prevY = 1;

	CurHP = 10;
	MaxHP = 10;
}

Enemy::Enemy(void)
{
}

NPC::NPC(void)
{
}

Item::Item(void)
{
}

Tile::Tile(void)
{
}



cBtn::cBtn(byte pin)
{
	buttonPin = pin;
	pinMode(pin, INPUT);
}

bool cBtn::UpdatePhysicalBtn()
{
	int reading = digitalRead(buttonPin);
	bool result = false;

	if (reading != lastBtnState)
	{
		lastDebounceTime = millis();
	}

	if ((millis() - lastDebounceTime) > debounceDelay && reading != buttonState)
	{
		buttonState = reading;
		if (buttonState == HIGH)
		{
			result = true;
		}
	}

	lastBtnState = reading;
	return result;
}



Explosion::Explosion()
{
	
}

void Explosion::Init(TFTScreen *screen, byte xPos, byte yPos, char uChar, uint16_t uColor)
{	
	_screen = screen;
	_x = xPos;
	_y = yPos;

	_curTime = 0;
	_maxTime = 300;

	_active = true;
	_uChar = uChar;
	_uColor = uColor;
	_curChar = 'l';
	Draw();
}

void Explosion::Update(unsigned long elapsed)
{
	if (_active == true)
	{
		_curTime += elapsed;
		
		if (_curTime > 100 && _curChar == 'l')
		{
			_curChar = 'k';
			Draw();
		}

		if (_curTime > 200 && _curChar == 'k')
		{
			_curChar = 'j';
			Draw();
		}

		if (_curTime > _maxTime)
		{
			_curTime = 0;
			_active = false;
			Undraw();
		}
	}
}

void Explosion::Draw()
{
	if (_active == true)
	{
		_screen->setCursor(_x * 16, _y * 16 + 16);

		switch (_curChar)
		{
			case 'l':
				_screen->setTextColor(WHITE);
			break;

			case 'k':
				_screen->setTextColor(RED);
			break;

			case 'j':
				_screen->setTextColor(YELLOW);
			break;
		}

		_screen->print(_curChar);
	}
}

void Explosion::Undraw()
{
	_screen->fillRect(_x * 16, _y * 16, 16, 16, BLACK);
	_screen->setCursor(_x * 16, _y * 16 + 16);
	_screen->setTextColor(_uColor);
	_screen->print(_uChar);
}



Game::Game()
{
	xChunk = 0;
	yChunk = 0;
}

void Game::Init()
{
	Serial.begin(9600);
	randomSeed(analogRead(6));

	CurrentGameState = World;

	pinMode(53, OUTPUT);
	digitalWrite(53, HIGH);

	tft.begin(tft.readID());
	tft.setRotation(1);
	tft.fillScreen(BLACK);
	
	if (!SD.begin(SD_CS)) 
	{ 
		tft.println("SD failed to initialise");
		while (1);
	}
	
	if (!SD.remove("ENEMIES.txt"))
	{
		Serial.println("Error removing file");
	}
	else
	{
		Serial.println("File removed");
	}
	
	Serial.println("Started copying");

	myOrigFile = SD.open("OENEMIES.txt", FILE_READ);

	if (!myOrigFile)
		Serial.println("Error opening source file");
	

	myDestFile = SD.open("ENEMIES.txt", FILE_WRITE);

	if (!myDestFile)
		Serial.println("Error opening destination file");
	

	size_t n;
	uint8_t buf[128];

	while ((n = myOrigFile.read(buf, sizeof(buf))) > 0)
	{
		myDestFile.write(buf, n);
		Serial.print('.');
	}

	myOrigFile.close();
	myDestFile.close();
	

	tft.setCursor(16 * 20 + 16, 16);
	tft.println("HP: " + (String)player.CurHP + '/' + (String)player.MaxHP);

	tft.setCursor(16 * 20 + 16, 24);
	tft.println("AP: " + (String)player.CurAP + '/' + (String)player.MaxAP);

	//tft.setCursor(16 * 20 + 16, 24);
	//tft.print("STR: " + player.STR);

	//tft.setCursor(16 * 20 + 16, 32);
	//tft.print("DEX: " + player.DEX);

	//tft.setCursor(16 * 20 + 16, 40);
	//tft.print("CON: " + player.CON);

	//tft.setCursor(16 * 20 + 16, 48);
	//tft.print("INT: " + player.INT);

	//tft.setCursor(16 * 20 + 16, 56);
	//tft.print("WIS: " + player.WIS);

	//tft.setCursor(16 * 20 + 16, 64);
	//tft.print("CHA: " + player.CHA);

	LoadChunk(xChunk, yChunk);

	tft.setFont();
	tft.setTextColor(WHITE);
	tft.setCursor(0, 16 * 15 - 32);
	tft.println("Dialogue goes here!");
	tft.println("Dialogue goes here 2!");
	tft.println("Dialogue goes here 3!");
	tft.println("Dialogue goes here 4!");

	tft.setFont(&Anims1);
}

void Game::Loop()
{
	ElaspedTime = millis() - Time;
	Time = millis();

	switch (CurrentGameState)
	{
	    case World:
	    {
			MovePlayer();

			explosion.Update(ElaspedTime);

			//if (myTFTButton.UpdateTFTButton() == true)
			//{
			//		DO STUFF
			//}
	    }
	    break;
	}

	CheckTouchScreen();

	 
}

void Game::LoadChunk(uint16_t xStart, uint16_t yStart)
{
	xStart *= xLength - 1;
	yStart *= yLength - 1;

	sScreen = "";

	//Should make a copy of enemy/level data first. e.g. Level is a copy of OriginalLevel and Enemies is a copy of OriginalEnemies
	//So that it's easy to reset data

	//Load Tiles
	myFile = SD.open("Level.txt");

	if (myFile)
	{
		myFile.seek(0);
		if (myFile.available())
		{
			for (int y = yStart; y < yStart + yLength-1; y++)
			{
				char buf[xLength-1];
				uint16_t pos = ((151 * y + y)) + xStart;
				myFile.seek(pos);
				myFile.readBytes(buf, xLength-1);
				
				sScreen += buf;
				sScreen = sScreen.substring(0, sScreen.length() - 1);
				sScreen += '\n';

				//myFile.seek(pos);
				//myFile.write("b");
			}
		}

		myFile.close();
	}
	else
	{
		tft.println("error opening Level.txt");
	}


	////Load enemies

	//When an enemy is destroyed, open the file in WRITE mode and rewrite over the enemy with a tile character
	
	//myFile2 = SD.open("Enemies.txt");
	//if (myFile2)
	//{
	//	if (myFile.available())
	//	{
	//		for (int y = yStart; y < yStart + yLength - 1; y++)
	//		{
	//			char buf[xLength - 1];
	//			uint16_t pos = ((151) * y + y)) + xStart;
	//			myFile2.seek(pos);
	//			myFile2.readBytes(buf, xLength - 1);

	//			for (int i = 0; i < xLength - 1; i++)
	//			{
	//				
	//			}

	//			//DO MORE STUFF. YOU'LL FIGURE IT OUT
	//		}
	//	}
	//}


	tft.fillRect(0, 0, 16 * (xLength-1), 16 * (yLength-1), BLACK);
	DrawScreen();

	tft.setTextColor(RED);
	tft.setCursor(player.x * 16, player.y * 16 + 16);
	tft.print('a');
}

void Game::DrawScreen()
{
	tft.setFont(&Anims1);
	tft.setCursor(0, 16);

	for (int y = 0; y < yLength-1; y++)
	{
		for (int x = 0; x < xLength; x++)
		{
			Tile newTile;
			newTile.x = x;
			newTile.y = y;

			newTile.style = sScreen[(xLength*y) + x];

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
					newTile.color = PINK;
					break;

				case 'e':
					newTile.color = WHITE;
					break;

				case 'g':
					newTile.color = DARKGREEN;
					break;

				case 'h':
					newTile.color = DARKGREY;
					break;
			}

			tScreen[y][x] = newTile;

			tft.setTextColor(newTile.color);
			tft.print(newTile.style);
		}
	}
}

void Game::MovePlayer()
{
	if (aButton.UpdatePhysicalBtn() == true)
	{
		explosion.Init(&tft, 5, 5, tScreen[5][5].style, tScreen[5][5].color);
	}

	if (bButton.UpdatePhysicalBtn() == true) 
	{
		double deltaX = (player.x * 16 + 8) - (160 + 8); //X Change
		double deltaY = (player.y * 16 + 8) - (160 + 8); //Y Change

		double grad = (double)deltaY / (double)deltaX;
		double d = sqrt((deltaX * deltaX) + (deltaY * deltaY));

		Serial.println(grad);
		Serial.println(d);
		Serial.println(deltaX);
		Serial.println(deltaY);

		uint16_t cols[(int)d];

		for (int i = 0; i < (int)abs(deltaX); i++)
		{
			cols[i] = tft.readPixel((player.x * 16) + 8 + i, (player.y * 16) + 8 + (i * grad));
			tft.drawPixel((player.x * 16) + 8 + i, (player.y * 16) + 8 + (i*grad), WHITE);
		}

		//delay(200);

		explosion.Init(&tft, 10, 10, tScreen[10][10].style, tScreen[10][10].color);

		//Read and store the colour of all the pixels in the line,
		//Then draw the white line
		//Wait, then draw the colour of the original pixels back again
		//ReadPixel()
		

		for (int i = 0; i < (int)abs(deltaX); i++)
		{
			tft.drawPixel((player.x * 16) + 8 + i, (player.y * 16) + 8 + (i*grad), cols[i]);
		}

		//for (int x = player.x; x < 10; x++)
		//{
		//	uint16_t blockX = x;
		//	uint16_t blockY = player.y + (x * grad);

		//	tft.fillRect(blockX * 16, blockY * 16 - 16, 16, 16, BLACK);
		//	tft.setCursor(blockX * 16, blockY * 16 + 16);
		//	tft.setTextColor(tScreen[blockY][blockX].color);
		//	tft.print(tScreen[blockY][blockX].style);
		//}


		//tft.fillRect(x * 16, y * 16, 16, 16, BLACK);
		//tft.setCursor(x * 16, y * 16);
		//tft.setTextColor(tScreen[y][x].color);
		//tft.print(tScreen[y][x].style);


		//Calculate which blocks it intersected along the path
		//Redraw those blocks when necessary?
		//Also solid blocks need to stop shots

		//Could also change to black screen for combat and make things easier
	}

	//MOVE UP
	if (topBtn.UpdatePhysicalBtn() == true)
	{
		if (player.y == 0)
		{
			player.y = yLength-2;
			yChunk -= 1;
			LoadChunk(xChunk, yChunk);
			return;
		}

		if (tScreen[player.y - 1][player.x].style != 'b')
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
	if (downBtn.UpdatePhysicalBtn() == true)
	{
		if (player.y >= yLength-2)
		{
			player.y = 0;
			yChunk += 1;
			LoadChunk(xChunk, yChunk);
			return;
		}

		if (tScreen[player.y + 1][player.x].style != 'b')
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
	if (leftBtn.UpdatePhysicalBtn() == true)
	{
		if (player.x == 0)
		{
			player.x = xLength-2;
			xChunk -= 1;
			LoadChunk(xChunk, yChunk);
			return;
		}

		if (tScreen[player.y][player.x - 1].style != 'b')
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
	if (rightBtn.UpdatePhysicalBtn() == true)
	{
		if (player.x >= xLength-2)
		{
			player.x = 0;
			xChunk += 1;
			LoadChunk(xChunk, yChunk);
			return;
		}

		if (tScreen[player.y][player.x + 1].style != 'b')
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

void Game::CheckTouchScreen()
{
	tp = ts.getPoint();

	pinMode(A2, OUTPUT);
	pinMode(A1, OUTPUT);
	pinMode(6, OUTPUT);
	pinMode(7, OUTPUT);

	if (tp.z > MINPRESSURE && tp.z < MAXPRESSURE)
	{
		uint16_t xpos, ypos;

		SWAP(tp.x, tp.y);

		xpos = map(tp.x, TS_LEFT, TS_RT, 0, tft.width());
		ypos = map(tp.y, TS_TOP, TS_BOT, 0, tft.height());

		switch (CurrentGameState)
		{
		case World:
		{
			//myTFTButton.CheckButton(xpos, ypos);
		}
		break;
		}
	}
	else
	{
		switch (CurrentGameState)
		{
		case World:
		{
			//myTFTButton.pressed = false;
		}
		break;
		}
	}
}







