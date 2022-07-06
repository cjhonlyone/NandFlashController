# SHELL := /bin/bash

DEVICE=xc7k325tffg900-2
Vivado_DIR=/cygdrive/c/Xilinx/Vivado/2017.4/bin

tb_NFC_Physical_FILES = $(wildcard ../tb/tb_NFC_Physical*.v)
NFC_Physical_FILES = $(wildcard ../rtl/NFC_Physical*.v) $(wildcard ../rtl/NFC_Pin*.v)

tb_NFC_Atom_FILES = $(wildcard ../tb/tb_NFC_Atom*.v)
NFC_Atom_FILES = $(wildcard ../rtl/NFC_Atom*.v) 

tb_NFC_Command_FILES = $(wildcard ../tb/tb_NFC_Command*.v)
NFC_Command_FILES = $(wildcard ../rtl/NFC_Command*.v) 

tb_NFC_Top_FILES = ../tb/tb_NandFlashController_Top.v ../tb/tb_NandFlashController_Top_AXI.v
NFC_Top_FILES = ../rtl/NandFlashController_Top.v ../rtl/NandFlashController_Top_AXI.v\
	../rtl/NandFlashController_Interface_adapter.v ../rtl/NandFlashController_AXIL_Reg.v 

nand_model_FILES = $(wildcard ../tb/m73a_nand_model/*.vh) $(wildcard ../tb/m73a_nand_model/*.v)

AXILIB_FILES = ../lib/axis/rtl/axis_fifo.v ../lib/verilog-axi/rtl/axi_dma.v \
	../lib/verilog-axi/rtl/axi_dma_rd.v ../lib/verilog-axi/rtl/axi_dma_wr.v \
	../lib/verilog-axi/rtl/axi_ram.v ../lib/axis/rtl/axis_async_fifo.v \
	../lib/axis/rtl/axis_async_fifo_adapter.v ../lib/axis/rtl/axis_adapter.v

CommonPrj: $(tb_NFC_Physical_FILES) $(NFC_Physical_FILES) $(NFC_Command_FILES)
	echo "create_project -force NandFlashController ./NandFlashController -part xc7z030fbg676-2" > create_project.tcl

	for x in $(NFC_Atom_FILES);        do echo "add_files -fileset sources_1 $$x" >> create_project.tcl; done
	for x in $(NFC_Physical_FILES);    do echo "add_files -fileset sources_1 $$x" >> create_project.tcl; done
	for x in $(NFC_Command_FILES);     do echo "add_files -fileset sources_1 $$x" >> create_project.tcl; done
	for x in $(NFC_Top_FILES);         do echo "add_files -fileset sources_1 $$x" >> create_project.tcl; done

	for x in $(tb_NFC_Atom_FILES);     do echo "add_files -fileset sim_1 $$x" >> create_project.tcl; done
	for x in $(tb_NFC_Physical_FILES); do echo "add_files -fileset sim_1 $$x" >> create_project.tcl; done
	for x in $(tb_NFC_Command_FILES);  do echo "add_files -fileset sim_1 $$x" >> create_project.tcl; done
	for x in $(tb_NFC_Top_FILES);      do echo "add_files -fileset sim_1 $$x" >> create_project.tcl; done
	
	for x in $(nand_model_FILES);      do echo "add_files -fileset sim_1 $$x" >> create_project.tcl; done

	for x in $(AXILIB_FILES);         do echo "add_files -fileset sources_1 $$x" >> create_project.tcl; done

	echo "add_files -fileset sources_1 ../lib/axis/rtl/axis_fifo.v" >> create_project.tcl;
	
	echo "set_property top NandFlashController_Top_AXI [get_filesets sources_1]" >> create_project.tcl;
	echo "set_property top tb_NandFlashController_Top_AXI [get_filesets sim_1]" >> create_project.tcl;

	echo "update_compile_order -fileset sources_1" >> create_project.tcl;
	echo "update_compile_order -fileset sim_1" >> create_project.tcl;

	echo "set_property target_simulator ModelSim [current_project]" >> create_project.tcl;
	echo "set_property -name {modelsim.simulate.log_all_signals} -value {true} -objects [get_filesets sim_1]" >> create_project.tcl;
	echo "set_property -name {modelsim.simulate.custom_wave_do} -value {../../../../../../tb/tb_NandFlashController_Top_wave.do} -objects [get_filesets sim_1]" >> create_project.tcl;
	echo "set_property -name {modelsim.simulate.runtime} -value {3.5ms} -objects [get_filesets sim_1]" >> create_project.tcl;
	${Vivado_DIR}/vivado -mode tcl -source create_project.tcl
# 	echo "exit" >> create_project.tcl;
	
# ip:
ZynqPrj:
	${Vivado_DIR}/vivado -mode tcl -source ./BuildZynqPrj.tcl

clean:
	rm -r create_project.tcl
	rm -rf .Xil
	rm -rf NandFlashController
	rm -rf ZynqNandFlash
	rm -r *.log *.jou *.dmp