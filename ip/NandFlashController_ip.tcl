# ip

source ./generate_ip.tcl

ip_create NandFlashController

ip_files NandFlashController [list \
  "../rtl/NFC_Atom_Command_Async.v" \
  "../rtl/NFC_Atom_Command_Generator_to_Physical_Mux.v" \
  "../rtl/NFC_Atom_Command_Generator_Top.v" \
  "../rtl/NFC_Atom_Command_Sync.v" \
  "../rtl/NFC_Atom_Datainput_Sync.v" \
  "../rtl/NFC_Atom_Dataoutput_Async.v" \
  "../rtl/NFC_Atom_Dataoutput_Sync.v" \
  "../rtl/NFC_Atom_Idle.v" \
  "../rtl/NFC_Physical_Input.v" \
  "../rtl/NFC_Physical_Output.v" \
  "../rtl/NFC_Physical_Top.v" \
  "../rtl/NFC_Pinpad.v" \
  "../rtl/NFC_Command_EraseBlock.v" \
  "../rtl/NFC_Command_GetFeature.v" \
  "../rtl/NFC_Command_Idle.v" \
  "../rtl/NFC_Command_Issue_to_Atom_Command_Generator_Mux.v" \
  "../rtl/NFC_Command_Issue_Top.v" \
  "../rtl/NFC_Command_ProgramPage.v" \
  "../rtl/NFC_Command_ReadPage.v" \
  "../rtl/NFC_Command_ReadStatus.v" \
  "../rtl/NFC_Command_Reset.v" \
  "../rtl/NFC_Command_SetFeature.v" \
  "../rtl/NandFlashController_Top.v" \
  "../rtl/NandFlashController_Top_AXI.v" \
  "../rtl/NandFlashController_Interface_adapter.v" \
  "../rtl/NandFlashController_AXIL_Reg.v" \
  "../lib/axis/rtl/axis_fifo.v" \
  "../lib/verilog-axi/rtl/axi_dma.v" \
  "../lib/verilog-axi/rtl/axi_dma_rd.v" \
  "../lib/verilog-axi/rtl/axi_dma_wr.v" \
  "../lib/axis/rtl/axis_async_fifo.v" \
  "../lib/axis/rtl/axis_async_fifo_adapter.v" \
  "../lib/axis/rtl/axis_adapter.v" \
  "../lib/axis/rtl/axis_fifo.v"]
  # "../lib/axis_async_fifo.tcl"]

ip_properties_lite NandFlashController



add_bus "m_axi" "master" \
    "xilinx.com:interface:aximm_rtl:1.0" \
    "xilinx.com:interface:aximm:1.0" \
    { \
        {"m_axi_awid" "AWID"} \
        {"m_axi_awaddr" "AWADDR"} \
        {"m_axi_awlen" "AWLEN"} \
        {"m_axi_awsize" "AWSIZE"} \
        {"m_axi_awburst" "AWBURST"} \
        {"m_axi_awlock" "AWLOCK"} \
        {"m_axi_awcache" "AWCACHE"} \
        {"m_axi_awprot" "AWPROT"} \
        {"m_axi_awqos" "AWQOS"} \
        {"m_axi_awvalid" "AWVALID"} \
        {"m_axi_awready" "AWREADY"} \
        {"m_axi_wdata" "WDATA"} \
        {"m_axi_wstrb" "WSTRB"} \
        {"m_axi_wlast" "WLAST"} \
        {"m_axi_wvalid" "WVALID"} \
        {"m_axi_wready" "WREADY"} \
        {"m_axi_bid" "BID"} \
        {"m_axi_bresp" "BRESP"} \
        {"m_axi_bvalid" "BVALID"} \
        {"m_axi_bready" "BREADY"} \
        {"m_axi_arid" "ARID"} \
        {"m_axi_araddr" "ARADDR"} \
        {"m_axi_arlen" "ARLEN"} \
        {"m_axi_arsize" "ARSIZE"} \
        {"m_axi_arburst" "ARBURST"} \
        {"m_axi_arlock" "ARLOCK"} \
        {"m_axi_arcache" "ARCACHE"} \
        {"m_axi_arprot" "ARPROT"} \
        {"m_axi_arqos" "ARQOS"} \
        {"m_axi_arvalid" "ARVALID"} \
        {"m_axi_arready" "ARREADY"} \
        {"m_axi_rid" "RID"} \
        {"m_axi_rdata" "RDATA"} \
        {"m_axi_rresp" "RRESP"} \
        {"m_axi_rlast" "RLAST"} \
        {"m_axi_rvalid" "RVALID"} \
        {"m_axi_rready" "RREADY" } \
    }

ipx::infer_bus_interface m_axi_clk xilinx.com:signal:clock_rtl:1.0 [ipx::current_core]
ipx::infer_bus_interface m_axi_rst xilinx.com:signal:reset_rtl:1.0 [ipx::current_core]

set_property master_address_space_ref m_axi \
    [ipx::get_bus_interfaces m_axi \
    -of_objects [ipx::current_core]]

# ipx::add_memory_map {m_axi} [ipx::current_core]
# set_property slave_memory_map_ref {m_axi} [ipx::get_bus_interfaces m_axi -of_objects [ipx::current_core]]

# set range 65536

# ipx::add_address_block {axi} [ipx::get_memory_maps m_axi -of_objects [ipx::current_core]]
# set_property range $range [ipx::get_address_blocks axi \
#   -of_objects [ipx::get_memory_maps m_axi -of_objects [ipx::current_core]]]

# ipx::add_bus_parameter ASSOCIATED_BUSIF [ipx::get_bus_interfaces m_axi_clk \
#   -of_objects [ipx::current_core]]
# set_property value m_axi [ipx::get_bus_parameters ASSOCIATED_BUSIF \
#   -of_objects [ipx::get_bus_interfaces m_axi_clk \
#   -of_objects [ipx::current_core]]]


add_bus "s_axil" "slave" \
    "xilinx.com:interface:aximm_rtl:1.0" \
    "xilinx.com:interface:aximm:1.0" \
    { \
        {"s_axil_awvalid" "AWVALID"} \
        {"s_axil_awaddr" "AWADDR"} \
        {"s_axil_awprot" "AWPROT"} \
        {"s_axil_awready" "AWREADY"} \
        {"s_axil_wvalid" "WVALID"} \
        {"s_axil_wdata" "WDATA"} \
        {"s_axil_wstrb" "WSTRB"} \
        {"s_axil_wready" "WREADY"} \
        {"s_axil_bvalid" "BVALID"} \
        {"s_axil_bresp" "BRESP"} \
        {"s_axil_bready" "BREADY"} \
        {"s_axil_arvalid" "ARVALID"} \
        {"s_axil_araddr" "ARADDR"} \
        {"s_axil_arprot" "ARPROT"} \
        {"s_axil_arready" "ARREADY"} \
        {"s_axil_rvalid" "RVALID"} \
        {"s_axil_rdata" "RDATA"} \
        {"s_axil_rresp" "RRESP"} \
        {"s_axil_rready" "RREADY"} \
    }

ipx::infer_bus_interface s_axil_clk xilinx.com:signal:clock_rtl:1.0 [ipx::current_core]
ipx::infer_bus_interface s_axil_rst xilinx.com:signal:reset_rtl:1.0 [ipx::current_core]

ipx::add_memory_map {s_axil} [ipx::current_core]
set_property slave_memory_map_ref {s_axil} [ipx::get_bus_interfaces s_axil -of_objects [ipx::current_core]]

set range 65536

ipx::add_address_block {axi_lite} [ipx::get_memory_maps s_axil -of_objects [ipx::current_core]]
set_property range $range [ipx::get_address_blocks axi_lite \
  -of_objects [ipx::get_memory_maps s_axil -of_objects [ipx::current_core]]]

ipx::add_bus_parameter ASSOCIATED_BUSIF [ipx::get_bus_interfaces s_axil_clk \
  -of_objects [ipx::current_core]]
set_property value s_axil [ipx::get_bus_parameters ASSOCIATED_BUSIF \
  -of_objects [ipx::get_bus_interfaces s_axil_clk \
  -of_objects [ipx::current_core]]]


ipx::infer_bus_interface iSystemClock xilinx.com:signal:clock_rtl:1.0 [ipx::current_core]
ipx::infer_bus_interface iDelayRefClock xilinx.com:signal:clock_rtl:1.0 [ipx::current_core]
ipx::infer_bus_interface iSystemClock_120 xilinx.com:signal:clock_rtl:1.0 [ipx::current_core]
ipx::infer_bus_interface iReset xilinx.com:signal:reset_rtl:1.0 [ipx::current_core]

ipx::save_core [ipx::current_core]

