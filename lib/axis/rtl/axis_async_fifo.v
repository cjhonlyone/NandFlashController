/*

Copyright (c) 2014-2018 Alex Forencich

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/

// Language: Verilog 2001

`timescale 1ns / 1ps

/*
 * AXI4-Stream asynchronous FIFO
 */
module axis_async_fifo #
(
    // FIFO depth in words
    // KEEP_WIDTH words per cycle if KEEP_ENABLE set
    // Rounded up to nearest power of 2 cycles
    parameter DEPTH = 4096,
    // Width of AXI stream interfaces in bits
    parameter DATA_WIDTH = 8,
    // Propagate tkeep signal
    // If disabled, tkeep assumed to be 1'b1
    parameter KEEP_ENABLE = (DATA_WIDTH>8),
    // tkeep signal width (words per cycle)
    parameter KEEP_WIDTH = (DATA_WIDTH/8),
    // Propagate tlast signal
    parameter LAST_ENABLE = 1,
    // Propagate tid signal
    parameter ID_ENABLE = 0,
    // tid signal width
    parameter ID_WIDTH = 8,
    // Propagate tdest signal
    parameter DEST_ENABLE = 0,
    // tdest signal width
    parameter DEST_WIDTH = 8,
    // Propagate tuser signal
    parameter USER_ENABLE = 1,
    // tuser signal width
    parameter USER_WIDTH = 1,
    // Frame FIFO mode - operate on frames instead of cycles
    // When set, m_axis_tvalid will not be deasserted within a frame
    // Requires LAST_ENABLE set
    parameter FRAME_FIFO = 0,
    // tuser value for bad frame marker
    parameter USER_BAD_FRAME_VALUE = 1'b1,
    // tuser mask for bad frame marker
    parameter USER_BAD_FRAME_MASK = 1'b1,
    // Drop frames marked bad
    // Requires FRAME_FIFO set
    parameter DROP_BAD_FRAME = 0,
    // Drop incoming frames when full
    // When set, s_axis_tready is always asserted
    // Requires FRAME_FIFO set
    parameter DROP_WHEN_FULL = 0
)
(
    /*
     * Common asynchronous reset
     */
    input  wire                   async_rst,

    /*
     * AXI input
     */
    input  wire                   s_clk,
    input  wire [DATA_WIDTH-1:0]  s_axis_tdata,
    input  wire [KEEP_WIDTH-1:0]  s_axis_tkeep,
    input  wire                   s_axis_tvalid,
    output wire                   s_axis_tready,
    input  wire                   s_axis_tlast,
    input  wire [ID_WIDTH-1:0]    s_axis_tid,
    input  wire [DEST_WIDTH-1:0]  s_axis_tdest,
    input  wire [USER_WIDTH-1:0]  s_axis_tuser,

    /*
     * AXI output
     */
    input  wire                   m_clk,
    output wire [DATA_WIDTH-1:0]  m_axis_tdata,
    output wire [KEEP_WIDTH-1:0]  m_axis_tkeep,
    output wire                   m_axis_tvalid,
    input  wire                   m_axis_tready,
    output wire                   m_axis_tlast,
    output wire [ID_WIDTH-1:0]    m_axis_tid,
    output wire [DEST_WIDTH-1:0]  m_axis_tdest,
    output wire [USER_WIDTH-1:0]  m_axis_tuser,

    /*
     * Status
     */
    output wire                   s_status_overflow,
    output wire                   s_status_bad_frame,
    output wire                   s_status_good_frame,
    output wire                   m_status_overflow,
    output wire                   m_status_bad_frame,
    output wire                   m_status_good_frame
);

parameter ADDR_WIDTH = (KEEP_ENABLE && KEEP_WIDTH > 1) ? $clog2(DEPTH/KEEP_WIDTH) : $clog2(DEPTH);

// check configuration
initial begin
    if (FRAME_FIFO && !LAST_ENABLE) begin
        $error("Error: FRAME_FIFO set requires LAST_ENABLE set (instance %m)");
        $finish;
    end

    if (DROP_BAD_FRAME && !FRAME_FIFO) begin
        $error("Error: DROP_BAD_FRAME set requires FRAME_FIFO set (instance %m)");
        $finish;
    end

    if (DROP_WHEN_FULL && !FRAME_FIFO) begin
        $error("Error: DROP_WHEN_FULL set requires FRAME_FIFO set (instance %m)");
        $finish;
    end

    if (DROP_BAD_FRAME && (USER_BAD_FRAME_MASK & {USER_WIDTH{1'b1}}) == 0) begin
        $error("Error: Invalid USER_BAD_FRAME_MASK value (instance %m)");
        $finish;
    end
end

localparam KEEP_OFFSET = DATA_WIDTH;
localparam LAST_OFFSET = KEEP_OFFSET + (KEEP_ENABLE ? KEEP_WIDTH : 0);
localparam ID_OFFSET   = LAST_OFFSET + (LAST_ENABLE ? 1          : 0);
localparam DEST_OFFSET = ID_OFFSET   + (ID_ENABLE   ? ID_WIDTH   : 0);
localparam USER_OFFSET = DEST_OFFSET + (DEST_ENABLE ? DEST_WIDTH : 0);
localparam WIDTH       = USER_OFFSET + (USER_ENABLE ? USER_WIDTH : 0);

reg [ADDR_WIDTH:0] wr_ptr_reg = {ADDR_WIDTH+1{1'b0}}, wr_ptr_next;
reg [ADDR_WIDTH:0] wr_ptr_cur_reg = {ADDR_WIDTH+1{1'b0}}, wr_ptr_cur_next;
reg [ADDR_WIDTH:0] wr_ptr_gray_reg = {ADDR_WIDTH+1{1'b0}}, wr_ptr_gray_next;
reg [ADDR_WIDTH:0] wr_ptr_sync_gray_reg = {ADDR_WIDTH+1{1'b0}}, wr_ptr_sync_gray_next;
reg [ADDR_WIDTH:0] wr_ptr_cur_gray_reg = {ADDR_WIDTH+1{1'b0}}, wr_ptr_cur_gray_next;
reg [ADDR_WIDTH:0] wr_addr_reg = {ADDR_WIDTH+1{1'b0}};
reg [ADDR_WIDTH:0] rd_ptr_reg = {ADDR_WIDTH+1{1'b0}}, rd_ptr_next;
reg [ADDR_WIDTH:0] rd_ptr_gray_reg = {ADDR_WIDTH+1{1'b0}}, rd_ptr_gray_next;
reg [ADDR_WIDTH:0] rd_addr_reg = {ADDR_WIDTH+1{1'b0}};

reg [ADDR_WIDTH:0] wr_ptr_gray_sync1_reg = {ADDR_WIDTH+1{1'b0}};
reg [ADDR_WIDTH:0] wr_ptr_gray_sync2_reg = {ADDR_WIDTH+1{1'b0}};
reg [ADDR_WIDTH:0] rd_ptr_gray_sync1_reg = {ADDR_WIDTH+1{1'b0}};
reg [ADDR_WIDTH:0] rd_ptr_gray_sync2_reg = {ADDR_WIDTH+1{1'b0}};

reg wr_ptr_update_valid_reg = 1'b0, wr_ptr_update_valid_next;
reg wr_ptr_update_reg = 1'b0, wr_ptr_update_next;
reg wr_ptr_update_sync1_reg = 1'b0;
reg wr_ptr_update_sync2_reg = 1'b0;
reg wr_ptr_update_sync3_reg = 1'b0;
reg wr_ptr_update_ack_sync1_reg = 1'b0;
reg wr_ptr_update_ack_sync2_reg = 1'b0;

reg s_rst_sync1_reg = 1'b1;
reg s_rst_sync2_reg = 1'b1;
reg s_rst_sync3_reg = 1'b1;
reg m_rst_sync1_reg = 1'b1;
reg m_rst_sync2_reg = 1'b1;
reg m_rst_sync3_reg = 1'b1;

reg [WIDTH-1:0] mem[(2**ADDR_WIDTH)-1:0];
reg [WIDTH-1:0] mem_read_data_reg;
reg mem_read_data_valid_reg = 1'b0, mem_read_data_valid_next;

wire [WIDTH-1:0] s_axis;

reg [WIDTH-1:0] m_axis_reg;
reg m_axis_tvalid_reg = 1'b0, m_axis_tvalid_next;

// full when first TWO MSBs do NOT match, but rest matches
// (gray code equivalent of first MSB different but rest same)
wire full = ((wr_ptr_gray_reg[ADDR_WIDTH] != rd_ptr_gray_sync2_reg[ADDR_WIDTH]) &&
             (wr_ptr_gray_reg[ADDR_WIDTH-1] != rd_ptr_gray_sync2_reg[ADDR_WIDTH-1]) &&
             (wr_ptr_gray_reg[ADDR_WIDTH-2:0] == rd_ptr_gray_sync2_reg[ADDR_WIDTH-2:0]));
wire full_cur = ((wr_ptr_cur_gray_reg[ADDR_WIDTH] != rd_ptr_gray_sync2_reg[ADDR_WIDTH]) &&
                 (wr_ptr_cur_gray_reg[ADDR_WIDTH-1] != rd_ptr_gray_sync2_reg[ADDR_WIDTH-1]) &&
                 (wr_ptr_cur_gray_reg[ADDR_WIDTH-2:0] == rd_ptr_gray_sync2_reg[ADDR_WIDTH-2:0]));
// empty when pointers match exactly
wire empty = rd_ptr_gray_reg == (FRAME_FIFO ? wr_ptr_gray_sync1_reg : wr_ptr_gray_sync2_reg);
// overflow within packet
wire full_wr = ((wr_ptr_reg[ADDR_WIDTH] != wr_ptr_cur_reg[ADDR_WIDTH]) &&
                (wr_ptr_reg[ADDR_WIDTH-1:0] == wr_ptr_cur_reg[ADDR_WIDTH-1:0]));

// control signals
reg write;
reg read;
reg store_output;

reg drop_frame_reg = 1'b0, drop_frame_next;
reg overflow_reg = 1'b0, overflow_next;
reg bad_frame_reg = 1'b0, bad_frame_next;
reg good_frame_reg = 1'b0, good_frame_next;

reg overflow_sync1_reg = 1'b0;
reg overflow_sync2_reg = 1'b0;
reg overflow_sync3_reg = 1'b0;
reg overflow_sync4_reg = 1'b0;
reg bad_frame_sync1_reg = 1'b0;
reg bad_frame_sync2_reg = 1'b0;
reg bad_frame_sync3_reg = 1'b0;
reg bad_frame_sync4_reg = 1'b0;
reg good_frame_sync1_reg = 1'b0;
reg good_frame_sync2_reg = 1'b0;
reg good_frame_sync3_reg = 1'b0;
reg good_frame_sync4_reg = 1'b0;

assign s_axis_tready = (FRAME_FIFO ? (!full_cur || full_wr || DROP_WHEN_FULL) : !full) && !s_rst_sync3_reg;

generate
    assign s_axis[DATA_WIDTH-1:0] = s_axis_tdata;
    if (KEEP_ENABLE) assign s_axis[KEEP_OFFSET +: KEEP_WIDTH] = s_axis_tkeep;
    if (LAST_ENABLE) assign s_axis[LAST_OFFSET]               = s_axis_tlast;
    if (ID_ENABLE)   assign s_axis[ID_OFFSET   +: ID_WIDTH]   = s_axis_tid;
    if (DEST_ENABLE) assign s_axis[DEST_OFFSET +: DEST_WIDTH] = s_axis_tdest;
    if (USER_ENABLE) assign s_axis[USER_OFFSET +: USER_WIDTH] = s_axis_tuser;
endgenerate

assign m_axis_tvalid = m_axis_tvalid_reg;

assign m_axis_tdata = m_axis_reg[DATA_WIDTH-1:0];
assign m_axis_tkeep = KEEP_ENABLE ? m_axis_reg[KEEP_OFFSET +: KEEP_WIDTH] : {KEEP_WIDTH{1'b1}};
assign m_axis_tlast = LAST_ENABLE ? m_axis_reg[LAST_OFFSET]               : 1'b1;
assign m_axis_tid   = ID_ENABLE   ? m_axis_reg[ID_OFFSET   +: ID_WIDTH]   : {ID_WIDTH{1'b0}};
assign m_axis_tdest = DEST_ENABLE ? m_axis_reg[DEST_OFFSET +: DEST_WIDTH] : {DEST_WIDTH{1'b0}};
assign m_axis_tuser = USER_ENABLE ? m_axis_reg[USER_OFFSET +: USER_WIDTH] : {USER_WIDTH{1'b0}};

assign s_status_overflow = overflow_reg;
assign s_status_bad_frame = bad_frame_reg;
assign s_status_good_frame = good_frame_reg;

assign m_status_overflow = overflow_sync3_reg ^ overflow_sync4_reg;
assign m_status_bad_frame = bad_frame_sync3_reg ^ bad_frame_sync4_reg;
assign m_status_good_frame = good_frame_sync3_reg ^ good_frame_sync4_reg;

// reset synchronization
always @(posedge s_clk or posedge async_rst) begin
    if (async_rst) begin
        s_rst_sync1_reg <= 1'b1;
        s_rst_sync2_reg <= 1'b1;
        s_rst_sync3_reg <= 1'b1;
    end else begin
        s_rst_sync1_reg <= 1'b0;
        s_rst_sync2_reg <= s_rst_sync1_reg || m_rst_sync1_reg;
        s_rst_sync3_reg <= s_rst_sync2_reg;
    end
end

always @(posedge m_clk or posedge async_rst) begin
    if (async_rst) begin
        m_rst_sync1_reg <= 1'b1;
        m_rst_sync2_reg <= 1'b1;
        m_rst_sync3_reg <= 1'b1;
    end else begin
        m_rst_sync1_reg <= 1'b0;
        m_rst_sync2_reg <= s_rst_sync1_reg || m_rst_sync1_reg;
        m_rst_sync3_reg <= m_rst_sync2_reg;
    end
end

// Write logic
always @* begin
    write = 1'b0;

    drop_frame_next = drop_frame_reg;
    overflow_next = 1'b0;
    bad_frame_next = 1'b0;
    good_frame_next = 1'b0;

    wr_ptr_next = wr_ptr_reg;
    wr_ptr_cur_next = wr_ptr_cur_reg;
    wr_ptr_gray_next = wr_ptr_gray_reg;
    wr_ptr_sync_gray_next = wr_ptr_sync_gray_reg;
    wr_ptr_cur_gray_next = wr_ptr_cur_gray_reg;

    wr_ptr_update_valid_next = wr_ptr_update_valid_reg;
    wr_ptr_update_next = wr_ptr_update_reg;

    if (FRAME_FIFO && wr_ptr_update_valid_reg) begin
        // have updated pointer to sync
        if (wr_ptr_update_next == wr_ptr_update_ack_sync2_reg) begin
            // no sync in progress; sync update
            wr_ptr_update_valid_next = 1'b0;
            wr_ptr_sync_gray_next = wr_ptr_gray_reg;
            wr_ptr_update_next = !wr_ptr_update_ack_sync2_reg;
        end
    end

    if (s_axis_tready && s_axis_tvalid) begin
        // transfer in
        if (!FRAME_FIFO) begin
            // normal FIFO mode
            write = 1'b1;
            wr_ptr_next = wr_ptr_reg + 1;
            wr_ptr_gray_next = wr_ptr_next ^ (wr_ptr_next >> 1);
        end else if (full_cur || full_wr || drop_frame_reg) begin
            // full, packet overflow, or currently dropping frame
            // drop frame
            drop_frame_next = 1'b1;
            if (s_axis_tlast) begin
                // end of frame, reset write pointer
                wr_ptr_cur_next = wr_ptr_reg;
                wr_ptr_cur_gray_next = wr_ptr_cur_next ^ (wr_ptr_cur_next >> 1);
                drop_frame_next = 1'b0;
                overflow_next = 1'b1;
            end
        end else begin
            write = 1'b1;
            wr_ptr_cur_next = wr_ptr_cur_reg + 1;
            wr_ptr_cur_gray_next = wr_ptr_cur_next ^ (wr_ptr_cur_next >> 1);
            if (s_axis_tlast) begin
                // end of frame
                if (DROP_BAD_FRAME && USER_BAD_FRAME_MASK & ~(s_axis_tuser ^ USER_BAD_FRAME_VALUE)) begin
                    // bad packet, reset write pointer
                    wr_ptr_cur_next = wr_ptr_reg;
                    wr_ptr_cur_gray_next = wr_ptr_cur_next ^ (wr_ptr_cur_next >> 1);
                    bad_frame_next = 1'b1;
                end else begin
                    // good packet, update write pointer
                    wr_ptr_next = wr_ptr_cur_reg + 1;
                    wr_ptr_gray_next = wr_ptr_next ^ (wr_ptr_next >> 1);

                    if (wr_ptr_update_next == wr_ptr_update_ack_sync2_reg) begin
                        // no sync in progress; sync update
                        wr_ptr_update_valid_next = 1'b0;
                        wr_ptr_sync_gray_next = wr_ptr_gray_next;
                        wr_ptr_update_next = !wr_ptr_update_ack_sync2_reg;
                    end else begin
                        // sync in progress; flag it for later
                        wr_ptr_update_valid_next = 1'b1;
                    end

                    good_frame_next = 1'b1;
                end
            end
        end
    end
end

always @(posedge s_clk) begin
    if (s_rst_sync3_reg) begin
        wr_ptr_reg <= {ADDR_WIDTH+1{1'b0}};
        wr_ptr_cur_reg <= {ADDR_WIDTH+1{1'b0}};
        wr_ptr_gray_reg <= {ADDR_WIDTH+1{1'b0}};
        wr_ptr_sync_gray_reg <= {ADDR_WIDTH+1{1'b0}};
        wr_ptr_cur_gray_reg <= {ADDR_WIDTH+1{1'b0}};

        wr_ptr_update_valid_reg <= 1'b0;
        wr_ptr_update_reg <= 1'b0;

        drop_frame_reg <= 1'b0;
        overflow_reg <= 1'b0;
        bad_frame_reg <= 1'b0;
        good_frame_reg <= 1'b0;
    end else begin
        wr_ptr_reg <= wr_ptr_next;
        wr_ptr_cur_reg <= wr_ptr_cur_next;
        wr_ptr_gray_reg <= wr_ptr_gray_next;
        wr_ptr_sync_gray_reg <= wr_ptr_sync_gray_next;
        wr_ptr_cur_gray_reg <= wr_ptr_cur_gray_next;

        wr_ptr_update_valid_reg <= wr_ptr_update_valid_next;
        wr_ptr_update_reg <= wr_ptr_update_next;

        drop_frame_reg <= drop_frame_next;
        overflow_reg <= overflow_next;
        bad_frame_reg <= bad_frame_next;
        good_frame_reg <= good_frame_next;
    end

    if (FRAME_FIFO) begin
        wr_addr_reg <= wr_ptr_cur_next;
    end else begin
        wr_addr_reg <= wr_ptr_next;
    end

    if (write) begin
        mem[wr_addr_reg[ADDR_WIDTH-1:0]] <= s_axis;
    end
end

// pointer synchronization
always @(posedge s_clk) begin
    if (s_rst_sync3_reg) begin
        rd_ptr_gray_sync1_reg <= {ADDR_WIDTH+1{1'b0}};
        rd_ptr_gray_sync2_reg <= {ADDR_WIDTH+1{1'b0}};
        wr_ptr_update_ack_sync1_reg <= 1'b0;
        wr_ptr_update_ack_sync2_reg <= 1'b0;
    end else begin
        rd_ptr_gray_sync1_reg <= rd_ptr_gray_reg;
        rd_ptr_gray_sync2_reg <= rd_ptr_gray_sync1_reg;
        wr_ptr_update_ack_sync1_reg <= wr_ptr_update_sync3_reg;
        wr_ptr_update_ack_sync2_reg <= wr_ptr_update_ack_sync1_reg;
    end
end

always @(posedge m_clk) begin
    if (m_rst_sync3_reg) begin
        wr_ptr_gray_sync1_reg <= {ADDR_WIDTH+1{1'b0}};
        wr_ptr_gray_sync2_reg <= {ADDR_WIDTH+1{1'b0}};
        wr_ptr_update_sync1_reg <= 1'b0;
        wr_ptr_update_sync2_reg <= 1'b0;
        wr_ptr_update_sync3_reg <= 1'b0;
    end else begin
        if (!FRAME_FIFO) begin
            wr_ptr_gray_sync1_reg <= wr_ptr_gray_reg;
        end else if (wr_ptr_update_sync2_reg ^ wr_ptr_update_sync3_reg) begin
            wr_ptr_gray_sync1_reg <= wr_ptr_sync_gray_reg;
        end
        wr_ptr_gray_sync2_reg <= wr_ptr_gray_sync1_reg;
        wr_ptr_update_sync1_reg <= wr_ptr_update_reg;
        wr_ptr_update_sync2_reg <= wr_ptr_update_sync1_reg;
        wr_ptr_update_sync3_reg <= wr_ptr_update_sync2_reg;
    end
end

// status synchronization
always @(posedge s_clk) begin
    if (s_rst_sync3_reg) begin
        overflow_sync1_reg <= 1'b0;
        bad_frame_sync1_reg <= 1'b0;
        good_frame_sync1_reg <= 1'b0;
    end else begin
        overflow_sync1_reg <= overflow_sync1_reg ^ overflow_reg;
        bad_frame_sync1_reg <= bad_frame_sync1_reg ^ bad_frame_reg;
        good_frame_sync1_reg <= good_frame_sync1_reg ^ good_frame_reg;
    end
end

always @(posedge m_clk) begin
    if (m_rst_sync3_reg) begin
        overflow_sync2_reg <= 1'b0;
        overflow_sync3_reg <= 1'b0;
        overflow_sync4_reg <= 1'b0;
        bad_frame_sync2_reg <= 1'b0;
        bad_frame_sync3_reg <= 1'b0;
        bad_frame_sync4_reg <= 1'b0;
        good_frame_sync2_reg <= 1'b0;
        good_frame_sync3_reg <= 1'b0;
        good_frame_sync4_reg <= 1'b0;
    end else begin
        overflow_sync2_reg <= overflow_sync1_reg;
        overflow_sync3_reg <= overflow_sync2_reg;
        overflow_sync4_reg <= overflow_sync3_reg;
        bad_frame_sync2_reg <= bad_frame_sync1_reg;
        bad_frame_sync3_reg <= bad_frame_sync2_reg;
        bad_frame_sync4_reg <= bad_frame_sync3_reg;
        good_frame_sync2_reg <= good_frame_sync1_reg;
        good_frame_sync3_reg <= good_frame_sync2_reg;
        good_frame_sync4_reg <= good_frame_sync3_reg;
    end
end

// Read logic
always @* begin
    read = 1'b0;

    rd_ptr_next = rd_ptr_reg;
    rd_ptr_gray_next = rd_ptr_gray_reg;

    mem_read_data_valid_next = mem_read_data_valid_reg;

    if (store_output || !mem_read_data_valid_reg) begin
        // output data not valid OR currently being transferred
        if (!empty) begin
            // not empty, perform read
            read = 1'b1;
            mem_read_data_valid_next = 1'b1;
            rd_ptr_next = rd_ptr_reg + 1;
            rd_ptr_gray_next = rd_ptr_next ^ (rd_ptr_next >> 1);
        end else begin
            // empty, invalidate
            mem_read_data_valid_next = 1'b0;
        end
    end
end

always @(posedge m_clk) begin
    if (m_rst_sync3_reg) begin
        rd_ptr_reg <= {ADDR_WIDTH+1{1'b0}};
        rd_ptr_gray_reg <= {ADDR_WIDTH+1{1'b0}};
        mem_read_data_valid_reg <= 1'b0;
    end else begin
        rd_ptr_reg <= rd_ptr_next;
        rd_ptr_gray_reg <= rd_ptr_gray_next;
        mem_read_data_valid_reg <= mem_read_data_valid_next;
    end

    rd_addr_reg <= rd_ptr_next;

    if (read) begin
        mem_read_data_reg <= mem[rd_addr_reg[ADDR_WIDTH-1:0]];
    end
end

// Output register
always @* begin
    store_output = 1'b0;

    m_axis_tvalid_next = m_axis_tvalid_reg;

    if (m_axis_tready || !m_axis_tvalid) begin
        store_output = 1'b1;
        m_axis_tvalid_next = mem_read_data_valid_reg;
    end
end

always @(posedge m_clk) begin
    if (m_rst_sync3_reg) begin
        m_axis_tvalid_reg <= 1'b0;
    end else begin
        m_axis_tvalid_reg <= m_axis_tvalid_next;
    end

    if (store_output) begin
        m_axis_reg <= mem_read_data_reg;
    end
end

endmodule
