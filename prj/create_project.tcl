create_project -force NandFlashController ./NandFlashController -part xc7z030fbg676-2
add_files -fileset sources_1 ../rtl/NFC_Atom_Command_Async.v
add_files -fileset sources_1 ../rtl/NFC_Atom_Command_Generator_to_Physical_Mux.v
add_files -fileset sources_1 ../rtl/NFC_Atom_Command_Generator_Top.v
add_files -fileset sources_1 ../rtl/NFC_Atom_Command_Sync.v
add_files -fileset sources_1 ../rtl/NFC_Atom_Datainput_Async.v
add_files -fileset sources_1 ../rtl/NFC_Atom_Datainput_Sync.v
add_files -fileset sources_1 ../rtl/NFC_Atom_Dataoutput_Async.v
add_files -fileset sources_1 ../rtl/NFC_Atom_Dataoutput_Sync.v
add_files -fileset sources_1 ../rtl/NFC_Atom_Idle.v
add_files -fileset sources_1 ../rtl/NFC_Physical_Input.v
add_files -fileset sources_1 ../rtl/NFC_Physical_Output.v
add_files -fileset sources_1 ../rtl/NFC_Physical_Top.v
add_files -fileset sources_1 ../rtl/NFC_Pinpad.v
add_files -fileset sources_1 ../rtl/NFC_Command_EraseBlock.v
add_files -fileset sources_1 ../rtl/NFC_Command_GetFeature.v
add_files -fileset sources_1 ../rtl/NFC_Command_Idle.v
add_files -fileset sources_1 ../rtl/NFC_Command_Issue_to_Atom_Command_Generator_Mux.v
add_files -fileset sources_1 ../rtl/NFC_Command_Issue_Top.v
add_files -fileset sources_1 ../rtl/NFC_Command_ProgramPage.v
add_files -fileset sources_1 ../rtl/NFC_Command_ReadPage.v
add_files -fileset sources_1 ../rtl/NFC_Command_Reset.v
add_files -fileset sources_1 ../rtl/NFC_Command_SetFeature.v
add_files -fileset sim_1 ../tb/tb_NFC_Atom_Command_Generator_Top.v
add_files -fileset sim_1 ../tb/tb_NFC_Physical_Top.v
add_files -fileset sim_1 ../tb/tb_NFC_Command_Issue_Top.v
add_files -fileset sim_1 ../tb/nandmodel/nand_defines.vh
add_files -fileset sim_1 ../tb/nandmodel/nand_parameters.vh
add_files -fileset sim_1 ../tb/nandmodel/nand_die_model.v
add_files -fileset sim_1 ../tb/nandmodel/nand_model.v
add_files -fileset sources_1 ../lib/axis/rtl/axis_fifo.v
set_property top NFC_Atom_Command_Generator_Top [get_filesets sources_1]
set_property top tb_NFC_Atom_Command_Generator_Top [get_filesets sim_1]
update_compile_order -fileset sources_1
update_compile_order -fileset sim_1
set_property target_simulator ModelSim [current_project]
set_property -name {modelsim.simulate.log_all_signals} -value {true} -objects [get_filesets sim_1]
set_property -name {modelsim.simulate.runtime} -value {10us} -objects [get_filesets sim_1]
exit
