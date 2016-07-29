#ifndef TableCorresp_h
#define TableCorresp_h

#define MAX_ENTRY 1000
#define MAX_NEIGHBORS 10

class TableCorresp
{
	
	static unsigned short CorrespEntry[MAX_ENTRY] ; //table de correspondance Cluster tree address
	static unsigned short CorrespParent[MAX_ENTRY] ; // L'adresse CT du parent du noeud
	static unsigned short CorrespDepth[MAX_ENTRY]; //La profondeur du noeud 
	static bool isFFD[MAX_ENTRY];
	static short params[3];
	
	//New modification : Add neighbor list
 	static unsigned short Neighbors[MAX_ENTRY][MAX_NEIGHBORS];
	//end modification
	
	public:
	//----------------------Renvoie et Ajout de l'adresse Cluster Tree du noeud-------------------------------
	static unsigned short returnCTAddr(unsigned short entry)
	{
		return CorrespEntry[entry]; 
	}
	
	static void addCTAddr(unsigned short nodeId, unsigned short CTAddr)
	{
		CorrespEntry[nodeId]= CTAddr;
	}
	//----------------------Renvoie et Ajout de l'adresse des parents-------------------------------
	static unsigned short returnCTParent(unsigned short entry)
	{
		return CorrespParent[entry]; 
	}
	
	static void addCTParent(unsigned short nodeId, unsigned short CTParentAddr)
	{
		CorrespParent[nodeId]=CTParentAddr;
	}
	//----------------------Renvoie et Ajout du depth du noeud-------------------------------
	static unsigned short returnCTDepth(unsigned short entry)
	{
		return CorrespDepth[entry]; 
	}
	
	static void addCTDepth(unsigned short nodeId, unsigned short CTDepth)
	{
		CorrespDepth[nodeId]=CTDepth;
	}
	//----------------- Renvoie et ajout  de Lm Rm et Cm-------------------
	static short Lm()
	{
		return params[0];
	}
	
	static short Cm()
	{
		return params[1];
	}
	
	static short Rm()
	{
		return params[2];
	}
	static void addParams(short Lm, short Cm, short Rm)
	{
		params[0] = Lm;
		params[1] = Cm;
		params[2] = Rm;
	}
	//-------------------Renvoie et ajout si le noeud est FFD-------------
	static bool returnIsFFD(unsigned short entry)
	{
		return isFFD[entry];
	}
	static void addIsFFD(unsigned short nodeId, bool ffd)
	{
		isFFD[nodeId]=ffd;
	}
		//---------------Renvoie et ajoute la liste des voisins---------------
	static unsigned short *returnNeighborList(unsigned short entry)
	{
		return Neighbors[entry]; 
	}
	
	static void addNeighbor(unsigned short nodeId, unsigned short num, unsigned short neighbor)
	{
		Neighbors[nodeId][num]=neighbor;
	
	}
	void updateNeighborList(unsigned short nodeId, unsigned short neighbor);
	
};

#endif
