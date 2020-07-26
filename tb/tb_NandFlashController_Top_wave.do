add wave -noupdate -divider {NFC Interface}
add wave sim:/tb_NandFlashController_Top_AXI/inst_NandFlashController_Top_AXI/inst_NandFlashController_Top/iOpcode
add wave sim:/tb_NandFlashController_Top_AXI/inst_NandFlashController_Top_AXI/inst_NandFlashController_Top/iTargetID
add wave sim:/tb_NandFlashController_Top_AXI/inst_NandFlashController_Top_AXI/inst_NandFlashController_Top/iSourceID
add wave sim:/tb_NandFlashController_Top_AXI/inst_NandFlashController_Top_AXI/inst_NandFlashController_Top/iAddress
add wave sim:/tb_NandFlashController_Top_AXI/inst_NandFlashController_Top_AXI/inst_NandFlashController_Top/iLength
add wave sim:/tb_NandFlashController_Top_AXI/inst_NandFlashController_Top_AXI/inst_NandFlashController_Top/iCMDValid
add wave sim:/tb_NandFlashController_Top_AXI/inst_NandFlashController_Top_AXI/inst_NandFlashController_Top/oCMDReady
add wave sim:/tb_NandFlashController_Top_AXI/inst_NandFlashController_Top_AXI/inst_NandFlashController_Top/iWriteData
add wave sim:/tb_NandFlashController_Top_AXI/inst_NandFlashController_Top_AXI/inst_NandFlashController_Top/iWriteLast
add wave sim:/tb_NandFlashController_Top_AXI/inst_NandFlashController_Top_AXI/inst_NandFlashController_Top/iWriteValid
add wave sim:/tb_NandFlashController_Top_AXI/inst_NandFlashController_Top_AXI/inst_NandFlashController_Top/iWriteKeep
add wave sim:/tb_NandFlashController_Top_AXI/inst_NandFlashController_Top_AXI/inst_NandFlashController_Top/oWriteReady
add wave sim:/tb_NandFlashController_Top_AXI/inst_NandFlashController_Top_AXI/inst_NandFlashController_Top/oReadValid
add wave sim:/tb_NandFlashController_Top_AXI/inst_NandFlashController_Top_AXI/inst_NandFlashController_Top/oReadKeep
add wave sim:/tb_NandFlashController_Top_AXI/inst_NandFlashController_Top_AXI/inst_NandFlashController_Top/oReadData
add wave sim:/tb_NandFlashController_Top_AXI/inst_NandFlashController_Top_AXI/inst_NandFlashController_Top/oReadLast
add wave sim:/tb_NandFlashController_Top_AXI/inst_NandFlashController_Top_AXI/inst_NandFlashController_Top/iReadReady
add wave sim:/tb_NandFlashController_Top_AXI/inst_NandFlashController_Top_AXI/inst_NandFlashController_Top/oReadTransValid
add wave sim:/tb_NandFlashController_Top_AXI/inst_NandFlashController_Top_AXI/inst_NandFlashController_Top/oReadyBusy
add wave sim:/tb_NandFlashController_Top_AXI/inst_NandFlashController_Top_AXI/inst_NandFlashController_Top/oStatus
add wave sim:/tb_NandFlashController_Top_AXI/inst_NandFlashController_Top_AXI/inst_NandFlashController_Top/oStatusValid
add wave -noupdate -divider {NAND IO}
add wave sim:/tb_NandFlashController_Top_AXI/IO_NAND_DQS
add wave sim:/tb_NandFlashController_Top_AXI/IO_NAND_DQ
add wave sim:/tb_NandFlashController_Top_AXI/O_NAND_CE
add wave sim:/tb_NandFlashController_Top_AXI/O_NAND_WE
add wave sim:/tb_NandFlashController_Top_AXI/O_NAND_RE
add wave sim:/tb_NandFlashController_Top_AXI/O_NAND_ALE
add wave sim:/tb_NandFlashController_Top_AXI/O_NAND_CLE
add wave sim:/tb_NandFlashController_Top_AXI/I_NAND_RB
add wave sim:/tb_NandFlashController_Top_AXI/O_NAND_WP
add wave -noupdate -divider {DMA Write Channel}
# add wave sim:/tb_NandFlashController_Top_AXI/inst_NandFlashController_Top_AXI/m_axis_write_desc_status_valid
# add wave sim:/tb_NandFlashController_Top_AXI/inst_NandFlashController_Top_AXI/m_axis_write_desc_status_len
# add wave sim:/tb_NandFlashController_Top_AXI/inst_NandFlashController_Top_AXI/s_axis_write_desc_valid
# add wave sim:/tb_NandFlashController_Top_AXI/inst_NandFlashController_Top_AXI/s_axis_write_desc_ready
# add wave sim:/tb_NandFlashController_Top_AXI/inst_NandFlashController_Top_AXI/s_axis_write_desc_addr
# add wave sim:/tb_NandFlashController_Top_AXI/inst_NandFlashController_Top_AXI/s_axis_write_desc_len
# add wave sim:/tb_NandFlashController_Top_AXI/inst_NandFlashController_Top_AXI/s_axis_write_data_tvalid
# add wave sim:/tb_NandFlashController_Top_AXI/inst_NandFlashController_Top_AXI/s_axis_write_data_tready
# add wave sim:/tb_NandFlashController_Top_AXI/inst_NandFlashController_Top_AXI/s_axis_write_data_tdata
# add wave sim:/tb_NandFlashController_Top_AXI/inst_NandFlashController_Top_AXI/s_axis_write_data_tkeep
# add wave sim:/tb_NandFlashController_Top_AXI/inst_NandFlashController_Top_AXI/s_axis_write_data_tlast
add wave sim:/tb_NandFlashController_Top_AXI/m_axi_rid
add wave sim:/tb_NandFlashController_Top_AXI/m_axi_awid
add wave sim:/tb_NandFlashController_Top_AXI/m_axi_awlen
add wave sim:/tb_NandFlashController_Top_AXI/m_axi_awsize
add wave sim:/tb_NandFlashController_Top_AXI/m_axi_awburst
add wave sim:/tb_NandFlashController_Top_AXI/m_axi_awlock
add wave sim:/tb_NandFlashController_Top_AXI/m_axi_awcache
add wave sim:/tb_NandFlashController_Top_AXI/m_axi_awprot
add wave sim:/tb_NandFlashController_Top_AXI/m_axi_awvalid
add wave sim:/tb_NandFlashController_Top_AXI/m_axi_awready
add wave sim:/tb_NandFlashController_Top_AXI/m_axi_awaddr
add wave sim:/tb_NandFlashController_Top_AXI/m_axi_wvalid
add wave sim:/tb_NandFlashController_Top_AXI/m_axi_wready
add wave sim:/tb_NandFlashController_Top_AXI/m_axi_wdata
add wave sim:/tb_NandFlashController_Top_AXI/m_axi_wstrb
add wave sim:/tb_NandFlashController_Top_AXI/m_axi_wlast
add wave sim:/tb_NandFlashController_Top_AXI/m_axi_bvalid
add wave sim:/tb_NandFlashController_Top_AXI/m_axi_bready
add wave sim:/tb_NandFlashController_Top_AXI/m_axi_bid
add wave sim:/tb_NandFlashController_Top_AXI/m_axi_bresp
add wave -noupdate -divider {DMA Read Channel}
# add wave sim:/tb_NandFlashController_Top_AXI/inst_NandFlashController_Top_AXI/m_axis_read_desc_status_tag
# add wave sim:/tb_NandFlashController_Top_AXI/inst_NandFlashController_Top_AXI/m_axis_read_desc_status_valid
# add wave sim:/tb_NandFlashController_Top_AXI/inst_NandFlashController_Top_AXI/s_axis_read_desc_valid
# add wave sim:/tb_NandFlashController_Top_AXI/inst_NandFlashController_Top_AXI/s_axis_read_desc_ready
# add wave sim:/tb_NandFlashController_Top_AXI/inst_NandFlashController_Top_AXI/s_axis_read_desc_addr
# add wave sim:/tb_NandFlashController_Top_AXI/inst_NandFlashController_Top_AXI/s_axis_read_desc_len
# add wave sim:/tb_NandFlashController_Top_AXI/inst_NandFlashController_Top_AXI/m_axis_read_data_tvalid
# add wave sim:/tb_NandFlashController_Top_AXI/inst_NandFlashController_Top_AXI/m_axis_read_data_tready
# add wave sim:/tb_NandFlashController_Top_AXI/inst_NandFlashController_Top_AXI/m_axis_read_data_tdata
# add wave sim:/tb_NandFlashController_Top_AXI/inst_NandFlashController_Top_AXI/m_axis_read_data_tkeep
# add wave sim:/tb_NandFlashController_Top_AXI/inst_NandFlashController_Top_AXI/m_axis_read_data_tlast
add wave sim:/tb_NandFlashController_Top_AXI/m_axi_arid
add wave sim:/tb_NandFlashController_Top_AXI/m_axi_arlen
add wave sim:/tb_NandFlashController_Top_AXI/m_axi_arcache
add wave sim:/tb_NandFlashController_Top_AXI/m_axi_arsize
add wave sim:/tb_NandFlashController_Top_AXI/m_axi_arburst
add wave sim:/tb_NandFlashController_Top_AXI/m_axi_arprot
add wave sim:/tb_NandFlashController_Top_AXI/m_axi_arlock
add wave sim:/tb_NandFlashController_Top_AXI/m_axi_arvalid
add wave sim:/tb_NandFlashController_Top_AXI/m_axi_arready
add wave sim:/tb_NandFlashController_Top_AXI/m_axi_araddr
add wave sim:/tb_NandFlashController_Top_AXI/m_axi_rvalid
add wave sim:/tb_NandFlashController_Top_AXI/m_axi_rready
add wave sim:/tb_NandFlashController_Top_AXI/m_axi_rdata
add wave sim:/tb_NandFlashController_Top_AXI/m_axi_rlast
add wave sim:/tb_NandFlashController_Top_AXI/m_axi_rresp

add wave -noupdate -divider {AXIlite Read Interface}
add wave sim:/tb_NandFlashController_Top_AXI/s_axil_arprot
add wave sim:/tb_NandFlashController_Top_AXI/s_axil_arvalid
add wave sim:/tb_NandFlashController_Top_AXI/s_axil_arready
add wave sim:/tb_NandFlashController_Top_AXI/s_axil_araddr
add wave sim:/tb_NandFlashController_Top_AXI/s_axil_rvalid
add wave sim:/tb_NandFlashController_Top_AXI/s_axil_rready
add wave sim:/tb_NandFlashController_Top_AXI/s_axil_rdata
add wave sim:/tb_NandFlashController_Top_AXI/s_axil_rresp

add wave -noupdate -divider {AXIlite Write Interface}
add wave sim:/tb_NandFlashController_Top_AXI/s_axil_awprot
add wave sim:/tb_NandFlashController_Top_AXI/s_axil_awvalid
add wave sim:/tb_NandFlashController_Top_AXI/s_axil_awready
add wave sim:/tb_NandFlashController_Top_AXI/s_axil_awaddr
add wave sim:/tb_NandFlashController_Top_AXI/s_axil_wvalid
add wave sim:/tb_NandFlashController_Top_AXI/s_axil_wready
add wave sim:/tb_NandFlashController_Top_AXI/s_axil_wstrb
add wave sim:/tb_NandFlashController_Top_AXI/s_axil_wdata
add wave sim:/tb_NandFlashController_Top_AXI/s_axil_bvalid
add wave sim:/tb_NandFlashController_Top_AXI/s_axil_bready
add wave sim:/tb_NandFlashController_Top_AXI/s_axil_bresp