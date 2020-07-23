#ifndef Pathfinder_h
#define Pathfinder_h

#include <Arduino.h>
#include <LinkedList.h>

#define xLength 21
#define yLength 14

struct Vector2
{
	byte X, Y;
};

struct SearchNode
{
	Vector2 Position;
	uint16_t DistToGoal;
	uint16_t DistTraveled;
};


class Pathfinder
{
	public:
		Pathfinder();
		void Init();
		void Loop();
		uint16_t StepDistance(Vector2 pointA, Vector2 pointB);
		uint16_t StepDistanceToEnd(Vector2 point);
		void PrintOpenMapTiles(Vector2 position);
		void PrintOpenNodes();
		void PrintClosedNodes();
		void PrintInList(LinkedList<SearchNode*> *list, Vector2 position);
		void OpenMapTiles(Vector2 mapLoc, LinkedList<Vector2*> *list);
		bool SelectNodeToVisit(SearchNode *result);
		void DoSearchStep();
		void PrintMap();
		Vector2 FindStart();
		Vector2 FindEnd();

		Vector2 StartPos, EndPos;

		LinkedList<SearchNode*> OpenList = LinkedList<SearchNode*>();
		LinkedList<SearchNode*> ClosedList = LinkedList<SearchNode*>();
		bool InList(LinkedList<SearchNode*> *list, Vector2 position);
		byte IndexOf(LinkedList<SearchNode*> *list, SearchNode *node);
		bool FindPath = false;
		

		char tScreen[yLength][xLength] =
		{ { 'b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b' },
		  { 'b','s','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','b' },
		  { 'b','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','b' },
		  { 'b','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','b' },
		  { 'b','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','b' },
		  { 'b','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','b' },
		  { 'b','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','b' },
		  { 'b','.','.','.','.','.','.','.','.','.','.','.','.','.','.','b','.','.','.','b' },
		  { 'b','.','.','.','.','.','.','.','.','.','.','.','.','.','.','b','.','.','.','b' },
		  { 'b','.','.','.','.','.','.','.','.','.','.','.','.','.','.','b','.','.','.','b' },
		  { 'b','.','.','.','.','.','.','.','.','.','.','.','.','.','.','b','b','b','.','b' },
		  { 'b','.','.','.','.','.','.','.','.','.','.','.','.','.','.','b','e','.','.','b' },
		  { 'b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b','b' } };
		 
	private:
};
#endif