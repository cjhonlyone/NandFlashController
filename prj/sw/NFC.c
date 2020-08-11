#inlcude "NFC.h"

void select_way(uint32_t way)
{
	Xil_Out32(NFC+rAddress, way);
	Xil_Out32(NFC+rCommand, 0x00000020);	
}
void select_col(uint32_t col)
{
	Xil_Out32(NFC+rAddress, col);
	Xil_Out32(NFC+rCommand, 0x00000022);	
}
void select_row(uint32_t row)
{
	Xil_Out32(NFC+rAddress, row);
	Xil_Out32(NFC+rCommand, 0x00000024);	
}
void set_feature(uint32_t feature)
{
	Xil_Out32(NFC+rAddress, feature);
	Xil_Out32(NFC+rCommand, 0x00000028);	
}
void set_length(uint32_t length)
{
	Xil_Out32(NFC+rLength, length);
}

void set_DelayTap(uint32_t DelayTap)
{
	Xil_Out32(NFC+rDelayTap, DelayTap);
}
void reset_ffh(uint32_t way)
{
	select_way(way);
	Xil_Out32(NFC+rCommand, 0x00000001);	
	while(((Xil_In32(NFC+rNFCStatus) & 0x00000001) == 0x00000000));
	while(((Xil_In32(NFC+rNFCStatus) & 0x00000001) == 0x00000001));
}
void setfeature_efh(uint32_t way)
{
	select_way(way);
	set_feature(0x15000000);
	Xil_Out32(NFC+rCommand, 0x00000002);	
	while(((Xil_In32(NFC+rNFCStatus) & 0x00000001) == 0x00000000));
	while(((Xil_In32(NFC+rNFCStatus) & 0x00000001) == 0x00000001));
}
void getfeature_eeh(uint32_t way)
{
	select_way(way);
	Xil_Out32(NFC+rCommand, 0x00000005);	
	while(((Xil_In32(NFC+rNFCStatus) & 0x00000001) == 0x00000000));
	while(((Xil_In32(NFC+rNFCStatus) & 0x00000001) == 0x00000001));
}
void progpage_80h_10h(uint32_t way, uint32_t col, uint32_t row, uint32_t length, uint32_t DMARAddress)
{
	select_way(way);
	select_col(col);
	select_row(row);
	set_length(length);
	Xil_Out32(NFC+rDMARAddress, DMARAddress);
	Xil_Out32(NFC+rCommand, 0x00000003);	
	while(((Xil_In32(NFC+rNFCStatus) & 0x00000001) == 0x00000000));
	while(((Xil_In32(NFC+rNFCStatus) & 0x00000001) == 0x00000001));
}
void progpage_80h_15h_cache(uint32_t way, uint32_t col, uint32_t row, uint32_t length, uint32_t DMARAddress)
{
	select_way(way);
	select_col(col);
	select_row(row);
	set_length(length);
	Xil_Out32(NFC+rDMARAddress, DMARAddress);
	Xil_Out32(NFC+rCommand, 0x00010003);	
	while(((Xil_In32(NFC+rNFCStatus) & 0x00000001) == 0x00000000));
	while(((Xil_In32(NFC+rNFCStatus) & 0x00000001) == 0x00000001));
}
void progpage_80h_11h_multplane(uint32_t way, uint32_t col, uint32_t row, uint32_t length, uint32_t DMARAddress)
{
	select_way(way);
	select_col(col);
	select_row(row);
	set_length(length);
	Xil_Out32(NFC+rDMARAddress, DMARAddress);
	Xil_Out32(NFC+rCommand, 0x00020003);	
	while(((Xil_In32(NFC+rNFCStatus) & 0x00000001) == 0x00000000));
	while(((Xil_In32(NFC+rNFCStatus) & 0x00000001) == 0x00000001));
}
void readpage_00h_30h(uint32_t way, uint32_t col, uint32_t row, uint32_t length, uint32_t DMAWAddress)
{
	select_way(way);
	select_col(col);
	select_row(row);
	set_length(length);
	Xil_Out32(NFC+rDMAWAddress, DMAWAddress);
	Xil_Out32(NFC+rCommand, 0x00000004);	
	while(((Xil_In32(NFC+rNFCStatus) & 0x00000001) == 0x00000000));
	while(((Xil_In32(NFC+rNFCStatus) & 0x00000001) == 0x00000001));
}
void eraseblock_60h_d0h(uint32_t way, uint32_t row)
{
	select_way(way);
	select_row(row);
	Xil_Out32(NFC+rCommand, 0x00000006);	
	while(((Xil_In32(NFC+rNFCStatus) & 0x00000001) == 0x00000000));
	while(((Xil_In32(NFC+rNFCStatus) & 0x00000001) == 0x00000001));
}
void eraseblock_60h_d1h_multiplane(uint32_t way, uint32_t row)
{
	select_way(way);
	select_row(row);
	Xil_Out32(NFC+rCommand, 0x00020006);	
	while(((Xil_In32(NFC+rNFCStatus) & 0x00000001) == 0x00000000));
	while(((Xil_In32(NFC+rNFCStatus) & 0x00000001) == 0x00000001));
}

uint8_t readstatus_70h(uint32_t way)
{
	select_way(way);
	Xil_Out32(NFC+rCommand, 0x00000007);	
	while(((Xil_In32(NFC+rNFCStatus) & 0x00000001) == 0x00000000));
	while(((Xil_In32(NFC+rNFCStatus) & 0x00000001) == 0x00000001));

	u8 status = ((Xil_In32(NFC+rNFCStatus) & 0x0000ff00) >> 8);
	return status;
}
uint8_t readstatus_78h(uint32_t way, uint32_t row)
{
	select_way(way);
	select_row(row);
	Xil_Out32(NFC+rCommand, 0x00010007);	

	while(((Xil_In32(NFC+rNFCStatus) & 0x00000001) == 0x00000000));
	while(((Xil_In32(NFC+rNFCStatus) & 0x00000001) == 0x00000001));

	u8 status = ((Xil_In32(NFC+rNFCStatus) & 0x0000ff00) >> 8);
	return status;
}