//均值滤波仿真模块
//time:2023-12-26

`timescale 1ns/1ns
module tb_ave_filter ();

reg             sys_clk             ;//100m                
reg             sys_rst_n           ;//active low          
reg     [12:0]  data_bin            ;//距离数据                
reg             data_flag           ;//距离标志信号                              
wire    [12:0]  data_ave            ;//均值滤波后的数据            

initial begin
    sys_clk = 1'b0;
    sys_rst_n<=1'b0;
    data_bin<=13'd0;
    data_flag<=1'b0;
    #30;
    sys_rst_n<=1'b1;
    #50;
    data_bin<=13'd2;//2
    data_flag<=1'b1;
    #10;
    data_flag<=1'b0;
    #50;
    data_bin<=13'd3;//3
    data_flag<=1'b1;
    #10;
    data_flag<=1'b0;
    #50;
    data_bin<=13'd4;//4
    data_flag<=1'b1;
    #10;
    data_flag<=1'b0;
    #50;
    data_bin<=13'd5;//5
    data_flag<=1'b1;
    #10;
    data_flag<=1'b0;
    #50;
    data_bin<=13'd6;//6
    data_flag<=1'b1;
    #10;
    data_flag<=1'b0;
    #50;
    data_bin<=13'd7;//7
    data_flag<=1'b1;
    #10;
    data_flag<=1'b0;
    #50;
    data_bin<=13'd8;//8
    data_flag<=1'b1;
    #10;
    data_flag<=1'b0;
    #50;
    data_bin<=13'd9;//9
    data_flag<=1'b1;
    #10;
    data_flag<=1'b0;
    #50;
    $stop;
    
end
always #5 sys_clk = ~sys_clk;//100MHz的时钟，周期是10ns

ave_filter ave_filter_inst
(
    . sys_clk        ( sys_clk   )     ,//100m
    . sys_rst_n      ( sys_rst_n )     ,//active low
    . data_bin       ( data_bin  )     ,//距离数据
    . data_flag      ( data_flag )     ,//距离标志信号
    . data_ave       ( data_ave  )      //均值滤波后的数据
);

endmodule
