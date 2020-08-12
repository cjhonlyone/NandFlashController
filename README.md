# Nand Flash Controller

- ONFI 2.1
- Code idea from Cosmos-plus-OpenSSD
- DDR mode
- DMA Transfer

# Table of Contents

- [Performance](#Performance)
- [NFC RAW Interface](#NFC-RAW-Interface)
- [AXI Interface](#AXI-Interface)
- [Modules and Files ](#Modules-and-Files)
- [How to Use](#How-to-Use)
- [Building IP and simulation](#Building-IP-and-simulation)

### Performance 

-----

- Tested device : MT29F64GAECA
- Program(multi-cache, 1 Data Bus 1 RB) : 33MB/s


### NFC RAW Interface

------

#### Configure Interface

``` verilog
    input   [5:0]                  iOpcode                 ;
    input   [4:0]                  iTargetID               ;
    input   [31:0]                 iAddress                ;
    input   [15:0]                 iLength                 ;
    input                          iCMDValid               ;
    output                         oCMDReady               ;
```

#### Data Output  Interface

```verilog
    input   [15:0]                 iWriteData              ;
    input                          iWriteLast              ;
    input                          iWriteValid             ;
    input   [1:0]                  iWriteKeep              ;
    output                         oWriteReady             ;
```

#### Data Input  Interface

```verilog
    output  [15:0]                 oReadData               ;
    output                         oReadLast               ;
    output                         oReadValid              ;
    output  [1:0]                  oReadKeep               ;
    input                          iReadReady              ;
```

#### Status Interface

```verilog
    output  [NumberOfWays - 1:0]   oReadyBusy              ;
    
    output  [23:0]                 oStatus                 ;
    output                         oStatusValid            ;
```

#### Nand Flash Physics Interface

```verilog
    inout                          IO_NAND_DQS             ;
    inout                  [7:0]   IO_NAND_DQ              ;
    output  [NumberOfWays - 1:0]   O_NAND_CE               ;
    output                         O_NAND_WE               ;
    output                         O_NAND_RE               ;
    output                         O_NAND_ALE              ;
    output                         O_NAND_CLE              ;
    input   [NumberOfWays - 1:0]   I_NAND_RB               ;
    output                         O_NAND_WP               ;
```

### AXI Interface

------

#### AXI-lite for Configuration

```verilog
    /*
    * AXI-lite slave interface
    */
    input  wire [ADDR_WIDTH-1:0]      s_axil_awaddr      ,
    input  wire [2:0]                 s_axil_awprot      ,
    input  wire                       s_axil_awvalid     ,
    output wire                       s_axil_awready     ,
    input  wire [DATA_WIDTH-1:0]      s_axil_wdata       ,
    input  wire [STRB_WIDTH-1:0]      s_axil_wstrb       ,
    input  wire                       s_axil_wvalid      ,
    output wire                       s_axil_wready      ,
    output wire [1:0]                 s_axil_bresp       ,
    output wire                       s_axil_bvalid      ,
    input  wire                       s_axil_bready      ,
    input  wire [ADDR_WIDTH-1:0]      s_axil_araddr      ,
    input  wire [2:0]                 s_axil_arprot      ,
    input  wire                       s_axil_arvalid     ,
    output wire                       s_axil_arready     ,
    output wire [DATA_WIDTH-1:0]      s_axil_rdata       ,
    output wire [1:0]                 s_axil_rresp       ,
    output wire                       s_axil_rvalid      ,
    input  wire                       s_axil_rready      ,
```

| Configuration Offset | Register Name | Description |
| :----: | ---- | ---- |
| 0x00 | rCommand      | {targetID, opcode} |
| 0x04 | rAddress      | Way/Col/Row/Features |
| 0x08 | rLength       | Data length, 8 Bytes Alignment |
| 0x0C | rDMARAddress  | Data Address ,flash read from DDR |
| 0x10 | rDMAWAddress  | Data Address ,flash write to DDR |
| 0x14 | rFeature      | Features for Nand |
| 0x18 | rCommandFail  | Last Command status, last bit high valid |
| 0x1C | rNFCStatus    | [15:8]Nand Flash Status, [7:0]NFC status |
| 0x20 | rNandRBStatus | [7:0] Nand R/B Status |

#### AXI for Data Transform

```verilog
    /*
    * AXI master interface
    */
    output wire [AXI_ID_WIDTH-1:0]    m_axi_awid         ,
    output wire [AXI_ADDR_WIDTH-1:0]  m_axi_awaddr       ,
    output wire [7:0]                 m_axi_awlen        ,
    output wire [2:0]                 m_axi_awsize       ,
    output wire [1:0]                 m_axi_awburst      ,
    output wire                       m_axi_awlock       ,
    output wire [3:0]                 m_axi_awcache      ,
    output wire [2:0]                 m_axi_awprot       ,
    output wire                       m_axi_awvalid      ,
    input  wire                       m_axi_awready      ,
    output wire [AXI_DATA_WIDTH-1:0]  m_axi_wdata        ,
    output wire [AXI_STRB_WIDTH-1:0]  m_axi_wstrb        ,
    output wire                       m_axi_wlast        ,
    output wire                       m_axi_wvalid       ,
    input  wire                       m_axi_wready       ,
    input  wire [AXI_ID_WIDTH-1:0]    m_axi_bid          ,
    input  wire [1:0]                 m_axi_bresp        ,
    input  wire                       m_axi_bvalid       ,
    output wire                       m_axi_bready       ,
    output wire [AXI_ID_WIDTH-1:0]    m_axi_arid         ,
    output wire [AXI_ADDR_WIDTH-1:0]  m_axi_araddr       ,
    output wire [7:0]                 m_axi_arlen        ,
    output wire [2:0]                 m_axi_arsize       ,
    output wire [1:0]                 m_axi_arburst      ,
    output wire                       m_axi_arlock       ,
    output wire [3:0]                 m_axi_arcache      ,
    output wire [2:0]                 m_axi_arprot       ,
    output wire                       m_axi_arvalid      ,
    input  wire                       m_axi_arready      ,
    input  wire [AXI_ID_WIDTH-1:0]    m_axi_rid          ,
    input  wire [AXI_DATA_WIDTH-1:0]  m_axi_rdata        ,
    input  wire [1:0]                 m_axi_rresp        ,
    input  wire                       m_axi_rlast        ,
    input  wire                       m_axi_rvalid       ,
    output wire                       m_axi_rready       ,
```
### Clock Domain

`s_axi_clk` AXI interface clock (100MHz)

`iSystemClock` Nand Controllor logic clock (83.333MHz, sync mode 4)

`iDelayRefClock` 200MHz for IODELAY2 

`iSystemClock_90` Nand Controllor DQ output clock

### Modules and Files 

------

| Module/Files                   | Description                                                           |
| ------------------------ | --------------------------------------------------------------------- |
| `NandFlashController_Top_AXI`               | AXI and AXIS Interface Top Module                   |
| `NandFlashController_Top`               | Raw Interface Top Module                      |
| `NFC_Command_*`           | Command Decode   |
| `NFC_Atom_*`   | Atom Command (Command/Address/DataIn/DataOut) |
| `NFC_Phy*`            | Pinpad |

### How to Use

------

#### Select Way

- opcode : 6'b100000
- address : {24'd0, 8'b0000_0001} means select way 0

#### Set Column Address

- opcode : 6'b100010
- address : {16'd0, Col_addr_2Bytes}

#### Set Row Address

- opcode : 6'b100100
- address : {8'd0, Row_addr_3Bytes}

#### Reset(FFh)

- Async
- opcode : 6'b000001

Nand Flash need Reset when power up.

#### Set Feature(EFh)

- Async
- opcode : 6'b000010

Set Timing mode to Sync mode 0-5.

MT29F64G can run Sync mode 4.

#### Get Feature(EEh)

- Sync 
- opcode : 6'b000101

Get the current Feature.

#### Read Parameter Page(ECh)

- Sync
- opcode : 6'b000001
- length : 4320 means 4320 Bytes, must be even

 Read Parameter Page

#### Program Page(80h)

- Sync
- opcode : 6'b000011
- targetID : 5'b00000 Program page normal(10h)
- targetID : 5'b00001 Program page cache(15h)
- targetID : 5'b00010 Program page multi-plane(11h)
- length : 4320 means 4320 Bytes, must be even

#### Read Page(00h)

- Sync
- opcode : 6'b000100
- length : 4320 means 4320 Bytes, must be even

#### Erase Block(60h)

- Sync
- opcode : 6'b000110
- targetID : 5'b00000 Erase Block normal(D0h)
- targetID : 5'b00010 Erase Block multi-plane(D1h)

#### Read Status(70h/78h)

- Sync
- opcode : 6'b000111
- targetID : 5'b00000 Read Status normal(70h)
- targetID : 5'b00001 Read Status Enhanced(78h)

### Building IP and simulation

------

- need modelsim 10.6d / Vivado Simualtor

```bash
make prj
```
