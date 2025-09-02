//��������תΪ8421BCD��ģ��
//time:2023-12-26

//1���жϡ���λ�����ڵ���5����3��
//2������N�Σ�NΪ����λ��

module bin_to_bcd
(
    input   wire            sys_clk         ,
    input   wire            sys_rst_n       ,
    input   wire    [12:0]  data_ave        ,//������
    output  wire    [15:0]  data_bcd         //8421bcd��
);
//wire or reg define
reg     [28:0]      data_temp       ;//�м����


always @(posedge sys_clk or negedge sys_rst_n) begin
    if(!sys_rst_n)
        data_temp=29'd0;
    else begin
        data_temp={16'b0000_0000_0000_0000,data_ave};
        repeat(13) begin //�ظ�N���ж���λ����
            if(data_temp[16:13]>=4'd5)//�жϣ����ڵ���5����3��
                data_temp[16:13]=data_temp[16:13]+4'd3;
            
            if(data_temp[20:17]>=4'd5)//�жϣ����ڵ���5����3��
                data_temp[20:17]=data_temp[20:17]+4'd3;
            
            if(data_temp[24:21]>=4'd5)//�жϣ����ڵ���5����3��
                data_temp[24:21]=data_temp[24:21]+4'd3;
            
            if(data_temp[28:25]>=4'd5)//�жϣ����ڵ���5����3��
                data_temp[28:25]=data_temp[28:25]+4'd3;
            //�ж����֮�������λ
            data_temp=data_temp<<1'b1;
        end 
    end 
end 

assign data_bcd = data_temp[28:13] ;//�����8421BCD��

endmodule
