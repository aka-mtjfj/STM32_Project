//均值滤波模块
//time:2023-12-26

//1、缓存8个距离数据
//2、对这8个数据做均值

module ave_filter
(
    input   wire            sys_clk             ,//100m
    input   wire            sys_rst_n           ,//active low
    input   wire    [12:0]  data_bin            ,//距离数据
    input   wire            data_flag           ,//距离标志信号
    
    output  wire    [12:0]  data_ave             //均值滤波后的数据
);

//wire or reg define 
reg     [12:0]  data_reg    [7:0]   ;//用于缓存8个数据
reg     [15:0]  data_add            ;//加法计算



//main code
//数据缓存
always @(posedge sys_clk or negedge sys_rst_n) begin
    if(!sys_rst_n) begin 
        data_reg[0]<=13'd0;
        data_reg[1]<=13'd0;
        data_reg[2]<=13'd0;
        data_reg[3]<=13'd0;
        data_reg[4]<=13'd0;
        data_reg[5]<=13'd0;
        data_reg[6]<=13'd0;
        data_reg[7]<=13'd0;
    end 
    else if(data_flag) begin//移位寄存
        data_reg[0]<=data_bin;
        data_reg[1]<=data_reg[0];
        data_reg[2]<=data_reg[1];
        data_reg[3]<=data_reg[2];
        data_reg[4]<=data_reg[3];
        data_reg[5]<=data_reg[4];
        data_reg[6]<=data_reg[5];
        data_reg[7]<=data_reg[6];
    end
    //else begin //保持
    
    //end
end

//data_add:加法
always @(posedge sys_clk or negedge sys_rst_n) begin
    if(!sys_rst_n)
        data_add<=16'd0;
    else
        data_add<=data_reg[0]+data_reg[1]+data_reg[2]+data_reg[3]+data_reg[4]+data_reg[5]+data_reg[6]+data_reg[7];
end

//均值滤波后的数据输出
assign data_ave = data_add[15:3] ;//把低3位截断，等效于除以8

endmodule 
