initial begin : test
Lock  <= 1'b0; 
Pre   <= 1'b0; 
Cle   <= 1'b0; 
Ce_n  <= 1'b1; 
Ce2_n <= 1'b1; 
We_n  <= 1'b1; 
Ale   <= 1'b0; 
Re_n  <= 1'b1; 
Wp_n  <= 1'b1; 
Io   = {DQ_BITS{1'bz}};
nop(tWHR_min);
nop(tRHW_min);
     wait_ready;
Ce_n = 1'b0;
Ce2_n = 1'b0;
`ifdef MIN_CYCLE
    set_read_cycle(tRP_min, (tRC_min-tRP_min));
    set_write_cycle(tWP_min, (tWC_min-tWP_min));
`else
    set_read_cycle(tCEA_cache_max, tRHZ_max);
    set_write_cycle(tWP_cache_min, (tWC_cache_min-tWP_cache_min));
`endif
reset;
activate_device(0);
wait_ready;
//program_page(block,page,column,data,size,pat,tp,idm,otp,cache,copyback2, final)
program_page(2, 0, 0, 8'h0, 15, 2, 0, 0, 0, 0, 0, 1);
wait_ready;
//read_page(block,page,column,tp,idm,otp,copyback2)
read_page(2, 0, 0, 0, 0, 0, 0);
wait_ready;
serial_read(8'h0, 2'h2, 15);
$display("SIMULATION ENDING NORMALLY");

test_done =1;
end


