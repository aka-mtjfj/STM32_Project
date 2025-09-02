//超声波控制模块
//time:2023-12-25

//1、trig：超声波触发信号，高电平至少为10us,同时考虑到信号不要重叠，所以当距离5000mm时的来回时间作为超声波触发信号的周期T
//T=(5/340) * 1000ms = 14.7ms，来回就是29.4ms，所以就让触发脉冲的周期至少为30ms（100ms），高电平时间为10us

//2、echo：回响信号，通过对回响信号的高电平时间计数，计算距离
//计算：s=340*t/2 m   =   340_000 * t /2 mm
//记周期数：N
//s=N*10*340_000/1000_000_000/2 mm = N*0.0017 mm = N*17/10000 ;
//当距离为5m时，5000/0.0017=2941176     22bit      ; 


module ultrasound_ctrl
(
    input   wire            sys_clk         ,//100MHz
    input   wire            sys_rst_n       ,//active low
    input   wire            echo            ,//回响信号
    output  reg             fall_flag_r1    ,//标志信号
    output  reg     [12:0]  data_bin        ,//数据
    output  reg             trig             //触发信号
);

//parameter define
parameter   CNT_100MS_MAX = 24'D10_00_000;//100ms/10ns = 10*10^6
parameter   CNT_10US_MAX  = 10'd1000 ;//10us/10ns = 1000

//wire or reg define
reg     [23:0]  cnt_100ms   ;
reg             echo_r      ;//对echo打一拍的信号
reg     [21:0]  cnt_echo    ;//计数echo的高电平时间
wire            echo_neg    ;//echo信号的下降沿
reg             echo_neg_r  ;
reg     [21:0]  cnt_echo_r  ;

//main code

//100ms的周期计数
always @(posedge sys_clk or negedge sys_rst_n) begin
    if(!sys_rst_n)
        cnt_100ms<=24'd0;
    else if(cnt_100ms==CNT_100MS_MAX-1'b1)
        cnt_100ms<=24'd0;
    else
        cnt_100ms<=cnt_100ms+1'b1;
end

//trig:触发脉冲trig周期为100ms，高电平时间为10us
always @(posedge sys_clk or negedge sys_rst_n) begin
    if(!sys_rst_n)
        trig<=1'b0;
    else if(cnt_100ms<=CNT_10US_MAX-1'b1)
        trig<=1'b1;
    else
        trig<=1'b0;
end 

//echo_r:打一拍
assign  echo_neg = (~echo) & echo_r ;// ? 1'b1 : 1'b0 ;
always @(posedge sys_clk or negedge sys_rst_n) begin
    if(!sys_rst_n)
        echo_r<=1'b0;
    else
        echo_r<=echo;
end

//cnt_echo:计数echo信号的高电平时间
always @(posedge sys_clk or negedge sys_rst_n) begin
    if(!sys_rst_n)
        cnt_echo<=22'd0;
    else if(echo_r)
        cnt_echo<=cnt_echo+1'b1;
    else
        cnt_echo<=22'd0;
end 

//cnt_echo_r
always @(posedge sys_clk or negedge sys_rst_n) begin
    if(!sys_rst_n)
        cnt_echo_r<=22'd0;
    else if(echo_neg)
        cnt_echo_r<=cnt_echo;
    else
        cnt_echo_r<=cnt_echo_r;
end 

//echo_neg_r
always @(posedge sys_clk or negedge sys_rst_n) begin
    if(!sys_rst_n)
        echo_neg_r<=1'b0;
    else
        echo_neg_r<=echo_neg;
end 

//计算距离s=N*10*340_000/1000_000_000/2 mm = N*0.0017 mm = N*17/10000 ;
//data_bin
always @(posedge sys_clk or negedge sys_rst_n) begin
    if(!sys_rst_n)
        data_bin<=13'd0;
    else if(echo_neg_r)
        data_bin<=cnt_echo_r*17/10000; //--
    else
        data_bin<=data_bin;
end 

//fall_flag_r1
always @(posedge sys_clk or negedge sys_rst_n) begin
    if(!sys_rst_n)
        fall_flag_r1<=1'b0;
    else
        fall_flag_r1<=echo_neg_r;
end 

endmodule
