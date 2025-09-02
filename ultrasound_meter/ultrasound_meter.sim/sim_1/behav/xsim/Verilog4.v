//����������ģ��
//time:2023-12-25

//1��trig�������������źţ��ߵ�ƽ����Ϊ10us,ͬʱ���ǵ��źŲ�Ҫ�ص������Ե�����5000mmʱ������ʱ����Ϊ�����������źŵ�����T
//T=(5/340) * 1000ms = 14.7ms�����ؾ���29.4ms�����Ծ��ô����������������Ϊ30ms��100ms�����ߵ�ƽʱ��Ϊ10us

//2��echo�������źţ�ͨ���Ի����źŵĸߵ�ƽʱ��������������
//���㣺s=340*t/2 m   =   340_000 * t /2 mm
//����������N
//s=N*10*340_000/1000_000_000/2 mm = N*0.0017 mm = N*17/10000 ;
//������Ϊ5mʱ��5000/0.0017=2941176     22bit      ; 


module ultrasound_ctrl
(
    input   wire            sys_clk         ,//100MHz
    input   wire            sys_rst_n       ,//active low
    input   wire            echo            ,//�����ź�
    output  reg             fall_flag_r1    ,//��־�ź�
    output  reg     [12:0]  data_bin        ,//����
    output  reg             trig             //�����ź�
);

//parameter define
parameter   CNT_100MS_MAX = 24'D10_00_000;//100ms/10ns = 10*10^6
parameter   CNT_10US_MAX  = 10'd1000 ;//10us/10ns = 1000

//wire or reg define
reg     [23:0]  cnt_100ms   ;
reg             echo_r      ;//��echo��һ�ĵ��ź�
reg     [21:0]  cnt_echo    ;//����echo�ĸߵ�ƽʱ��
wire            echo_neg    ;//echo�źŵ��½���
reg             echo_neg_r  ;
reg     [21:0]  cnt_echo_r  ;

//main code

//100ms�����ڼ���
always @(posedge sys_clk or negedge sys_rst_n) begin
    if(!sys_rst_n)
        cnt_100ms<=24'd0;
    else if(cnt_100ms==CNT_100MS_MAX-1'b1)
        cnt_100ms<=24'd0;
    else
        cnt_100ms<=cnt_100ms+1'b1;
end

//trig:��������trig����Ϊ100ms���ߵ�ƽʱ��Ϊ10us
always @(posedge sys_clk or negedge sys_rst_n) begin
    if(!sys_rst_n)
        trig<=1'b0;
    else if(cnt_100ms<=CNT_10US_MAX-1'b1)
        trig<=1'b1;
    else
        trig<=1'b0;
end 

//echo_r:��һ��
assign  echo_neg = (~echo) & echo_r ;// ? 1'b1 : 1'b0 ;
always @(posedge sys_clk or negedge sys_rst_n) begin
    if(!sys_rst_n)
        echo_r<=1'b0;
    else
        echo_r<=echo;
end

//cnt_echo:����echo�źŵĸߵ�ƽʱ��
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

//�������s=N*10*340_000/1000_000_000/2 mm = N*0.0017 mm = N*17/10000 ;
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
