/****************************************************************************************
*
*   Disclaimer   This software code and all associated documentation, comments or other 
*  of Warranty:  information (collectively "Software") is provided "AS IS" without 
*                warranty of any kind. MICRON TECHNOLOGY, INC. ("MTI") EXPRESSLY 
*                DISCLAIMS ALL WARRANTIES EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED 
*                TO, NONINFRINGEMENT OF THIRD PARTY RIGHTS, AND ANY IMIED WARRANTIES 
*                OF MERCHANTABILITY OR FITNESS FOR ANY PARTICULAR PURPOSE. MTI DOES NOT 
*                WARRANT THAT THE SOFTWARE WILL MEET YOUR REQUIREMENTS, OR THAT THE 
*                OPERATION OF THE SOFTWARE WILL BE UNINTERRUPTED OR ERROR-FREE. 
*                FURTHERMORE, MTI DOES NOT MAKE ANY REPRESENTATIONS REGARDING THE USE OR 
*                THE RESULTS OF THE USE OF THE SOFTWARE IN TERMS OF ITS CORRECTNESS, 
*                ACCURACY, RELIABILITY, OR OTHERWISE. THE ENTIRE RISK ARISING OUT OF USE 
*                OR PERFORMANCE OF THE SOFTWARE REMAINS WITH YOU. IN NO EVENT SHALL MTI, 
*                ITS AFFILIATED COMPANIES OR THEIR SUPPLIERS BE LIABLE FOR ANY DIRECT, 
*                INDIRECT, CONSEQUENTIAL, INCIDENTAL, OR SPECIAL DAMAGES (INCLUDING, 
*                WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS INTERRUPTION, 
*                OR LOSS OF INFORMATION) ARISING OUT OF YOUR USE OF OR INABILITY TO USE 
*                THE SOFTWARE, EVEN IF MTI HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH 
*                DAMAGES. Because some jurisdictions prohibit the exclusion or 
*                limitation of liability for consequential or incidental damages, the 
*                above limitation may not apply to you.
*
*                Copyright 2007 Micron Technology, Inc. All rights reserved.
*
****************************************************************************************/

`define CLASS
`define CLASSF

`ifdef CLASSE
    `define T2B2C2D2;  // 2 Die, 2 Target, 2 R/B, Separate Cmd (2 cmd buses), Separate Data (2 data buses)
`else `ifdef CLASSF
    `define T2B2C1D1;  // 2 Die, 2 Target, 2 R/B, Common Cmd (1 cmd bus), Common Data (1 data bus)
`else `ifdef CLASSJ
    `define T2B2C1D1;  // 4 Die, 2 Target, 2 R/B, Common Cmd (1 cmd bus), Common Data (1 data bus)
`else `ifdef CLASSK
    `define T2B2C2D2;  // 4 Die, 2 Target, 2 R/B, Separate Cmd (2 cmd buses), Separate Data (2 data buses)
`else `ifdef CLASSM
    `define T4B4C2D2;  // 4 Die, 4 Target, 4 R/B, Separate Cmd (2 cmd buses), Separate Data (2 data buses)
`else `ifdef CLASSU
    `define T4B4C2D2;  // 8 Die, 4 Target, 4 R/B, Separate Cmd (2 cmd buses), Separate Data (2 data buses)
`else  // DEFAULT = CLASSB
    `define T1B1C1D1;  // 1 Die, 1 Target, 1 R/B, Common Cmd (1 cmd bus), Common Data (1 data bus)
`endif `endif `endif `endif `endif `endif
