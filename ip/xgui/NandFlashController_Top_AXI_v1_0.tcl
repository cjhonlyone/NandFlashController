# Definitional proc to organize widgets for parameters.
proc init_gui { IPINST } {
  ipgui::add_param $IPINST -name "Component_Name"
  #Adding Page
  set Page_0 [ipgui::add_page $IPINST -name "Page 0"]
  ipgui::add_param $IPINST -name "AXIL_ADDR_WIDTH" -parent ${Page_0}
  ipgui::add_param $IPINST -name "AXIL_DATA_WIDTH" -parent ${Page_0}
  ipgui::add_param $IPINST -name "AXIL_STRB_WIDTH" -parent ${Page_0}
  ipgui::add_param $IPINST -name "AXI_ADDR_WIDTH" -parent ${Page_0}
  ipgui::add_param $IPINST -name "AXI_ARUSER_WIDTH" -parent ${Page_0}
  ipgui::add_param $IPINST -name "AXI_AWUSER_WIDTH" -parent ${Page_0}
  ipgui::add_param $IPINST -name "AXI_DATA_WIDTH" -parent ${Page_0}
  ipgui::add_param $IPINST -name "AXI_HPorACP" -parent ${Page_0}
  ipgui::add_param $IPINST -name "AXI_ID_WIDTH" -parent ${Page_0}
  ipgui::add_param $IPINST -name "AXI_MAX_BURST_LEN" -parent ${Page_0}
  ipgui::add_param $IPINST -name "AXI_STRB_WIDTH" -parent ${Page_0}
  ipgui::add_param $IPINST -name "IDelayValue" -parent ${Page_0}
  ipgui::add_param $IPINST -name "InputClockBufferType" -parent ${Page_0}
  ipgui::add_param $IPINST -name "NumberOfWays" -parent ${Page_0}
  ipgui::add_param $IPINST -name "PageSize" -parent ${Page_0}


}

proc update_PARAM_VALUE.AXIL_ADDR_WIDTH { PARAM_VALUE.AXIL_ADDR_WIDTH } {
	# Procedure called to update AXIL_ADDR_WIDTH when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.AXIL_ADDR_WIDTH { PARAM_VALUE.AXIL_ADDR_WIDTH } {
	# Procedure called to validate AXIL_ADDR_WIDTH
	return true
}

proc update_PARAM_VALUE.AXIL_DATA_WIDTH { PARAM_VALUE.AXIL_DATA_WIDTH } {
	# Procedure called to update AXIL_DATA_WIDTH when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.AXIL_DATA_WIDTH { PARAM_VALUE.AXIL_DATA_WIDTH } {
	# Procedure called to validate AXIL_DATA_WIDTH
	return true
}

proc update_PARAM_VALUE.AXIL_STRB_WIDTH { PARAM_VALUE.AXIL_STRB_WIDTH } {
	# Procedure called to update AXIL_STRB_WIDTH when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.AXIL_STRB_WIDTH { PARAM_VALUE.AXIL_STRB_WIDTH } {
	# Procedure called to validate AXIL_STRB_WIDTH
	return true
}

proc update_PARAM_VALUE.AXI_ADDR_WIDTH { PARAM_VALUE.AXI_ADDR_WIDTH } {
	# Procedure called to update AXI_ADDR_WIDTH when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.AXI_ADDR_WIDTH { PARAM_VALUE.AXI_ADDR_WIDTH } {
	# Procedure called to validate AXI_ADDR_WIDTH
	return true
}

proc update_PARAM_VALUE.AXI_ARUSER_WIDTH { PARAM_VALUE.AXI_ARUSER_WIDTH } {
	# Procedure called to update AXI_ARUSER_WIDTH when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.AXI_ARUSER_WIDTH { PARAM_VALUE.AXI_ARUSER_WIDTH } {
	# Procedure called to validate AXI_ARUSER_WIDTH
	return true
}

proc update_PARAM_VALUE.AXI_AWUSER_WIDTH { PARAM_VALUE.AXI_AWUSER_WIDTH } {
	# Procedure called to update AXI_AWUSER_WIDTH when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.AXI_AWUSER_WIDTH { PARAM_VALUE.AXI_AWUSER_WIDTH } {
	# Procedure called to validate AXI_AWUSER_WIDTH
	return true
}

proc update_PARAM_VALUE.AXI_DATA_WIDTH { PARAM_VALUE.AXI_DATA_WIDTH } {
	# Procedure called to update AXI_DATA_WIDTH when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.AXI_DATA_WIDTH { PARAM_VALUE.AXI_DATA_WIDTH } {
	# Procedure called to validate AXI_DATA_WIDTH
	return true
}

proc update_PARAM_VALUE.AXI_HPorACP { PARAM_VALUE.AXI_HPorACP } {
	# Procedure called to update AXI_HPorACP when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.AXI_HPorACP { PARAM_VALUE.AXI_HPorACP } {
	# Procedure called to validate AXI_HPorACP
	return true
}

proc update_PARAM_VALUE.AXI_ID_WIDTH { PARAM_VALUE.AXI_ID_WIDTH } {
	# Procedure called to update AXI_ID_WIDTH when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.AXI_ID_WIDTH { PARAM_VALUE.AXI_ID_WIDTH } {
	# Procedure called to validate AXI_ID_WIDTH
	return true
}

proc update_PARAM_VALUE.AXI_MAX_BURST_LEN { PARAM_VALUE.AXI_MAX_BURST_LEN } {
	# Procedure called to update AXI_MAX_BURST_LEN when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.AXI_MAX_BURST_LEN { PARAM_VALUE.AXI_MAX_BURST_LEN } {
	# Procedure called to validate AXI_MAX_BURST_LEN
	return true
}

proc update_PARAM_VALUE.AXI_STRB_WIDTH { PARAM_VALUE.AXI_STRB_WIDTH } {
	# Procedure called to update AXI_STRB_WIDTH when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.AXI_STRB_WIDTH { PARAM_VALUE.AXI_STRB_WIDTH } {
	# Procedure called to validate AXI_STRB_WIDTH
	return true
}

proc update_PARAM_VALUE.IDelayValue { PARAM_VALUE.IDelayValue } {
	# Procedure called to update IDelayValue when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.IDelayValue { PARAM_VALUE.IDelayValue } {
	# Procedure called to validate IDelayValue
	return true
}

proc update_PARAM_VALUE.InputClockBufferType { PARAM_VALUE.InputClockBufferType } {
	# Procedure called to update InputClockBufferType when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.InputClockBufferType { PARAM_VALUE.InputClockBufferType } {
	# Procedure called to validate InputClockBufferType
	return true
}

proc update_PARAM_VALUE.NumberOfWays { PARAM_VALUE.NumberOfWays } {
	# Procedure called to update NumberOfWays when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.NumberOfWays { PARAM_VALUE.NumberOfWays } {
	# Procedure called to validate NumberOfWays
	return true
}

proc update_PARAM_VALUE.PageSize { PARAM_VALUE.PageSize } {
	# Procedure called to update PageSize when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.PageSize { PARAM_VALUE.PageSize } {
	# Procedure called to validate PageSize
	return true
}


proc update_MODELPARAM_VALUE.AXI_HPorACP { MODELPARAM_VALUE.AXI_HPorACP PARAM_VALUE.AXI_HPorACP } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.AXI_HPorACP}] ${MODELPARAM_VALUE.AXI_HPorACP}
}

proc update_MODELPARAM_VALUE.AXIL_ADDR_WIDTH { MODELPARAM_VALUE.AXIL_ADDR_WIDTH PARAM_VALUE.AXIL_ADDR_WIDTH } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.AXIL_ADDR_WIDTH}] ${MODELPARAM_VALUE.AXIL_ADDR_WIDTH}
}

proc update_MODELPARAM_VALUE.AXIL_DATA_WIDTH { MODELPARAM_VALUE.AXIL_DATA_WIDTH PARAM_VALUE.AXIL_DATA_WIDTH } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.AXIL_DATA_WIDTH}] ${MODELPARAM_VALUE.AXIL_DATA_WIDTH}
}

proc update_MODELPARAM_VALUE.AXIL_STRB_WIDTH { MODELPARAM_VALUE.AXIL_STRB_WIDTH PARAM_VALUE.AXIL_STRB_WIDTH } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.AXIL_STRB_WIDTH}] ${MODELPARAM_VALUE.AXIL_STRB_WIDTH}
}

proc update_MODELPARAM_VALUE.AXI_ID_WIDTH { MODELPARAM_VALUE.AXI_ID_WIDTH PARAM_VALUE.AXI_ID_WIDTH } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.AXI_ID_WIDTH}] ${MODELPARAM_VALUE.AXI_ID_WIDTH}
}

proc update_MODELPARAM_VALUE.AXI_ADDR_WIDTH { MODELPARAM_VALUE.AXI_ADDR_WIDTH PARAM_VALUE.AXI_ADDR_WIDTH } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.AXI_ADDR_WIDTH}] ${MODELPARAM_VALUE.AXI_ADDR_WIDTH}
}

proc update_MODELPARAM_VALUE.AXI_DATA_WIDTH { MODELPARAM_VALUE.AXI_DATA_WIDTH PARAM_VALUE.AXI_DATA_WIDTH } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.AXI_DATA_WIDTH}] ${MODELPARAM_VALUE.AXI_DATA_WIDTH}
}

proc update_MODELPARAM_VALUE.AXI_MAX_BURST_LEN { MODELPARAM_VALUE.AXI_MAX_BURST_LEN PARAM_VALUE.AXI_MAX_BURST_LEN } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.AXI_MAX_BURST_LEN}] ${MODELPARAM_VALUE.AXI_MAX_BURST_LEN}
}

proc update_MODELPARAM_VALUE.AXI_STRB_WIDTH { MODELPARAM_VALUE.AXI_STRB_WIDTH PARAM_VALUE.AXI_STRB_WIDTH } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.AXI_STRB_WIDTH}] ${MODELPARAM_VALUE.AXI_STRB_WIDTH}
}

proc update_MODELPARAM_VALUE.AXI_ARUSER_WIDTH { MODELPARAM_VALUE.AXI_ARUSER_WIDTH PARAM_VALUE.AXI_ARUSER_WIDTH } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.AXI_ARUSER_WIDTH}] ${MODELPARAM_VALUE.AXI_ARUSER_WIDTH}
}

proc update_MODELPARAM_VALUE.AXI_AWUSER_WIDTH { MODELPARAM_VALUE.AXI_AWUSER_WIDTH PARAM_VALUE.AXI_AWUSER_WIDTH } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.AXI_AWUSER_WIDTH}] ${MODELPARAM_VALUE.AXI_AWUSER_WIDTH}
}

proc update_MODELPARAM_VALUE.IDelayValue { MODELPARAM_VALUE.IDelayValue PARAM_VALUE.IDelayValue } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.IDelayValue}] ${MODELPARAM_VALUE.IDelayValue}
}

proc update_MODELPARAM_VALUE.InputClockBufferType { MODELPARAM_VALUE.InputClockBufferType PARAM_VALUE.InputClockBufferType } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.InputClockBufferType}] ${MODELPARAM_VALUE.InputClockBufferType}
}

proc update_MODELPARAM_VALUE.NumberOfWays { MODELPARAM_VALUE.NumberOfWays PARAM_VALUE.NumberOfWays } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.NumberOfWays}] ${MODELPARAM_VALUE.NumberOfWays}
}

proc update_MODELPARAM_VALUE.PageSize { MODELPARAM_VALUE.PageSize PARAM_VALUE.PageSize } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.PageSize}] ${MODELPARAM_VALUE.PageSize}
}

