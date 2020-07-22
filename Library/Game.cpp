#include "Game.h"

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


Player::Player(void)
{
	x = 1;
	y = 1;
	prevX = 1;
	prevY = 1;

	CurHP = 10;
	MaxHP = 10;
}


Explosion::Explosion()
{
	
}

void Explosion::Init(byte xPos, byte yPos, char uChar, uint16_t uColor)
{
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



Selector::Selector()
{
}

void Selector::Init(byte xPos, byte yPos, char uChar, uint16_t uColor)
{
	_x = xPos;
	_y = yPos;

	_curTime = 0;
	_maxTime = 1000;

	_flash = true;
	_active = true;
	_uChar = uChar;
	_uColor = uColor;	
}

void Selector::Update(unsigned long elapsed)
{
	_curTime += elapsed;

	if (_curTime < 500 && _flash == false)
	{
		_flash = true;
		Draw();
	}

	if (_curTime >= 500 && _flash == true)
	{
		_flash = false;
		Draw();
	}

	if (_curTime >= _maxTime)
	{
		_curTime = 0;
	}
}

void Selector::Draw()
{
	if (_active == true)
	{
		if (_flash == true)
		{
			_screen->drawRect(_x * 16, _y * 16, 16, 16, WHITE);
		}
		else
		{
			_screen->drawRect(_x * 16, _y * 16, 16, 16, BLACK);
			_screen->setCursor(_x * 16, _y * 16 + 16);
			_screen->setTextColor(_uColor);
			_screen->print(_uChar);
			_game->DrawEnemies();
		}
	}
}

void Selector::Undraw()
{
	_screen->drawRect(_x * 16, _y * 16, 16, 16, BLACK);
	_screen->setCursor(_x * 16, _y * 16 + 16);
	_screen->setTextColor(_uColor);
	_screen->print(_uChar);
	_game->DrawEnemies();
}

void Selector:: Move(int xpos, int ypos, char uChar, uint16_t uColor)
{
	_flash = false;
	_curTime = 0;
	Draw();

	_x += xpos;
	_y += ypos;

	_flash = true;
	_uChar = uChar;
	_uColor = uColor;

	Draw();
}



ActivityFeed::ActivityFeed()
{
}

void ActivityFeed::Draw()
{
	_screen->setFont();	
	_screen->setCursor(0, 240 - 32);
	_screen->fillRect(0, 240 - 32, 200, 32, BLACK);
	
	
	for (byte i = 0; i < 4; i++)
	{
		if (i == 3)
		{
			_screen->setTextColor(GREENYELLOW);
			_screen->println(">" + Lines[i]);
		}
		else
		{
			_screen->setTextColor(WHITE);
			_screen->println(Lines[i]);
		}
	}

	_screen->setFont(&Anims1);
}

void ActivityFeed::Update(String activity)
{
	for (byte i = 0; i < 3; i++)
	{
		Lines[i] = Lines[i + 1];
	}

	Lines[3] = activity;

	Draw();
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
	
	//Start the SD card
	if (!SD.begin(SD_CS)) { tft.println("SD failed to initialise"); while (1); }

	//CopyFile("OENS.txt", "ENS.txt");

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

	selector.GetGame(this);
	explosion.GetGame(this);
	activityFeed.GetGame(this);

	selector.GetScreen(&tft);
	explosion.GetScreen(&tft);
	activityFeed.GetScreen(&tft);

	LoadEnemies(xChunk, yChunk);
	LoadChunk(xChunk, yChunk);
	
	DrawEnemies();

	activityFeed.Update("WELCOME!");
	

	//tft.setFont();
	//tft.setTextColor(WHITE);
	//tft.setCursor(0, 16 * 15 - 32);
	//tft.println("Dialogue goes here!");
	//tft.println("Dialogue goes here 2!");
	//tft.println("Dialogue goes here 3!");
	//tft.println("Dialogue goes here 4!");

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
			Input();

			explosion.Update(ElaspedTime);
			selector.Update(ElaspedTime);

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
	
	//Load Tiles
	myFile = SD.open("LVL.txt");

	if (myFile)
	{
		if (myFile.available())
		{
			for (byte y = yStart; y < yStart + yLength-1; y++)
			{
				char buf[xLength-1];
				uint16_t pos = ((151 * y + y)) + xStart;
				myFile.seek(pos);
				myFile.readBytes(buf, sizeof(buf));
				
				sScreen += buf;
				sScreen += '\n';
			}
		}

		myFile.close();
	}
	else
	{
		tft.println("error opening LVL.txt");
	}

	LoadEnemies(xStart, yStart);

	tft.fillRect(0, 0, 16 * (xLength-1), 16 * (yLength-1), BLACK);
	DrawScreen();
	DrawEnemies();

	tft.setTextColor(RED);
	tft.setCursor(player.x * 16, player.y * 16 + 16);
	tft.print('a');
}

void Game::LoadEnemies(uint16_t xStart, uint16_t yStart)
{
	//xStart *= xLength - 1;
	//yStart *= yLength - 1;
	for (byte i = 0; i < 10; i++)
	{
		enemies[i].Active = false;
	}

	myFile = SD.open("ENS.txt");
	uint8_t eInd = 0;

	if (myFile)
	{
		if (myFile.available())
		{
			for (byte y = yStart; y < yStart + yLength - 1; y++)
			{
				char buf[xLength - 1];
				uint16_t pos = ((151 * y + y)) + xStart;
				myFile.seek(pos);
				myFile.readBytes(buf, sizeof(buf));
				
				for (byte x = 0; x < sizeof(buf); x++)
				{
					switch (buf[x])
					{
						default:
						break;

						case 'h':
						{
							enemies[eInd].x = x;
							enemies[eInd].y = y;
							enemies[eInd].Style = 'h';
							enemies[eInd].Name = "Alien";
							enemies[eInd].Active = true;
							enemies[eInd].Color = PINK;
							eInd += 1;
						}
						break;
					}
				}
			}
		}

		myFile.close();
	}
	else
	{
		tft.println("error opening ENS.txt");
	}
}

void Game::DrawScreen()
{
	tft.setFont(&Anims1);
	tft.setCursor(0, 16);

	for (uint16_t y = 0; y < yLength-1; y++)
	{
		for (uint16_t x = 0; x < xLength; x++)
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

void Game::DrawEnemies()
{
	for (uint8_t i = 0; i < 10; i++)
	{
		if (enemies[i].Active == true)
		{
			tft.setCursor(enemies[i].x * 16, enemies[i].y * 16 + 16);

			if (enemies[i].Targeted == true)
				tft.setTextColor(RED);
			else
				tft.setTextColor(enemies[i].Color);

			tft.print(enemies[i].Style);
		}
	}
}

void Game::Input()
{
	switch (player.PlayerState)
	{
		case Normal:
		{
			if (aButton.UpdatePhysicalBtn() == true)
			{
				//selector.GetGame(this);
				selector.Init(player.x, player.y, tScreen[player.y][player.x].style, tScreen[player.y][player.x].color);

				for (byte i = 0; i < 10; i++)
				{
					enemies[i].Targeted = false;
				}

				player.PlayerState = SelectMode;
				activityFeed.Update("Select Target");
			}

			if (bButton.UpdatePhysicalBtn() == true)
			{
				if (player.HasTarget == true)
				{
					double deltaX = (player.x * 16 + 8) - (160 + 8); //X Change
					double deltaY = (player.y * 16 + 8) - (160 + 8); //Y Change

					double grad = (double)deltaY / (double)deltaX;
					double d = sqrt((deltaX * deltaX) + (deltaY * deltaY));

					Serial.println(grad);
					Serial.println(d);
					Serial.println(deltaX);
					Serial.println(deltaY);

					uint16_t cols[(uint16_t)d];

					for (uint16_t i = 0; i < (uint16_t)abs(deltaX); i++)
					{
						cols[i] = tft.readPixel((player.x * 16) + 8 + i, (player.y * 16) + 8 + (i * grad));
						tft.drawPixel((player.x * 16) + 8 + i, (player.y * 16) + 8 + (i*grad), WHITE);
					}

					//delay(200);

	//				explosion.GetGame(this);
					explosion.Init(selector._x, selector._y, tScreen[selector._y][selector._x].style, tScreen[selector._y][selector._x].color);

					//Read and store the colour of all the pixels in the line,
					//Then draw the white line
					//Wait, then draw the colour of the original pixels back again
					//ReadPixel()


					for (uint16_t i = 0; i < (uint16_t)abs(deltaX); i++)
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
			}
			
			MovePlayer();
		}
		break;

		case SelectMode:
		{
			if (aButton.UpdatePhysicalBtn() == true)
			{
				player.PlayerState = Normal;
				selector._active = false;
				selector.Undraw();
				activityFeed.Update("Targeting cancelled");
				player.HasTarget = false;
				return;
			}

			if (bButton.UpdatePhysicalBtn() == true)
			{
				player.PlayerState = Normal;
				bool foundTarget = false;

				for (byte i = 0; i < 10; i++)
				{
					if (enemies[i].x == selector._x &&
						enemies[i].y == selector._y)
					{
						activityFeed.Update("Target selected: " + enemies[i].Name);
						enemies[i].Targeted = true;
						player.HasTarget = true;
						foundTarget = true;
						player.SelectEnemy(&enemies[i]);
					}
				}

				if (foundTarget == false)
				{
					player.HasTarget = false;
					activityFeed.Update("No target selected");
				}

				selector._active = false;
				selector.Undraw();
			}

			if (rightBtn.UpdatePhysicalBtn() == true)
			{
				selector.Move(1, 0, tScreen[selector._y][selector._x+1].style, tScreen[selector._y][selector._x+1].color);
			}

			if (leftBtn.UpdatePhysicalBtn() == true)
			{
				selector.Move(-1, 0, tScreen[selector._y][selector._x - 1].style, tScreen[selector._y][selector._x - 1].color);
			}

			if (topBtn.UpdatePhysicalBtn() == true)
			{
				selector.Move(0, -1, tScreen[selector._y - 1][selector._x].style, tScreen[selector._y - 1][selector._x].color);
			}

			if (downBtn.UpdatePhysicalBtn() == true)
			{
				selector.Move(0, 1, tScreen[selector._y + 1][selector._x].style, tScreen[selector._y + 1][selector._x].color);
			}
		}
		break;
	}
}

void Game::MovePlayer()
{
	//MOVE UP
	if (topBtn.UpdatePhysicalBtn() == true)
	{
		if (player.y == 0)
		{
			player.y = yLength - 2;
			yChunk -= 1;
			LoadChunk(xChunk, yChunk);
			return;
		}

		for (uint8_t i = 0; i < 10; i++)
		{
			if (enemies[i].Active == true)
				if (enemies[i].x == player.x &&
					enemies[i].y == player.y - 1)
				{
					return;
				}
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
		if (player.y >= yLength - 2)
		{
			player.y = 0;
			yChunk += 1;
			LoadChunk(xChunk, yChunk);
			return;
		}

		for (uint8_t i = 0; i < 10; i++)
		{
			if (enemies[i].Active == true)
				if (enemies[i].x == player.x &&
					enemies[i].y == player.y + 1)
				{
					return;
				}
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
			player.x = xLength - 2;
			xChunk -= 1;
			LoadChunk(xChunk, yChunk);
			return;
		}

		for (uint8_t i = 0; i < 10; i++)
		{
			if (enemies[i].Active == true)
				if (enemies[i].x == player.x - 1 &&
					enemies[i].y == player.y)
				{
					return;
				}
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
		if (player.x >= xLength - 2)
		{
			player.x = 0;
			xChunk += 1;
			LoadChunk(xChunk, yChunk);
			return;
		}

		for (uint8_t i = 0; i < 10; i++)
		{
			if (enemies[i].Active == true)
				if (enemies[i].x == player.x + 1 &&
					enemies[i].y == player.y)
				{
					return;
				}
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
			/*for (byte i = 0; i < 10; i++)
			{
				if (xpos > enemies[i].x * 16 &&
					xpos < enemies[i].x * 16 + 16 &&
					ypos > enemies[i].y * 16 &&
					ypos < enemies[i].y * 16 + 16)
				{
					tft.setCursor(enemies[i].x * 16, enemies[i].y * 16 + 16);
					tft.print('g');
				}
			}*/
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

void Game::CopyFile(String sourceFile, String destFile)
{
	File myOrigFile;
	File myDestFile;
	
	if (!SD.remove(destFile)) { Serial.println("Error removing file"); }
	else { Serial.println("File removed"); }

	Serial.println("Started copying");

	myOrigFile = SD.open(sourceFile, FILE_READ);
	if (!myOrigFile)
		Serial.println("Error opening source file");

	myDestFile = SD.open(destFile, FILE_WRITE);
	if (!myDestFile)
		Serial.println("Error opening destination file");

	size_t n;
	uint8_t buf2[256];

	while ((n = myOrigFile.read(buf2, sizeof(buf2))) > 0)
	{
		myDestFile.write(buf2, n);
		Serial.print('.');
	}

	myOrigFile.close();
	myDestFile.close();
}
