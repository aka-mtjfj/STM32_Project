//超声波控制模块仿真代码
//time:2023-12-25

`timescale 1ns/1ns
 module tb_ultrasound_ctrl ();

defparam ultrasound_ctrl_inst.CNT_100MS_MAX = 24'd10;
defparam ultrasound_ctrl_inst.CNT_10US_MAX  = 10'd4 ; 

reg             sys_clk         ;//100MHz
reg             sys_rst_n       ;//active low
reg             echo            ;//回响信号
wire            fall_flag_r1    ;//标志信号
wire    [12:0]  data_bin        ;//数据
wire            trig            ;//触发信号

initial begin
    sys_clk = 1'b0;
    sys_rst_n<=1'b0;
    echo<=1'b0;
    #30;
    sys_rst_n<=1'b1;
    #20;
    echo<=1'b1;
    #100000;//10000个时钟周期，100us
    echo<=1'b0;
    #100;//10个时钟周期
    $stop;
    
end
always #5 sys_clk = ~sys_clk;//100MHz的时钟，周期是10ns

ultrasound_ctrl ultrasound_ctrl_inst
(
    . sys_clk      ( sys_clk      )   ,//100MHz
    . sys_rst_n    ( sys_rst_n    )   ,//active low
    . echo         ( echo         )   ,//回响信号
    . fall_flag_r1 ( fall_flag_r1 )   ,//标志信号
    . data_bin     ( data_bin     )   ,//数据
    . trig         ( trig         )    //触发信号
);

endmodule 
