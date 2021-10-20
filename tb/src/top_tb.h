///////////////////////////////////////////////////////////
//Project: Carbon
//Description: SystemC 虚拟项目1
//Module: tb_top.h
//Description: 测试台 top 文件，实例化各测试模块
//Group: Nebula-Matrix 预研组
//Author: Shawn
//Hierarchy：
//             tb_top __:stim
//                    |_:stat
//                    |_:test_ctrl
////////////////////////////////////////////////////////////

//1.防重定义
#ifndef __TOP_TB_H__
#define __TOP_TB_H__

//2.include工具库
#include "comm_def.h"      //通用定义

//3.include子模块库
#include "mod_stim.h"      //激励模块
#include "mod_stat.h"      //统计模块
#include "mod_test_ctrl.h" //测试控制模块

//using namespace std;   
class top_tb: public sc_module
{

//4.端口声明
  public: 
    sc_in_clk in_clk;                   //全局时钟输入
//    sc_out<int> clk_cnt_out;          //全局时钟计数输出，由test_ctrl根据全局时钟输入产生
//    sc_out<dut_cfg> dut_cfg_out;      //由test_ctrl读取文件产生
//    sc_in<pkt_desc> pkt_stat_in;      //sample 1个
//    sc_out<pkt_desc> pkt_stim_out;    //sample 1个
    std::array<  sc_in<pkt_desc>, G_INTEL_NUM > in_pkt_stat;   //由stat模块接收的报文
    std::array< sc_out<pkt_desc>, G_INTEL_NUM > out_pkt_stim;  //由stim产生的激励报文

//5.信号声明
    sc_signal<int> sig_gclk_cnt;           //由test_ctrl产生，需要输出给外部和tb_top内部其他子模块
//    sc_signal<tb_cfg> tb_cfg_sig;      //由test_ctrl读取文件产生，输送给其他的testbench内部模块

//5.模块声明
    //tb_top(sc_module_name name); 
    //SC_HAS_PROCESS(tb_top);
//6.函数声明
//    void tb_top_process()

//7.子模块声明

    mod_stim *stim_inst;
    mod_stat *stat_inst;
    mod_test_ctrl *test_ctrl_inst;

    SC_CTOR(top_tb){
    //8.子模块例化    
        stim_inst = new mod_stim("stim_inst");
        stat_inst = new mod_stat("stat_inst");
        test_ctrl_inst = new mod_test_ctrl("test_ctrl_inst");
    //stim_mod stim_inst("stim_init");
    //stat_mod stat_inst("stat_inst");
    //test_ctrl_mod test_ctrl_inst("test_ctrl_inst");
    
    //9.信号连接 
        stim_inst->in_clk_cnt(sig_gclk_cnt);
        //stim_inst->tb_cfg_in(tb_cfg_sig);
        for (int i=0;i<G_INTEL_NUM;i++){
            stim_inst->out_pkt_stim[i](out_pkt_stim[i]);
        }
        stat_inst->in_clk_cnt(sig_gclk_cnt);
        //stat_inst->tb_cfg_in(tb_cfg_sig);
        for (int i=0;i<G_INTEL_NUM;i++){
            stat_inst->in_pkt_stat[i](in_pkt_stat[i]);
        }
        test_ctrl_inst->in_clk(in_clk);
        test_ctrl_inst->out_clk_cnt(sig_gclk_cnt);
        //test_ctrl_inst->dut_cfg_out(dut_cfg_out);
        //test_ctrl_inst->tb_cfg_out(tb_cfg_sig);
        //clk_cnt_out = gclk_cnt;
   };
};

#endif  //__TOP_TB_H__