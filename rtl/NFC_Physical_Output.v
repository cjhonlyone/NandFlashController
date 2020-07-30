`timescale 1ns / 1ps

module NFC_Physical_Output
#
(
    parameter NumberOfWays    =   4
)
(
    iSystemClock            ,
    iOutputDrivingClock     ,
    iModuleReset            ,
    iDQSOutEnable           ,
    iDQOutEnable            ,
    iPO_DQStrobe            ,
    iPO_DQ                  ,
    iPO_ChipEnable          ,
    iPO_ReadEnable          ,
    iPO_WriteEnable         ,
    iPO_AddressLatchEnable  ,
    iPO_CommandLatchEnable  ,
    oDQSOutEnableToPinpad   ,
    oDQOutEnableToPinpad    ,
    oDQSToNAND              ,
    oDQToNAND               ,
    oCEToNAND               ,
    oWEToNAND               ,
    oREToNAND               ,
    oALEToNAND              ,
    oCLEToNAND
);
    // Data Width (DQ): 8 bit
    
    // 4:1 DDR Serialization with OSERDESE2
    // OSERDESE2, 4:1 DDR Serialization
    //            CLKDIV: SDR 100MHz CLK: SDR 200MHz OQ: DDR 200MHz
    //            output resolution: 2.50 ns
    input                           iSystemClock            ;
    input                           iOutputDrivingClock     ;
    input                           iModuleReset            ;
    input                           iDQSOutEnable           ;
    input                           iDQOutEnable            ;
    input   [7:0]                   iPO_DQStrobe            ; // DQS, full res.
    input   [31:0]                  iPO_DQ                  ; // DQ, half res., 2 bit * 8 bit data width = 16 bit interface width
    input   [2*NumberOfWays - 1:0]  iPO_ChipEnable          ; // CE, quater res., 1 bit * 4 way = 4 bit interface width
    input   [3:0]                   iPO_ReadEnable          ; // RE, half res.
    input   [3:0]                   iPO_WriteEnable         ; // WE, half res.
    input   [3:0]                   iPO_AddressLatchEnable  ; // ALE, half res.
    input   [3:0]                   iPO_CommandLatchEnable  ; // CLE, half res.
    output                          oDQSOutEnableToPinpad   ;
    output  [7:0]                   oDQOutEnableToPinpad    ;
    output                          oDQSToNAND              ;
    output  [7:0]                   oDQToNAND               ;
    output  [NumberOfWays - 1:0]    oCEToNAND               ;
    output                          oWEToNAND               ;
    output                          oREToNAND               ;
    output                          oALEToNAND              ;
    output                          oCLEToNAND              ;

    reg                           r0_DQSOutEnable           ;
    reg                           r0_DQOutEnable            ;
    reg   [7:0]                   r0_PO_DQStrobe            ;
    reg   [31:0]                  r0_PO_DQ                  ;
    reg   [2*NumberOfWays - 1:0]  r0_PO_ChipEnable          ;
    reg   [3:0]                   r0_PO_ReadEnable          ;
    reg   [3:0]                   r0_PO_WriteEnable         ;
    reg   [3:0]                   r0_PO_AddressLatchEnable  ;
    reg   [3:0]                   r0_PO_CommandLatchEnable  ;

    reg                           r1_DQSOutEnable           ;
    reg                           r1_DQOutEnable            ;
    reg   [7:0]                   r1_PO_DQStrobe            ;
    reg   [31:0]                  r1_PO_DQ                  ;
    reg   [2*NumberOfWays - 1:0]  r1_PO_ChipEnable          ;
    reg   [3:0]                   r1_PO_ReadEnable          ;
    reg   [3:0]                   r1_PO_WriteEnable         ;
    reg   [3:0]                   r1_PO_AddressLatchEnable  ;
    reg   [3:0]                   r1_PO_CommandLatchEnable  ;

    // always @ (posedge iSystemClock) begin
    //     r0_DQSOutEnable          <= ~iDQSOutEnable         ;
    //     r0_DQOutEnable           <= ~iDQOutEnable          ;
    //     r0_PO_DQStrobe           <= iPO_DQStrobe          ;
    //     r0_PO_DQ                 <= iPO_DQ                ;
    //     r0_PO_ChipEnable         <= iPO_ChipEnable        ;
    //     r0_PO_ReadEnable         <= iPO_ReadEnable        ;
    //     r0_PO_WriteEnable        <= iPO_WriteEnable       ;
    //     r0_PO_AddressLatchEnable <= iPO_AddressLatchEnable;
    //     r0_PO_CommandLatchEnable <= iPO_CommandLatchEnable;
        
    //     r1_DQSOutEnable          <= r0_DQSOutEnable         ;
    //     r1_DQOutEnable           <= r0_DQOutEnable          ;
    //     r1_PO_DQStrobe           <= r0_PO_DQStrobe          ;
    //     r1_PO_DQ                 <= r0_PO_DQ                ;
    //     r1_PO_ChipEnable         <= r0_PO_ChipEnable        ;
    //     r1_PO_ReadEnable         <= r0_PO_ReadEnable        ;
    //     r1_PO_WriteEnable        <= r0_PO_WriteEnable       ;
    //     r1_PO_AddressLatchEnable <= r0_PO_AddressLatchEnable;
    //     r1_PO_CommandLatchEnable <= r0_PO_CommandLatchEnable;
    // end

    // reg       dbg_DQS;
    // reg [7:0] dbg_DQ;

    // always @(posedge iOutputDrivingClock) begin
    //     dbg_DQS <= oDQSToNAND;
    //     dbg_DQ <= oDQToNAND;
    // end

    // ila_0 ila0(
    // .clk(iOutputDrivingClock),
    // .probe0(dbg_DQS),
    // .probe1(dbg_DQ));

    reg     rDQSOutEnable_buffer;
    reg     rDQSOut_IOBUF_T;
    reg     rDQOutEnable_buffer;
    reg     rDQOut_IOBUF_T;
    
    always @ (posedge iSystemClock) begin
        if (iModuleReset) begin
            rDQSOutEnable_buffer <= 0;
            rDQSOut_IOBUF_T      <= 1;
            rDQOutEnable_buffer  <= 0;
            rDQOut_IOBUF_T       <= 1;
        end else begin
            rDQSOutEnable_buffer <= iDQSOutEnable;
            rDQSOut_IOBUF_T      <= ~rDQSOutEnable_buffer;
            rDQOutEnable_buffer  <= iDQOutEnable;
            rDQOut_IOBUF_T       <= ~rDQOutEnable_buffer;
        end       
    end

    genvar c, d;
    ODDR
    #(
        .DDR_CLK_EDGE   ("SAME_EDGE"), //"OPPOSITE_EDGE"  "SAME_EDGE
        .INIT           (1'b0),
        .SRTYPE         ("ASYNC")
    )
    Inst_DQSODDR
    (
        .D1             (iPO_DQStrobe[0]),
        .D2             (iPO_DQStrobe[1]),
        .C              (iSystemClock),
        .CE             (1),
        .Q              (oDQSToNAND),
        .R              (iModuleReset),
        .S              (1'b0)
    );

    ODDR
    #(
        .DDR_CLK_EDGE   ("SAME_EDGE"), //"OPPOSITE_EDGE"  "SAME_EDGE
        .INIT           (1'b0),
        .SRTYPE         ("ASYNC")
    )
    Inst_DQSTODDR
    (
        .D1             (rDQSOut_IOBUF_T),
        .D2             (rDQSOut_IOBUF_T),
        .C              (iSystemClock),
        .CE             (1),
        .Q              (oDQSOutEnableToPinpad),
        .R              (iModuleReset),
        .S              (1'b0)
    );

    generate
    for (c = 0; c < 8; c = c + 1)
    begin : DQOSERDESBits

        ODDR
        #(
            .DDR_CLK_EDGE   ("SAME_EDGE"), //"OPPOSITE_EDGE"  "SAME_EDGE
            .INIT           (1'b0),
            .SRTYPE         ("ASYNC")
        )
        Inst_DQODDR
        (
            .D1             (iPO_DQ[ 0 + c]),
            .D2             (iPO_DQ[16 + c]),
            .C              (iSystemClock),
            .CE             (1),
            .Q              (oDQToNAND[c]),
            .R              (iModuleReset),
            .S              (1'b0)
        );

        ODDR
        #(
            .DDR_CLK_EDGE   ("SAME_EDGE"), //"OPPOSITE_EDGE"  "SAME_EDGE
            .INIT           (1'b0),
            .SRTYPE         ("ASYNC")
        )
        Inst_DQTODDR
        (
            .D1             (rDQOut_IOBUF_T),
            .D2             (rDQOut_IOBUF_T),
            .C              (iSystemClock),
            .CE             (1),
            .Q              (oDQOutEnableToPinpad[c]),
            .R              (iModuleReset),
            .S              (1'b0)
        );
    end
    endgenerate

    generate
    for (d = 0; d < NumberOfWays; d = d + 1)
    begin : CEOSERDESBits
        ODDR
        #(
            .DDR_CLK_EDGE   ("SAME_EDGE"), //"OPPOSITE_EDGE"  "SAME_EDGE
            .INIT           (1'b0),
            .SRTYPE         ("ASYNC")
        )
        Inst_CEODDR
        (
            .D1             (iPO_ChipEnable[0 + d]),
            .D2             (iPO_ChipEnable[0 + d]),
            .C              (iSystemClock),
            .CE             (1),
            .Q              (oCEToNAND[d]),
            .R              (iModuleReset),
            .S              (1'b0)
        );
    end
    endgenerate

    ODDR
    #(
        .DDR_CLK_EDGE   ("SAME_EDGE"), //"OPPOSITE_EDGE"  "SAME_EDGE
        .INIT           (1'b0),
        .SRTYPE         ("ASYNC")
    )
    Inst_REODDR
    (
        .D1             (iPO_ReadEnable[0]),
        .D2             (iPO_ReadEnable[1]),
        .C              (iSystemClock),
        .CE             (1),
        .Q              (oREToNAND),
        .R              (iModuleReset),
        .S              (1'b0)
    );

    ODDR
    #(
        .DDR_CLK_EDGE   ("SAME_EDGE"), //"OPPOSITE_EDGE"  "SAME_EDGE
        .INIT           (1'b0),
        .SRTYPE         ("ASYNC")
    )
    Inst_WEODDR
    (
        .D1             (iPO_WriteEnable[0]),
        .D2             (iPO_WriteEnable[1]),
        .C              (iSystemClock),
        .CE             (1),
        .Q              (oWEToNAND),
        .R              (iModuleReset),
        .S              (1'b0)
    );

    ODDR
    #(
        .DDR_CLK_EDGE   ("SAME_EDGE"), //"OPPOSITE_EDGE"  "SAME_EDGE
        .INIT           (1'b0),
        .SRTYPE         ("ASYNC")
    )
    Inst_ALEODDR
    (
        .D1             (iPO_AddressLatchEnable[0]),
        .D2             (iPO_AddressLatchEnable[1]),
        .C              (iSystemClock),
        .CE             (1),
        .Q              (oALEToNAND),
        .R              (iModuleReset),
        .S              (1'b0)
    );

    ODDR
    #(
        .DDR_CLK_EDGE   ("SAME_EDGE"), //"OPPOSITE_EDGE"  "SAME_EDGE
        .INIT           (1'b0),
        .SRTYPE         ("ASYNC")
    )
    Inst_CLEODDR
    (
        .D1             (iPO_CommandLatchEnable[0]),
        .D2             (iPO_CommandLatchEnable[1]),
        .C              (iSystemClock),
        .CE             (1),
        .Q              (oCLEToNAND),
        .R              (iModuleReset),
        .S              (1'b0)
    );


endmodule
