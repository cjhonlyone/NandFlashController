#include <stdio.h>
#include "xparameters.h"
#include <xil_io.h>
#include "sleep.h"
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

#define WP    0x80
#define RDY   0x40
#define ARDY  0x20
#define FAILC 0x02
#define FAIL  0x01

void select_way(uint32_t way);

void select_col(uint32_t col);

void select_row(uint32_t row);

void set_feature(uint32_t feature);

void set_length(uint32_t length);

void set_DelayTap(uint32_t DelayTap);

void reset_ffh(uint32_t way);

void setfeature_efh(uint32_t way, uint32_t feature);

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

#pragma pack(1)
struct nand_onfi_params {
	/* rev info and features block */
	/* 'O' 'N' 'F' 'I'  */
	u8 sig[4];
	u16 revision;
	u16 features;
	u16 opt_cmd;
	u8 reserved0[2];
	u16 ext_param_page_length; /* since ONFI 2.1 */
	u8 num_of_param_pages;        /* since ONFI 2.1 */
	u8 reserved1[17];

	/* manufacturer information block */
	char manufacturer[12];
	char model[20];
	u8 jedec_id;
	u16 date_code;
	u8 reserved2[13];

	/* memory organization block */
	u32 byte_per_page;
	u16 spare_bytes_per_page;
	u32 data_bytes_per_ppage;
	u16 spare_bytes_per_ppage;
	u32 pages_per_block;
	u32 blocks_per_lun;
	u8 lun_count;
	u8 addr_cycles;
	u8 bits_per_cell;
	u16 bb_per_lun;
	u16 block_endurance;
	u8 guaranteed_good_blocks;
	u16 guaranteed_block_endurance;
	u8 programs_per_page;
	u8 ppage_attr;
	u8 ecc_bits;
	u8 interleaved_bits;
	u8 interleaved_ops;
	u8 reserved3[13];

	/* electrical parameter block */
	u8 io_pin_capacitance_max;
	u16 async_timing_mode;
	u16 program_cache_timing_mode;
	u16 t_prog;
	u16 t_bers;
	u16 t_r;
	u16 t_ccs;
	u16 src_sync_timing_mode;
	u8 src_ssync_features;
	u16 clk_pin_capacitance_typ;
	u16 io_pin_capacitance_typ;
	u16 input_pin_capacitance_typ;
	u8 input_pin_capacitance_max;
	u8 driver_strength_support;
	u16 t_int_r;
	u16 t_adl;
	u8 reserved4[8];

	/* vendor */
	u16 vendor_revision;
	u8 vendor[88];

	u16 crc;
};

struct nand_onfi_vendor_micron {
	u8 two_plane_read;
	u8 read_cache;
	u8 read_unique_id;
	u8 dq_imped;
	u8 dq_imped_num_settings;
	u8 dq_imped_feat_addr;
	u8 rb_pulldown_strength;
	u8 rb_pulldown_strength_feat_addr;
	u8 rb_pulldown_strength_num_settings;
	u8 otp_mode;
	u8 otp_page_start;
	u8 otp_data_prot_addr;
	u8 otp_num_pages;
	u8 otp_feat_addr;
	u8 read_retry_options;
	u8 reserved[72];
	u8 param_revision;
};
