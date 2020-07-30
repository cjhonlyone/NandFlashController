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
    // ila_0 ila0(
    // .clk(iSystemClock),
    // .probe0(iPO_DQStrobe[3:0]),
    // .probe1(iPO_DQ[15:0])
    // );
    OSERDESE2
    #
    (
        .DATA_RATE_OQ   ("DDR"      ),
        //.DATA_RATE_TQ   ("SDR"      ),
        .DATA_RATE_TQ   ("BUF"      ),
        .DATA_WIDTH     (4          ),
        .INIT_OQ        (1'b0       ),
        .INIT_TQ        (1'b1       ),
        .SERDES_MODE    ("MASTER"   ),
        .SRVAL_OQ       (1'b0       ),
        .SRVAL_TQ       (1'b1       ),
        .TRISTATE_WIDTH (1          )
    )
    Inst_DQSOSERDES
    (
        .OFB        (                       ),
        .OQ         (oDQSToNAND             ),
        .SHIFTOUT1  (                       ),
        .SHIFTOUT2  (                       ),
        .TBYTEOUT   (                       ),
        .TFB        (                       ),
        .TQ         (oDQSOutEnableToPinpad  ), // to pinpad

        .CLK        (iOutputDrivingClock    ),
        .CLKDIV     (iSystemClock           ),
        .D1         (iPO_DQStrobe[0]        ),
        .D2         (iPO_DQStrobe[1]        ),
        .D3         (iPO_DQStrobe[2]        ),
        .D4         (iPO_DQStrobe[3]        ),
        .D5         (iPO_DQStrobe[4]        ),
        .D6         (iPO_DQStrobe[5]        ),
        .D7         (iPO_DQStrobe[6]        ),
        .D8         (iPO_DQStrobe[7]        ),
        .OCE        (1'b1                   ),
        .RST        (iModuleReset           ),
        .SHIFTIN1   (0                      ),
        .SHIFTIN2   (0                      ),
        .T1         (rDQSOut_IOBUF_T        ), // from P.M.
        .T2         (0                      ),
        .T3         (0                      ),
        .T4         (0                      ),
        .TBYTEIN    (0                      ),
        .TCE        (1'b1                   )
    );

    generate
    for (c = 0; c < 8; c = c + 1)
    begin : DQOSERDESBits
        OSERDESE2
        #
        (
            .DATA_RATE_OQ   ("DDR"      ),
            //.DATA_RATE_TQ   ("SDR"      ),
            .DATA_RATE_TQ   ("BUF"      ),
            .DATA_WIDTH     (4          ),
            .INIT_OQ        (1'b0       ),
            .INIT_TQ        (1'b1       ),
            .SERDES_MODE    ("MASTER"   ),
            .SRVAL_OQ       (1'b0       ),
            .SRVAL_TQ       (1'b1       ),
            .TRISTATE_WIDTH (1          )
        )
        Inst_DQOSERDES
        (
            .OFB        (                   ),
            .OQ         (oDQToNAND[c]       ),
            .SHIFTOUT1  (                   ),
            .SHIFTOUT2  (                   ),
            .TBYTEOUT   (                   ),
            .TFB        (                   ),
            .TQ         (oDQOutEnableToPinpad[c]), // to pinpad

            .CLK        (iOutputDrivingClock),
            .CLKDIV     (iSystemClock       ),
            .D1         (iPO_DQ[ 0 + c]     ),
            .D2         (iPO_DQ[ 8 + c]     ),
            .D3         (iPO_DQ[16 + c]     ),
            .D4         (iPO_DQ[24 + c]     ),
            .D5         (iPO_DQ[16 + c]     ),
            .D6         (iPO_DQ[16 + c]     ),
            .D7         (iPO_DQ[24 + c]     ),
            .D8         (iPO_DQ[24 + c]     ),
            .OCE        (1'b1               ),
            .RST        (iModuleReset       ),
            .SHIFTIN1   (0                  ),
            .SHIFTIN2   (0                  ),
            .T1         (rDQOut_IOBUF_T     ), // from P.M.
            .T2         (0                  ),
            .T3         (0                  ),
            .T4         (0                  ),
            .TBYTEIN    (0                  ),
            .TCE        (1'b1               )
        );
    end
    endgenerate

    generate
    for (d = 0; d < NumberOfWays; d = d + 1)
    begin : CEOSERDESBits
        OSERDESE2
        #
        (
            .DATA_RATE_OQ   ("DDR"      ),
            //.DATA_RATE_TQ   ("SDR"      ),
            .DATA_RATE_TQ   ("BUF"      ),
            .DATA_WIDTH     (4          ),
            .INIT_OQ        (1'b1       ),
            //.INIT_OQ        (1'b0       ),
            .INIT_TQ        (1'b0       ),
            .SERDES_MODE    ("MASTER"   ),
            .SRVAL_OQ       (1'b1       ),
            //.SRVAL_OQ       (1'b0       ),
            .SRVAL_TQ       (1'b0       ),
            .TRISTATE_WIDTH (1          ),
            
            
            .IS_D1_INVERTED (1'b1       ),
            .IS_D2_INVERTED (1'b1       ),
            .IS_D3_INVERTED (1'b1       ),
            .IS_D4_INVERTED (1'b1       ),
            .IS_D5_INVERTED (1'b1       ),
            .IS_D6_INVERTED (1'b1       ),
            .IS_D7_INVERTED (1'b1       ),
            .IS_D8_INVERTED (1'b1       )
        )
        Inst_CEOSERDES
        (
            .OFB        (                       ),
            .OQ         (oCEToNAND[d]           ),
            .SHIFTOUT1  (                       ),
            .SHIFTOUT2  (                       ),
            .TBYTEOUT   (                       ),
            .TFB        (                       ),
            .TQ         (                       ),

            .CLK        (iOutputDrivingClock    ),
            .CLKDIV     (iSystemClock           ),
            .D1         (iPO_ChipEnable[0 + d]  ),
            .D2         (iPO_ChipEnable[0 + d]  ),
            .D3         (iPO_ChipEnable[0 + d]  ),
            .D4         (iPO_ChipEnable[0 + d]  ),
            .D5         (iPO_ChipEnable[NumberOfWays + d]),
            .D6         (iPO_ChipEnable[NumberOfWays + d]),
            .D7         (iPO_ChipEnable[NumberOfWays + d]),
            .D8         (iPO_ChipEnable[NumberOfWays + d]),
            .OCE        (1'b1                   ),
            .RST        (iModuleReset           ),
            .SHIFTIN1   (0                      ),
            .SHIFTIN2   (0                      ),
            .T1         (1'b0                   ),
            .T2         (0                      ),
            .T3         (0                      ),
            .T4         (0                      ),
            .TBYTEIN    (0                      ),
            .TCE        (1'b1                   )
        );
    end
    endgenerate
    
    OSERDESE2
    #
    (
        .DATA_RATE_OQ   ("DDR"      ),
        //.DATA_RATE_TQ   ("SDR"      ),
        .DATA_RATE_TQ   ("BUF"      ),
        .DATA_WIDTH     (4          ),
        .INIT_OQ        (1'b1       ),
        //.INIT_OQ        (1'b0       ),
        .INIT_TQ        (1'b0       ),
        .SERDES_MODE    ("MASTER"   ),
        .SRVAL_OQ       (1'b1       ),
        //.SRVAL_OQ       (1'b0       ),
        .SRVAL_TQ       (1'b0       ),
        .TRISTATE_WIDTH (1          )
        
        /* // single-ended
        .IS_D1_INVERTED (1'b1       ),
        .IS_D2_INVERTED (1'b1       ),
        .IS_D3_INVERTED (1'b1       ),
        .IS_D4_INVERTED (1'b1       ),
        .IS_D5_INVERTED (1'b1       ),
        .IS_D6_INVERTED (1'b1       ),
        .IS_D7_INVERTED (1'b1       ),
        .IS_D8_INVERTED (1'b1       )
        */
    )
    Inst_REOSERDES
    (
        .OFB        (                       ),
        .OQ         (oREToNAND              ),
        .SHIFTOUT1  (                       ),
        .SHIFTOUT2  (                       ),
        .TBYTEOUT   (                       ),
        .TFB        (                       ),
        .TQ         (                       ),

        .CLK        (iOutputDrivingClock    ),
        .CLKDIV     (iSystemClock           ),
        .D1         (iPO_ReadEnable[0]      ),
        .D2         (iPO_ReadEnable[0]      ),
        .D3         (iPO_ReadEnable[1]      ),
        .D4         (iPO_ReadEnable[1]      ),
        .D5         (iPO_ReadEnable[2]      ),
        .D6         (iPO_ReadEnable[2]      ),
        .D7         (iPO_ReadEnable[3]      ),
        .D8         (iPO_ReadEnable[3]      ),
        .OCE        (1'b1                   ),
        .RST        (iModuleReset           ),
        .SHIFTIN1   (0                      ),
        .SHIFTIN2   (0                      ),
        .T1         (1'b0                   ),
        .T2         (0                      ),
        .T3         (0                      ),
        .T4         (0                      ),
        .TBYTEIN    (0                      ),
        .TCE        (1'b1                   )
    );
    
    OSERDESE2
    #
    (
        .DATA_RATE_OQ   ("DDR"      ),
        //.DATA_RATE_TQ   ("SDR"      ),
        .DATA_RATE_TQ   ("BUF"      ),
        .DATA_WIDTH     (4          ),
        .INIT_OQ        (1'b1       ),
        //.INIT_OQ        (1'b0       ),
        .INIT_TQ        (1'b0       ),
        .SERDES_MODE    ("MASTER"   ),
        .SRVAL_OQ       (1'b1       ),
        //.SRVAL_OQ       (1'b0       ),
        .SRVAL_TQ       (1'b0       ),
        .TRISTATE_WIDTH (1          ),
        
        .IS_D1_INVERTED (1'b1       ),
        .IS_D2_INVERTED (1'b1       ),
        .IS_D3_INVERTED (1'b1       ),
        .IS_D4_INVERTED (1'b1       ),
        .IS_D5_INVERTED (1'b1       ),
        .IS_D6_INVERTED (1'b1       ),
        .IS_D7_INVERTED (1'b1       ),
        .IS_D8_INVERTED (1'b1       )
    )
    Inst_WEOSERDES
    (
        .OFB        (                       ),
        .OQ         (oWEToNAND              ),
        .SHIFTOUT1  (                       ),
        .SHIFTOUT2  (                       ),
        .TBYTEOUT   (                       ),
        .TFB        (                       ),
        .TQ         (                       ),

        .CLK        (iOutputDrivingClock    ),
        .CLKDIV     (iSystemClock           ),
        .D1         (iPO_WriteEnable[0]     ),
        .D2         (iPO_WriteEnable[0]     ),
        .D3         (iPO_WriteEnable[1]     ),
        .D4         (iPO_WriteEnable[1]     ),
        .D5         (iPO_WriteEnable[2]     ),
        .D6         (iPO_WriteEnable[2]     ),
        .D7         (iPO_WriteEnable[3]     ),
        .D8         (iPO_WriteEnable[3]     ),
        .OCE        (1'b1                   ),
        .RST        (iModuleReset           ),
        .SHIFTIN1   (0                      ),
        .SHIFTIN2   (0                      ),
        .T1         (1'b0                   ),
        .T2         (0                      ),
        .T3         (0                      ),
        .T4         (0                      ),
        .TBYTEIN    (0                      ),
        .TCE        (1'b1                   )
    );
    
    OSERDESE2
    #
    (
        .DATA_RATE_OQ   ("DDR"      ),
        //.DATA_RATE_TQ   ("SDR"      ),
        .DATA_RATE_TQ   ("BUF"      ),
        .DATA_WIDTH     (4          ),
        .INIT_OQ        (1'b0       ),
        .INIT_TQ        (1'b0       ),
        .SERDES_MODE    ("MASTER"   ),
        .SRVAL_OQ       (1'b0       ),
        .SRVAL_TQ       (1'b0       ),
        .TRISTATE_WIDTH (1          )
    )
    Inst_ALEOSERDES
    (
        .OFB        (                       ),
        .OQ         (oALEToNAND             ),
        .SHIFTOUT1  (                       ),
        .SHIFTOUT2  (                       ),
        .TBYTEOUT   (                       ),
        .TFB        (                       ),
        .TQ         (                       ),

        .CLK        (iOutputDrivingClock    ),
        .CLKDIV     (iSystemClock           ),
        .D1         (iPO_AddressLatchEnable[0]),
        .D2         (iPO_AddressLatchEnable[0]),
        .D3         (iPO_AddressLatchEnable[1]),
        .D4         (iPO_AddressLatchEnable[1]),
        .D5         (iPO_AddressLatchEnable[2]),
        .D6         (iPO_AddressLatchEnable[2]),
        .D7         (iPO_AddressLatchEnable[3]),
        .D8         (iPO_AddressLatchEnable[3]),
        .OCE        (1'b1                   ),
        .RST        (iModuleReset           ),
        .SHIFTIN1   (0                      ),
        .SHIFTIN2   (0                      ),
        .T1         (1'b0                   ),
        .T2         (0                      ),
        .T3         (0                      ),
        .T4         (0                      ),
        .TBYTEIN    (0                      ),
        .TCE        (1'b1                   )
    );

    OSERDESE2
    #
    (
        .DATA_RATE_OQ   ("DDR"      ),
        //.DATA_RATE_TQ   ("SDR"      ),
        .DATA_RATE_TQ   ("BUF"      ),
        .DATA_WIDTH     (4          ),
        .INIT_OQ        (1'b0       ),
        .INIT_TQ        (1'b0       ),
        .SERDES_MODE    ("MASTER"   ),
        .SRVAL_OQ       (1'b0       ),
        .SRVAL_TQ       (1'b0       ),
        .TRISTATE_WIDTH (1          )
    )
    Inst_CLEOSERDES
    (
        .OFB        (                       ),
        .OQ         (oCLEToNAND             ),
        .SHIFTOUT1  (                       ),
        .SHIFTOUT2  (                       ),
        .TBYTEOUT   (                       ),
        .TFB        (                       ),
        .TQ         (                       ),

        .CLK        (iOutputDrivingClock    ),
        .CLKDIV     (iSystemClock           ),
        .D1         (iPO_CommandLatchEnable[0]),
        .D2         (iPO_CommandLatchEnable[0]),
        .D3         (iPO_CommandLatchEnable[1]),
        .D4         (iPO_CommandLatchEnable[1]),
        .D5         (iPO_CommandLatchEnable[2]),
        .D6         (iPO_CommandLatchEnable[2]),
        .D7         (iPO_CommandLatchEnable[3]),
        .D8         (iPO_CommandLatchEnable[3]),
        .OCE        (1'b1                   ),
        .RST        (iModuleReset           ),
        .SHIFTIN1   (0                      ),
        .SHIFTIN2   (0                      ),
        .T1         (1'b0                   ),
        .T2         (0                      ),
        .T3         (0                      ),
        .T4         (0                      ),
        .TBYTEIN    (0                      ),
        .TCE        (1'b1                   )
    );

endmodule
