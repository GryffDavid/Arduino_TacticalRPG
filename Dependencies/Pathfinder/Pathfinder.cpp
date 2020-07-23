#include "Pathfinder.h"
#include <MemoryFree.h>


Pathfinder::Pathfinder()
{

}

void Pathfinder::Init()
{
	Serial.begin(115200);
	
	StartPos = FindStart();
	EndPos = FindEnd();

	Serial.println("StartPos: " + (String)StartPos.X + ", " + (String)StartPos.Y);
	Serial.println("EndPos: " + (String)EndPos.X + ", " + (String)EndPos.Y);
	
	SearchNode *startTile = new SearchNode();
	startTile->Position = StartPos;
	startTile->DistToGoal = StepDistanceToEnd(startTile->Position);
	startTile->DistTraveled = 0;

	OpenList.add(startTile);

	PrintMap();
	
	Serial.println((String)millis());

	//for (int i = 0; i < 30; i++)
	
}

void Pathfinder::Loop()
{
	while (FindPath == false)
	{
		DoSearchStep();
	}
	
	if (FindPath == true)
	{
		Serial.println("Found path");
	}

	Serial.println((String)millis());

	PrintMap();
}


bool Pathfinder::SelectNodeToVisit(SearchNode *result)
{
	bool success = false;
	uint16_t smallestDistance = 65508;
	uint16_t currentDistance = 0;

	if (OpenList.size() > 0)
	{
		for (int i = 0; i < OpenList.size(); i++)
		{
			SearchNode node = *OpenList.get(i);
			currentDistance = node.DistToGoal;

			if (currentDistance < smallestDistance)
			{
				success = true;
				*result = node;
				smallestDistance = currentDistance;
			}
		}
	}

	return success;
}

void Pathfinder::DoSearchStep()
{
	Serial.println("Free memory: " + (String)freeMemory());
	Serial.println("Delaying...");
	delay(1000);
	SearchNode *newOpenListNode = new SearchNode();

	Serial.println("Selecting new node to visit");
	bool foundNewNode = SelectNodeToVisit(newOpenListNode);

	if (foundNewNode == true)
	{
		Serial.println("Found new node");
		Vector2 currentPos = newOpenListNode->Position;

		LinkedList<Vector2*> VecList = LinkedList<Vector2*>();
		Serial.println("Created VecList");
		OpenMapTiles(currentPos, &VecList);

		//PrintOpenMapTiles(currentPos);
		Serial.println("VecList Size: " + (String)VecList.size());

		for (byte i = 0; i < VecList.size(); i++)
		{
			Serial.println("Vec list: " + (String)i);
			SearchNode *tile = new SearchNode();
			tile->Position = *VecList.get(i);
			tile->DistToGoal = StepDistanceToEnd(*VecList.get(i));
			tile->DistTraveled = newOpenListNode->DistTraveled + 1;

			if (!InList(&OpenList, *VecList.get(i)) && 
				!InList(&ClosedList, *VecList.get(i)))
			{	
				Serial.println("Free memory: " + (String)freeMemory());
				OpenList.add(tile);
				Serial.println("Added tile");
				Serial.println("Free memory after add: " + (String)freeMemory());
			}
			else
			{
				Serial.println("In lists");
			}

			Serial.println("Finished VecList loop " + (String)i);
		}

		Serial.println("Done with VecList loop now...");

		int OpenIndex;// , ClosedIndex;

		Serial.println("Index of: " + (String)newOpenListNode->Position.X + ", " + (String)newOpenListNode->Position.Y);
		OpenIndex = IndexOf(&OpenList, newOpenListNode);

		Serial.println("Got index of");
		//ClosedIndex = IndexOf(&ClosedList, newOpenListNode);

		if (OpenIndex >= 0)
		{
			OpenList.remove(OpenIndex);
			Serial.println("Removed tile");
		}

		Serial.println("Free memory: " + (String)freeMemory());
		ClosedList.add(newOpenListNode);
		Serial.println("Added closed tile");
		Serial.println("Free memory after add: " + (String)freeMemory());
		
		if (currentPos.X == EndPos.X && 
			currentPos.Y == EndPos.Y)
		{
			Serial.println("FOUND PATH!");
			FindPath = true;
			return;
		}

		Serial.println("Closed List size: " + (String)ClosedList.size());
		Serial.println("Open List size: " + (String)OpenList.size());

	}
	else
	{
		Serial.println("No new node found");
	}

	
	PrintMap();

}


void Pathfinder::OpenMapTiles(Vector2 mapLoc, LinkedList<Vector2*> *list)
{
	Serial.println("Finding open map tiles");
	Vector2 testLoc = mapLoc;

	//Down
	if (tScreen[testLoc.Y + 1][testLoc.X] != 'b')
	{
		Vector2 *myVec = new Vector2();
		myVec->X = testLoc.X;
		myVec->Y = testLoc.Y + 1;
		list->add(myVec);
	}

	//Up
	if (tScreen[testLoc.Y - 1][testLoc.X] != 'b')
	{
		Vector2 *myVec = new Vector2();
		myVec->X = testLoc.X;
		myVec->Y = testLoc.Y - 1;
		list->add(myVec);
	}

	//Left
	if (tScreen[testLoc.Y][testLoc.X - 1] != 'b')
	{
		Vector2 *myVec = new Vector2();
		myVec->X = testLoc.X - 1;
		myVec->Y = testLoc.Y;
		list->add(myVec);
	}

	//Right
	if (tScreen[testLoc.Y][testLoc.X + 1] != 'b')
	{
		Vector2 *myVec = new Vector2();
		myVec->X = testLoc.X + 1;
		myVec->Y = testLoc.Y;
		list->add(myVec);
	}

	Serial.println("Found open map tiles");
}

bool Pathfinder::InList(LinkedList<SearchNode*> *list, Vector2 position)
{
	bool inList = false;
	for (byte i = 0; i < list->size(); i++)
	{
		if (list->get(i)->Position.X == position.X &&
			list->get(i)->Position.Y == position.Y)
		{
			inList = true;
		}
	}

	return inList;
}

byte Pathfinder::IndexOf(LinkedList<SearchNode*> *list, SearchNode *node)
{
	for (byte i = 0; i < list->size(); i++)
	{
		SearchNode *testNode = new SearchNode();
		testNode = list->get(i);

		//Serial.println((String)i + " Position: " + (String)testNode->Position.X + ", " + (String)testNode->Position.Y);
		//Serial.println("pNode Position: " + (String)node->Position.X + ", " + (String)node->Position.Y);

		if (testNode->Position.X == node->Position.X &&
			testNode->Position.Y == node->Position.Y &&
			testNode->DistToGoal == node->DistToGoal &&
			testNode->DistTraveled == node->DistTraveled)
		{
			return i;
		}
	}

	return -1;
}


Vector2 Pathfinder::FindStart()
{
	for (byte x = 0; x < xLength; x++)
	{
		for (byte y = 0; y < yLength; y++)
		{
			if (tScreen[y][x] == 's')
				return Vector2{ x, y };
		}
	}
}

Vector2 Pathfinder::FindEnd()
{
	for (byte x = 0; x < xLength; x++)
	{
		for (byte y = 0; y < yLength; y++)
		{
			if (tScreen[y][x] == 'e')
				return Vector2{ x, y };
		}
	}
}


uint16_t Pathfinder::StepDistance(Vector2 pointA, Vector2 pointB)
{
	int distanceX = pointA.X - pointB.X;
	distanceX = abs(distanceX);

	int distanceY = pointA.Y - pointB.Y;
	distanceY = abs(distanceY);

	return distanceX + distanceY;
}

uint16_t Pathfinder::StepDistanceToEnd(Vector2 point)
{
	return StepDistance(point, EndPos);
}


void Pathfinder::PrintMap()
{
	Serial.println(" ");

	for (byte i = 0; i < OpenList.size(); i++)
	{
		int myX = OpenList.get(i)->Position.X;
		int myY = OpenList.get(i)->Position.Y;

		tScreen[myY][myX] = '.';
	}

	for (byte i = 0; i < ClosedList.size(); i++)
	{
		int myX = ClosedList.get(i)->Position.X;
		int myY = ClosedList.get(i)->Position.Y;

		tScreen[myY][myX] = '&';
	}

	for (byte y = 0; y < yLength; y++)
	{
		Serial.println(tScreen[y]);
	}

	Serial.println(" ");
}

void Pathfinder::PrintInList(LinkedList<SearchNode*> *list, Vector2 position)
{
	if (InList(list, position) == true)
		Serial.println("Position " + (String)position.X + ", " + (String)position.Y + " found in list");
	else
		Serial.println("Position " + (String)position.X + ", " + (String)position.Y + " not found in list");
}

void Pathfinder::PrintOpenMapTiles(Vector2 position)
{
	Serial.println("---Test Tile: " + (String)position.X + "," + (String)position.Y + "---");
	LinkedList<Vector2*> VecList = LinkedList<Vector2*>();
	OpenMapTiles(position, &VecList);

	for (byte i = 0; i < VecList.size(); i++)
	{
		Serial.println("Position " + (String)VecList.get(i)->X + ", " + (String)VecList.get(i)->Y + " Open");
	}
}

void Pathfinder::PrintOpenNodes()
{
	Serial.println("---List Open Nodes---");
	for (byte i = 0; i < OpenList.size(); i++)
	{
		SearchNode openNode = *OpenList.get(i);
		Serial.println("Open node: " + (String)openNode.Position.X + "," + (String)openNode.Position.Y);
	}

	Serial.println("------");
}

void Pathfinder::PrintClosedNodes()
{
	Serial.println("---List Closed Nodes---");
	for (byte i = 0; i < ClosedList.size(); i++)
	{
		SearchNode closedNode = *ClosedList.get(i);
		Serial.println("Closed node: " + (String)closedNode.Position.X + "," + (String)closedNode.Position.Y);
	}
	Serial.println("------");
}