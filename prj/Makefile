# SHELL := /bin/bash

DEVICE=xc7k325tffg900-2
Vivado_DIR=/cygdrive/d/Xilinx/Vivado/2017.4/bin

tb_NFC_Physical_FILES = $(wildcard ../tb/tb_NFC_Physical*.v)
NFC_Physical_FILES = $(wildcard ../rtl/NFC_Physical*.v) $(wildcard ../rtl/NFC_Pin*.v)

tb_NFC_Atom_FILES = $(wildcard ../tb/tb_NFC_Atom*.v)
NFC_Atom_FILES = $(wildcard ../rtl/NFC_Atom*.v) 

tb_NFC_Command_FILES = $(wildcard ../tb/tb_NFC_Command*.v)
NFC_Command_FILES = $(wildcard ../rtl/NFC_Command*.v) 

nand_model_FILES = $(wildcard ../tb/nandmodel/*.vh) $(wildcard ../tb/nandmodel/*.v)


prj: $(tb_NFC_Physical_FILES) $(NFC_Physical_FILES) $(NFC_Command_FILES)
	echo "create_project -force NandFlashController ./NandFlashController -part xc7z030fbg676-2" > create_project.tcl

	for x in $(NFC_Atom_FILES);        do echo "add_files -fileset sources_1 $$x" >> create_project.tcl; done
	for x in $(NFC_Physical_FILES);    do echo "add_files -fileset sources_1 $$x" >> create_project.tcl; done
	for x in $(NFC_Command_FILES);     do echo "add_files -fileset sources_1 $$x" >> create_project.tcl; done

	for x in $(tb_NFC_Atom_FILES);     do echo "add_files -fileset sim_1 $$x" >> create_project.tcl; done
	for x in $(tb_NFC_Physical_FILES); do echo "add_files -fileset sim_1 $$x" >> create_project.tcl; done
	for x in $(tb_NFC_Command_FILES);  do echo "add_files -fileset sim_1 $$x" >> create_project.tcl; done

	for x in $(nand_model_FILES);      do echo "add_files -fileset sim_1 $$x" >> create_project.tcl; done

	echo "add_files -fileset sources_1 ../lib/axis/rtl/axis_fifo.v" >> create_project.tcl;
	
	echo "set_property top NFC_Command_Issue_Top [get_filesets sources_1]" >> create_project.tcl;
	echo "set_property top tb_NFC_Command_Issue_Top [get_filesets sim_1]" >> create_project.tcl;

	echo "update_compile_order -fileset sources_1" >> create_project.tcl;
	echo "update_compile_order -fileset sim_1" >> create_project.tcl;
	echo "set_property target_simulator ModelSim [current_project]" >> create_project.tcl;

	echo "set_property -name {modelsim.simulate.log_all_signals} -value {true} -objects [get_filesets sim_1]" >> create_project.tcl;
	echo "set_property -name {modelsim.simulate.runtime} -value {10us} -objects [get_filesets sim_1]" >> create_project.tcl;

	echo "exit" >> create_project.tcl;
	${Vivado_DIR}/vivado -mode tcl -source create_project.tcl

clean:
	rm -rf .Xil
	rm -rf NandFlashController
	rm -r *.log *.jou