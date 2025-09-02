module final2
(
    input   wire            sys_clk         ,//100MHz
    input   wire            sys_rst_n       ,//active low
    input   wire            echo            ,//�����ź�
    output  wire            trig            ,//�����ź�
    input   wire            key,
    output  wire            beep            ,
    output  wire            led             ,
    output  wire    [3:0]   nixie_cs        ,//�����Ƭѡ�ź�
    output  wire    [7:0]   nixie_seg        //����ܶ�ѡ�ź�
);

//wire or reg define
wire            data_flag       ;
wire    [12:0]  data_bin        ;
wire    [12:0]  data_ave        ;
wire    [15:0]  data_bcd        ;

//Instantiation
//������ģ��
ultrasound_ctrl u_ultrasound_ctrl
(
    . sys_clk      ( sys_clk      )   ,//100MHz
    . sys_rst_n    ( sys_rst_n    )   ,//active low
    . echo         ( echo         )   ,//�����ź�
    . fall_flag_r1 ( data_flag    )   ,//��־�ź�
    . data_bin     ( data_bin     )   ,//����
    . trig         ( trig         )    //�����ź�
);
//��ֵ�˲�ģ��
ave_filter u_ave_filter
(
    . sys_clk      (sys_clk   )       ,//100m
    . sys_rst_n    (sys_rst_n )       ,//active low
    . data_bin     (data_bin  )       ,//��������
    . data_flag    (data_flag )       ,//�����־�ź�
    . data_ave     (data_ave  )        //��ֵ�˲��������
);
//��������LED�Ƹ澯ģ��
beep_led_alarm u_beep_led_alarm
(
    . sys_clk     (sys_clk    )    ,
    . sys_rst_n   (sys_rst_n  )    ,
    . data_ave    (data_ave   )    ,//��ֵ�˲���ľ�������
    . beep        (beep       )    ,//������
    . led         (led        )     //LED
);
//������ת8421BCD��ģ��
bin_to_bcd u_bin_to_bcd
(
    . sys_clk     ( sys_clk   )    ,
    . sys_rst_n   ( sys_rst_n )    ,
    . data_ave    ( data_ave  )    ,//������
    . data_bcd    ( data_bcd  )     //8421bcd��
);
//�������ʾģ��
nixie_display u_nixie_display
(
    .key     (key),
    . sys_clk     ( sys_clk   )    ,
    . sys_rst_n   ( sys_rst_n )    ,
    . data_bcd    ( data_bcd  )    ,
    . nixie_cs    ( nixie_cs  )    ,//����ܵ�Ƭѡ�ź�
    . nixie_seg   ( nixie_seg )     //����ܵĶ�ѡ�ź�
);

endmodule 