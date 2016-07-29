#include "zigBR.h"
#include <math.h>


int zigBR::Cskip(short d)
{
	if (d>Lm) return -1;
	if (Rm==1)
	{
		return 1+Cm*(Lm-d-1);
	}
	else 
	{
		return (1+Cm-Rm-Cm* (int) pow(Rm,Lm-d-1))/(1-Rm);
	}
}
