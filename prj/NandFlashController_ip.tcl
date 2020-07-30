# ip

source ../generate_ip.tcl

ip_create NandFlashController
ip_files NandFlashController [list \
  "../../rtl/NFC_Atom_Command_Async.v" \
  "../../rtl/NFC_Atom_Command_Generator_to_Physical_Mux.v" \
  "../../rtl/NFC_Atom_Command_Generator_Top.v" \
  "../../rtl/NFC_Atom_Command_Sync.v" \
  "../../rtl/NFC_Atom_Datainput_Sync.v" \
  "../../rtl/NFC_Atom_Dataoutput_Async.v" \
  "../../rtl/NFC_Atom_Dataoutput_Sync.v" \
  "../../rtl/NFC_Atom_Idle.v" \
  "../../rtl/NFC_Physical_Input.v" \
  "../../rtl/NFC_Physical_Output.v" \
  "../../rtl/NFC_Physical_Top.v" \
  "../../rtl/NFC_Pinpad.v" \
  "../../rtl/NFC_Command_EraseBlock.v" \
  "../../rtl/NFC_Command_GetFeature.v" \
  "../../rtl/NFC_Command_Idle.v" \
  "../../rtl/NFC_Command_Issue_to_Atom_Command_Generator_Mux.v" \
  "../../rtl/NFC_Command_Issue_Top.v" \
  "../../rtl/NFC_Command_ProgramPage.v" \
  "../../rtl/NFC_Command_ReadPage.v" \
  "../../rtl/NFC_Command_ReadStatus.v" \
  "../../rtl/NFC_Command_Reset.v" \
  "../../rtl/NFC_Command_SetFeature.v" \
  "../../rtl/NandFlashController_Top.v" \
  "../../rtl/NandFlashController_Top_AXI.v" \
  "../../rtl/NandFlashController_Interface_adapter.v" \
  "../../rtl/NandFlashController_AXIL_Reg.v" \
  "../../lib/axis/rtl/axis_fifo.v" \
  "../../lib/verilog-axi/rtl/axi_dma.v" \
  "../../lib/verilog-axi/rtl/axi_dma_rd.v" \
  "../../lib/verilog-axi/rtl/axi_dma_wr.v" \
  "../../lib/axis/rtl/axis_async_fifo.v" \
  "../../lib/axis/rtl/axis_async_fifo_adapter.v" \
  "../../lib/axis/rtl/axis_adapter.v" \
  "../../lib/axis/rtl/axis_fifo.v"]

ip_properties NandFlashController

ipx::infer_bus_interface iSystemClock xilinx.com:signal:clock_rtl:1.0 [ipx::current_core]
ipx::infer_bus_interface iDelayRefClock xilinx.com:signal:clock_rtl:1.0 [ipx::current_core]
ipx::infer_bus_interface iOutputDrivingClock xilinx.com:signal:clock_rtl:1.0 [ipx::current_core]

ipx::save_core [ipx::current_core]

