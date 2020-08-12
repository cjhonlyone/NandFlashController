#include <stdio.h>
#include "xparameters.h"
#include <xil_io.h>

#define NFC XPAR_NANDFLASHCONTROLLER_0_BASEADDR

#define rCommand      0x00
#define rAddress      0x04
#define rLength       0x08
#define rDMARAddress  0x0C
#define rDMAWAddress  0x10
#define rFeature      0x14
#define rCommandFail  0x18
#define rNFCStatus    0x1C
#define rNandRBStatus 0x20
#define rDelayTap     0x24


void select_way(uint32_t way);

void select_col(uint32_t col);

void select_row(uint32_t row);

void set_feature(uint32_t feature);

void set_length(uint32_t length);

void set_DelayTap(uint32_t DelayTap);

void reset_ffh(uint32_t way);

void setfeature_efh(uint32_t way);

void getfeature_eeh(uint32_t way);

void readparameterpage(uint32_t way, uint32_t DMAWAddress);

void progpage_80h_10h(uint32_t way, uint32_t col, uint32_t row, uint32_t length, uint32_t DMARAddress);

void progpage_80h_15h_cache(uint32_t way, uint32_t col, uint32_t row, uint32_t length, uint32_t DMARAddress);

void progpage_80h_11h_multplane(uint32_t way, uint32_t col, uint32_t row, uint32_t length, uint32_t DMARAddress);

void readpage_00h_30h(uint32_t way, uint32_t col, uint32_t row, uint32_t length, uint32_t DMAWAddress);

void eraseblock_60h_d0h(uint32_t way, uint32_t row);

void eraseblock_60h_d1h_multiplane(uint32_t way, uint32_t row);

uint8_t readstatus_70h(uint32_t way);

uint8_t readstatus_78h(uint32_t way, uint32_t row);
