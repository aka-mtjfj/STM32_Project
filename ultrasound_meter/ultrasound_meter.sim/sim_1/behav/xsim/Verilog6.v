//��ֵ�˲�ģ��
//time:2023-12-26

//1������8����������
//2������8����������ֵ

module ave_filter
(
    input   wire            sys_clk             ,//100m
    input   wire            sys_rst_n           ,//active low
    input   wire    [12:0]  data_bin            ,//��������
    input   wire            data_flag           ,//�����־�ź�
    
    output  wire    [12:0]  data_ave             //��ֵ�˲��������
);

//wire or reg define 
reg     [12:0]  data_reg    [7:0]   ;//���ڻ���8������
reg     [15:0]  data_add            ;//�ӷ�����



//main code
//���ݻ���
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
    else if(data_flag) begin//��λ�Ĵ�
        data_reg[0]<=data_bin;
        data_reg[1]<=data_reg[0];
        data_reg[2]<=data_reg[1];
        data_reg[3]<=data_reg[2];
        data_reg[4]<=data_reg[3];
        data_reg[5]<=data_reg[4];
        data_reg[6]<=data_reg[5];
        data_reg[7]<=data_reg[6];
    end
    //else begin //����
    
    //end
end

//data_add:�ӷ�
always @(posedge sys_clk or negedge sys_rst_n) begin
    if(!sys_rst_n)
        data_add<=16'd0;
    else
        data_add<=data_reg[0]+data_reg[1]+data_reg[2]+data_reg[3]+data_reg[4]+data_reg[5]+data_reg[6]+data_reg[7];
end

//��ֵ�˲�����������
assign data_ave = data_add[15:3] ;//�ѵ�3λ�ضϣ���Ч�ڳ���8

endmodule 
