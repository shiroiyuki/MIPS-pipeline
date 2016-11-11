#ifndef pipeline_struct_H_INCLUDED
#define pipeLine_struct_H_INCLUDED
#include<stdio.h>
#include<string.h>
#include<math.h>
class IF_ID
{
public:
    int pc;
    char instruction[33];
    IF_ID()
    {
        pc=0;
        memset(instruction,'0',32);
        instruction[32]='\0';
    }
    void reset()
    {
        pc=0;
        memset(instruction,'0',32);
        instruction[32]='\0';
    }
};
class ID_EX
{
public:
    int Rs,Rt,Rd,imm,data1,data2,pc;
    char Control_signals[10];
    ID_EX()
    {
        Rs=0;
        Rt=0;
        Rd=0;
        imm=0;
        data1=0;
        data2=0;
        pc=0;
        memset(Control_signals,'0',9);
        Control_signals[9]='\0';
    }
    void reset()
    {
        Rs=0;
        Rt=0;
        Rd=0;
        imm=0;
        data1=0;
        data2=0;
        pc=0;
        memset(Control_signals,'0',9);
        Control_signals[9]='\0';
    }
};
class EX_MEM
{
public:
    int ALUout,WriteData,dest,pc;
    char Control_signals[6],destname[3];
    EX_MEM()
    {
        ALUout=0;
        WriteData=0;
        dest=0;
        pc=0;
        memset(Control_signals,'0',5);
        strcpy(destname,"Rt");
        Control_signals[5]='\0';
    }
    void reset()
    {
        ALUout=0;
        WriteData=0;
        dest=0;
        pc=0;
        memset(Control_signals,'0',5);
        strcpy(destname,"Rt");
        Control_signals[5]='\0';
    }
};
class MEM_WB
{
public:
    int ALUout,ReadData,DEST,temp;
    char Control_signals[3];
    MEM_WB()
    {
        temp=0;
        ALUout=0;
        ReadData=0;
        DEST=0;
        memset(Control_signals,'0',2);
        Control_signals[2]='\0';
    }
    void reset()
    {
        temp=0;
        ALUout=0;
        ReadData=0;
        DEST=0;
        memset(Control_signals,'0',2);
        Control_signals[2]='\0';
    }
};
class Forwarding
{
public:
    int IE_Rs,IE_Rt,EM_DEST,MW_DEST;
    Forwarding()
    {
        IE_Rs=0;
        IE_Rt=0;
        EM_DEST=0;
        MW_DEST=0;
    }
    void reset()
    {
        IE_Rs=0;
        IE_Rt=0;
        EM_DEST=0;
        MW_DEST=0;
    }
};
class Stalling
{
public:
    int IE_Rt,IF_Rs,IF_Rt;
    Stalling()
    {
        IE_Rt=0;
        IF_Rs=0;
        IF_Rt=0;
    }
    void reset()
    {
        IE_Rt=0;
        IF_Rs=0;
        IF_Rt=0;
    }
};
#endif
