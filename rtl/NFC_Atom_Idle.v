`timescale 1ns / 1ps

module NFC_Atom_Command_Idle
#
(
    parameter NumberOfWays    =   4
)
(
    oDQSOutEnable           ,
    oDQOutEnable            ,
    oDQStrobe               ,
    oDQ                     ,
    oChipEnable             ,
    oReadEnable             ,
    oWriteEnable            ,
    oAddressLatchEnable     ,
    oCommandLatchEnable  
);
    output                          oDQSOutEnable           ;
    output                          oDQOutEnable            ;

    output  [7:0]                   oDQStrobe               ;
    output  [31:0]                  oDQ                     ;
    output  [2*NumberOfWays - 1:0]  oChipEnable             ;
    output  [3:0]                   oReadEnable             ;
    output  [3:0]                   oWriteEnable            ;
    output  [3:0]                   oAddressLatchEnable     ;
    output  [3:0]                   oCommandLatchEnable     ;

    assign oDQSOutEnable       = 1                          ;
    assign oDQOutEnable        = 1                          ;   

    assign oDQStrobe           = 8'h0                       ;   
    assign oDQ                 = 32'h0000                   ;   
    assign oChipEnable         = { 2*NumberOfWays{1'b0} }   ;   
    assign oReadEnable         = 4'b0011                    ;   
    assign oWriteEnable        = 4'b0000                    ;   
    assign oAddressLatchEnable = 4'h0                       ;   
    assign oCommandLatchEnable = 4'h0                       ; 

endmodule
