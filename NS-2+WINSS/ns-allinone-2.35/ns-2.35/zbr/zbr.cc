#include "zbr.h"


static class ZbrClass : public TclClass {

public : 
	ZbrClass() : TclClass("Agent/Zbr") {}
	TclObject* create(int argc, const char*const* argv) {
		assert(argc == 5);
		return (new Zbr((nsaddr_t)Address::instance().str2addr(argv[4])));
	}
} class_rtProtoZbr;

void Zbr_PktTimer::expire(Event *e) {

// 	agent_->send_zbr_pkt();
// 	agent_->reset_zbr_pkt_timer();
}

// Zbr constructor
Zbr::Zbr(nsaddr_t id) : Agent(PT_ZBR), pkt_timer_(this),table_() {

	bind_bool("accessible_var_", &accessible_var_);
	ra_addr_ = id;
// 	fprintf(stdout,"(node %d) ZBR constructor\n",id);
	
}

int Zbr::command(int argc, const char*const* argv)
{
	if (argc == 2)
	{
		if (strcasecmp(argv[1], "start") == 0)
		{
			pkt_timer_.resched(0.0);
			return TCL_OK;
		}
		else 
		if (strcasecmp(argv[1], "print_rtable") == 0)
		{
			if (logtarget_ !=0) 
			{
				sprintf(logtarget_->pt_->buffer(), "P %f _%d_ Routing Table",CURRENT_TIME, ra_addr());
				logtarget_->pt_->dump();
				rtable_.print(logtarget_);
			}
			else 
			{
				fprintf(stdout, "%f _%d_ If you want to print this routing table you must create a trace file in your tcl script",CURRENT_TIME,ra_addr());	
			}
			return TCL_OK;
		}
			
	} 
	else
	if (argc ==3)
	{
		//Obtains corresponding dmux to carry packets to upper layers
		if (strcmp(argv[1], "port-dmux")==0)
		{
			dmux_=(PortClassifier*) TclObject::lookup(argv[2]);
			if (dmux_==0)
			{
				fprintf(stdout,"%s:%s lookup of %s failed \n",__FILE__,argv[1],argv[2]);
				return TCL_ERROR;
			}
			return TCL_OK;
					
		}
		else
		if (strcmp(argv[1], "tracetarget")==0 || strcmp(argv[1], "log-target")==0 )
		{
			logtarget_ = (Trace*) TclObject::lookup(argv[2]);
			if (logtarget_ == 0) return TCL_ERROR;
			return TCL_OK;
		}
	}
	//Pass the command to the base class
	return Agent::command(argc,argv);
}

void Zbr::recv(Packet* p, Handler* h)
{
	struct hdr_cmn* ch = HDR_CMN(p);
	struct hdr_ip*	ih = HDR_IP(p);
	
// 	
	
	if (ih->saddr() == ra_addr())
	{
		//if there exists  a loop, we must drop the packet
		if (ch->num_forwards() > 0)
		{
			drop(p, DROP_RTR_ROUTE_LOOP);
			return;
		}
		else
		if (ch->num_forwards() == 0) ch->size()+=IP_HDR_LEN;	
	}
	//otherwise, must forward the packet, unless TTL has reached zero
	
// 		fprintf(stdout,"we are here in the recv method\n");
		ih->ttl_--;
		if (ih->ttl_ == 0)
		{
			drop(p, DROP_RTR_TTL);
			return;
		}
	forward_data(p);

}



void Zbr::forward_data(Packet* p)
{
	struct hdr_cmn* ch = HDR_CMN(p);
	struct hdr_ip*	ih = HDR_IP(p);
	
	unsigned short destCT;
	
	if (ch->direction() == hdr_cmn::UP  && ((u_int32_t)ih->daddr() == IP_BROADCAST || ih->daddr() == ra_addr()))
	{
		dmux_->recv(p, 0);
		return;
	}
	else
	{
		ch->direction() = hdr_cmn::DOWN;
		ch->addr_type() = NS_AF_INET;
		if ((u_int32_t)ih->daddr() == IP_BROADCAST)
		{
			ch->next_hop() = IP_BROADCAST;
		}
		else
		{
			// here we add our protocol
			myNodeId = table_.returnCTAddr((unsigned short) ra_addr());
			myDepth = table_.returnCTDepth((unsigned short) ra_addr());
			myParent = table_.returnCTParent((unsigned short) ra_addr());
			destCT = table_.returnCTAddr((unsigned int) ih->daddr());
			//fprintf(stdout, "RTR -- Id = %d, depth = %d, parent =%d \n",myNodeId,myDepth,myParent);
			if (myNodeId == 0)
			{
				ch->next_hop() = (nsaddr_t) search(nextHop(destCT));
// 				fprintf(stdout, "[%d] Next hop is a child= %d\n",ra_addr(), search(nextHop(destCT)));
			} else
			if ((myNodeId < destCT) && (destCT < myNodeId+Cskip(myDepth-1, table_.Lm(), table_.Cm(), table_.Rm())))
			{
				
				ch->next_hop() = (nsaddr_t) search(nextHop(destCT));
// 				fprintf(stdout, "[%d] Next hop is a child= %d\n",ra_addr(), search(nextHop(destCT)));
				
			}			
			else			
			{
				ch->next_hop() = (nsaddr_t) search(myParent);
// 				fprintf(stdout, "[%d] Next hop is the father = %d\n",ra_addr(),ch->next_hop());
			} 
			
		}
		Scheduler::instance().schedule(target_,p, 0.0);
	}

}
// Cskip calculation

int Zbr::Cskip(short d, short Lm, short Cm, short Rm)
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

unsigned short Zbr::search(unsigned int CTAdd)
{	
	int i = 0;
	bool find = false;
	while (!find && i<1000)
	{
		if(table_.returnCTAddr(i)== (unsigned short) CTAdd)
		{
			find = true;
			break;
		}
		else 
		{
			i++;
		}
	}
	if (find == true) return i; 
	else return 1001;
}

unsigned short Zbr::nextHop(unsigned short dest)
{
	short Lm = table_.Lm();
	short Cm = table_.Cm();
	short Rm = table_.Rm();
	
	
	return myNodeId+1+ ((int) ((dest-(myNodeId+1))/Cskip(myDepth,Lm,Cm,Rm)))*Cskip(myDepth,Lm,Cm,Rm);
	
}