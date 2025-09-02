//数码管显示模块
//time:2023-12-26

/*1、数码管高电平点亮，高电平片选和段选

2、	  _0 = 8'b0011_1111   ,
            _1 = 8'b0000_0110   ,
            _2 = 8'b0101_1011   ,
            _3 = 8'b0100_1111   ,
            _4 = 8'b0110_0110   ,
            _5 = 8'b0110_1101   ,
            _6 = 8'b0111_1101   ,
            _7 = 8'b0000_0111   ,
            _8 = 8'b0111_1111   ,
            _9 = 8'b0110_1111   ;*/

//人眼视觉暂留效应、余晖效应
//数码管的动态显示
module nixie_display
(
    input   wire            sys_clk         ,
    input   wire            sys_rst_n       ,
    input    wire              key,
    input   wire    [15:0]  data_bcd        ,
    output  reg     [3:0]   nixie_cs        ,//数码管的片选信号
    output  reg     [7:0]   nixie_seg        //数码管的段选信号
);

//parameter define
parameter   CNT_1MS_MAX = 17'd100_000 ;//51ms/10ns = 10^5
parameter  _0 = 8'b0011_1111   ,
            _1 = 8'b0000_0110   ,
            _2 = 8'b0101_1011   ,
            _3 = 8'b0100_1111   ,
            _4 = 8'b0110_0110   ,
            _5 = 8'b0110_1101   ,
            _6 = 8'b0111_1101   ,
            _7 = 8'b0000_0111   ,
            _8 = 8'b0111_1111   ,
            _9 = 8'b0110_1111   ;

//wire or reg define
reg     [16:0]  cnt_1ms     ;
wire            cnt_1ms_flag    ;
reg     [1:0]   cnt_nixie       ;//计数，标志第几个数码管亮
reg     [3:0]   data_nixie      ;//标志亮的那个数码管需要显示的数
reg  [2:0] keyflag;
reg     [15:0]  pause;
//main code
//1ms循环计数
assign cnt_1ms_flag = (cnt_1ms==CNT_1MS_MAX-1'B1) ? 1'b1 : 1'b0  ;
always @(posedge sys_clk or negedge sys_rst_n) begin //0~99_999
    if(!sys_rst_n)
        cnt_1ms<=17'd0;
    else if(cnt_1ms_flag)
        cnt_1ms<=17'd0;
    else
        cnt_1ms<=cnt_1ms+1'b1;
end 

always @(negedge key or negedge sys_rst_n) begin //0~99_999
    if(!sys_rst_n)
        keyflag<=0;
    else
    begin
        if(keyflag==0)
        keyflag<=1;
        else if(keyflag==1)
        begin
         keyflag<=2;  
         pause<= data_bcd;
        end
        
        else if(keyflag==2)
        keyflag<=1;
    end
        
end 
//cnt_nixie
always @(posedge sys_clk or negedge sys_rst_n) begin
    if(!sys_rst_n)
        cnt_nixie<=2'd0;
    else if(cnt_1ms_flag)
        cnt_nixie<=cnt_nixie+1'b1;
    else
        cnt_nixie<=cnt_nixie;
end 

//片选信号
always @(*) begin //高电平片选
if(keyflag==0)
begin
    data_nixie<=4'ha;
    nixie_cs<=4'b0001;
end
    else if(keyflag==2)
    begin
     case(cnt_nixie)
        2'd0: begin nixie_cs<=4'b0001; data_nixie<= pause[3:0]; end 
        2'd1: begin nixie_cs<=4'b0010; data_nixie<= pause[7:4]; end
        2'd2: begin nixie_cs<=4'b0100; data_nixie<= pause[11:8]; end
        2'd3: begin nixie_cs<=4'b1000; data_nixie<= pause[15:12]; end
        default: begin nixie_cs<=4'b0000; data_nixie<= pause[3:0]; end
    endcase   
    end
    else
    case(cnt_nixie)
        2'd0: begin nixie_cs<=4'b0001; data_nixie<= data_bcd[3:0]; end 
        2'd1: begin nixie_cs<=4'b0010; data_nixie<= data_bcd[7:4]; end
        2'd2: begin nixie_cs<=4'b0100; data_nixie<= data_bcd[11:8]; end
        2'd3: begin nixie_cs<=4'b1000; data_nixie<= data_bcd[15:12]; end
        default: begin nixie_cs<=4'b0000; data_nixie<= data_bcd[3:0]; end
    endcase
end 

//段选信号
always @(*) begin //高电平段选
if(cnt_nixie==1)
case(data_nixie)
        4'd0: nixie_seg<=_0|8'h80;
        4'd1: nixie_seg<=_1|8'h80;
        4'd2: nixie_seg<=_2|8'h80;
        4'd3: nixie_seg<=_3|8'h80;
        4'd4: nixie_seg<=_4|8'h80;
        4'd5: nixie_seg<=_5|8'h80;
        4'd6: nixie_seg<=_6|8'h80;
        4'd7: nixie_seg<=_7|8'h80;
        4'd8: nixie_seg<=_8|8'h80;
        4'd9: nixie_seg<=_9|8'h80;
        4'ha:nixie_seg<=_0;
        default:nixie_seg<=_0;
    endcase

else
    case(data_nixie)
        4'd0: nixie_seg<=_0;
        4'd1: nixie_seg<=_1;
        4'd2: nixie_seg<=_2;
        4'd3: nixie_seg<=_3;
        4'd4: nixie_seg<=_4;
        4'd5: nixie_seg<=_5;
        4'd6: nixie_seg<=_6;
        4'd7: nixie_seg<=_7;
        4'd8: nixie_seg<=_8;
        4'd9: nixie_seg<=_9;
        4'ha:nixie_seg<=_0;
        default:nixie_seg<=_0;
    endcase
end 

endmodule 
