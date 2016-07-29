#include "zbr_pkt.h"

int hdr_zbr_pkt::offset_;
static class ZbrHeaderClass : public PacketHeaderClass {
public : 
 ZbrHeaderClass() : PacketHeaderClass ("PacketHeader/Zbr", sizeof(hdr_zbr_pkt)) {
	
	bind_offset(&hdr_zbr_pkt::offset_);
 
 }
} class_rtProtoZbr_hdr;
