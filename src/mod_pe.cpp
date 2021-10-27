#include "mod_pe.h"

////////////////////////////////////////////////////////
// Project： SystemC虚拟项目
// Module:   mod_pe
// Description: mod_pe PE模块cpp文件
// Group：预研组
// Author: Newton
// Date: 2021.10.14 第一版
// Hierarchy : 编号，索引公共库
////////////////////////////////////////////////////////
mod_pe::mod_pe(sc_module_name name):
    sc_module(name),
    clk_gap(G_FREQ_MHZ / G_PE_MPPS),
    clk_wait(0),
    is_busy(false)
{
    assert(clk_gap > 0);

    SC_METHOD(on_recv_cell);
    sensitive << in_cell_que;
    dont_initialize();

    SC_METHOD(on_send_pkt);
    sensitive << in_clk_cnt;
    dont_initialize();
}

void mod_pe::on_recv_cell()
{
    const s_pkt_desc &cell = in_cell_que.read();

    if (cell.type != DESC_TYPE_CELL) {
        fprintf(stderr, "%s:%d: cell type error\n", __FUNCTION__, __LINE__);
        return;
    }

    // TODO add config
    if (pkt_que.size() == 2) {
        fprintf(stderr, "%s:%d: pe queue full, drop it\n", __FUNCTION__, __LINE__);
        if (!is_busy) {
            out_pe_busy.write(1);
            is_busy = true;
        }
        return;
    }

    // no pending packet
    if (pkt_que.empty() || pkt_que.back().type == DESC_TYPE_PKT) {
        if (!cell.sop) {
            fprintf(stderr, "%s:%d: sop error\n", __FUNCTION__, __LINE__);
            return;
        }

        pkt_que.push_back(cell);
        pkt_que.back().len = 0;
    }

    s_pkt_desc &pkt = pkt_que.back();

    // append cell data here
    pkt.len += cell.vldl;

    if (cell.eop) {
        pkt.type = DESC_TYPE_PKT;
        pkt.vldl = -1;
        pkt.csn = -1;
        pkt.sop = -1;
        pkt.eop = -1;
    }
}

void mod_pe::on_send_pkt()
{
    if (!clk_wait) {
        clk_wait--;
        return;
    }

    if (pkt_que.empty())
        return;

    s_pkt_desc &pkt = pkt_que.front();

    if (g_flow_rule_tab.size() < (unsigned int)pkt.fid) {
        fprintf(stderr, "%s:%d: fid error\n", __FUNCTION__, __LINE__);
        pkt_que.pop_front();
        return;
    }

    // edit packet
    pkt.len += g_flow_rule_tab[pkt.fid].len2add;

    out_cell_que.write(pkt);
    pkt_que.pop_front();

    clk_wait = clk_gap - 1;

    if (is_busy)
        out_pe_busy.write(0);
}
