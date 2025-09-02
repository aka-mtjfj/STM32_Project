//二进制码转为8421BCD码模块
//time:2023-12-26

//1、判断、移位（大于等于5，加3）
//2、进行N次，N为数据位宽

module bin_to_bcd
(
    input   wire            sys_clk         ,
    input   wire            sys_rst_n       ,
    input   wire    [12:0]  data_ave        ,//二进制
    output  wire    [15:0]  data_bcd         //8421bcd码
);
//wire or reg define
reg     [28:0]      data_temp       ;//中间变量


always @(posedge sys_clk or negedge sys_rst_n) begin
    if(!sys_rst_n)
        data_temp=29'd0;
    else begin
        data_temp={16'b0000_0000_0000_0000,data_ave};
        repeat(13) begin //重复N次判断移位操作
            if(data_temp[16:13]>=4'd5)//判断（大于等于5，加3）
                data_temp[16:13]=data_temp[16:13]+4'd3;
            
            if(data_temp[20:17]>=4'd5)//判断（大于等于5，加3）
                data_temp[20:17]=data_temp[20:17]+4'd3;
            
            if(data_temp[24:21]>=4'd5)//判断（大于等于5，加3）
                data_temp[24:21]=data_temp[24:21]+4'd3;
            
            if(data_temp[28:25]>=4'd5)//判断（大于等于5，加3）
                data_temp[28:25]=data_temp[28:25]+4'd3;
            //判断完成之后进行移位
            data_temp=data_temp<<1'b1;
        end 
    end 
end 

assign data_bcd = data_temp[28:13] ;//输出的8421BCD码

endmodule
