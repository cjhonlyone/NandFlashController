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


module tb_NandFlashController_Top;

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
            #6000;
            iSystemClock <= 1'b0;
            #6000;
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
            #3000;
            iDelayRefClock      <= 1'b0;
            iOutputDrivingClock <= 1'b0;
            #3000;
        end
    end
 
    reg   [5:0]                   iOpcode              ;
    reg   [4:0]                   iTargetID            ;
    reg   [4:0]                   iSourceID            ;
    reg   [31:0]                  iAddress             ;
    reg   [15:0]                  iLength              ;
    reg                           iCMDValid            ;
    wire                          oCMDReady            ;

    reg   [15:0]                  iWriteData           ;
    reg                           iWriteLast           ;
    reg                           iWriteValid          ;
    reg   [1:0]                   iWriteKeep           ;
    wire                          oWriteReady          ;

    wire  [15:0]                  oReadData            ;
    wire                          oReadLast            ;
    wire                          oReadValid           ;
    wire   [1:0]                  oReadKeep            ;
    reg                           iReadReady           ;

    wire  [NumberOfWays - 1:0]    oReadyBusy           ;

    wire  [23:0]                  oStatus                 ;
    wire                          oStatusValid            ;

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

    NandFlashController_Top #(
            .IDelayValue(IDelayValue),
            .InputClockBufferType(InputClockBufferType),
            .NumberOfWays(NumberOfWays)
        ) inst_NandFlashController_Top (
            .iSystemClock        (iSystemClock),
            .iDelayRefClock      (iDelayRefClock),
            .iOutputDrivingClock (iOutputDrivingClock),
            .iReset              (iReset),

            .iOpcode             (iOpcode),
            .iTargetID           (iTargetID),
            .iSourceID           (iSourceID),
            .iAddress            (iAddress),
            .iLength             (iLength),
            .iCMDValid           (iCMDValid),
            .oCMDReady           (oCMDReady),

            .iWriteData          (iWriteData),
            .iWriteLast          (iWriteLast),
            .iWriteValid         (iWriteValid),
            .iWriteKeep          (iWriteKeep),
            .oWriteReady         (oWriteReady),

            .oReadData           (oReadData),
            .oReadLast           (oReadLast),
            .oReadValid          (oReadValid),
            .oReadKeep           (oReadKeep),
            .iReadReady          (iReadReady),

            .oReadyBusy          (oReadyBusy),

            .oStatus             (oStatus     ),
            .oStatusValid        (oStatusValid),

            .IO_NAND_DQS         (IO_NAND_DQS),
            .IO_NAND_DQ          (IO_NAND_DQ),
            .O_NAND_CE           (O_NAND_CE),
            .O_NAND_WE           (O_NAND_WE),
            .O_NAND_RE           (O_NAND_RE),
            .O_NAND_ALE          (O_NAND_ALE),
            .O_NAND_CLE          (O_NAND_CLE),
            .I_NAND_RB           (I_NAND_RB),
            .O_NAND_WP           (O_NAND_WP)
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


    reg [7:0]  global_way;
    reg [15:0] global_col;
    reg [23:0] global_row;

    reg RDY  ;
    reg ARDY ;

    integer seed;

    initial  begin seed =  0; end

    reg [15:0] memory [0:12*2160-1];

    task NFC_signal;
        input   [5:0]                   rOpcode              ;
        input   [4:0]                   rTargetID            ;
        input   [4:0]                   rSourceID            ;
        input   [31:0]                  rAddress             ;
        input   [15:0]                  rLength              ;
        input                           rCMDValid            ;
        input   [31:0]                  rWriteData           ;
        input                           rWriteLast           ;
        input                           rWriteValid          ;
        input                           rReadReady           ;

        begin
            @(posedge iSystemClock);   
                iOpcode     <= rOpcode    ;
                iTargetID   <= rTargetID  ;
                iSourceID   <= rSourceID  ;
                iAddress    <= rAddress   ;
                iLength     <= rLength    ;
                iCMDValid   <= rCMDValid  ;

                iWriteData  <= rWriteData ;
                iWriteLast  <= rWriteLast ;
                iWriteValid <= rWriteValid;
                iReadReady  <= 1 ;      
        end
    endtask

    task reset_ffh;
        begin

        NFC_signal(6'b000001, 5'b00101, 0, 32'h00000000, 16'h0000, 1, 16'h0000, 0, 0, 0);
        NFC_signal(6'b000001, 5'b00101, 0, 32'h00000000, 16'h0000, 0, 16'h0000, 0, 0, 0);
        @(posedge iSystemClock);
        wait(oCMDReady == 0);
        end
    endtask

    task setfeature_efh;
        begin
        NFC_signal(6'b000010, 5'b00101, 0, 32'h00000000, 16'h0000, 1, 16'h0000, 0, 0, 0);
        NFC_signal(6'b000010, 5'b00101, 0, 32'h00000000, 16'h0000, 0, 16'h0000, 0, 0, 0);
        @(posedge iSystemClock);
        wait(oCMDReady == 0);
        end
    endtask


    task getfeature_eeh;
        begin
        NFC_signal(6'b000101, 5'b00101, 0, 32'h00000000, 16'h0008, 1, 16'h0000, 0, 0, 0);
        NFC_signal(6'b000101, 5'b00101, 0, 32'h00000000, 16'h0000, 0, 16'h0000, 0, 0, 0);
        @(posedge iSystemClock);
        wait(oCMDReady == 0);
        end
    endtask



    task progpage_80h_10h;
        input [15:0] number;
        begin
        NFC_signal(6'b000011, 5'b00000, 0, 32'h00000000,   number, 1, 16'h0000, 0, 0, 0);
        NFC_signal(6'b000011, 5'b00000, 0, 32'h00000000, 16'h0000, 0, 16'h0000, 0, 0, 0);
        @(posedge iSystemClock);
        wait(oCMDReady == 0);
        end
    endtask

    task progpage_80h_15h_cache;
        input [15:0] number;
        begin
        NFC_signal(6'b000011, 5'b00001, 0, 32'h00000000,   number, 1, 16'h0000, 0, 0, 0);
        NFC_signal(6'b000011, 5'b00001, 0, 32'h00000000, 16'h0000, 0, 16'h0000, 0, 0, 0);
        @(posedge iSystemClock);
        wait(oCMDReady == 0);
        end
    endtask

    task progpage_80h_10h_multplane;
        input [15:0] number;
        begin
        NFC_signal(6'b000011, 5'b00010, 0, 32'h00000000,   number, 1, 16'h0000, 0, 0, 0);
        NFC_signal(6'b000011, 5'b00010, 0, 32'h00000000, 16'h0000, 0, 16'h0000, 0, 0, 0);
        @(posedge iSystemClock);
        wait(oCMDReady == 0);
        end
    endtask

    task readpage_00h_30h;
        input [15:0] number;
        input check;
        integer m;
        integer base_adr;
        begin
        NFC_signal(6'b000100, 5'b00101, 0, 32'h00000000,   number, 1, 16'h0000, 0, 0, 0);
        NFC_signal(6'b000100, 5'b00101, 0, 32'h00000000, 16'h0000, 0, 16'h0000, 0, 0, 0);

        if (check) begin
            m = 0;
            base_adr = global_row[7]*6 + global_row[2:0];
            @(posedge iSystemClock);
            wait(oReadValid == 1);
            while (oReadValid & (~ oReadLast)) begin
                @(posedge iSystemClock);
                if (memory[base_adr*2160 + m] != oReadData) begin
                    $display("Error Read data wrong %d %d %04x %04x",base_adr,m,memory[base_adr*2160 + m],oReadData);
                    $stop;
                end
                m <= m + 1;
            end 
        end else begin
            @(posedge iSystemClock);
            wait(oCMDReady == 0);
        end

        end
    endtask

    task eraseblock_60h_d0h;
        begin
        NFC_signal(6'b000110, 5'b00000, 0, 32'h00000000, 16'h0008, 1, 16'h0000, 0, 0, 0);
        NFC_signal(6'b000110, 5'b00000, 0, 32'h00000000, 16'h0000, 0, 16'h0000, 0, 0, 0);
        @(posedge iSystemClock);
        wait(oCMDReady == 0);
        end
    endtask

    task eraseblock_60h_d1h_multiplane;
        begin
        NFC_signal(6'b000110, 5'b00010, 0, 32'h00000000, 16'h0008, 1, 16'h0000, 0, 0, 0);
        NFC_signal(6'b000110, 5'b00010, 0, 32'h00000000, 16'h0000, 0, 16'h0000, 0, 0, 0);
        @(posedge iSystemClock);
        wait(oCMDReady == 0);
        end
    endtask

    task readstatus_70h;
        begin
        NFC_signal(6'b000111, 5'b00100, 0, 32'h00000000, 16'h0008, 1, 16'h0000, 0, 0, 0);
        NFC_signal(6'b000111, 5'b00100, 0, 32'h00000000, 16'h0000, 0, 16'h0000, 0, 0, 0);
        @(posedge iSystemClock);
        wait(oCMDReady == 0);
        end
    endtask

    task readstatus_78h;
        begin
        NFC_signal(6'b000111, 5'b00101, 0, 32'h00000000, 16'h0008, 1, 16'h0000, 0, 0, 0);
        NFC_signal(6'b000111, 5'b00101, 0, 32'h00000000, 16'h0000, 0, 16'h0000, 0, 0, 0);
        @(posedge iSystemClock);
        wait(oCMDReady == 0);
        end
    endtask



    task select_way;
        input [7:0] way;
        begin
        global_way = way;
        NFC_signal(6'b100000, 5'b00000, 0, {24'd0,way}, 16'h0008, 1, 16'h0000, 0, 0, 0);
        end
    endtask

    task set_coladdr;
        input [15:0] col;
        begin
        global_col = col;
        NFC_signal(6'b100010, 5'b00000, 0, {16'd0,col}, 16'h0008, 1, 16'h0000, 0, 0, 0);
        end
    endtask

    task set_rowaddr;
        input [23:0] row;
        begin
        global_row = row;
        NFC_signal(6'b100100, 5'b00000, 0, {8'd0,row}, 16'h0008, 1, 16'h0000, 0, 0, 0);
        end
    endtask



    always @ (posedge iSystemClock) begin
        if (iReset) begin
            RDY  <= 0;
            ARDY <= 0;
        end else if (oStatusValid)  begin
            RDY  <= oStatus[6];
            ARDY <= oStatus[5];
        end
    end


    task s_axis_input;
        input [23:0] rowaddr;
        input [15:0] number;
        integer base_adr;
        integer j;
        begin
            base_adr = rowaddr[7]*6 + rowaddr[2:0];
            for(j=0;j<number[15:1];j=j+1)
            begin
                if (j == 0) begin
                    @(posedge iSystemClock);   
                    iWriteValid = 1;
                    iWriteData  = rowaddr[15:0];
                    iWriteKeep  = 2'b11;
                    iWriteLast  = 0;
                end else if (j == 1) begin
                    @(posedge iSystemClock);   
                    iWriteValid = 1;
                    iWriteData  = {8'h00, rowaddr[23:16]};
                    iWriteKeep  = 2'b11;
                    iWriteLast  = 0;
                end else if (j == 2159) begin
                    @(posedge iSystemClock);   
                    iWriteValid = 1;
                    iWriteData  = $random(seed);
                    iWriteKeep  = 2'b11;
                    iWriteLast  = 1;
                end else begin
                    @(posedge iSystemClock);   
                    iWriteValid = 1;
                    iWriteData  = $random(seed);
                    iWriteKeep  = 2'b11;
                    iWriteLast  = 0;
                end
                memory[base_adr*2160 + j] = iWriteData;
            end
            @(posedge iSystemClock);   
            iWriteValid = 0;
            iWriteData  = 16'h0000;
            iWriteKeep  = 2'b00;
            iWriteLast  = 1;
        end
    endtask


    integer I;
    task program_multiplane_cache;
        input [11:0] block;
        input [6:0] page;
        input finished;

        reg [11:0] rblock;
        reg [6:0] rpage;
        begin
            rblock <= block + 1'b1;
            rpage <= page + 1'b1;
            // plane0 page0
            s_axis_input({{5'd0},{block},{page}}, 16'd4320);
            set_rowaddr({{5'd0},{block},page});
            readstatus_70h;
            while (RDY == 0) begin
                readstatus_70h;
            end
            progpage_80h_10h_multplane(16'd4320);

            // plane1 page0
            s_axis_input({{5'd0},{rblock},{page}}, 16'd4320);
            set_rowaddr({{5'd0},{rblock},page});
            readstatus_78h;
            while (RDY == 0) begin
                readstatus_78h;
            end
            progpage_80h_15h_cache(16'd4320);

            // plane0 page1 cache
            s_axis_input({{5'd0},{block},{rpage}}, 16'd4320);
            set_rowaddr({{5'd0},{block},{rpage}});
            readstatus_78h;
            while (RDY == 0) begin
                readstatus_78h;
            end
            progpage_80h_10h_multplane(16'd4320);

            // plane1 page1 cache
            s_axis_input({{5'd0},{rblock},{rpage}}, 16'd4320);
            set_rowaddr({{5'd0},{rblock},{rpage}});
            readstatus_78h;
            while (RDY == 0) begin
                readstatus_78h;
            end
            if (finished)
                progpage_80h_10h(16'd4320);
            else
                progpage_80h_15h_cache(16'd4320);

        end
    endtask

    initial
    
        begin
        // $dumpfile("./tb_NFC_Physical_Top.vcd");
        // $dumpvars(0, tb_NFC_Physical_Top);
        iReset <= 1;
        NFC_signal(6'b000001, 5'b00101, 0, 32'h00000000, 16'h0000, 0, 16'h0000, 0, 0, 0);
        for (I = 0; I < 10; I = I + 1)
            @(posedge iSystemClock);
        iReset <= 0;
        # 1000000
        for (I = 0; I < 10; I = I + 1)
            @(posedge iSystemClock);
        
        select_way(8'd1);
        reset_ffh;
        setfeature_efh;
        getfeature_eeh;

        program_multiplane_cache(11'd0, 7'd0, 0);
        program_multiplane_cache(11'd0, 7'd2, 0);
        program_multiplane_cache(11'd0, 7'd4, 1);

        while (ARDY == 0) begin
            readstatus_70h;
        end

        set_rowaddr({{5'd0},{11'd0},{7'd0}});
        readpage_00h_30h(16'd4320,1);
        set_rowaddr({{5'd0},{11'd0},{7'd1}});
        readpage_00h_30h(16'd4320,1);
        set_rowaddr({{5'd0},{11'd0},{7'd2}});
        readpage_00h_30h(16'd4320,1);
        set_rowaddr({{5'd0},{11'd0},{7'd3}});
        readpage_00h_30h(16'd4320,1);
        set_rowaddr({{5'd0},{11'd0},{7'd4}});
        readpage_00h_30h(16'd4320,1);
        set_rowaddr({{5'd0},{11'd0},{7'd5}});
        readpage_00h_30h(16'd4320,1);


        set_rowaddr({{5'd0},{11'd1},{7'd0}});
        readpage_00h_30h(16'd4320,1);
        set_rowaddr({{5'd0},{11'd1},{7'd1}});
        readpage_00h_30h(16'd4320,1);
        set_rowaddr({{5'd0},{11'd1},{7'd2}});
        readpage_00h_30h(16'd4320,1);
        set_rowaddr({{5'd0},{11'd1},{7'd3}});
        readpage_00h_30h(16'd4320,1);
        set_rowaddr({{5'd0},{11'd1},{7'd4}});
        readpage_00h_30h(16'd4320,1);
        set_rowaddr({{5'd0},{11'd1},{7'd5}});
        readpage_00h_30h(16'd4320,1);


        set_rowaddr({{5'd0},{11'd0},{7'd0}});
        eraseblock_60h_d1h_multiplane;
        set_rowaddr({{5'd0},{11'd1},{7'd0}});
        readstatus_78h;
        while (ARDY == 0) begin
            readstatus_78h;
        end

        eraseblock_60h_d0h;
        readstatus_78h;
        while (ARDY == 0) begin
            readstatus_78h;
        end

        set_rowaddr({{5'd0},{11'd0},{7'd0}});
        readpage_00h_30h(16'd4320,0);
        set_rowaddr({{5'd0},{11'd0},{7'd1}});
        readpage_00h_30h(16'd4320,0);
        set_rowaddr({{5'd0},{11'd0},{7'd2}});
        readpage_00h_30h(16'd4320,0);
        set_rowaddr({{5'd0},{11'd0},{7'd3}});
        readpage_00h_30h(16'd4320,0);
        set_rowaddr({{5'd0},{11'd0},{7'd4}});
        readpage_00h_30h(16'd4320,0);
        set_rowaddr({{5'd0},{11'd0},{7'd5}});
        readpage_00h_30h(16'd4320,0);


        set_rowaddr({{5'd0},{11'd1},{7'd0}});
        readpage_00h_30h(16'd4320,0);
        set_rowaddr({{5'd0},{11'd1},{7'd1}});
        readpage_00h_30h(16'd4320,0);
        set_rowaddr({{5'd0},{11'd1},{7'd2}});
        readpage_00h_30h(16'd4320,0);
        set_rowaddr({{5'd0},{11'd1},{7'd3}});
        readpage_00h_30h(16'd4320,0);
        set_rowaddr({{5'd0},{11'd1},{7'd4}});
        readpage_00h_30h(16'd4320,0);
        set_rowaddr({{5'd0},{11'd1},{7'd5}});
        readpage_00h_30h(16'd4320,0);

        $display("test finished!");
        repeat (50) @(posedge iSystemClock);
        // $finish;
        end
    
endmodule
