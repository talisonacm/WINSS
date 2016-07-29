#ifndef __zbr_rtable_h__
#define __zbr_rtable_h__

#include <trace.h>
#include <map>

typedef std::map<nsaddr_t, nsaddr_t> rtable_t;

class zbr_rtable
{
	rtable_t rt_;
 public: 
	
 	zbr_rtable();
	void 		print(Trace*);
	void 		clear();
	void 		rm_entry(nsaddr_t);
	void 		add_entry(nsaddr_t, nsaddr_t);
	nsaddr_t	lookup(nsaddr_t);
	u_int32_t	size();
	
		
};

#endif