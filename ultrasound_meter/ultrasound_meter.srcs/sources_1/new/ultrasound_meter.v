//超声波测距顶层模块
//2023-12-26

module ultrasound_meter
(
    input   wire            sys_clk         ,//100MHz
    input   wire            sys_rst_n       ,//active low
    input   wire            echo            ,//回响信号
    output  wire            trig            ,//触发信号
    input   wire            key,
    output  wire            beep            ,
    output  wire            led             ,
    output  wire    [3:0]   nixie_cs        ,//数码管片选信号
    output  wire    [7:0]   nixie_seg        //数码管段选信号
);

//wire or reg define
wire            data_flag       ;
wire    [12:0]  data_bin        ;
wire    [12:0]  data_ave        ;
wire    [15:0]  data_bcd        ;

//Instantiation
//超声波模块
ultrasound_ctrl u_ultrasound_ctrl
(
    . sys_clk      ( sys_clk      )   ,//100MHz
    . sys_rst_n    ( sys_rst_n    )   ,//active low
    . echo         ( echo         )   ,//回响信号
    . fall_flag_r1 ( data_flag    )   ,//标志信号
    . data_bin     ( data_bin     )   ,//数据
    . trig         ( trig         )    //触发信号
);
//均值滤波模块
ave_filter u_ave_filter
(
    . sys_clk      (sys_clk   )       ,//100m
    . sys_rst_n    (sys_rst_n )       ,//active low
    . data_bin     (data_bin  )       ,//距离数据
    . data_flag    (data_flag )       ,//距离标志信号
    . data_ave     (data_ave  )        //均值滤波后的数据
);
//蜂鸣器和LED灯告警模块
beep_led_alarm u_beep_led_alarm
(
    . sys_clk     (sys_clk    )    ,
    . sys_rst_n   (sys_rst_n  )    ,
    . data_ave    (data_ave   )    ,//均值滤波后的距离数据
    . beep        (beep       )    ,//蜂鸣器
    . led         (led        )     //LED
);
//二进制转8421BCD码模块
bin_to_bcd u_bin_to_bcd
(
    . sys_clk     ( sys_clk   )    ,
    . sys_rst_n   ( sys_rst_n )    ,
    . data_ave    ( data_ave  )    ,//二进制
    . data_bcd    ( data_bcd  )     //8421bcd码
);
//数码管显示模块
nixie_display u_nixie_display
(
    .key     (key),
    . sys_clk     ( sys_clk   )    ,
    . sys_rst_n   ( sys_rst_n )    ,
    . data_bcd    ( data_bcd  )    ,
    . nixie_cs    ( nixie_cs  )    ,//数码管的片选信号
    . nixie_seg   ( nixie_seg )     //数码管的段选信号
);

endmodule 
