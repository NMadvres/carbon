#ifndef __TOP_CARBON_H__
#define __TOP_CARBON_H__

#include "comm_def.h"
#include "mod_ing.h"
#include "mod_sch.h"
#include "mod_pe.h"
#include "mod_egr.h"

////////////////////////////////////////////////////////
// Project： SystemC虚拟项目
// Module:   top_carbon
// Description: top_carbon 顶层互联
// Group：预研组
// Author: Newton
// Date: 2021.10.14 第一版
// Hierarchy : 编号，索引公共库
////////////////////////////////////////////////////////
class top_carbon: public sc_module
{
public:
    mod_ing *ing_mod;
    mod_sch *sch_mod;
    mod_pe *pe_mod;
    mod_egr *egr_mod;
    std::array<sc_in<s_pkt_desc> *, G_INTER_NUM> in_ing_port;   // 用于透传连线ing的输入端口信号,在顶层连接发包器
    std::array<sc_out<s_pkt_desc> *, G_INTER_NUM> out_egr_port; // 用于透传连线egr的输出端口信号,在顶层连接stat
    sc_fifo<s_pkt_desc> *ing_sch_sig;                           // 用于连接ing和sch
    sc_fifo<s_pkt_desc> *sch_pe_sig;                            // 用于连接ing和sch
    sc_fifo<s_pkt_desc> *ing_egr_bcpu_sig;                      // 用于连接ing和egr
    sc_signal<s_pkt_desc> pe_egr_sig;                           // 用于连接ing和sch
    sc_signal<int> pe_sch_fc_sig;                               // 用于连接pe和sch,反压信号
    sc_in<int> in_clk_cnt;                                      // 全局时钟计数，用于互联

public: // 例化及互联部分
    top_carbon(sc_module_name name, func_stat *base_top_stat):
        sc_module(name)
    {
        // 例化
        ing_mod = new mod_ing("mod_ing");
        sch_mod = new mod_sch("mod_sch");
        pe_mod = new mod_pe("mod_pe");
        egr_mod = new mod_egr("mod_egr", base_top_stat);

        for (int i = 0; i < G_INTER_NUM; i++) {
            in_ing_port[i] = new sc_in<s_pkt_desc>();
            out_egr_port[i] = new sc_out<s_pkt_desc>();
        }

        // 互联
        ing_sch_sig = new sc_fifo<s_pkt_desc>(256);
        ing_mod->out_cell_que(*ing_sch_sig);
        sch_mod->in_cell_que(*ing_sch_sig); // 绑定ing和sch

        ing_egr_bcpu_sig = new sc_fifo<s_pkt_desc>(256);
        ing_mod->out_pkt_bcpu(*ing_egr_bcpu_sig); // 绑定ing和egr
        egr_mod->in_pkt_bcpu(*ing_egr_bcpu_sig);  // 绑定ing和egr

        sch_pe_sig = new sc_fifo<s_pkt_desc>(256);
        sch_mod->out_cell_que(*sch_pe_sig);
        pe_mod->in_cell_que(*sch_pe_sig); // 绑定sch和pe

        pe_mod->out_cell_que(pe_egr_sig);
        egr_mod->in_port(pe_egr_sig); // 绑定pe和egr

        pe_mod->out_pe_busy(pe_sch_fc_sig); // 绑定pe和egrs
        sch_mod->in_fc_port(pe_sch_fc_sig);

        // ing的入口和egr的出口，连线透传到顶层，待更高层进行连接
        for (int i = 0; i < G_INTER_NUM; i++) {
            ing_mod->in_port[i]->bind(*in_ing_port[i]);
            egr_mod->out_port[i]->bind(*out_egr_port[i]);
        }
        // 全局cnt互联
        ing_mod->in_clk_cnt(in_clk_cnt);
        sch_mod->in_clk_cnt(in_clk_cnt);
        pe_mod->in_clk_cnt(in_clk_cnt);
        egr_mod->in_clk_cnt(in_clk_cnt);
    }
    SC_HAS_PROCESS(top_carbon);
};
#endif
