#ifndef zigBR_h
#define zigBR_h

class zigBR
{
		
	public:
// 	bool firstServed;	
	short Lm; 		// Maximum depth
	short Cm;		// Network maximum children
	short Rm;		// Max Routers

	int myDepth;
	int myNodeID;
	int myParentNodeID;
	
// 	zigBR(short L, short C, short R)
	zigBR()
	{
		Lm=0;
		Cm=0;
		Rm=0;
		myDepth=0;
		myNodeID=0;
		myParentNodeID=0;
// 		firstServed = false;
	}
// 	zigBR(unsigned short L, unsigned short C, unsigned short R );
	int Cskip(short d);
	

};

#endif
