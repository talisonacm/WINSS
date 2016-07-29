/* -*-	Mode:C++; c-basic-offset:8; tab-width:8; indent-tabs-mode:t -*- */
/*
 * Copyright (c) 1994-1997 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the Computer Systems
 *	Engineering Group at Lawrence Berkeley Laboratory.
 * 4. Neither the name of the University nor of the Laboratory may be used
 *    to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/* This was created by Luigi Iannone <iannone@rp.lip6.fr>
 * To have a loss monitor that works for TCP connections
 * Code is based on NS-2 distribution, thus its copyright apply
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */



#include <tclcl.h>

#include "tcp-sink-monitor.h"

static class TcpSinkMonitorClass : public TclClass {
public:
	TcpSinkMonitorClass() : TclClass("Agent/TCPSinkMonitor") {}
	TclObject* create(int, const char*const*) {
		return (new TcpSinkMonitor(new Acker));
	}
} class_tcp_sink_mon;

TcpSinkMonitor::TcpSinkMonitor(Acker* acker): TcpSink(acker)
{
	recvbytes_ = 0;
	nlost_ = 0;
	ndup_ = 0;
	npkts_ = 0;
	expected_ = -1;
	last_packet_time_ = 0.;
	pktseqno_ = 0;
	bind("nlost_", &nlost_);
	bind("ndup_", &ndup_);
	bind("npkts_", &npkts_);
	bind("recvbytes_", &recvbytes_);
	bind("lastPktTime_", &last_packet_time_);
	bind("expected_", &expected_);
}

void TcpSinkMonitor::recv(Packet* pkt, Handler* handler_)
{

        // GgX- this is stripped from tcp-sink I've just added 
	// the update of other statistics

	int numToDeliver;
	int numBytes = hdr_cmn::access(pkt)->size();
	// number of bytes in the packet just received

	hdr_tcp *th = hdr_tcp::access(pkt);
	/* W.N. Check if packet is from previous incarnation */
	if (th->ts() < lastreset_) {
		// Remove packet and do nothing
		Packet::free(pkt);
		return;
	}
	acker_->update_ts(th->seqno(),th->ts(),ts_echo_rfc1323_);
	// update the timestamp to echo

	// GgX - updates received packets 
	// This counts also duplicated packets
	++npkts_;
	last_packet_time_ = Scheduler::instance().clock();

 	pktseqno_ = th->seqno();
	/*
	 * Check for lost packets based on TCP seqno
	 */
	if (expected_ >= 0) {
		int loss = pktseqno_ - expected_;
		// GgX - Some packets get lost before to arrive
		if (loss > 0) {
			nlost_ += loss;
			//Tcl::instance().evalf("%s log-loss", name());
		}
		// GgX - Some ACK packets get lost thus duplicated packets
		// Arrive
		if (loss < 0) {
			ndup_ += -loss;
			//Tcl::instance().evalf("%s log-dup", name());
		}
	}
	expected_ = pktseqno_ + 1;
	
	
      	numToDeliver = acker_->update(th->seqno(), numBytes);
	// update the recv window; figure out how many in-order-bytes
	// (if any) can be removed from the window and handed to the
	// application
	if (numToDeliver) {

		recvBytes(numToDeliver);
		// GgX - Update received bytes
		recvbytes_ += numToDeliver;
	};
	

	// send any packets to the application
      	ack(pkt);
	// ACK the packet
	Packet::free(pkt);
	// remove it from the system
      	
}

/*
 * $proc interval $interval
 * $proc size $size
 */
int TcpSinkMonitor::command(int argc, const char*const* argv)
{
	if (argc == 2) {
		if (strcmp(argv[1], "clear") == 0) {

			recvbytes_ = 0;
			nlost_ = 0;
			ndup_ = 0;
			npkts_ = 0;
			expected_ = -1;
			last_packet_time_ = 0.;
			pktseqno_ = 0;

			return (TCL_OK);
		}
	}
	return (TcpSink::command(argc, argv));
}
