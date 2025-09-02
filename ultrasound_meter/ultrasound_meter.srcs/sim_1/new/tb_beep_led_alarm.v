//蜂鸣器、LED灯告警仿真模块
//time:2023-12-26

`timescale 1ns/1ns

module tb_beep_led_alarm ();

defparam beep_led_alarm.CNT_1MS_MAX = 17'd3 ;

reg             sys_clk         ;                                
reg             sys_rst_n       ;                                
reg     [12:0]  data_ave        ;//均值滤波后的距离数据                    
wire            beep            ;//蜂鸣器                           
wire            led             ;//LED                           




initial begin
    sys_clk = 1'b0;
    sys_rst_n<=1'b0;
    data_ave<=13'd0;
    #30;
    sys_rst_n<=1'b1;
    #20;
    data_ave<=13'd250;
    #50;
    data_ave<=13'd190;//led
    #50;
    data_ave<=13'd45;//beep
    #600;
    data_ave<=13'd300;
    #100;//10个时钟周期
    $stop;
    
end
always #5 sys_clk = ~sys_clk;//100MHz的时钟，周期是10ns

beep_led_alarm beep_led_alarm
(
    . sys_clk     (sys_clk   )    ,
    . sys_rst_n   (sys_rst_n )    ,
    . data_ave    (data_ave  )    ,//均值滤波后的距离数据
    . beep        (beep      )    ,//蜂鸣器
    . led         (led       )     //LED
);

endmodule
