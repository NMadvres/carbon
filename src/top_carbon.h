#ifndef __TOP_CARBON_H__
#define __TOP_CARBON_H__
#include "comm_def.h"
#include"mod_ing.h"
#include"mod_sch.h"
#include"mod_pe.h"
#include"mod_egr.h"
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
        mod_ing  *ing_mod;
        mod_sch  *sch_mod;
        mod_pe   *pe_mod;
        mod_egr  *egr_mod;
        array<sc_in<s_pkt_desc> *, g_inter_num>   in_ing_port;  //用于透传连线ing的输入端口信号,在顶层连接发包器
        array<sc_out<s_pkt_desc> *,g_inter_num>  out_egr_port; //用于透传连线egr的输出端口信号,在顶层连接stat
        sc_signal<s_pkt_desc>         ing_sch_sig;  //用于连接ing和sch  
        sc_signal<s_pkt_desc>         sch_pe_sig;  //用于连接ing和sch  
        sc_signal<s_pkt_desc>         pe_egr_sig;  //用于连接ing和sch           
  
    public: //例化及互联部分
        top_carbon(sc_module_name name):sc_module(name)
        {
            //例化
            ing_mod =new mod_ing("mod_ing");
            sch_mod =new mod_sch("mod_sch");
            pe_mod = new mod_pe("mod_pe");
            egr_mod= new mod_egr("mod_egr");
            
            for(int i =0; i < g_inter_num; i++)
            {
                in_ing_port[i] = new sc_in<s_pkt_desc>(); 
                out_egr_port[i] = new sc_out<s_pkt_desc>(); 
                
            }

            //互联
            ing_mod->out_cell_que(ing_sch_sig);
            sch_mod->in_cell_que(ing_sch_sig); //绑定ing和sch

            sch_mod->out_cell_que(sch_pe_sig);
            pe_mod->in_cell_que(sch_pe_sig);  //绑定sch和pe

            pe_mod->out_cell_que(pe_egr_sig);
            egr_mod->in_port(pe_egr_sig);     //绑定pe和egr

            //ing的入口和egr的出口，连线透传到顶层，待更高层进行连接
            for(int i =0; i < g_inter_num; i++)
            {
                ing_mod->in_port[i]->bind(*in_ing_port[i]);
                egr_mod->out_port[i]->bind(*out_egr_port[i]);
            }

        }
        SC_HAS_PROCESS(mod_ing);

};
#endif