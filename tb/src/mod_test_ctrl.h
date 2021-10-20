///////////////////////////////////////////////////////////
//Project: Carbon
//Description: SystemC 虚拟项目1
//Module: function_module.h
//Description:  底层功能模块
//Group: Nebula-Matrix 预研组
//Author: Destin/Shawn
//Hierarchy：内部无嵌套模块的功能模块
////////////////////////////////////////////////////////////

//1.防重定义
#ifndef __MOD_TEST_CTRL_H__
#define __MOD_TEST_CTRL_H__

//2.include工具库
#include "comm_def.h"

//using namespace std;   
class mod_test_ctrl: public sc_module
{

//3.端口声明
  public: 
    sc_in_clk             in_clk;
    sc_out<int>      out_clk_cnt;
//    sc_out<dut_cfg>  dut_cfg_out;  
//    sc_out< tb_cfg>   tb_cfg_out;  

//4.信号声明
    int       clk_cnt_tmp ;  
//    sc_signal<dut_cfg>  dut_cfg_sig;  
//    sc_signal< tb_cfg>   tb_cfg_sig;  

//5.模块声明
    mod_test_ctrl(sc_module_name name); 
    SC_HAS_PROCESS(mod_test_ctrl);

//6.函数声明    
    void clk_count();    

};

mod_test_ctrl::
mod_test_ctrl(sc_module_name name):sc_module(name)
{
    //    dut_cfg_sig = f(配置文件)；
    //    tb_cfg_sig  = f(配置文件)；
    //    dut_cfg_out = dut_cfg_sig；
    //    tb_cfg_out  = tb_cfg_sig；
    cout<<"读取文件预留位置.\n";

    SC_THREAD(clk_count);
    sensitive << in_clk.pos();
}


void mod_test_ctrl::
clk_count() {
    clk_cnt_tmp=0;
    wait();
    wait();
    wait();
    while(true){
        clk_cnt_tmp++;
        out_clk_cnt = clk_cnt_tmp;
        wait();
    }
}


#endif  //__MOD_TEST_CTRL_H__