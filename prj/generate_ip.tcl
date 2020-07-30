

proc ip_create {ip_name} {

  global ad_hdl_dir
  global ad_phdl_dir
  global ip_constr_files

  create_project $ip_name . -force

  set ip_constr_files ""
  set lib_dirs .

  set_property ip_repo_paths $lib_dirs [current_fileset]
  update_ip_catalog
}

proc ip_files {ip_name ip_files} {

  global ip_constr_files

  set ip_constr_files ""
  foreach m_file $ip_files {
    if {[file extension $m_file] eq ".xdc"} {
      lappend ip_constr_files $m_file
    }
  }

  set proj_fileset [get_filesets sources_1]
  add_files -norecurse -scan_for_includes -fileset $proj_fileset $ip_files
  set_property "top" "$ip_name" $proj_fileset
}

proc ip_properties_lite {ip_name} {

  global ip_constr_files

  ipx::package_project -root_dir . -vendor Opensource -library user 
  set_property name $ip_name [ipx::current_core]
  set_property vendor_display_name {Opensource} [ipx::current_core]

  set i_families ""
  foreach i_part [get_parts] {
    lappend i_families [get_property FAMILY $i_part]
  }
  set i_families [lsort -unique $i_families]
  set s_families [get_property supported_families [ipx::current_core]]
  foreach i_family $i_families {
    set s_families "$s_families $i_family Production"
    set s_families "$s_families $i_family Beta"
  }
  set_property supported_families $s_families [ipx::current_core]
  ipx::save_core

  ipx::remove_all_bus_interface [ipx::current_core]
  set memory_maps [ipx::get_memory_maps * -of_objects [ipx::current_core]]
  foreach map $memory_maps {
    ipx::remove_memory_map [lindex $map 2] [ipx::current_core ]
  }
  ipx::save_core

  set i_filegroup [ipx::get_file_groups -of_objects [ipx::current_core] -filter {NAME =~ *synthesis*}]
  foreach i_file $ip_constr_files {
    set i_module [file tail $i_file]
    regsub {_constr\.xdc} $i_module {} i_module
    ipx::add_file $i_file $i_filegroup
    ipx::reorder_files -front $i_file $i_filegroup
    set_property SCOPED_TO_REF $i_module [ipx::get_files $i_file -of_objects $i_filegroup]
  }
  ipx::save_core
}

proc ip_properties {ip_name} {

  ip_properties_lite $ip_name

  ipx::infer_bus_interface {\
    s_axil_awvalid \
    s_axil_awaddr \
    s_axil_awprot \
    s_axil_awready \
    s_axil_wvalid \
    s_axil_wdata \
    s_axil_wstrb \
    s_axil_wready \
    s_axil_bvalid \
    s_axil_bresp \
    s_axil_bready \
    s_axil_arvalid \
    s_axil_araddr \
    s_axil_arprot \
    s_axil_arready \
    s_axil_rvalid \
    s_axil_rdata \
    s_axil_rresp \
    s_axil_rready} \
  xilinx.com:interface:aximm_rtl:1.0 [ipx::current_core]

  ipx::infer_bus_interface s_axil_clk xilinx.com:signal:clock_rtl:1.0 [ipx::current_core]
  ipx::infer_bus_interface s_axil_rst xilinx.com:signal:reset_rtl:1.0 [ipx::current_core]

  set raddr_width [expr [get_property SIZE_LEFT [ipx::get_ports -nocase true s_axil_araddr -of_objects [ipx::current_core]]] + 1]
  set waddr_width [expr [get_property SIZE_LEFT [ipx::get_ports -nocase true s_axil_awaddr -of_objects [ipx::current_core]]] + 1]

  if {$raddr_width != $waddr_width} {
    puts [format "WARNING: AXI address width mismatch for %s (r=%d, w=%d)" $ip_name $raddr_width, $waddr_width]
    set range 65536
  } else {
    if {$raddr_width >= 16} {
      set range 65536
    } else {
      set range [expr 1 << $raddr_width]
    }
  }

  ipx::add_memory_map {s_axil} [ipx::current_core]
  set_property slave_memory_map_ref {s_axil} [ipx::get_bus_interfaces s_axil -of_objects [ipx::current_core]]
  ipx::add_address_block {axi_lite} [ipx::get_memory_maps s_axil -of_objects [ipx::current_core]]
  set_property range $range [ipx::get_address_blocks axi_lite \
    -of_objects [ipx::get_memory_maps s_axil -of_objects [ipx::current_core]]]
  ipx::add_bus_parameter ASSOCIATED_BUSIF [ipx::get_bus_interfaces s_axil_clk \
    -of_objects [ipx::current_core]]
  set_property value s_axil [ipx::get_bus_parameters ASSOCIATED_BUSIF \
    -of_objects [ipx::get_bus_interfaces s_axil_clk \
    -of_objects [ipx::current_core]]]
  ipx::save_core
}

proc add_port_map {bus phys logic} {
  set map [ipx::add_port_map $phys $bus]
  set_property "PHYSICAL_NAME" $phys $map
  set_property "LOGICAL_NAME" $logic $map
}

proc add_bus {bus_name mode abs_type bus_type port_maps} {
  set bus [ipx::add_bus_interface $bus_name [ipx::current_core]]

  set_property "ABSTRACTION_TYPE_VLNV" $abs_type $bus
  set_property "BUS_TYPE_VLNV" $bus_type $bus
  set_property "INTERFACE_MODE" $mode $bus

  foreach port_map $port_maps {
    add_port_map $bus {*}$port_map
  }
}