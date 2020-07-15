`timescale 1ns / 1ps

module NFC_Command_Issue_Top
#
(
    parameter NumberOfWays    =   4
)
(
    iSystemClock             ,  
    iReset                   ,  

    iTop_CI_Opcode           ,  
    iTop_CI_TargetID         ,  
    iTop_CI_SourceID         ,  
    iTop_CI_Address          ,  
    iTop_CI_Length           ,  
    iTop_CI_CMDValid         ,  
    oCI_Top_CMDReady         ,  

    iTop_CI_WriteData        ,  
    iTop_CI_WriteLast        ,  
    iTop_CI_WriteValid       ,  
    iTop_CI_WriteKeep        ,  
    oCI_Top_WriteReady       ,  

    oCI_Top_ReadData         ,  
    oCI_Top_ReadLast         ,  
    oCI_Top_ReadValid        ,  
    oCI_Top_ReadKeep         ,  
    iTop_CI_ReadReady        , 

    oCI_Top_ReadyBusy        ,  

    oCI_ACG_Command          ,  
    oCI_ACG_CommandOption    ,  

    iACG_CI_Ready            ,  
    iACG_CI_LastStep         ,  
    oCI_ACG_TargetWay        ,  
    oCI_ACG_NumOfData        ,  

    oCI_ACG_CASelect         ,  
    oCI_ACG_CAData           ,  

    oCI_ACG_WriteData        ,  
    oCI_ACG_WriteLast        ,  
    oCI_ACG_WriteValid       ,  
    iACG_CI_WriteReady       ,  

    iACG_CI_ReadData         ,  
    iACG_CI_ReadLast         ,  
    iACG_CI_ReadValid        ,  
    oCI_ACG_ReadReady        ,  

    iACG_CI_ReadyBusy        
);

    input                           iSystemClock                ;
    input                           iReset                      ;
 
    input   [5:0]                   iTop_CI_Opcode              ;
    input   [4:0]                   iTop_CI_TargetID            ;
    input   [4:0]                   iTop_CI_SourceID            ;
    input   [31:0]                  iTop_CI_Address             ;
    input   [15:0]                  iTop_CI_Length              ;
    input                           iTop_CI_CMDValid            ;
    output                          oCI_Top_CMDReady            ;

    input   [15:0]                  iTop_CI_WriteData           ;
    input                           iTop_CI_WriteLast           ;
    input                           iTop_CI_WriteValid          ;
    input   [1:0]                   iTop_CI_WriteKeep           ;
    output                          oCI_Top_WriteReady          ;

    output  [15:0]                  oCI_Top_ReadData            ;
    output                          oCI_Top_ReadLast            ;
    output                          oCI_Top_ReadValid           ;
    output  [1:0]                   oCI_Top_ReadKeep            ;
    input                           iTop_CI_ReadReady           ;

    output  [NumberOfWays - 1:0]    oCI_Top_ReadyBusy           ;


    output   [7:0]                  oCI_ACG_Command             ;
    output   [2:0]                  oCI_ACG_CommandOption       ;

    input    [7:0]                  iACG_CI_Ready               ;
    input    [7:0]                  iACG_CI_LastStep            ;
    output   [NumberOfWays - 1:0]   oCI_ACG_TargetWay           ;
    output   [15:0]                 oCI_ACG_NumOfData           ;

    output                          oCI_ACG_CASelect            ;
    output   [39:0]                 oCI_ACG_CAData              ;

    output   [15:0]                 oCI_ACG_WriteData           ;
    output                          oCI_ACG_WriteLast           ;
    output                          oCI_ACG_WriteValid          ;
    input                           iACG_CI_WriteReady          ;
  
    input    [15:0]                 iACG_CI_ReadData            ;
    input                           iACG_CI_ReadLast            ;
    input                           iACG_CI_ReadValid           ;
    output                          oCI_ACG_ReadReady           ;

    input    [NumberOfWays - 1:0]   iACG_CI_ReadyBusy           ;


    reg    [NumberOfWays - 1:0]   rWaySelect                 ;

    wire                          wCI_Top_CMDReady            ;

    wire   [7:0]                  wACG_Idle_Command          ;
    wire   [2:0]                  wACG_Idle_CommandOption    ;
    wire   [NumberOfWays - 1:0]   wACG_Idle_TargetWay        ;
    wire   [15:0]                 wACG_Idle_NumOfData        ;
    wire                          wACG_Idle_CASelect         ;
    wire   [39:0]                 wACG_Idle_CAData           ;
    wire   [15:0]                 wACG_Idle_WriteData        ;
    wire                          wACG_Idle_WriteLast        ;
    wire                          wACG_Idle_WriteValid       ;
    wire                          wACG_Idle_ReadReady        ;

    wire  [5:0]                   wReset_Opcode              ;
    wire  [4:0]                   wReset_TargetID            ;
    wire  [4:0]                   wReset_SourceID            ;
    wire  [31:0]                  wReset_Address             ;
    wire  [15:0]                  wReset_Length              ;
    wire                          wReset_CMDValid            ;
    wire                          wReset_CMDReady            ;
    wire  [NumberOfWays - 1:0]    wReset_WaySelect           ;
    wire                          wReset_Start               ;
    wire                          wReset_LastStep            ;

    wire   [7:0]                  wACG_Reset_Command         ;
    wire   [2:0]                  wACG_Reset_CommandOption   ;
    wire   [7:0]                  wACG_Reset_Ready           ;
    wire   [7:0]                  wACG_Reset_LastStep        ;
    wire   [NumberOfWays - 1:0]   wACG_Reset_TargetWay       ;
    wire   [15:0]                 wACG_Reset_NumOfData       ;
    wire                          wACG_Reset_CASelect        ;
    wire   [39:0]                 wACG_Reset_CAData          ;
    wire   [NumberOfWays - 1:0]   wACG_Reset_ReadyBusy       ;

    wire  [5:0]                   wSTF_Opcode              ;
    wire  [4:0]                   wSTF_TargetID            ;
    wire  [4:0]                   wSTF_SourceID            ;
    wire  [31:0]                  wSTF_Address             ;
    wire  [15:0]                  wSTF_Length              ;
    wire                          wSTF_CMDValid            ;
    wire                          wSTF_CMDReady            ;
    wire  [NumberOfWays - 1:0]    wSTF_WaySelect           ;
    wire                          wSTF_Start               ;
    wire                          wSTF_LastStep            ;

    wire   [7:0]                  wACG_STF_Command         ;
    wire   [2:0]                  wACG_STF_CommandOption   ;
    wire   [7:0]                  wACG_STF_Ready           ;
    wire   [7:0]                  wACG_STF_LastStep        ;
    wire   [NumberOfWays - 1:0]   wACG_STF_TargetWay       ;
    wire   [15:0]                 wACG_STF_NumOfData       ;
    wire                          wACG_STF_CASelect        ;
    wire   [39:0]                 wACG_STF_CAData          ;
    wire   [NumberOfWays - 1:0]   wACG_STF_ReadyBusy       ;

    wire   [15:0]                 wACG_STF_WriteData       ;
    wire                          wACG_STF_WriteLast       ;
    wire                          wACG_STF_WriteValid      ;
    wire                          wACG_STF_WriteReady      ;

    wire  [5:0]                   wProg_Opcode              ;
    wire  [4:0]                   wProg_TargetID            ;
    wire  [4:0]                   wProg_SourceID            ;
    wire  [31:0]                  wProg_Address             ;
    wire  [15:0]                  wProg_Length              ;
    wire                          wProg_CMDValid            ;
    wire                          wProg_CMDReady            ;
    wire  [NumberOfWays - 1:0]    wProg_WaySelect           ;
    wire                          wProg_Start               ;
    wire                          wProg_LastStep            ;

    wire   [7:0]                  wACG_Prog_Command         ;
    wire   [2:0]                  wACG_Prog_CommandOption   ;
    wire   [7:0]                  wACG_Prog_Ready           ;
    wire   [7:0]                  wACG_Prog_LastStep        ;
    wire   [NumberOfWays - 1:0]   wACG_Prog_TargetWay       ;
    wire   [15:0]                 wACG_Prog_NumOfData       ;
    wire                          wACG_Prog_CASelect        ;
    wire   [39:0]                 wACG_Prog_CAData          ;
    wire   [NumberOfWays - 1:0]   wACG_Prog_ReadyBusy       ;

    wire  [5:0]                   wRead_Opcode              ;
    wire  [4:0]                   wRead_TargetID            ;
    wire  [4:0]                   wRead_SourceID            ;
    wire  [31:0]                  wRead_Address             ;
    wire  [15:0]                  wRead_Length              ;
    wire                          wRead_CMDValid            ;
    wire                          wRead_CMDReady            ;
    wire  [NumberOfWays - 1:0]    wRead_WaySelect           ;
    wire                          wRead_Start               ;
    wire                          wRead_LastStep            ;

    wire   [7:0]                  wACG_Read_Command         ;
    wire   [2:0]                  wACG_Read_CommandOption   ;
    wire   [7:0]                  wACG_Read_Ready           ;
    wire   [7:0]                  wACG_Read_LastStep        ;
    wire   [NumberOfWays - 1:0]   wACG_Read_TargetWay       ;
    wire   [15:0]                 wACG_Read_NumOfData       ;
    wire                          wACG_Read_CASelect        ;
    wire   [39:0]                 wACG_Read_CAData          ;
    wire   [NumberOfWays - 1:0]   wACG_Read_ReadyBusy       ;

    wire  [5:0]                   wGTF_Opcode              ;
    wire  [4:0]                   wGTF_TargetID            ;
    wire  [4:0]                   wGTF_SourceID            ;
    wire  [31:0]                  wGTF_Address             ;
    wire  [15:0]                  wGTF_Length              ;
    wire                          wGTF_CMDValid            ;
    wire                          wGTF_CMDReady            ;
    wire  [NumberOfWays - 1:0]    wGTF_WaySelect           ;
    wire                          wGTF_Start               ;
    wire                          wGTF_LastStep            ;

    wire   [7:0]                  wACG_GTF_Command         ;
    wire   [2:0]                  wACG_GTF_CommandOption   ;
    wire   [7:0]                  wACG_GTF_Ready           ;
    wire   [7:0]                  wACG_GTF_LastStep        ;
    wire   [NumberOfWays - 1:0]   wACG_GTF_TargetWay       ;
    wire   [15:0]                 wACG_GTF_NumOfData       ;
    wire                          wACG_GTF_CASelect        ;
    wire   [39:0]                 wACG_GTF_CAData          ;
    wire   [NumberOfWays - 1:0]   wACG_GTF_ReadyBusy       ;

    wire  [5:0]                   wEB_Opcode              ;
    wire  [4:0]                   wEB_TargetID            ;
    wire  [4:0]                   wEB_SourceID            ;
    wire  [31:0]                  wEB_Address             ;
    wire  [15:0]                  wEB_Length              ;
    wire                          wEB_CMDValid            ;
    wire                          wEB_CMDReady            ;
    wire  [NumberOfWays - 1:0]    wEB_WaySelect           ;
    wire                          wEB_Start               ;
    wire                          wEB_LastStep            ;

    wire   [7:0]                  wACG_EB_Command         ;
    wire   [2:0]                  wACG_EB_CommandOption   ;
    wire   [7:0]                  wACG_EB_Ready           ;
    wire   [7:0]                  wACG_EB_LastStep        ;
    wire   [NumberOfWays - 1:0]   wACG_EB_TargetWay       ;
    wire   [15:0]                 wACG_EB_NumOfData       ;
    wire                          wACG_EB_CASelect        ;
    wire   [39:0]                 wACG_EB_CAData          ;
    wire   [NumberOfWays - 1:0]   wACG_EB_ReadyBusy       ;

    wire    [NumberOfWays - 1:0]    wTargetWay              ;
    wire    [15:0]                  wTargetCol              ;
    wire    [23:0]                  wTargetRow              ;

    reg     [7:0]                   rTargetWay1B            ;
    reg     [15:0]                  rColAddr2B              ;
    reg     [23:0]                  rRowAddr3B              ;

    wire   [15:0]                  wFifo_WriteData           ;
    wire                           wFifo_WriteLast           ;
    wire                           wFifo_WriteValid          ;
    wire                           wFifo_WriteReady          ;

    assign wReset_Opcode        = iTop_CI_Opcode        ;
    assign wReset_TargetID      = iTop_CI_TargetID      ;
    assign wReset_SourceID      = iTop_CI_SourceID      ;
    assign wReset_Address       = iTop_CI_Address       ;
    assign wReset_Length        = iTop_CI_Length        ;
    assign wReset_CMDValid      = iTop_CI_CMDValid      ;
    assign wReset_WaySelect     = rTargetWay1B   ;

    assign wACG_Reset_Ready     = iACG_CI_Ready     ;
    assign wACG_Reset_LastStep  = iACG_CI_LastStep  ;
    assign wACG_Reset_ReadyBusy = iACG_CI_ReadyBusy ;

    assign wSTF_Opcode        = iTop_CI_Opcode        ;
    assign wSTF_TargetID      = iTop_CI_TargetID      ;
    assign wSTF_SourceID      = iTop_CI_SourceID      ;
    assign wSTF_Address       = iTop_CI_Address       ;
    assign wSTF_Length        = iTop_CI_Length        ;
    assign wSTF_CMDValid      = iTop_CI_CMDValid      ;
    assign wSTF_WaySelect     = rTargetWay1B   ;

    assign wACG_STF_Ready     = iACG_CI_Ready     ;
    assign wACG_STF_LastStep  = iACG_CI_LastStep  ;
    assign wACG_STF_ReadyBusy = iACG_CI_ReadyBusy ;

    assign wACG_STF_WriteReady = iACG_CI_WriteReady;

    assign wProg_Opcode        = iTop_CI_Opcode        ;
    assign wProg_TargetID      = iTop_CI_TargetID      ;
    assign wProg_SourceID      = iTop_CI_SourceID      ;
    assign wProg_Address       = iTop_CI_Address       ;
    assign wProg_Length        = iTop_CI_Length        ;
    assign wProg_CMDValid      = iTop_CI_CMDValid      ;
    assign wProg_WaySelect     = rTargetWay1B   ;

    assign wACG_Prog_Ready     = iACG_CI_Ready     ;
    assign wACG_Prog_LastStep  = iACG_CI_LastStep  ;
    assign wACG_Prog_ReadyBusy = iACG_CI_ReadyBusy ;

    assign wFifo_WriteReady = iACG_CI_WriteReady & (~wProg_CMDReady);

    assign wRead_Opcode        = iTop_CI_Opcode        ;
    assign wRead_TargetID      = iTop_CI_TargetID      ;
    assign wRead_SourceID      = iTop_CI_SourceID      ;
    assign wRead_Address       = iTop_CI_Address       ;
    assign wRead_Length        = iTop_CI_Length        ;
    assign wRead_CMDValid      = iTop_CI_CMDValid      ;
    assign wRead_WaySelect     = rTargetWay1B   ;

    assign wACG_Read_Ready     = iACG_CI_Ready     ;
    assign wACG_Read_LastStep  = iACG_CI_LastStep  ;
    assign wACG_Read_ReadyBusy = iACG_CI_ReadyBusy ;

    assign wGTF_Opcode        = iTop_CI_Opcode        ;
    assign wGTF_TargetID      = iTop_CI_TargetID      ;
    assign wGTF_SourceID      = iTop_CI_SourceID      ;
    assign wGTF_Address       = iTop_CI_Address       ;
    assign wGTF_Length        = iTop_CI_Length        ;
    assign wGTF_CMDValid      = iTop_CI_CMDValid      ;
    assign wGTF_WaySelect     = rTargetWay1B   ;

    assign wACG_GTF_Ready     = iACG_CI_Ready     ;
    assign wACG_GTF_LastStep  = iACG_CI_LastStep  ;
    assign wACG_GTF_ReadyBusy = iACG_CI_ReadyBusy ;

    assign wEB_Opcode        = iTop_CI_Opcode        ;
    assign wEB_TargetID      = iTop_CI_TargetID      ;
    assign wEB_SourceID      = iTop_CI_SourceID      ;
    assign wEB_Address       = iTop_CI_Address       ;
    assign wEB_Length        = iTop_CI_Length        ;
    assign wEB_CMDValid      = iTop_CI_CMDValid      ;
    assign wEB_WaySelect     = rTargetWay1B   ;

    assign wACG_EB_Ready     = iACG_CI_Ready     ;
    assign wACG_EB_LastStep  = iACG_CI_LastStep  ;
    assign wACG_EB_ReadyBusy = iACG_CI_ReadyBusy ;

    assign wTGC_waySELECT = (iTop_CI_TargetID[4:0] == 5'b00100) & (iTop_CI_Opcode[5:0] == 6'b000000) & (iTop_CI_CMDValid);
    assign wTGC_colADDR   = (iTop_CI_TargetID[4:0] == 5'b00100) & (iTop_CI_Opcode[5:0] == 6'b000010) & (iTop_CI_CMDValid);
    assign wTGC_rowADDR   = (iTop_CI_TargetID[4:0] == 5'b00100) & (iTop_CI_Opcode[5:0] == 6'b000100) & (iTop_CI_CMDValid);

    always @ (posedge iSystemClock, posedge iReset) begin
        if (iReset) begin
            rTargetWay1B[7:0]   <= 0;
            rColAddr2B[15:0]    <= 0;
            rRowAddr3B[23:0]    <= 0;
        end else begin
            if (wTGC_waySELECT) begin
                rTargetWay1B[7:0]   <= iTop_CI_Address[7:0];
                rColAddr2B[15:0]    <= rColAddr2B[15:0];
                rRowAddr3B[23:0]    <= rRowAddr3B[23:0];
            end else if (wTGC_colADDR) begin
                rTargetWay1B[7:0]   <= rTargetWay1B[7:0];
                rColAddr2B[15:0]    <= iTop_CI_Address[15:0];
                rRowAddr3B[23:0]    <= rRowAddr3B[23:0];
            end else if (wTGC_rowADDR) begin
                rTargetWay1B[7:0]   <= rTargetWay1B[7:0];
                rColAddr2B[15:0]    <= rColAddr2B[15:0];
                rRowAddr3B[23:0]    <= iTop_CI_Address[23:0];
            end else begin
                rTargetWay1B[7:0]   <= rTargetWay1B[7:0];
                rColAddr2B[15:0]    <= rColAddr2B[15:0];
                rRowAddr3B[23:0]    <= rRowAddr3B[23:0];
            end
        end
    end

    parameter Reset_CommandID      = 6'b000001;
    parameter SetFeature_CommandID = 6'b000010;
    parameter Progpage_CommandID   = 6'b000011;
    parameter Readpage_CommandID   = 6'b000100;
    parameter GetFeature_CommandID = 6'b000101;
    parameter EraseBlock_CommandID = 6'b000110;
    parameter TargetID             = 5'b00101;
    parameter NumofCMD             = 14;

    wire [NumofCMD - 1:0]        wCMD_Active                ;
    assign wCMD_Active = ~{wReset_CMDReady, wSTF_CMDReady,wProg_CMDReady,wRead_CMDReady, wGTF_CMDReady,wEB_CMDReady,8'b1111_1111};

    NFC_Command_Idle #(
            .NumberOfWays(NumberOfWays)
        ) inst_NFC_Command_Idle (
            .oACG_Command       (wACG_Idle_Command),
            .oACG_CommandOption (wACG_Idle_CommandOption),
            .oACG_TargetWay     (wACG_Idle_TargetWay),
            .oACG_NumOfData     (wACG_Idle_NumOfData),
            .oACG_CASelect      (wACG_Idle_CASelect),
            .oACG_CAData        (wACG_Idle_CAData),
            .oACG_WriteData     (wACG_Idle_WriteData),
            .oACG_WriteLast     (wACG_Idle_WriteLast),
            .oACG_WriteValid    (wACG_Idle_WriteValid),
            .oACG_ReadReady     (wACG_Idle_ReadReady)
        );

    NFC_Command_Reset #(
            .NumberOfWays(NumberOfWays),
            .CommandID(Reset_CommandID),
            .TargetID(TargetID)
        ) inst_NFC_Command_Reset (
            .iSystemClock       (iSystemClock),
            .iReset             (iReset),

            .iOpcode            (wReset_Opcode),
            .iTargetID          (wReset_TargetID),
            .iSourceID          (wReset_SourceID),
            .iCMDValid          (wReset_CMDValid),
            .oCMDReady          (wReset_CMDReady),
            .iWaySelect         (wReset_WaySelect),
            .oStart             (wReset_Start),
            .oLastStep          (wReset_LastStep),

            .oACG_Command       (wACG_Reset_Command),
            .oACG_CommandOption (wACG_Reset_CommandOption),
            .iACG_Ready         (wACG_Reset_Ready),
            .iACG_LastStep      (wACG_Reset_LastStep),
            .oACG_TargetWay     (wACG_Reset_TargetWay),
            .oACG_NumOfData     (wACG_Reset_NumOfData),
            .oACG_CASelect      (wACG_Reset_CASelect),
            .oACG_CAData        (wACG_Reset_CAData),
            .iACG_ReadyBusy     (wACG_Reset_ReadyBusy)
        );

    NFC_Command_SetFeature #(
            .NumberOfWays(NumberOfWays),
            .CommandID(SetFeature_CommandID),
            .TargetID(TargetID)
        ) inst_NFC_Command_SetFeature (
            .iSystemClock       (iSystemClock),
            .iReset             (iReset),

            .iOpcode            (wSTF_Opcode),
            .iTargetID          (wSTF_TargetID),
            .iSourceID          (wSTF_SourceID),
            .iCMDValid          (wSTF_CMDValid),
            .oCMDReady          (wSTF_CMDReady),
            .iWaySelect         (wSTF_WaySelect),
            .oStart             (wSTF_Start),
            .oLastStep          (wSTF_LastStep),

            .oACG_Command       (wACG_STF_Command),
            .oACG_CommandOption (wACG_STF_CommandOption),
            .iACG_Ready         (wACG_STF_Ready),
            .iACG_LastStep      (wACG_STF_LastStep),
            .oACG_TargetWay     (wACG_STF_TargetWay),
            .oACG_NumOfData     (wACG_STF_NumOfData),
            .oACG_CASelect      (wACG_STF_CASelect),
            .oACG_CAData        (wACG_STF_CAData),

            .oACG_WriteData     (wACG_STF_WriteData),
            .oACG_WriteLast     (wACG_STF_WriteLast),
            .oACG_WriteValid    (wACG_STF_WriteValid),
            .iACG_WriteReady    (wACG_STF_WriteReady),

            .iACG_ReadyBusy     (wACG_STF_ReadyBusy)
        );

    NFC_Command_ProgramPage #(
            .NumberOfWays(NumberOfWays),
            .CommandID(Progpage_CommandID),
            .TargetID(TargetID)
        ) inst_NFC_Command_ProgramPage (
            .iSystemClock       (iSystemClock),
            .iReset             (iReset),

            .iOpcode            (wProg_Opcode),
            .iTargetID          (wProg_TargetID),
            .iSourceID          (wProg_SourceID),
            .iAddress           (wProg_Address),
            .iLength            (wProg_Length),
            .iCMDValid          (wProg_CMDValid),
            .oCMDReady          (wProg_CMDReady),
            .iWaySelect         (wProg_WaySelect),
            .oStart             (wProg_Start),
            .oLastStep          (wProg_LastStep),

            .oACG_Command       (wACG_Prog_Command),
            .oACG_CommandOption (wACG_Prog_CommandOption),
            .iACG_Ready         (wACG_Prog_Ready),
            .iACG_LastStep      (wACG_Prog_LastStep),
            .oACG_TargetWay     (wACG_Prog_TargetWay),
            .oACG_NumOfData     (wACG_Prog_NumOfData),
            .oACG_CASelect      (wACG_Prog_CASelect),
            .oACG_CAData        (wACG_Prog_CAData),

            .iACG_ReadyBusy     (wACG_Prog_ReadyBusy)
        );

    NFC_Command_ReadPage #(
            .NumberOfWays(NumberOfWays),
            .CommandID(Readpage_CommandID),
            .TargetID(TargetID)
        ) inst_NFC_Command_ReadPage (
            .iSystemClock       (iSystemClock),
            .iReset             (iReset),

            .iOpcode            (wRead_Opcode),
            .iTargetID          (wRead_TargetID),
            .iSourceID          (wRead_SourceID),
            .iAddress           (wRead_Address),
            .iLength            (wRead_Length),
            .iCMDValid          (wRead_CMDValid),
            .oCMDReady          (wRead_CMDReady),
            .iWaySelect         (wRead_WaySelect),
            .oStart             (wRead_Start),
            .oLastStep          (wRead_LastStep),

            .oACG_Command       (wACG_Read_Command),
            .oACG_CommandOption (wACG_Read_CommandOption),
            .iACG_Ready         (wACG_Read_Ready),
            .iACG_LastStep      (wACG_Read_LastStep),
            .oACG_TargetWay     (wACG_Read_TargetWay),
            .oACG_NumOfData     (wACG_Read_NumOfData),
            .oACG_CASelect      (wACG_Read_CASelect),
            .oACG_CAData        (wACG_Read_CAData),

            .iACG_ReadyBusy     (wACG_Read_ReadyBusy)
        );

    NFC_Command_GetFeature #(
            .NumberOfWays(NumberOfWays),
            .CommandID(GetFeature_CommandID),
            .TargetID(TargetID)
        ) inst_NFC_Command_GetFeature (
            .iSystemClock       (iSystemClock),
            .iReset             (wReset),

            .iOpcode            (wGTF_Opcode),
            .iTargetID          (wGTF_TargetID),
            .iSourceID          (wGTF_SourceID),
            .iAddress           (wGTF_Address),
            .iLength            (wGTF_Length),
            .iCMDValid          (wGTF_CMDValid),
            .oCMDReady          (wGTF_CMDReady),
            .iWaySelect         (wGTF_WaySelect),
            .oStart             (wGTF_Start),
            .oLastStep          (wGTFLastStep),

            .oACG_Command       (wACG_GTF_Command),
            .oACG_CommandOption (wACG_GTF_CommandOption),
            .iACG_Ready         (wACG_GTF_Ready),
            .iACG_LastStep      (wACG_GTF_LastStep),
            .oACG_TargetWay     (wACG_GTF_TargetWay),
            .oACG_NumOfData     (wACG_GTF_NumOfData),
            .oACG_CASelect      (wACG_GTF_CASelect),
            .oACG_CAData        (wACG_GTF_CAData),
            .iACG_ReadyBusy     (wACG_GTF_ReadyBusy)
        );

    NFC_Command_EraseBlock #(
            .NumberOfWays(NumberOfWays),
            .CommandID(EraseBlock_CommandID),
            .TargetID(TargetID)
        ) inst_NFC_Command_EraseBlock (
            .iSystemClock       (iSystemClock),
            .iReset             (iReset),

            .iOpcode            (wEB_Opcode),
            .iTargetID          (wEB_TargetID),
            .iSourceID          (wEB_SourceID),
            .iAddress           (wEB_Address),
            .iLength            (wEB_Length),
            .iCMDValid          (wEB_CMDValid),
            .oCMDReady          (wEB_CMDReady),
            .iWaySelect         (wEB_WaySelect),
            .oStart             (wEB_Start),
            .oLastStep          (wEB_LastStep),

            .oACG_Command       (wACG_EB_Command),
            .oACG_CommandOption (wACG_EB_CommandOption),
            .iACG_Ready         (wACG_EB_Ready),
            .iACG_LastStep      (wACG_EB_LastStep),
            .oACG_TargetWay     (wACG_EB_TargetWay),
            .oACG_NumOfData     (wACG_EB_NumOfData),
            .oACG_CASelect      (wACG_EB_CASelect),
            .oACG_CAData        (wACG_EB_CAData),
            .iACG_ReadyBusy     (wACG_EB_ReadyBusy)
        );


    NFC_Atom_Command_Issue_to_Atom_Command_Generator_Mux #(
            .NumofCMD(NumofCMD),
            .NumberOfWays(NumberOfWays)
        ) inst_NFC_Atom_Command_Issue_to_Atom_Command_Generator_Mux (
            .iCMD_Active              (wCMD_Active),

            .iACG_Idle_Command        (wACG_Idle_Command),
            .iACG_Idle_CommandOption  (wACG_Idle_CommandOption),
            .iACG_Idle_TargetWay      (wACG_Idle_TargetWay),
            .iACG_Idle_NumOfData      (wACG_Idle_NumOfData),
            .iACG_Idle_CASelect       (wACG_Idle_CASelect),
            .iACG_Idle_CAData         (wACG_Idle_CAData),
            .iACG_Idle_WriteData      (wACG_Idle_WriteData),
            .iACG_Idle_WriteLast      (wACG_Idle_WriteLast),
            .iACG_Idle_WriteValid     (wACG_Idle_WriteValid),
            .iACG_Idle_ReadReady      (wACG_Idle_ReadReady),

            .iACG_Reset_Command       (wACG_Reset_Command),
            .iACG_Reset_CommandOption (wACG_Reset_CommandOption),
            .iACG_Reset_TargetWay     (wACG_Reset_TargetWay),
            .iACG_Reset_NumOfData     (wACG_Reset_NumOfData),
            .iACG_Reset_CASelect      (wACG_Reset_CASelect),
            .iACG_Reset_CAData        (wACG_Reset_CAData),

            .iACG_STF_Command         (wACG_STF_Command),
            .iACG_STF_CommandOption   (wACG_STF_CommandOption),
            .iACG_STF_TargetWay       (wACG_STF_TargetWay),
            .iACG_STF_NumOfData       (wACG_STF_NumOfData),
            .iACG_STF_CASelect        (wACG_STF_CASelect),
            .iACG_STF_CAData          (wACG_STF_CAData),
            .iACG_STF_WriteData       (wACG_STF_WriteData),
            .iACG_STF_WriteLast       (wACG_STF_WriteLast),
            .iACG_STF_WriteValid      (wACG_STF_WriteValid),

            .iACG_Prog_Command        (wACG_Prog_Command),
            .iACG_Prog_CommandOption  (wACG_Prog_CommandOption),
            .iACG_Prog_TargetWay      (wACG_Prog_TargetWay),
            .iACG_Prog_NumOfData      (wACG_Prog_NumOfData),
            .iACG_Prog_CASelect       (wACG_Prog_CASelect),
            .iACG_Prog_CAData         (wACG_Prog_CAData),

            .iACG_Read_Command        (wACG_Read_Command),
            .iACG_Read_CommandOption  (wACG_Read_CommandOption),
            .iACG_Read_TargetWay      (wACG_Read_TargetWay),
            .iACG_Read_NumOfData      (wACG_Read_NumOfData),
            .iACG_Read_CASelect       (wACG_Read_CASelect),
            .iACG_Read_CAData         (wACG_Read_CAData),

            .iACG_GTF_Command         (wACG_GTF_Command),
            .iACG_GTF_CommandOption   (wACG_GTF_CommandOption),
            .iACG_GTF_TargetWay       (wACG_GTF_TargetWay),
            .iACG_GTF_NumOfData       (wACG_GTF_NumOfData),
            .iACG_GTF_CASelect        (wACG_GTF_CASelect),
            .iACG_GTF_CAData          (wACG_GTF_CAData),

            .iACG_EB_Command          (wACG_EB_Command),
            .iACG_EB_CommandOption    (wACG_EB_CommandOption),
            .iACG_EB_TargetWay        (wACG_EB_TargetWay),
            .iACG_EB_NumOfData        (wACG_EB_NumOfData),
            .iACG_EB_CASelect         (wACG_EB_CASelect),
            .iACG_EB_CAData           (wACG_EB_CAData),

            .iFifo_WriteData          (wFifo_WriteData),
            .iFifo_WriteLast          (wFifo_WriteLast),
            .iFifo_WriteValid         (wFifo_WriteValid),

            .oCI_ACG_Command          (oCI_ACG_Command),
            .oCI_ACG_CommandOption    (oCI_ACG_CommandOption),
            .oCI_ACG_TargetWay        (oCI_ACG_TargetWay),
            .oCI_ACG_NumOfData        (oCI_ACG_NumOfData),
            .oCI_ACG_CASelect         (oCI_ACG_CASelect),
            .oCI_ACG_CAData           (oCI_ACG_CAData),
            .oCI_ACG_WriteData        (oCI_ACG_WriteData),
            .oCI_ACG_WriteLast        (oCI_ACG_WriteLast),
            .oCI_ACG_WriteValid       (oCI_ACG_WriteValid),
            .oCI_ACG_ReadReady        ( )
        );


    // Parameters
    parameter DEPTH = 32;
    parameter DATA_WIDTH = 16;
    parameter KEEP_ENABLE = (DATA_WIDTH>8);
    parameter KEEP_WIDTH = (DATA_WIDTH/8);
    parameter LAST_ENABLE = 1;
    parameter ID_ENABLE = 1;
    parameter ID_WIDTH = 8;
    parameter DEST_ENABLE = 1;
    parameter DEST_WIDTH = 8;
    parameter USER_ENABLE = 1;
    parameter USER_WIDTH = 1;
    parameter FRAME_FIFO = 1;
    parameter USER_BAD_FRAME_VALUE = 1'b1;
    parameter USER_BAD_FRAME_MASK = 1'b1;
    parameter DROP_BAD_FRAME = 0;
    parameter DROP_WHEN_FULL = 0;

    wire [ 1:0]      m_axis_tkeep  ;
    axis_fifo #(
        .DEPTH(DEPTH),
        .DATA_WIDTH(DATA_WIDTH),
        .KEEP_ENABLE(KEEP_ENABLE),
        .KEEP_WIDTH(KEEP_WIDTH),
        .LAST_ENABLE(LAST_ENABLE),
        .ID_ENABLE(ID_ENABLE),
        .ID_WIDTH(ID_WIDTH),
        .DEST_ENABLE(DEST_ENABLE),
        .DEST_WIDTH(DEST_WIDTH),
        .USER_ENABLE(USER_ENABLE),
        .USER_WIDTH(USER_WIDTH),
        .FRAME_FIFO(FRAME_FIFO),
        .USER_BAD_FRAME_VALUE(USER_BAD_FRAME_VALUE),
        .USER_BAD_FRAME_MASK(USER_BAD_FRAME_MASK),
        .DROP_BAD_FRAME(DROP_BAD_FRAME),
        .DROP_WHEN_FULL(DROP_WHEN_FULL)
    ) write_fifo (
            .clk               (iSystemClock),
            .rst               (iReset),

            .s_axis_tdata      (iTop_CI_WriteData),
            .s_axis_tkeep      (iTop_CI_WriteKeep),
            .s_axis_tvalid     (iTop_CI_WriteValid),
            .s_axis_tready     (oCI_Top_WriteReady),
            .s_axis_tlast      (iTop_CI_WriteLast),

            .m_axis_tdata      (wFifo_WriteData),
            .m_axis_tkeep      (m_axis_tkeep),
            .m_axis_tvalid     (wFifo_WriteValid),
            .m_axis_tready     (wFifo_WriteReady),
            .m_axis_tlast      (wFifo_WriteLast),

            .status_overflow   (status_overflow),
            .status_bad_frame  (status_bad_frame),
            .status_good_frame (status_good_frame)
        );

    axis_fifo #(
        .DEPTH(DEPTH),
        .DATA_WIDTH(DATA_WIDTH),
        .KEEP_ENABLE(KEEP_ENABLE),
        .KEEP_WIDTH(KEEP_WIDTH),
        .LAST_ENABLE(LAST_ENABLE),
        .ID_ENABLE(ID_ENABLE),
        .ID_WIDTH(ID_WIDTH),
        .DEST_ENABLE(DEST_ENABLE),
        .DEST_WIDTH(DEST_WIDTH),
        .USER_ENABLE(USER_ENABLE),
        .USER_WIDTH(USER_WIDTH),
        .FRAME_FIFO(FRAME_FIFO),
        .USER_BAD_FRAME_VALUE(USER_BAD_FRAME_VALUE),
        .USER_BAD_FRAME_MASK(USER_BAD_FRAME_MASK),
        .DROP_BAD_FRAME(DROP_BAD_FRAME),
        .DROP_WHEN_FULL(DROP_WHEN_FULL)
    ) read_fifo (
            .clk               (iSystemClock),
            .rst               (iReset),

            .s_axis_tdata      (iACG_CI_ReadData),
            .s_axis_tkeep      (2'b11),
            .s_axis_tvalid     (iACG_CI_ReadValid),
            .s_axis_tready     (oCI_ACG_ReadReady),
            .s_axis_tlast      (iACG_CI_ReadLast),

            .m_axis_tdata      (oCI_Top_ReadData),
            .m_axis_tkeep      (oCI_Top_ReadKeep),
            .m_axis_tvalid     (oCI_Top_ReadValid),
            .m_axis_tready     (iTop_CI_ReadReady),
            .m_axis_tlast      (oCI_Top_ReadLast),

            .status_overflow   (status_overflow),
            .status_bad_frame  (status_bad_frame),
            .status_good_frame (status_good_frame)
        );

    assign oCI_Top_CMDReady = |wCMD_Active;
endmodule
