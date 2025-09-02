//蜂鸣器、LED灯告警模块
//time:2023-12-26

//1、距离小于5cm时，蜂鸣器发出di(频率500Hz)
///2、距离小于20cm时，LED灯点亮

module beep_led_alarm
(
    input   wire            sys_clk         ,
    input   wire            sys_rst_n       ,
    input   wire    [12:0]  data_ave        ,//均值滤波后的距离数据
    output  reg             beep            ,//蜂鸣器
    output  reg             led              //LED
);

//parameter define
parameter   CNT_1MS_MAX = 17'd100_000 ;//500Hz，2ms，1ms翻转一次 1ms/10ns = 10^5
//wire or reg define
reg     [16:0]  cnt_1ms     ;
reg             clk_500hz   ;

//main code
//LED告警
//assign led = (data_ave<=13'd200) ? 1'b1 : 1'b0 ;//距离小于200mm时，LED灯点亮
always @(posedge sys_clk or negedge sys_rst_n) begin
    if(!sys_rst_n)
        led<=1'b0;
    else if(data_ave<=13'd200)
        led<=1'b1;
    else
        led<=1'b0;
end 

//蜂鸣器告警
//assign beep = (data_ave<=13'd50) ? clk_500hz : 1'b0 ;//距离小于50mm时，蜂鸣器发出di
always @(posedge sys_clk or negedge sys_rst_n) begin
    if(!sys_rst_n)
        beep<=1'b0;
    else if(data_ave<=13'd50)
        beep<=clk_500hz;
    else
        beep<=1'b0;
end 

//1ms循环计数
always @(posedge sys_clk or negedge sys_rst_n) begin
    if(!sys_rst_n) begin
        cnt_1ms<=17'd0;
        clk_500hz<=1'd0;
    end 
    else if(cnt_1ms==CNT_1MS_MAX-1'B1) begin //0~99_999
        cnt_1ms<=17'd0;
        clk_500hz<=~clk_500hz;
    end 
    else begin
        cnt_1ms<=cnt_1ms+1'b1;
        clk_500hz<=clk_500hz;
    end 
end 

endmodule 
