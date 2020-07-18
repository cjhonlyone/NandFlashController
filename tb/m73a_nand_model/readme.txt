Confidential:
-------------
This file and all files delivered herewith are Micron Confidential Information.


Disclaimer of Warranty:
-----------------------
This software code and all associated documentation, comments
or other information (collectively "Software") is provided 
"AS IS" without warranty of any kind. MICRON TECHNOLOGY, INC. 
("MTI") EXPRESSLY DISCLAIMS ALL WARRANTIES EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO, NONINFRINGEMENT OF THIRD PARTY
RIGHTS, AND ANY IMPLIED WARRANTIES OF MERCHANTABILITY OR FITNESS
FOR ANY PARTICULAR PURPOSE. MTI DOES NOT WARRANT THAT THE
SOFTWARE WILL MEET YOUR REQUIREMENTS, OR THAT THE OPERATION OF
THE SOFTWARE WILL BE UNINTERRUPTED OR ERROR-FREE. FURTHERMORE,
MTI DOES NOT MAKE ANY REPRESENTATIONS REGARDING THE USE OR THE
RESULTS OF THE USE OF THE SOFTWARE IN TERMS OF ITS CORRECTNESS,
ACCURACY, RELIABILITY, OR OTHERWISE. THE ENTIRE RISK ARISING OUT
OF USE OR PERFORMANCE OF THE SOFTWARE REMAINS WITH YOU. IN NO
EVENT SHALL MTI, ITS AFFILIATED COMPANIES OR THEIR SUPPLIERS BE
LIABLE FOR ANY DIRECT, INDIRECT, CONSEQUENTIAL, INCIDENTAL, OR
SPECIAL DAMAGES (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS
OF PROFITS, BUSINESS INTERRUPTION, OR LOSS OF INFORMATION)
ARISING OUT OF YOUR USE OF OR INABILITY TO USE THE SOFTWARE,
EVEN IF MTI HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
Because some jurisdictions prohibit the exclusion or limitation
of liability for consequential or incidental damages, the above
limitation may not apply to you.

Copyright 2008 Micron Technology, Inc. All rights reserved.

Getting Started:
----------------
Unzip the included files to a folder.
Compile nand_model.v, nand_die_model.v, and tb.v using a verilog simulator.
Simulate the top level test bench tb.
Or, if you are using the ModelSim simulator, type "do tb.do" at the prompt.

File Descriptions:
------------------
nand_model.v        -structural wrapper for nand_die_model
nand_die_model.v    -nand model of a single die
nand_defines.vh     -file used to generate correct port maps for nand_model instanciation.  
nand_parameters.vh  -file that contains all parameters used by the model
readme.txt          -this file
tb.v                -nand model test bench
tb.do               -compiles and runs the nand_model and test bench

Defining the Operating Voltage:
-------------------------------
The verilog compiler directive "`define" may be used to choose between 
multiple operating voltages supported by the nand model.  Valid 
operating voltages include V18, and V33, and are listed in the 
nand_parameters.vh file.  The operating voltage is used to select the 
timings associated with the nand model.  The following are examples of 
defining the operating voltage.

    simulator   command line
    ---------   ------------
    ModelSim    vlog +define+V33 nand_die_model.v
    NC-Verilog  ncverilog +v2k +define+V33 nand_die_model.v
    VCS         vcs +v2k +define+V33 nand_die_model.v

Defining the Width:
--------------------------
The verilog compiler directive "`define" may be used to choose between 
multiple widths supported by the nand model.  Valid widths include x8, 
and x16, and are listed in the nand_parameters.vh file.  The width is 
used to select the amount of memory and the port sizes of the nand 
model.  The following are examples of defining the width.

    simulator   command line
    ---------   ------------
    ModelSim    vlog +define+x8 nand_die_model.v
    NC-Verilog  ncverilog +v2k +define+x8 nand_die_model.v
    VCS         vcs +v2k +define+x8 nand_die_model.v

Defining the Classification (Multidie Configurations):
-------------------------
The verilog compiler directive "`define" may be used to choose between 
multiple part classifications supported by the nand model.  The classification is 
referenced in the Part Numbering Information section of the NAND Spec.
The classification sets NUM_DIE, NUM_CE, and NUM_RB parameters for nand_die_model, 
and is used to instantiate the correct ports on the nand_model.
The following are examples of defining the classification.

    simulator   command line
    ---------   ------------
    ModelSim    vlog +define+CLASSD nand_die_model.v
    NC-Verilog  ncverilog +v2k +define+CLASSD nand_die_model.v
    VCS         vcs +v2k +define+CLASSD nand_die_model.v

All combinations of classification are considered 
valid by the nand model even though a Micron part may not exist for 
every combination.

Allocating Memory:
------------------
An associative array has been implemented to reduce the amount of 
static memory allocated by the nand model.  The size of each entry in 
the associative array is determided by the width (x8 or x16).  The 
number of entries in the array is controlled by the NUM_ROW parameter, 
and is equal to NUM_ROW*NUM_COL.  For example, if the NUM_ROW parameter 
is equal to 10, the associative array will be large enough to store 
10*NUM_COL writes to unique addresses.  The following are examples of 
setting the NUM_ROW parameter to 8.

    simulator   command line
    ---------   ------------
    ModelSim    vsim -GNUM_ROW=8 nand_model
    NC-Verilog  ncverilog +v2k +defparam+nand_die_model.NUM_ROW=8 nand_die_model.v
    VCS         vcs +v2k -pvalue+NUM_ROW=8 nand_die_model.v

It is possible to allocate memory for every address supported by the 
nand model by using the verilog compiler directive "`define FullMem".
This procedure will improve simulation performance at the expense of 
system memory.  The following are examples of allocating memory for
every address.

    Simulator   command line
    ---------   ------------
    ModelSim    vlog +define+FullMem nand_die_model.v
    NC-Verilog  ncverilog +v2k +define+FullMem nand_die_model.v
    VCS         vcs +v2k +define+FullMem nand_die_model.v

Reduced Reset Timing:
---------------------
In order to reduce simulation time due to Power-On-Reset and Soft-Reset, the nand
model has a define called "SHORT_RESET"
    simulator   command line
    ---------   ------------
    ModelSim    vlog +define+SHORT_RESET nand_die_model.v
    NC-Verilog  ncverilog +v2k +define+SHORT_RESET nand_die_model.v
    VCS         vcs +v2k +define+SHORT_RESET nand_die_model.v


Sync Mode Nand Interface define for interface selectable models:
---------------------
Some Nand parts support both async only interfaces and sync/async interfaces in the same Nand part.
The Part Number Chart in the Nand spec is used to select which interface will be used by the part.
Because the same Nand part supports both interfaces, the nand model related to that part needs to 
be able to determine which of the two interfaces are to be used by the nand model in simulation.  
"NAND_SYNC" is the define that the model uses to make that determination.  

"NAND_SYNC" creates the needed ports in the Nand Model declaration for the synchronous interface.  
For async only interfaces, do not "define" "NAND_SYNC".  
For sync/async interfaces, "define" "NAND_SYNC".

Parts/models that do not support both interfaces (have no interface definition or
only have one interface definition in the part number chart,
i.e synchronous/asynchronous mode (but not having the selection to be async only also), already 
have the correct settings of "NAND_SYNC", the user should not use this define in this case.  

This does not switch the nand model from asynchronous mode to synchronous mode.  
The user will need to switch from async mode to sync mode as normally explained in the spec.  
    simulator   command line
    ---------   ------------
    ModelSim    vlog +define+NAND_SYNC nand_die_model.v
    NC-Verilog  ncverilog +v2k +define+NAND_SYNC nand_die_model.v
    VCS         vcs +v2k +define+NAND_SYNC nand_die_model.v

