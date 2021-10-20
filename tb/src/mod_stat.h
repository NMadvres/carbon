///////////////////////////////////////////////////////////
//Project: Carbon
//Description: SystemC 虚拟项目1
//Module: mod_stat.h
//Description:  收包模块
//Group: Nebula-Matrix 预研组
//Author: Jeff/Shawn
//Hierarchy：内部无嵌套模块的功能模块
////////////////////////////////////////////////////////////

//1.防重定义
#ifndef __MOD_STAT_H__
#define __MOD_STAT_H__

//2.include工具库
#include "comm_def.h"
  
//using namespace std;   
class mod_stat: public sc_module
{

//3.端口声明
  public: 
    sc_in<int> in_clk_cnt;
    //sc_in<tb_cfg> tb_cfg_in; 
    //sc_in<pkt_desc> pkt_stat_in;  //sample 1
    std::array< sc_in<pkt_desc>, G_INTEL_NUM > in_pkt_stat;  

//4.信号声明
    //std::array< pkt_desc, NO_PORTS > pkt_stat_buf; 

//5.模块声明
    mod_stat(sc_module_name name); 
    SC_HAS_PROCESS(mod_stat);

//6.函数声明    
    void pkt_dump();

};

mod_stat::
mod_stat(sc_module_name name):sc_module(name){
    SC_METHOD(pkt_dump);
    for(int i=0; i < G_INTEL_NUM; i++){
        sensitive<< in_pkt_stat[i];
    }
}

void mod_stat::
pkt_dump() {
    for(int i=0; i < G_INTEL_NUM; i++){
        if ((in_pkt_stat[i].read().fsn >= 0) && 
           in_pkt_stat[i].event()){
            cout << "@"<< in_clk_cnt <<"_clks stat rcv  <=:"
                 << in_pkt_stat[i];
        }
    }
}

#endif //__MOD_STAT_H__