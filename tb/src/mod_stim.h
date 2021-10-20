///////////////////////////////////////////////////////////
//Project: Carbon
//Description: SystemC 虚拟项目1
//Module: stim_mod.h
//Description:  发包模块
//Group: Nebula-Matrix 预研组
//Author: Shawn
//Hierarchy：内部无嵌套模块的功能模块
////////////////////////////////////////////////////////////

//1.防重定义
#ifndef __MOD_STIM_H__
#define __MOD_STIM_H__

//2.include工具库
#include "comm_def.h"

//using namespace std;   
class mod_stim: public sc_module
{

//3.端口声明
  public: 
    sc_in<int> in_clk_cnt;
//    sc_in<tb_cfg> tb_cfg_in; 
//    sc_out<pkt_desc> pkt_stim_out;  //sample 1个
    std::array<sc_out<pkt_desc>, G_INTEL_NUM > out_pkt_stim;  
    
//4.信号声明
    // pkt_desc pkt_desc_tmp;  
    std::array<pkt_desc, G_INTEL_NUM > pkt_desc_tmp;  
    std::array<pkt_desc, G_INTEL_NUM > pkt_desc_buf;

//5.模块声明
    mod_stim(sc_module_name name); 
    SC_HAS_PROCESS(mod_stim);

//6.函数声明 
    void pkt_gen();
    //void function_module_action2();

};

mod_stim::
mod_stim(sc_module_name name):sc_module(name)
{
    for(int i=0; i < G_INTEL_NUM; i++){
        pkt_desc_tmp[i].sid = i;
        pkt_desc_tmp[i].did = NO_PORTS-1-i;
        pkt_desc_tmp[i].fsn = 0;
        pkt_desc_tmp[i].len = 64;
        pkt_desc_tmp[i].pri = 0;
        pkt_desc_tmp[i].fid = i;
    }
    /*
        pkt_desc_tmp.sid = 0;//sample 1
        pkt_desc_tmp.did = NO_PORTS-1;//sample 1
        pkt_desc_tmp.fsn = 0;//sample 1
        pkt_desc_tmp.len = 64;//sample 1
        pkt_desc_tmp.pri = 0;//sample 1
        pkt_desc_tmp.fid = 0; //sample 1*/

    SC_METHOD(pkt_gen);
    sensitive << in_clk_cnt;
 //   SC_THREAD(function_module_action2);
 //   sensitive << 全局时钟计数器1;
}

void mod_stim::
pkt_gen() {
    for(int i=0; i < G_INTEL_NUM; i++){
        if (!(pkt_desc_buf[i]==pkt_desc_tmp[i])) {
            cout << "@" << in_clk_cnt << "_clks stim sent =>:"
                 << pkt_desc_tmp[i];
            pkt_desc_buf[i] = pkt_desc_tmp[i];
            out_pkt_stim[i] = pkt_desc_buf[i];
        }
    }
    pkt_desc_tmp[in_clk_cnt % G_INTEL_NUM].fsn++;
    //pkt_desc_tmp.fsn++; //sample 1
}
/*
void stim_mod::
function_module_action2() {
    while(true){
        变量2=f(变量1,信号1，报文输入口1,配置输入口1);
        信号2=f(变量1,变量2，信号1，报文输入口1,配置输入口1);
        报文输出口1=信号2;
        wait();
    }
}*/


#endif   //__MOD_STIM_H__