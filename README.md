### Nand Flash Controller

- ONFI 2.1
- Code idea from Cosmos-plus-OpenSSD
- DDR mode

### Command

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

