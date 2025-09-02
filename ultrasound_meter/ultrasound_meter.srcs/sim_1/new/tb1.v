
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2024/12/12 13:55:22
// Design Name: 
// Module Name: tb
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////

`timescale  1ns / 1ps

module tb_ultrasound_meter;

// ultrasound_meter Parameters
parameter PERIOD  = 10;


// ultrasound_meter Inputs
reg   sys_clk                              = 0 ;
reg   sys_rst_n                            = 0 ;
reg   echo                                 = 0 ;
reg   key                                  = 0 ;
reg  trig                                  = 0 ;

// ultrasound_meter Outputs

wire  led                                  ;
wire  [3:0]  nixie_cs                      ;
wire  [7:0]  nixie_seg                     ;


initial
begin
    forever #(PERIOD/2)  sys_clk=~sys_clk;
end

initial
begin
    #(PERIOD*2) sys_rst_n  =  1;
end

ultrasound_meter  u_ultrasound_meter (
    .sys_clk                 ( sys_clk          ),
    .sys_rst_n               ( sys_rst_n        ),
    .echo                    ( echo             ),
    .key                     ( key              ),

    .trig                    ( trig             ),
    .beep                    ( beep             ),
    .led                     ( led              ),
    .nixie_cs                ( nixie_cs   [3:0] ),
    .nixie_seg               ( nixie_seg  [7:0] )
);

initial
begin
    sys_rst_n=0;
    #20;
    sys_rst_n=1;
    trig=1;
    #2000000;
    trig=0;
    #1000;
    echo=1;
    #100000;
    echo=0;
end

endmodule

