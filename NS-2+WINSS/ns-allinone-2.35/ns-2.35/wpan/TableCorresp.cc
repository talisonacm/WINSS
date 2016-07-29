#include"TableCorresp.h"
#include<iostream>
#include<stdio.h>

using namespace std;

unsigned short TableCorresp::CorrespEntry[];
unsigned short TableCorresp::CorrespParent[];
unsigned short TableCorresp::CorrespDepth[];
unsigned short TableCorresp::Neighbors[][MAX_NEIGHBORS];
short TableCorresp::params[];
bool TableCorresp::isFFD[];
// TableCorresp::TableCorresp()
// {
// 	
// };

void TableCorresp::updateNeighborList(unsigned short nodeId, unsigned short neighbor)
{
	int i=0;
	bool found = false;
	while(Neighbors[nodeId][i]!=0)
	{
		if (Neighbors[nodeId][i] == neighbor)
		{
			found = true;
			break;
		}
		i++;
	}
	if((!found) && (neighbor !=0)) //add it
	{
		Neighbors[nodeId][i] = neighbor;
		fprintf(stdout, "node %d add node %d as neighbor \n",nodeId, neighbor);
	}
}
