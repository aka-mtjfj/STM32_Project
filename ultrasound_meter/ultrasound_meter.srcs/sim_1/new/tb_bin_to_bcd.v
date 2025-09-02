//二进制码转为8421BCD码仿真模块
//time:2023-12-25

`timescale 1ns/1ns
module tb_bin_to_bcd ();

reg            sys_clk         ;           
reg            sys_rst_n       ;           
reg    [12:0]  data_ave        ;//二进制      
wire   [15:0]  data_bcd        ; //8421bcd码 


initial begin
    sys_clk = 1'b0;
    sys_rst_n<=1'b0;
    data_ave<=13'd0;
    #30;
    sys_rst_n<=1'b1;
    #20;
    data_ave<=13'd4987;
    #50;
     data_ave<=13'd3214;
    #50;
     data_ave<=13'd1234;
    #50;
     data_ave<=13'd89;
    #50;
     data_ave<=13'd213;
    #50;
     data_ave<=13'd23;
    #50;
    #100;//10个时钟周期
    $stop;
    
end
always #5 sys_clk = ~sys_clk;//100MHz的时钟，周期是10ns

bin_to_bcd bin_to_bcd_inst
(
    . sys_clk    (sys_clk  )     ,
    . sys_rst_n  (sys_rst_n)     ,
    . data_ave   (data_ave )     ,//二进制
    . data_bcd   (data_bcd )      //8421bcd码
);

endmodule 
