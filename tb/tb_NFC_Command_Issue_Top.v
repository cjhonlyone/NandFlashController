`timescale 1ps / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2020/07/02 22:39:41
// Design Name: 
// Module Name: tb_phy
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module tb_NFC_Command_Issue_Top;

	parameter IDelayValue          = 7;
	parameter InputClockBufferType = 0;
	parameter NumberOfWays         = 2;
	
    reg                           iSystemClock            ; // SDR 100MHz
    reg                           iDelayRefClock          ; // SDR 200Mhz
    reg                           iOutputDrivingClock     ; // SDR 200Mhz
    reg                           iReset                  ;
	// glbl glbl();
	// 100 MHz
	initial                 
	begin
		iSystemClock     <= 1'b0;
		#10000;
		forever
		begin	 
			iSystemClock <= 1'b1;
			#5000;
			iSystemClock <= 1'b0;
			#5000;
		end
	end

	// 200 MHz
	initial                 
	begin
		iDelayRefClock          <= 1'b0;
		iOutputDrivingClock     <= 1'b0;
		#10000;
		forever
		begin	 
			iDelayRefClock      <= 1'b1;
			iOutputDrivingClock <= 1'b1;
			#2500;
			iDelayRefClock      <= 1'b0;
			iOutputDrivingClock <= 1'b0;
			#2500;
		end
	end
 
    reg   [5:0]                   iTop_CI_Opcode              ;
    reg   [4:0]                   iTop_CI_TargetID            ;
    reg   [4:0]                   iTop_CI_SourceID            ;
    reg   [31:0]                  iTop_CI_Address             ;
    reg   [15:0]                  iTop_CI_Length              ;
    reg                           iTop_CI_CMDValid            ;
    wire                          oCI_Top_CMDReady            ;

    reg   [15:0]                  iTop_CI_WriteData           ;
    reg                           iTop_CI_WriteLast           ;
    reg                           iTop_CI_WriteValid          ;
    reg   [1:0]                   iTop_CI_WriteKeep           ;
    wire                          oCI_Top_WriteReady          ;

    wire  [15:0]                  oCI_Top_ReadData            ;
    wire                          oCI_Top_ReadLast            ;
    wire                          oCI_Top_ReadValid           ;
    wire   [1:0]                  oCI_Top_ReadKeep            ;
    reg                           iTop_CI_ReadReady           ;

    wire  [NumberOfWays - 1:0]    oCI_Top_ReadyBusy           ;

    // CI with ACG
    wire  [7:0]                   wCI_ACG_Command             ;
    wire  [2:0]                   wCI_ACG_CommandOption       ;

    wire  [7:0]                   wACG_CI_Ready               ;
    wire  [7:0]                   wACG_CI_LastStep            ;

    wire  [NumberOfWays - 1:0]    wCI_ACG_TargetWay           ;
    wire  [15:0]                  wCI_ACG_NumOfData           ;

    wire                          wCI_ACG_CASelect            ;
    wire  [39:0]                  wCI_ACG_CAData              ;

    wire  [15:0]                  wCI_ACG_WriteData           ;
    wire                          wCI_ACG_WriteLast           ;
    wire                          wCI_ACG_WriteValid          ;
    wire                          wACG_CI_WriteReady          ;

    wire  [15:0]                  wACG_CI_ReadData            ;
    wire                          wACG_CI_ReadLast            ;
    wire                          wACG_CI_ReadValid           ;
    wire                          wCI_ACG_ReadReady           ;

    wire  [NumberOfWays - 1:0]    wACG_CI_ReadyBusy           ;

// ACG with PHY
    // reset from ACG
    wire                          wACG_PHY_PinIn_Reset        ;
    wire                          wACG_PHY_PinIn_BUFF_Reset   ;
    wire                          wACG_PHY_PinOut_Reset       ;

    // unused
    wire                          wPI_BUFF_RE                 ;
    wire  [2:0]                   wPI_BUFF_OutSel             ;
    wire  [31:0]                  wPI_DQ                      ;
    wire                          wPI_ValidFlag               ;

    // DQs delay tap for aligment with DQ
    wire                          wACG_PHY_DelayTapLoad       ;
    wire  [4:0]                   wACG_PHY_DelayTap           ;
    wire                          wPHY_ACG_DelayReady         ;
    wire                          wACG_PHY_DQSOutEnable       ;
    wire                          wACG_PHY_DQOutEnable        ;
    wire  [7:0]                   wACG_PHY_DQStrobe           ;
    wire  [31:0]                  wACG_PHY_DQ                 ;
    wire  [2*NumberOfWays - 1:0]  wACG_PHY_ChipEnable         ;
    wire  [3:0]                   wACG_PHY_ReadEnable         ;
    wire  [3:0]                   wACG_PHY_WriteEnable        ;
    wire  [3:0]                   wACG_PHY_AddressLatchEnable ;
    wire  [3:0]                   wACG_PHY_CommandLatchEnable ;

    wire  [NumberOfWays - 1:0]    wPHY_ACG_ReadyBusy          ;
    wire                          wACG_PHY_WriteProtect       ;

    // enable nand to PHY reg buffwr
    wire                          wACG_PHY_BUFF_WE            ;
    wire                          wPHY_ACG_BUFF_Empty         ;

    // read data from PHY reg
    wire                          wACG_PHY_Buff_Ready         ;
    wire                          wPHY_ACG_Buff_Valid         ;
    wire  [15:0]                  wPHY_ACG_Buff_Data          ;
    wire  [ 1:0]                  wPHY_ACG_Buff_Keep          ;
    wire                          wPHY_ACG_Buff_Last          ;

// pinpad
	wire                          IO_NAND_DQS                 ;
	wire                  [7:0]   IO_NAND_DQ                  ;
	wire   [NumberOfWays - 1:0]   O_NAND_CE                   ;
	wire                          O_NAND_WE                   ;
	wire                          O_NAND_RE                   ;
	wire                          O_NAND_ALE                  ;
	wire                          O_NAND_CLE                  ;
	wire   [NumberOfWays - 1:0]   I_NAND_RB                   ;
	wire                          O_NAND_WP                   ;

    NFC_Command_Issue_Top #(
            .NumberOfWays(NumberOfWays)
        ) inst_NFC_Command_Issue_Top (
            .iSystemClock          (iSystemClock),
            .iReset                (iReset),
            .iTop_CI_Opcode        (iTop_CI_Opcode),
            .iTop_CI_TargetID      (iTop_CI_TargetID),
            .iTop_CI_SourceID      (iTop_CI_SourceID),
            .iTop_CI_Address       (iTop_CI_Address),
            .iTop_CI_Length        (iTop_CI_Length),
            .iTop_CI_CMDValid      (iTop_CI_CMDValid),
            .oCI_Top_CMDReady      (oCI_Top_CMDReady),

            .iTop_CI_WriteData     (iTop_CI_WriteData),
            .iTop_CI_WriteLast     (iTop_CI_WriteLast),
            .iTop_CI_WriteValid    (iTop_CI_WriteValid),
            .iTop_CI_WriteKeep     (iTop_CI_WriteKeep),
            .oCI_Top_WriteReady    (oCI_Top_WriteReady),

            .oCI_Top_ReadData      (oCI_Top_ReadData),
            .oCI_Top_ReadLast      (oCI_Top_ReadLast),
            .oCI_Top_ReadValid     (oCI_Top_ReadValid),
            .oCI_Top_ReadKeep      (oCI_Top_ReadKeep),
            .iTop_CI_ReadReady     (iTop_CI_ReadReady),

            .oCI_Top_ReadyBusy     (oCI_Top_ReadyBusy),

            .oCI_ACG_Command       (wCI_ACG_Command),
            .oCI_ACG_CommandOption (wCI_ACG_CommandOption),
            .iACG_CI_Ready         (wACG_CI_Ready),
            .iACG_CI_LastStep      (wACG_CI_LastStep),
            .oCI_ACG_TargetWay     (wCI_ACG_TargetWay),
            .oCI_ACG_NumOfData     (wCI_ACG_NumOfData),
            .oCI_ACG_CASelect      (wCI_ACG_CASelect),
            .oCI_ACG_CAData        (wCI_ACG_CAData),
            .oCI_ACG_WriteData     (wCI_ACG_WriteData),
            .oCI_ACG_WriteLast     (wCI_ACG_WriteLast),
            .oCI_ACG_WriteValid    (wCI_ACG_WriteValid),
            .iACG_CI_WriteReady    (wACG_CI_WriteReady),
            .iACG_CI_ReadData      (wACG_CI_ReadData),
            .iACG_CI_ReadLast      (wACG_CI_ReadLast),
            .iACG_CI_ReadValid     (wACG_CI_ReadValid),
            .oCI_ACG_ReadReady     (wCI_ACG_ReadReady),
            .iACG_CI_ReadyBusy     (wACG_CI_ReadyBusy)
        );

    NFC_Atom_Command_Generator_Top #(
            .NumberOfWays(NumberOfWays)
        ) inst_NFC_Atom_Command_Generator_Top (
            .iSystemClock                (iSystemClock),
            .iReset                      (iReset),

            .iCI_ACG_Command             (wCI_ACG_Command),
            .iCI_ACG_CommandOption       (wCI_ACG_CommandOption),
            .oACG_CI_Ready               (wACG_CI_Ready),
            .oACG_CI_LastStep            (wACG_CI_LastStep),
            .iCI_ACG_TargetWay           (wCI_ACG_TargetWay),
            .iCI_ACG_NumOfData           (wCI_ACG_NumOfData),
            .iCI_ACG_CASelect            (wCI_ACG_CASelect),
            .iCI_ACG_CAData              (wCI_ACG_CAData),
            .iCI_ACG_WriteData           (wCI_ACG_WriteData),
            .iCI_ACG_WriteLast           (wCI_ACG_WriteLast),
            .iCI_ACG_WriteValid          (wCI_ACG_WriteValid),
            .oACG_CI_WriteReady          (wACG_CI_WriteReady),
            .oACG_CI_ReadData            (wACG_CI_ReadData),
            .oACG_CI_ReadLast            (wACG_CI_ReadLast),
            .oACG_CI_ReadValid           (wACG_CI_ReadValid),
            .iCI_ACG_ReadReady           (wCI_ACG_ReadReady),
            .oACG_CI_ReadyBusy           (wACG_CI_ReadyBusy),

            .oACG_PHY_PinIn_Reset        (wACG_PHY_PinIn_Reset),
            .oACG_PHY_PinIn_BUFF_Reset   (wACG_PHY_PinIn_BUFF_Reset),
            .oACG_PHY_PinOut_Reset       (wACG_PHY_PinOut_Reset),
            .iPI_BUFF_RE                 (wPI_BUFF_RE),
            .iPI_BUFF_OutSel             (wPI_BUFF_OutSel),
            .oPI_DQ                      (wPI_DQ),
            .oPI_ValidFlag               (wPI_ValidFlag),
            .oACG_PHY_DelayTapLoad       (wACG_PHY_DelayTapLoad),
            .oACG_PHY_DelayTap           (wACG_PHY_DelayTap),
            .iPHY_ACG_DelayReady         (wPHY_ACG_DelayReady),
            .oACG_PHY_DQSOutEnable       (wACG_PHY_DQSOutEnable),
            .oACG_PHY_DQOutEnable        (wACG_PHY_DQOutEnable),
            .oACG_PHY_DQStrobe           (wACG_PHY_DQStrobe),
            .oACG_PHY_DQ                 (wACG_PHY_DQ),
            .oACG_PHY_ChipEnable         (wACG_PHY_ChipEnable),
            .oACG_PHY_ReadEnable         (wACG_PHY_ReadEnable),
            .oACG_PHY_WriteEnable        (wACG_PHY_WriteEnable),
            .oACG_PHY_AddressLatchEnable (wACG_PHY_AddressLatchEnable),
            .oACG_PHY_CommandLatchEnable (wACG_PHY_CommandLatchEnable),
            .iPHY_ACG_ReadyBusy          (wPHY_ACG_ReadyBusy),
            .oACG_PHY_WriteProtect       (wACG_PHY_WriteProtect),
            .oACG_PHY_BUFF_WE            (wACG_PHY_BUFF_WE),
            .iPHY_ACG_BUFF_Empty         (wPHY_ACG_BUFF_Empty),
            .oACG_PHY_Buff_Ready         (wACG_PHY_Buff_Ready),
            .iPHY_ACG_Buff_Valid         (wPHY_ACG_Buff_Valid),
            .iPHY_ACG_Buff_Data          (wPHY_ACG_Buff_Data),
            .iPHY_ACG_Buff_Keep          (wPHY_ACG_Buff_Keep),
            .iPHY_ACG_Buff_Last          (wPHY_ACG_Buff_Last)
        );

    NFC_Physical_Top #(
            .IDelayValue(IDelayValue),
            .InputClockBufferType(InputClockBufferType),
            .NumberOfWays(NumberOfWays)
        ) inst_NFC_Physical_Top (
            .iSystemClock                (iSystemClock),
            .iDelayRefClock              (iDelayRefClock),
            .iOutputDrivingClock         (iOutputDrivingClock),
            .iACG_PHY_PinIn_Reset        (wACG_PHY_PinIn_Reset),
            .iACG_PHY_PinIn_BUFF_Reset   (wACG_PHY_PinIn_BUFF_Reset),
            .iACG_PHY_PinOut_Reset       (wACG_PHY_PinOut_Reset),
            .iPI_BUFF_RE                 (wPI_BUFF_RE),
            .iPI_BUFF_OutSel             (wPI_BUFF_OutSel),
            .oPI_DQ                      (wPI_DQ),
            .oPI_ValidFlag               (wPI_ValidFlag),
            .iACG_PHY_DelayTapLoad       (wACG_PHY_DelayTapLoad),
            .iACG_PHY_DelayTap           (wACG_PHY_DelayTap),
            .oPHY_ACG_DelayReady         (wPHY_ACG_DelayReady),
            .iACG_PHY_DQSOutEnable       (wACG_PHY_DQSOutEnable),
            .iACG_PHY_DQOutEnable        (wACG_PHY_DQOutEnable),
            .iACG_PHY_DQStrobe           (wACG_PHY_DQStrobe),
            .iACG_PHY_DQ                 (wACG_PHY_DQ),
            .iACG_PHY_ChipEnable         (wACG_PHY_ChipEnable),
            .iACG_PHY_ReadEnable         (wACG_PHY_ReadEnable),
            .iACG_PHY_WriteEnable        (wACG_PHY_WriteEnable),
            .iACG_PHY_AddressLatchEnable (wACG_PHY_AddressLatchEnable),
            .iACG_PHY_CommandLatchEnable (wACG_PHY_CommandLatchEnable),
            .oPHY_ACG_ReadyBusy          (wPHY_ACG_ReadyBusy),
            .iACG_PHY_WriteProtect       (wACG_PHY_WriteProtect),
            .iACG_PHY_BUFF_WE            (wACG_PHY_BUFF_WE),
            .oPHY_ACG_BUFF_Empty         (wPHY_ACG_BUFF_Empty),
            .iACG_PHY_Buff_Ready         (wACG_PHY_Buff_Ready),
            .oPHY_ACG_Buff_Valid         (wPHY_ACG_Buff_Valid),
            .oPHY_ACG_Buff_Data          (wPHY_ACG_Buff_Data),
            .oPHY_ACG_Buff_Keep          (wPHY_ACG_Buff_Keep),
            .oPHY_ACG_Buff_Last          (wPHY_ACG_Buff_Last),
            .IO_NAND_DQS                 (IO_NAND_DQS),
            .IO_NAND_DQ                  (IO_NAND_DQ),
            .O_NAND_CE                   (O_NAND_CE),
            .O_NAND_WE                   (O_NAND_WE),
            .O_NAND_RE                   (O_NAND_RE),
            .O_NAND_ALE                  (O_NAND_ALE),
            .O_NAND_CLE                  (O_NAND_CLE),
            .I_NAND_RB                   (I_NAND_RB),
            .O_NAND_WP                   (O_NAND_WP)
        );

	nand_model nand_b0_1 (
        //clocks
        .Clk_We_n(O_NAND_WE), //same connection to both wen/nclk
        
        //CE
        .Ce_n(O_NAND_CE[0]),
        .Ce2_n(O_NAND_CE[1]),
        
        //Ready/busy
        .Rb_n(I_NAND_RB[0]),
        .Rb2_n(I_NAND_RB[1]),
         
        //DQ DQS
        .Dqs(IO_NAND_DQS), 
        //Reversed DQ
        .Dq_Io(IO_NAND_DQ),
         
        //ALE CLE WR WP
        .Cle(O_NAND_CLE), 
        //.Cle2(),
        .Ale(O_NAND_ALE), 
        //.Ale2(),
        .Wr_Re_n(O_NAND_RE), 
        //.Wr_Re2_n(),
        .Wp_n(O_NAND_WP) 
        //.Wp2_n()
        );  

// // Parameters
// parameter DEPTH = 32;
// parameter DATA_WIDTH = 32;
// parameter KEEP_ENABLE = (DATA_WIDTH>8);
// parameter KEEP_WIDTH = (DATA_WIDTH/8);
// parameter LAST_ENABLE = 1;
// parameter ID_ENABLE = 1;
// parameter ID_WIDTH = 8;
// parameter DEST_ENABLE = 1;
// parameter DEST_WIDTH = 8;
// parameter USER_ENABLE = 1;
// parameter USER_WIDTH = 1;
// parameter FRAME_FIFO = 1;
// parameter USER_BAD_FRAME_VALUE = 1'b1;
// parameter USER_BAD_FRAME_MASK = 1'b1;
// parameter DROP_BAD_FRAME = 0;
// parameter DROP_WHEN_FULL = 0;

//     reg [31:0]       s_axis_tdata  ;
//     reg [ 3:0]       s_axis_tkeep  ;
//     reg              s_axis_tvalid ;
//     wire             s_axis_tready ;
//     reg              s_axis_tlast  ;

//     wire [ 3:0]      m_axis_tkeep  ;

//     axis_fifo #(
//         .DEPTH(DEPTH),
//         .DATA_WIDTH(DATA_WIDTH),
//         .KEEP_ENABLE(KEEP_ENABLE),
//         .KEEP_WIDTH(KEEP_WIDTH),
//         .LAST_ENABLE(LAST_ENABLE),
//         .ID_ENABLE(ID_ENABLE),
//         .ID_WIDTH(ID_WIDTH),
//         .DEST_ENABLE(DEST_ENABLE),
//         .DEST_WIDTH(DEST_WIDTH),
//         .USER_ENABLE(USER_ENABLE),
//         .USER_WIDTH(USER_WIDTH),
//         .FRAME_FIFO(FRAME_FIFO),
//         .USER_BAD_FRAME_VALUE(USER_BAD_FRAME_VALUE),
//         .USER_BAD_FRAME_MASK(USER_BAD_FRAME_MASK),
//         .DROP_BAD_FRAME(DROP_BAD_FRAME),
//         .DROP_WHEN_FULL(DROP_WHEN_FULL)
//     ) inst_axis_fifo (
//             .clk               (iSystemClock),
//             .rst               (iReset),

//             .s_axis_tdata      (s_axis_tdata),
//             .s_axis_tkeep      (s_axis_tkeep),
//             .s_axis_tvalid     (s_axis_tvalid),
//             .s_axis_tready     (s_axis_tready),
//             .s_axis_tlast      (s_axis_tlast),

//             .m_axis_tdata      (iCI_ACG_WriteData),
//             .m_axis_tkeep      (m_axis_tkeep),
//             .m_axis_tvalid     (iCI_ACG_WriteValid),
//             .m_axis_tready     (oACG_CI_WriteReady),
//             .m_axis_tlast      (iCI_ACG_WriteLast),

//             .status_overflow   (status_overflow),
//             .status_bad_frame  (status_bad_frame),
//             .status_good_frame (status_good_frame)
//         );

task s_axis_input;
    begin
        @(posedge iSystemClock);   
        // iTop_CI_WriteValid <= 0;
        // iTop_CI_WriteData<= 16'h0000;
        // iTop_CI_WriteKeep<= 2'b00;
        // iTop_CI_WriteLast<= 0;

        // @(posedge iSystemClock);   
        // iTop_CI_WriteValid <= 1;
        // iTop_CI_WriteData<= 16'h1500;
        // iTop_CI_WriteKeep<= 2'b11;
        // iTop_CI_WriteLast<= 0;

        // @(posedge iSystemClock);   
        // iTop_CI_WriteValid <= 1;
        // iTop_CI_WriteData<= 16'h0000;
        // iTop_CI_WriteKeep<= 2'b11;
        // iTop_CI_WriteLast<= 1;

        // @(posedge iSystemClock);   
        // iTop_CI_WriteValid <= 0;
        // iTop_CI_WriteData<= 16'h0000;
        // iTop_CI_WriteKeep<= 2'b11;
        // iTop_CI_WriteLast<= 1;

        @(posedge iSystemClock);   
        iTop_CI_WriteValid <= 1;
        iTop_CI_WriteData<= 16'h0102;
        iTop_CI_WriteKeep<= 2'b11;
        iTop_CI_WriteLast<= 0;
        @(posedge iSystemClock);   
        iTop_CI_WriteValid <= 1;
        iTop_CI_WriteData<= 16'h0304;
        iTop_CI_WriteKeep<= 2'b11;
        iTop_CI_WriteLast<= 0;
        @(posedge iSystemClock);   
        iTop_CI_WriteValid <= 1;
        iTop_CI_WriteData<= 16'h0506;
        iTop_CI_WriteKeep<= 2'b11;
        iTop_CI_WriteLast<= 0;
        @(posedge iSystemClock);   
        iTop_CI_WriteValid <= 1;
        iTop_CI_WriteData<= 16'h0708;
        iTop_CI_WriteKeep<= 2'b11;
        iTop_CI_WriteLast<= 1;
        @(posedge iSystemClock);   
        iTop_CI_WriteValid <= 0;
        iTop_CI_WriteData<= 16'h0000;
        iTop_CI_WriteKeep<= 2'b11;
        iTop_CI_WriteLast<= 1;
    end
endtask

task NFC_CI_signal;
    input   [5:0]                   rTop_CI_Opcode              ;
    input   [4:0]                   rTop_CI_TargetID            ;
    input   [4:0]                   rTop_CI_SourceID            ;
    input   [31:0]                  rTop_CI_Address             ;
    input   [15:0]                  rTop_CI_Length              ;
    input                           rTop_CI_CMDValid            ;
    input   [31:0]                  rTop_CI_WriteData           ;
    input                           rTop_CI_WriteLast           ;
    input                           rTop_CI_WriteValid          ;
    input                           rTop_CI_ReadReady           ;

	begin
		@(posedge iSystemClock);   
            iTop_CI_Opcode     <= rTop_CI_Opcode    ;
            iTop_CI_TargetID   <= rTop_CI_TargetID  ;
            iTop_CI_SourceID   <= rTop_CI_SourceID  ;
            iTop_CI_Address    <= rTop_CI_Address   ;
            iTop_CI_Length     <= rTop_CI_Length    ;
            iTop_CI_CMDValid   <= rTop_CI_CMDValid  ;

            iTop_CI_WriteData  <= rTop_CI_WriteData ;
            iTop_CI_WriteLast  <= rTop_CI_WriteLast ;
            iTop_CI_WriteValid <= rTop_CI_WriteValid;
            iTop_CI_ReadReady  <= rTop_CI_ReadReady ;      
	end
endtask

task reset_ffh;
    begin

    NFC_CI_signal(6'b000001, 5'b00101, 0, 32'h00000000, 16'h0000, 1, 16'h0000, 0, 0, 0);
    NFC_CI_signal(6'b000001, 5'b00101, 0, 32'h00000000, 16'h0000, 0, 16'h0000, 0, 0, 0);
    @(posedge iSystemClock);
    wait(oCI_Top_CMDReady == 0);
    end
endtask

task setfeature_efh;
    begin
    NFC_CI_signal(6'b000010, 5'b00101, 0, 32'h00000000, 16'h0000, 1, 16'h0000, 0, 0, 0);
    NFC_CI_signal(6'b000010, 5'b00101, 0, 32'h00000000, 16'h0000, 0, 16'h0000, 0, 0, 0);
    @(posedge iSystemClock);
    wait(oCI_Top_CMDReady == 0);
    end
endtask


task getfeature_eeh;
    begin
    NFC_CI_signal(6'b000101, 5'b00101, 0, 32'h00000000, 16'h0008, 1, 16'h0000, 0, 0, 0);
    NFC_CI_signal(6'b000101, 5'b00101, 0, 32'h00000000, 16'h0000, 0, 16'h0000, 0, 0, 0);
    @(posedge iSystemClock);
    wait(oCI_Top_CMDReady == 0);
    end
endtask



task progpage_80h_10h;
    begin
    NFC_CI_signal(6'b000011, 5'b00101, 0, 32'h00000000, 16'h0008, 1, 16'h0000, 0, 0, 0);
    NFC_CI_signal(6'b000011, 5'b00101, 0, 32'h00000000, 16'h0000, 0, 16'h0000, 0, 0, 0);
    @(posedge iSystemClock);
    wait(oCI_Top_CMDReady == 0);
    end
endtask

task readpage_00h_30h;
	begin
    NFC_CI_signal(6'b000100, 5'b00101, 0, 32'h00000000, 16'h0008, 1, 16'h0000, 0, 0, 0);
    NFC_CI_signal(6'b000100, 5'b00101, 0, 32'h00000000, 16'h0000, 0, 16'h0000, 0, 0, 0);
    @(posedge iSystemClock);
    wait(oCI_Top_CMDReady == 0);
	end
endtask

task eraseblock_60h_d0h;
	begin
    NFC_CI_signal(6'b000110, 5'b00101, 0, 32'h00000000, 16'h0008, 1, 16'h0000, 0, 0, 0);
    NFC_CI_signal(6'b000110, 5'b00101, 0, 32'h00000000, 16'h0000, 0, 16'h0000, 0, 0, 0);
    @(posedge iSystemClock);
    wait(oCI_Top_CMDReady == 0);
	end
endtask

    integer I;

    initial
    
        begin
		// $dumpfile("./tb_NFC_Physical_Top.vcd");
		// $dumpvars(0, tb_NFC_Physical_Top);
        iReset <= 1;
        NFC_CI_signal(6'b000001, 5'b00101, 0, 32'h00000000, 16'h0000, 0, 16'h0000, 0, 0, 0);
        for (I = 0; I < 10; I = I + 1)
            @(posedge iSystemClock);
        iReset <= 0;
        # 1000000
        for (I = 0; I < 10; I = I + 1)
            @(posedge iSystemClock);
        
        NFC_CI_signal(6'b000000, 5'b00100, 0, 32'h00000001, 16'h0000, 1, 16'h0000, 0, 0, 0);
        NFC_CI_signal(6'b000001, 5'b00101, 0, 32'h00000000, 16'h0000, 0, 16'h0000, 0, 0, 0);
        

        s_axis_input;
        reset_ffh;
        setfeature_efh;
        getfeature_eeh;
        NFC_CI_signal(6'b000001, 5'b00101, 0, 32'h00000000, 16'h0000, 0, 16'h0000, 0, 0, 1);
        progpage_80h_10h;
        # 2000000;
        readpage_00h_30h;
        NFC_CI_signal(6'b000001, 5'b00101, 0, 32'h00000000, 16'h0000, 0, 16'h0000, 0, 0, 1);
        
        eraseblock_60h_d0h;
        readpage_00h_30h;
        NFC_CI_signal(6'b000001, 5'b00101, 0, 32'h00000000, 16'h0000, 0, 16'h0000, 0, 0, 1);
        repeat (50) @(posedge iSystemClock);
        // $finish;
        end
    
endmodule
