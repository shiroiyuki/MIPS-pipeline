#include<stdio.h>
#include<string.h>
#include<cstdlib>
#include "pipeline_struct.h"
/********
32-bit MIPS Instruction
R-type =opcode(6) rs(5) rt(5) rd(5) shamt(5) funct(6)
I-type =opcode(6) rs(5) rd(5) imm(16)
J-type =opcode(6) pseudo-address(26)
opcode:
beq   =000100(0x04)
addi  =001000(0x08)
ori   =001101(0x0D)
lw    =100011(0x23)
R-type=000000(0x00)
function:
add   =100000(0x20)
sub   =100010(0x22)
or    =100101(0x25)
and   =100100(0x24)
*********/
int dataopen(const char *);// open txt file
void dataout(const char *,int); //ouput txt file
void Show(int*,int*);//print each stage register and memory
/*** pipeline five stage ***/
void WB(int*);
void MEM(int*);
void EX(int*);
void ID(int*,int);
void IF();
/*** run pipeline ***/
void MIPS_pipeLine(int);
/*******     Memory_name     *******/
const char mem_name[5][3]= {"00","04","08","12","16"};
/*******     File_input_name     *******/
const char general_in[]="General.txt",data_in[]="Datahazard.txt",load_in[]="Lwhazard.txt",branch_in[]="branchazard.txt";
/*******     File_output_name     *******/
const char general_out[]="genResult.txt",data_out[]="dataResult.txt",load_out[]="loadResult.txt",branch_out[]="branchResult.txt";
/*******     IF/ID      *******/
IF_ID II;
/*******     ID/EX      *******/
ID_EX IE;
/*******     EX/MEM     *******/
EX_MEM EM;
/*******     MEM_WB     *******/
MEM_WB MW;
/*******     Forwarding *******/
Forwarding FW;
/*******     Stalling   *******/
Stalling SL;
/*******     Read MIPS INSTRUCTION from Data     *******/
FILE *fp,*ft;
/*******     store Instruction     *******/
char instruction[20][33]= {};
/******* Instruction type *******/
int func;
/*******   start program  *******/
int main()
{
    int line;
    /*** General.txt ***/
    line=dataopen(general_in);
    dataout(general_out,line);
    /*** Datahazard.txt ***/
    line=dataopen(data_in);
    dataout(data_out,line);
    /*** Lwhazard.txt ***/
    line=dataopen(load_in);
    dataout(load_out,line);
    /*** branchazard.txt ***/
    line=dataopen(branch_in);
    dataout(branch_out,line);
    system("pause");
}
void MIPS_pipeLine(int line)
{
    /*** Initialize ****/
    II.reset();
    IE.reset();
    EM.reset();
    MW.reset();
    FW.reset();
    SL.reset();
    int reg[9]= {0,8,7,6,3,9,5,2,7};
    int mem[5]= {5,5,6,8,8};
    int cc=1;
    func=0;
    /*** pipeline ***/
    while(II.pc<4*(line+4))
    {
        fprintf(ft,"CC %d:\n",cc);
        WB(reg);
        MEM(mem);
        EX(reg);
        if((II.pc/4)<=line&&II.pc>0)
            ID(reg,(II.pc-4)/4);
        else
            IE.reset();
        IF();
        Show(reg,mem);
        cc++;
    }
    fclose(ft);
}
void IF()
{
    for(int i=0; i<32; i++)
        II.instruction[i]=instruction[II.pc/4][i];
    II.pc+=4;
}
void ID(int *reg,int line)
{
    int rt,rs;
    int num=0;
    char opcode[7],funct[7],temp[32];
    for(int i=6; i<11; i++)/*** rs ***/
    {
        temp[i]=II.instruction[i];
        num+=pow(2,10-i)*(int(temp[i])-48);
    }
    rs=num;
    SL.IF_Rs=num;
    num=0;
    for(int i=11; i<16; i++)/*** rt ***/
    {
        temp[i]=II.instruction[i];
        num+=pow(2,15-i)*(int(temp[i])-48);
    }
    rt=num;
    SL.IF_Rt=num;
    num=0;
    /***Stalling***/
    if ((IE.Control_signals[5]=='1')&&((SL.IE_Rt==SL.IF_Rs)||(SL.IE_Rt==SL.IF_Rt)))
        II.pc-=4;
    /**************/
    IE.Rs=rs;
    IE.Rt=rt;
    IE.pc=II.pc;
    for(int i=16; i<21; i++)/*** rd ***/
    {
        temp[i]=instruction[line][i];
        num+=pow(2,20-i)*(int(temp[i])-48);
    }
    IE.Rd=num;
    num=0;
    for(int i=16; i<32; i++)/*** imm ***/
    {
        temp[i]=II.instruction[i];
        num+=pow(2,31-i)*(int(temp[i])-48);
    }
    IE.imm=num;
    num=0;
    for(int i=0; i<6; i++)/*** opcode ***/
        opcode[i]=II.instruction[i];
    if(opcode[0]=='1')/**lw**/
    {
        func=1;
        strcpy(IE.Control_signals, "000101011");
    }
    else if((opcode[2]=='0')&&(opcode[3]=='1'))/**beq**/
    {
        func=2;
        strcpy(IE.Control_signals, "001010000");
    }
    else if((opcode[2]=='1')&&(opcode[3]=='0'))/**addi**/
    {
        func=3;
        strcpy(IE.Control_signals, "000100010");
    }
    else if((opcode[2]=='1')&&(opcode[3]=='1'))/**ori**/
    {
        func=4;
        strcpy(IE.Control_signals, "011000010");
    }
    else
    {
        for(int i=26; i<32; i++)/*** funct ***/
            funct[i-26]=instruction[line][i];
        if((funct[3]=='1')&&(funct[5]=='1'))
            func=5;/**or**/
        else if(funct[4]=='1')
            func=6;/**sub**/
        else if((funct[3]=='1')&&(funct[5]=='0'))
            func=7;/**and**/
        else
            func=8;/**add**/
        IE.imm=0;
        strcpy(IE.Control_signals, "110000010");
        if((funct[0]=='0')&&(funct[1]=='0')&&(funct[2]=='0')&&(funct[3]=='0')&&(funct[4]=='0')&&(funct[5]=='0'))
        strcpy(IE.Control_signals, "000000000");/*** no instruction***/
    }
    IE.data1=reg[IE.Rs];
    IE.data2=reg[IE.Rt];
    /*** Stalling ***/
    if ((IE.Control_signals[5]=='1')&&((SL.IE_Rt==SL.IF_Rs)||(SL.IE_Rt==SL.IF_Rt)))
        strcpy(IE.Control_signals,"000000000");
    /****************/
}
void EX(int *reg)
{
    SL.IE_Rt=IE.Rt;
    FW.IE_Rs=IE.Rs;
    FW.IE_Rt=IE.Rt;
    int ALUin1=IE.data1,ALUin2=IE.data2;
    /*** Data Hazard ***/
    if ((EM.Control_signals[3]!='0')&&(FW.EM_DEST!=0)&&(FW.EM_DEST==FW.IE_Rs))
        ALUin1=EM.ALUout;
    else if ((MW.Control_signals[0]!='0')&&(FW.MW_DEST!=0)&&(FW.MW_DEST==FW.IE_Rs)&&(FW.EM_DEST!=FW.IE_Rs))
        ALUin1=MW.temp;
    if ((EM.Control_signals[3]!='0')&&(FW.EM_DEST!=0)&&(FW.EM_DEST==FW.IE_Rt))
        ALUin2=EM.ALUout;
    else if ((MW.Control_signals[0]!='0')&&(FW.MW_DEST!=0)&&(FW.MW_DEST==FW.IE_Rt)&&(FW.EM_DEST!=FW.IE_Rt))
        ALUin2=MW.temp;
    /*******************/
    EM.WriteData=IE.data2;
    EM.dest=IE.Rt;
    EM.pc=IE.pc+IE.imm*4;
    strcpy(EM.destname, "Rt");
    for(int i=0; i<5; i++)
        EM.Control_signals[i]=IE.Control_signals[i+4];
    if(IE.Control_signals[1]=='1'&&IE.Control_signals[2]=='0')
    {
        if(func==5)
            EM.ALUout=ALUin1|ALUin2;/**or**/
        else if(func==6)
            EM.ALUout=ALUin1-ALUin2;/**sub**/
        else if(func==7)
            EM.ALUout=ALUin1&ALUin2;/**and**/
        else if(func==8)
            EM.ALUout=ALUin1+ALUin2;/**add**/
        EM.dest=IE.Rd;
        strcpy(EM.destname, "Rd");
    }
    else if(IE.Control_signals[1]=='0'&&IE.Control_signals[2]=='1')/**beq**/
        EM.ALUout=ALUin1-ALUin2;
    else if(IE.Control_signals[1]=='1'&&IE.Control_signals[2]=='1')/**ori**/
        EM.ALUout=ALUin1|IE.imm;
    else/**lw addi**/
        EM.ALUout=ALUin1+IE.imm;
}
void MEM(int *mem)
{
    FW.EM_DEST=EM.dest;
    for(int i=0; i<2; i++)
        MW.Control_signals[i]=EM.Control_signals[i+3];
    MW.ALUout=EM.ALUout;
    MW.ReadData=0;
    MW.DEST=EM.dest;
    if(EM.Control_signals[1]=='1')/** lw **/
    {
        int n=EM.ALUout/4;
        MW.ReadData=mem[n];
    }
    if((EM.Control_signals[0]=='1')&&(EM.ALUout==0))/** beq **/
    {
        II.reset();
        IE.reset();
        II.pc=EM.pc;
    }
}
void WB(int *reg)
{
    FW.MW_DEST=MW.DEST;
    MW.temp;
    if(MW.Control_signals[0]=='1' && MW.Control_signals[1]=='0' )/** write register **/
        MW.temp=MW.ALUout;
    else if(MW.Control_signals[0]=='1' && MW.Control_signals[1]=='1' )/** write register from memory**/
        MW.temp=MW.ReadData;
    else/** do nothing **/
        MW.temp=reg[MW.DEST];
    reg[MW.DEST]=MW.temp;
}
void Show(int *reg,int *mem)
{
    fprintf(ft,"\nRegister:\n");
    for(int i=0; i<9; i+=3)
        fprintf(ft,"$%d: %d\t$%d: %d\t$%d: %d\n",i,reg[i],i+1,reg[i+1],i+2,reg[i+2]);
    fprintf(ft,"\nData memory:\n");
    for(int i=0; i<5; i++)
        fprintf(ft,"%s:\t%d\n",mem_name[i],mem[i]);
    /**IF/ID**/
    fprintf(ft,"\nIF/ID :\nPC\t\t%d\nInstruction\t%s\n",II.pc,II.instruction);
    /**ID/EX**/
    fprintf(ft,"\nID/EX :\nReadData1\t%d\nReadData2\t%d\nsign_ext	%d\n",IE.data1,IE.data2,IE.imm);
    fprintf(ft,"Rs\t\t%d\nRt\t\t%d\nRd\t\t%d\nControl signals %s\n",IE.Rs,IE.Rt,IE.Rd,IE.Control_signals);
    /**EX/MEM**/
    fprintf(ft,"\nEX/MEM :\nALUout\t\t%d\nWriteData\t%d\n%s\t\t%d\nControl signals	%s\n",EM.ALUout,EM.WriteData,EM.destname,EM.dest,EM.Control_signals);
    /**EX/MEM**/
    fprintf(ft,"\nMEM/WB :\nReadData\t%d\nALUout\t\t%d\nControl signals	%s\n",MW.ReadData,MW.ALUout,MW.Control_signals);
    fprintf(ft,"=================================================================\n");
}
void dataout(const char*datapath,int line)
{
    ft=fopen(datapath,"w");
    MIPS_pipeLine(line);
    fclose(ft);
}
int dataopen(const char *datapath)
{
    int line=0;
    char content[33];
    if((fp=fopen(datapath,"r")) != NULL)
    {
        while(!feof(fp))
        {
            fscanf(fp,"%s",&content);
            for(int i=0; i<32; i++)
                instruction[line][i]=content[i];
            line++;
        }
        line-=1;
        for(int i=line; i<20; i++)
            memset(instruction[i],'0',32);
        fclose(fp);
        return line;/*** Instruction count ***/
    }
    else
        printf("ERROR!");/*** no file ***/
    return 0;
}
