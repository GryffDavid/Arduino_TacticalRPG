#include "Game.h"

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
	_game->DrawEnemies();
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
	Draw();
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



Enemy::Enemy(void)
{
}

void Enemy::EndTurn()
{
	MyTurn = false;
	_game->DrawEnemies();
}

void Enemy::StartTurn()
{
	MyTurn = true;
	_game->DrawEnemies();
}

void Enemy::Undraw()
{
	_screen->setCursor(x * 16, y * 16 + 16);
	_screen->setTextColor(_game->tScreen[y][x].color);
	_screen->print(_game->tScreen[y][x].style);
}



Player::Player(void)
{
	x = 1;
	y = 1;
	prevX = 1;
	prevY = 1;
}

void Player::Init()
{
	//SPECIAL stats
	STR = 5;
	PER = 5;
	END = 5;
	CHA = 5;
	INT = 5;
	AGI = 5;
	LUC = 5;

	//DERIVED stats
	MaxAP = (int)(5 / 2) + 5;
	CurAP = MaxAP;

	MaxHP = 15 + (2 * END) + STR;
	CurHP = MaxHP;

	ArmorClass = AGI;
	CarryWeight = 25 + (STR * 25);
	CritChance = LUC;
	DResist = 0;
	HealRate = max(END / 3, 1);
	MeleeD = max(STR - 5, 1);
	PerkRate = 3;
	PResist = (END * 5);
	RadResist = (END * 2);
	Sequence = PER * 2;
	SkillRate = (INT * 2) + 5;
	

	//COMBAT SKILLS
	SmallGuns = 5 + (4 * AGI);
	BigGuns = 2 * AGI;
	EnergyWeapons = 2 * AGI;
	Unarmed = 40 + (STR / 2) + (AGI / 2);
	MeleeWeapons = 55 + (STR / 2) + (AGI / 2);
	Throwing = 4 * AGI;

	//ACTIVE SKILLS
	FirstAid = 30 + (PER / 2) + (INT / 2);
	Doctor = 15 + (PER / 2) + (INT / 2);
	Sneak = 25 + AGI;
	LockPick = 20 + (PER / 2) + (AGI / 2);
	Steal = 20 + AGI;
	Traps = 20 + (PER / 2) + (AGI / 2);
	Science = 25 + (2 * INT);
	Repair = 20 + INT;

	//PASSIVE SKILLS
	Speech = 25 + (2 * CHA);
	Barter = 20 + (2 * CHA);
	Gambling = 5 * LUC;
	Outdoorsman = (2 * END) + (2 * INT);



	Gun tmpGun;
	tmpGun.GunType = Small;
	tmpGun.APCost = 5;
	tmpGun.MaxDamage = 15;
	tmpGun.MinDamage = 10;

	CurrentGun = tmpGun;
	PlayerState = Normal;
}

void Player::EndTurn()
{
	_game->activityFeed.Update("Turn Ended");
	MyTurn = false;
	_game->DrawEnemies();
}

void Player::StartTurn()
{
	_game->activityFeed.Update("Turn Started");
	MyTurn = true;
	CurAP = MaxAP;
	_game->DrawEnemies();
	_game->DrawUI();
}

bool Player::CanHit()
{
	byte Required = random(0, 100);
	Serial.println("Required " + (String)Required);
	byte BTH;

	switch (CurrentGun.GunType)
	{
		case Small:		
			BTH += SmallGuns;		
			break;

		case Big:
			BTH += BigGuns;
			break;

		case Energy:
			BTH += EnergyWeapons;
			break;
	}

	BTH -= 30;

	double deltaX = ((x * 16) + 8) - ((_selectedEnemy->x * 16) + 8); //X Change
	double deltaY = ((y * 16) + 8) - ((_selectedEnemy->y * 16) + 8); //Y Change
	double d = sqrt((deltaX * deltaX) + (deltaY * deltaY));

	BTH += ((PER - 2) * 16);
	BTH -= (d / 16) * 4;
	BTH -= _selectedEnemy->ArmorClass;

	Serial.println("BTH: " + (String)BTH);
	
	if (BTH > Required)
		return true;
	else
		return false;
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

	
	tft.begin();
	tft.setRotation(3);
	tft.fillScreen(BLACK);
	
	//Start the SD card
	if (!SD.begin(SD_CS)) { tft.println("SD failed to initialise"); while (1); }

	//CopyFile("OENS.txt", "ENS.txt");
	LoadPlayer();
	player.Init();	
	DrawUI();
	
	selector.GetGame(this);
	explosion.GetGame(this);
	activityFeed.GetGame(this);
	player.GetGame(this);
	

	//Could almost certainly do this better by making the _game variable static across all instances of enemy
	for (byte i = 0; i < 10; i++)
	{
		enemies[i].GetGame(this);
		enemies[i].GetScreen(&tft);
	}

	selector.GetScreen(&tft);
	explosion.GetScreen(&tft);
	activityFeed.GetScreen(&tft);
	

	//LoadEnemies(xChunk, yChunk);
	LoadChunk(xChunk, yChunk);
	
	
	DrawEnemies();

	activityFeed.Update("WELCOME!");
	
	tft.setFont(&Anims1);
}

void Game::Loop()
{
	ElaspedTime = millis() - Time;
	Time = millis();

	Input();
	
	if (CurrentGameState == Combat)
		UpdateEnemies();

	explosion.Update(ElaspedTime);
	selector.Update(ElaspedTime);
	
	CheckTouchScreen();
}

void Game::Input()
{	
	switch (CurrentGameState)
	{
		case World:
		{
			switch (player.PlayerState)
			{
				case Normal:
				{
					MovePlayer();

					//Enter targeting mode
					if (aButton.UpdatePhysicalBtn() == true)
					{
						selector.Init(player.x, player.y, tScreen[player.y][player.x].style, tScreen[player.y][player.x].color);

						for (byte i = 0; i < 10; i++)
						{
							enemies[i].Targeted = false;
						}

						player.PlayerState = TargetMode;
						activityFeed.Update("Select Target");
					}				
				}
				break;

				case TargetMode:
				{
					//Cancel Targeting
					if (aButton.UpdatePhysicalBtn() == true)
					{
						player.PlayerState = Normal;
						selector._active = false;
						selector.Undraw();
						activityFeed.Update("Targeting cancelled");
						player.HasTarget = false;
						return;
					}

					//Select Target
					if (bButton.UpdatePhysicalBtn() == true)
					{
						bool foundTarget = false;

						for (byte i = 0; i < 10; i++)
						{
							if (enemies[i].x == selector._x &&
								enemies[i].y == selector._y)
							{
								activityFeed.Update("Target selected: " + enemies[i].Name);
								CurrentGameState = Combat;
								activityFeed.Update("Entered combat");
								player.MyTurn = true;
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
						player.PlayerState = Normal;
					}

					//Move reticle
					if (rightBtn.UpdatePhysicalBtn() == true)
					{
						selector.Move(1, 0, tScreen[selector._y][selector._x + 1].style, tScreen[selector._y][selector._x + 1].color);
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
		break;

		case Combat:
		{
			switch (player.PlayerState)
			{
				case Normal:
				{
					if (player.MyTurn == true && player.CurAP > 0)
					{
						MovePlayer();
					}

					//Enter targeting mode
					if (aButton.UpdatePhysicalBtn() == true)
					{
						selector.Init(player.x, player.y, tScreen[player.y][player.x].style, tScreen[player.y][player.x].color);

						for (byte i = 0; i < 10; i++)
						{
							enemies[i].Targeted = false;
						}

						player.PlayerState = TargetMode;
						activityFeed.Update("Select Target");
					}

					//Pressed B button
					if (bButton.UpdatePhysicalBtn() == true)
					{
						if (player.CurAP < player.CurrentGun.APCost)
						{
							activityFeed.Update("Not enough action points");
						}

						if (player.HasTarget == true && player.CurAP >= player.CurrentGun.APCost)
						{
							player.CurAP -= player.CurrentGun.APCost;
							DrawUI();

							double deltaX = (player.x * 16 + 8) - (160 + 8); //X Change
							double deltaY = (player.y * 16 + 8) - (160 + 8); //Y Change

							double grad = (double)deltaY / (double)deltaX;
							double d = sqrt((deltaX * deltaX) + (deltaY * deltaY));
							
							uint16_t cols[(uint16_t)d];

							for (uint16_t i = 0; i < (uint16_t)abs(deltaX); i++)
							{
								cols[i] = tft.readPixel((player.x * 16) + 8 + i, (player.y * 16) + 8 + (i * grad));
								tft.drawPixel((player.x * 16) + 8 + i, (player.y * 16) + 8 + (i*grad), WHITE);
							}

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
							//https://fallout.gamepedia.com/Fallout_and_Fallout_2_combat
							uint16_t ND, RD;

							if (player.CanHit() == true)
							{
								RD = random(player.CurrentGun.MinDamage, player.CurrentGun.MaxDamage);
								ND = RD;

								player._selectedEnemy->CurHP -= ND;
								activityFeed.Update("Fired shot. Hit " + player._selectedEnemy->Name + " for " + ND);

								if (player._selectedEnemy->CurHP <= 0)
								{
									player._selectedEnemy->Active = false;
									player._selectedEnemy->Undraw();
									activityFeed.Update(player._selectedEnemy->Name + " was killed");
								}
								else
								{
									activityFeed.Update(player._selectedEnemy->Name + " has " + player._selectedEnemy->CurHP + " remaining");
								}
							}
							else
							{
								activityFeed.Update("Fired shot. Missed");
							}

							//If enemy dies, select next enemy? Or deselect completely
							//player.HasTarget = false;
							//player._selectedEnemy->Active = false;
							
		/*					bool ActiveEnemies = false;

							for (byte i = 0; i < 10; i++)
							{
								if (enemies[i].Active == true)
								{
									ActiveEnemies = true;
									break;
								}
							}

							if (ActiveEnemies == false)
							{
								player.HasTarget = false;
								player.CurAP = player.MaxAP;
								DrawUI();
								activityFeed.Update("Combat Ended");
								player.PlayerState = Normal;
								CurrentGameState = World;
								return;
							}*/

							//Could also change to black screen for combat and make things easier				
							return;
						}
					}

					//Ran out of moves - Wait for animations to finish before ending turn
					if (player.MyTurn == true && player.CurAP == 0 && explosion._active == false)
					{
						player.EndTurn();
					}
				}
				break;

				case TargetMode:
				{
					//Cancel Targeting
					if (aButton.UpdatePhysicalBtn() == true)
					{
						player.PlayerState = Normal;
						selector._active = false;
						selector.Undraw();
						activityFeed.Update("Targeting cancelled");
						player.HasTarget = false;
						return;
					}

					//Select Target
					if (bButton.UpdatePhysicalBtn() == true)
					{
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
						player.PlayerState = Normal;
					}

					//Move reticle
					if (rightBtn.UpdatePhysicalBtn() == true)
					{
						selector.Move(1, 0, tScreen[selector._y][selector._x + 1].style, tScreen[selector._y][selector._x + 1].color);
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
		break;
	}

}

void Game::MovePlayer()
{
	//MOVE UP
	if (topBtn.UpdatePhysicalBtn() == true)
	{
		//Load new Chunk if necessary
		if (player.y == 0)
		{
			if (CurrentGameState == Combat)
				return;

			player.y = yLength - 2;
			yChunk -= 1;
			LoadChunk(xChunk, yChunk);
			return;
		}

		//Use APs if in Combat
		if (CurrentGameState == Combat)
		{
			player.CurAP--;
			DrawUI();
		}

		//Bump into enemies
		for (uint8_t i = 0; i < 10; i++)
		{
			if (enemies[i].Active == true)
				if (enemies[i].x == player.x &&
					enemies[i].y == player.y - 1)
				{
					return;
				}
		}

		//Actually move player
		if (tScreen[player.y - 1][player.x].style != 'b' && 
			tScreen[player.y - 1][player.x].style != 'm')
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
		return;
	}

	//MOVE DOWN
	if (downBtn.UpdatePhysicalBtn() == true)
	{
		//Load new Chunk if necessary
		if (player.y >= yLength - 2)
		{
			if (CurrentGameState == Combat)
				return;

			player.y = 0;
			yChunk += 1;
			LoadChunk(xChunk, yChunk);
			return;
		}

		//Use APs if in Combat
		if (CurrentGameState == Combat)
		{
			player.CurAP--;
			DrawUI();
		}

		//Bump into enemies
		for (uint8_t i = 0; i < 10; i++)
		{
			if (enemies[i].Active == true)
				if (enemies[i].x == player.x &&
					enemies[i].y == player.y + 1)
				{
					return;
				}
		}

		//Actually move player
		if (tScreen[player.y + 1][player.x].style != 'b' &&
			tScreen[player.y + 1][player.x].style != 'm')
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
		return;
	}

	//MOVE LEFT
	if (leftBtn.UpdatePhysicalBtn() == true)
	{
		//Load new Chunk if necessary
		if (player.x == 0)
		{
			if (CurrentGameState == Combat)
				return;

			player.x = xLength - 2;
			xChunk -= 1;
			LoadChunk(xChunk, yChunk);
			return;
		}

		//Use APs if in Combat
		if (CurrentGameState == Combat)
		{
			player.CurAP--;
			DrawUI();
		}

		//Bump into enemies
		for (uint8_t i = 0; i < 10; i++)
		{
			if (enemies[i].Active == true)
				if (enemies[i].x == player.x - 1 &&
					enemies[i].y == player.y)
				{
					return;
				}
		}

		//Actually move player
		if (tScreen[player.y][player.x - 1].style != 'b' &&
			tScreen[player.y][player.x - 1].style != 'm')
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
		return;
	}

	//MOVE RIGHT
	if (rightBtn.UpdatePhysicalBtn() == true)
	{
		//Load new Chunk if necessary
		if (player.x >= xLength - 2)
		{
			if (CurrentGameState == Combat)
				return;

			player.x = 0;
			xChunk += 1;
			LoadChunk(xChunk, yChunk);
			return;
		}

		//Use APs if in Combat
		if (CurrentGameState == Combat)
		{
			player.CurAP--;
			DrawUI();
		}
		
		//Bump into enemies
		for (uint8_t i = 0; i < 10; i++)
		{
			if (enemies[i].Active == true)
			if (enemies[i].x == player.x + 1 &&
				enemies[i].y == player.y)
				{
					return;
				}
		}

		//Actually move player
		if (tScreen[player.y][player.x + 1].style != 'b' &&
			tScreen[player.y][player.x + 1].style != 'm')
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
		return;
	}
}



void Game::DrawScreen()
{
	//tft.setFont(&Anims1);
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

				case 'f':
				case 'b':
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

				case 'm':
					newTile.color = WHITE;
					break;
			}

			tScreen[y][x] = newTile;
			tft.setTextColor(newTile.color);
			tft.print(newTile.style);
		}
	}
}

void Game::DrawUI()
{
	tft.fillRect(16 * (xLength-1), 0, 5*16, 240, BLACK);
	tft.setFont();
	tft.setTextColor(GREEN);

	tft.setCursor(16 * 20 + 16, 16);
	tft.println("HP: " + (String)player.CurHP + '/' + (String)player.MaxHP);

	tft.setCursor(16 * 20 + 16, 24);
	tft.println("AP: " + (String)player.CurAP + '/' + (String)player.MaxAP);

	tft.setFont(&Anims1);
}

void Game::DrawEnemies()
{
	for (uint8_t i = 0; i < 10; i++)
	{
		if (enemies[i].Active == true)
		{
			tft.setCursor(enemies[i].x * 16, enemies[i].y * 16 + 16);

			if (player.MyTurn == true && enemies[i].Targeted == true)
				tft.setTextColor(RED);
			else if (player.MyTurn == false && enemies[i].MyTurn == true)
				tft.setTextColor(GREEN);
			else
				tft.setTextColor(enemies[i].Color);

			tft.print(enemies[i].Style);
		}
	}
}

void Game::UpdateEnemies()
{
	if (player.MyTurn == false)
	{
		bool ActiveEnemies = false;

		for (byte i = 0; i < 10; i++)
		{
			if (enemies[i].Active == true)
			{
				if (enemies[i].CurHP <= 0)
				{
					enemies[i].Active = false;
					enemies[i].Undraw();
				}
				else
				{
					ActiveEnemies = true;
					enemies[i].StartTurn();
					activityFeed.Update(enemies[i].Name + " " + i + " Turn");
					delay(1000);
					//Attack player
					//Add pauses in between so that the turn isn't over in 30 milliseconds
					enemies[i].EndTurn();
				}
			}
		}

		if (ActiveEnemies == false)
		{			
			player.HasTarget = false;			
			player.CurAP = player.MaxAP;
			DrawUI();
			activityFeed.Update("Combat Ended");
			player.PlayerState = Normal;
			CurrentGameState = World;
			return;
		}

		//Every enemy has had a turn. Player's turn now
		if (player.MyTurn == false)
		{
			player.StartTurn();
		}
	}
}



void Game::LoadPlayer()
{

}

void Game::LoadChunk(uint16_t xStart, uint16_t yStart)
{
	//Serial.println("Loading chunk...");

	/*tft.fillRect(0, 0, 16 * (xLength - 1), 16 * (yLength - 1), BLACK);*/
	//tft.setCursor(0, 16);
	//tft.setFont();
	//tft.setTextColor(WHITE);
	//tft.print("Loading chunk...");

	xStart *= xLength - 1;
	yStart *= yLength - 1;

	sScreen = "";

	player.HasTarget = false;

	//Load Tiles
	myFile = SD.open("LVL.txt");

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
				sScreen += buf;
				sScreen += '\n';
			}
		}

		myFile.close();
	}
	else
	{		
		tft.fillRect(0, 0, 16 * (xLength - 1), 16 * (yLength - 1), BLACK); 
		tft.setFont();
		tft.setTextColor(WHITE);
		tft.println("error opening LVL.txt");
	}

	LoadEnemies(xStart, yStart);

	//tft.fillRect(0, 8, 100, 16, BLACK);
	tft.fillRect(0, 0, 16 * (xLength - 1), 16 * (yLength - 1), BLACK);

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
	//Serial.println("Loading enemies...");

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
			//Serial.println("ENS exists. Opening...");

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
						Enemy *tmp = &enemies[eInd];
						tmp->x = x;
						tmp->y = y;
						tmp->MaxHP = 23;
						tmp->CurHP = tmp->MaxHP;
						tmp->MaxAP = 5;
						tmp->CurAP = tmp->MaxAP;
						tmp->Style = 'h';
						tmp->Name = "Alien";
						tmp->Active = true;
						tmp->Color = PINK;
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