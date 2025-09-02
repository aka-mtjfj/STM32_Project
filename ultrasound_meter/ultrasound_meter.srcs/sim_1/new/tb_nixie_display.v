//数码管显示仿真模块
//time:2023-12-26

module tb_nixie_display ();

defparam nixie_display_inst.CNT_1MS_MAX=17'd3 ;

reg             sys_clk         ;                 
reg             sys_rst_n       ;                 
reg     [15:0]  data_bcd        ;                 
wire    [3:0]   nixie_cs        ;//数码管的片选信号       
wire    [7:0]   nixie_seg       ;//数码管的段选信号       


initial begin
    sys_clk = 1'b0;
    sys_rst_n<=1'b0;
    data_bcd<=16'h0000;
    #30;
    sys_rst_n<=1'b1;
    #50;
    data_bcd<=16'h1234;
    #200;
    data_bcd<=16'h5678;
    #200
    data_bcd<=16'h9012;
    #200
    #100;//10个时钟周期
    $stop;
    
end
always #5 sys_clk = ~sys_clk;//100MHz的时钟，周期是10ns

nixie_display nixie_display_inst
(
    . sys_clk     ( sys_clk   )    ,
    . sys_rst_n   ( sys_rst_n )    ,
    . data_bcd    ( data_bcd  )    ,
    . nixie_cs    ( nixie_cs  )    ,//数码管的片选信号
    . nixie_seg   ( nixie_seg )     //数码管的段选信号
);

endmodule 
