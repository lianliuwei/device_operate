#include "canscope/device/frame_device/sja_btr_util.h"

namespace {
static const uint32 kClk = 16*1000*1000; // 16M

struct SJA_BRT {
  union {
  uint16 raw_value;
  struct {
    uint16 BRP:6;   
    uint16 SJW:2;   
    uint16 SEG1:4;  
    uint16 SEG2:3;  
    uint16 SAM:1; 
  } detial;
  };
};

}


int SJABtrToBaudRate(uint16 sja_btr) {
  SJA_BRT sja;
  sja.raw_value = sja_btr;
  return kClk / (2 * (sja.detial.BRP+1) * (sja.detial.SEG1+sja.detial.SEG2+3));
}
