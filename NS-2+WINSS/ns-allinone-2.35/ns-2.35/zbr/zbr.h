#ifndef __zbr_h__
#define __zbr_h__

#include "zbr_pkt.h"
#include "zbr_rtable.h"
#include <agent.h>
#include <packet.h>
#include <trace.h>
#include <timer-handler.h>
#include <random.h>
#include <classifier-port.h>
#include <address.h>
#include <cmu-trace.h>
#include "../wpan/TableCorresp.h"
#include <math.h>

#define CURRENT_TIME Scheduler::instance().clock()
#define JITTER (Random::uniform()*0.5)

class Zbr; //forward declaration

	/* Timers */

class Zbr_PktTimer : public TimerHandler {
public : 
	Zbr_PktTimer(Zbr* agent) : TimerHandler() {
		
		agent_ = agent;
	}
protected : 
	Zbr* agent_;
	virtual void expire(Event* e);
};

	/* Agent */
class Zbr : public Agent {
	
	/* Friends */
	friend class Zbr_PktTimer;
	
	/* Private members */
	nsaddr_t ra_addr_;
// 	zbr_state state_;
	zbr_rtable rtable_;
	int accessible_var_;
	u_int8_t seq_num_;
	TableCorresp table_;
	
		
protected : 
	
	PortClassifier* dmux_; // for passing packet up to agents
	Trace*		logtarget_; //for logging
	Zbr_PktTimer	pkt_timer_; //Timer for sending packets
	
	inline nsaddr_t&	ra_addr() 	{return ra_addr_;}
// 	inline zbr_state&	state() 	{return state_;}
	inline int&		accessible_var(){return accessible_var_;}
	
	void forward_data(Packet*);
	int Cskip(short d,short Lm, short Cm, short Rm);
	unsigned short search(unsigned int CTAdd);
	unsigned short nextHop(unsigned short dest);
	unsigned int myNodeId;
	unsigned short myDepth;
	unsigned short myParent;	
public : 
	Zbr(nsaddr_t);
	int command(int, const char*const*);
	void recv(Packet*, Handler*);
		

};


#endif