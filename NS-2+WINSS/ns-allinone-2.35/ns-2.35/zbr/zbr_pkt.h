#ifndef __zbr_pkt_h__
#define __zbr_pkt_h__

#include <packet.h>

#define HDR_ZBR_PKT(p) hdr_zbr_pkt::access(p)

struct hdr_zbr_pkt {

nsaddr_t pkt_src_; //node which originated this paquet
u_int16_t pkt_len_; //paquet lenght in bytes
u_int8_t pkt_seq_num_; //Paquet sequence number

inline nsaddr_t& pkt_src() { return pkt_src_;}
inline u_int16_t& pkt_len() { return pkt_len_;}
inline u_int8_t& pkt_seq_num(){return pkt_seq_num_;}

static int offset_;
inline static int& offset(){return offset_;}

inline static hdr_zbr_pkt* access(const Packet* p){
	return (hdr_zbr_pkt*)p->access(offset_);
}

};



#endif