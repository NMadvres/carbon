#include "mod_stim.h"

void port_fifo::pkt_in(const s_pkt_desc &data_pkt)
{
    regs[pntr++] = data_pkt;
    empty = false;
    if (pntr == (int)g_flow_rule_tab.size())
        full = true;
}

s_pkt_desc port_fifo::pkt_out()
{
    s_pkt_desc temp;
    temp = regs[0];
    if (--pntr == 0)
        empty = true;
    else
        for (int i = 0; i < (int)g_flow_rule_tab.size(); i++)
            regs[i] = regs[i + 1];
    return (temp);
}

s_pkt_desc port_fifo::pkt_pre_val()
{
    s_pkt_desc temp;
    temp = regs[0];
    return (temp);
}

void token_bucket::add_token(const int &add_token_val)
{
    if ((token + add_token_val) > TOKEN_MAX_BYTE)
        token = TOKEN_MAX_BYTE;
    else
        token = token + add_token_val;
}

void token_bucket::sub_token(const int &sub_token_val)
{
    if ((token - sub_token_val) < 0)
        token = 0;
    else
        token = token - sub_token_val;
}

int token_bucket::read_token()
{
    int temp;
    temp = token;
    return (temp);
}

void mod_stim ::stim_prc()
{
    int pkt_send_count;
    int send_pkt_port;
    int token_count;
    array<port_fifo, G_INTER_NUM> port_fifo_inst;
    array<token_bucket, G_INTER_NUM> port_token_bucket;
    vector<token_bucket> flow_token_bucket;
    vector<int> flow_sn;
    s_pkt_desc pkt_desc_tmp;

    pkt_send_count = 0;
    token_count = 0;

    flow_token_bucket.resize(g_flow_rule_tab.size());
    flow_sn.resize(g_flow_rule_tab.size());

    for (int i = 0; i < G_INTER_NUM; i++) {
        port_fifo_inst[i].pntr = 0;
        port_fifo_inst[i].full = false;
        port_fifo_inst[i].empty = true;
    }

    for (int i = 0; i < G_INTER_NUM; i++) {
        port_token_bucket[i].token = 0;
    }

    for (int i = 0; i < (int)g_flow_rule_tab.size(); i++) {
        flow_token_bucket[i].token = 0;
    }

    for (int i = 0; i < (int)g_flow_rule_tab.size(); i++) {
        flow_sn[i] = 0;
    }

    wait(1);
    while (1) {
        if (token_count < G_FREQ_MHZ) {
            token_count++;
        } else {
            token_count = 0;
            // add token to port token bucket
            for (int i = 0; i < G_INTER_NUM; i++) {
                port_token_bucket[i].add_token(125); // 1000Mbps=125MBPS
            }
            // add token to flow token bucket
            for (int i = 0; i < (int)g_flow_rule_tab.size(); i++) {
                flow_token_bucket[i].add_token(g_flow_rule_tab[i].flow_speed);
            }
        }

        // generate desc packet per flow
        //        if (pkt_send_count < SEND_FILE_NUM)
        //        {
        for (int fid = 0; fid < (int)g_flow_rule_tab.size(); fid++) {
            send_pkt_port = g_flow_rule_tab[fid].sport;

            pkt_desc_tmp.type = 0;
            pkt_desc_tmp.fid = fid;
            pkt_desc_tmp.sid = g_flow_rule_tab[fid].sid;
            pkt_desc_tmp.did = g_flow_rule_tab[fid].did;
            pkt_desc_tmp.fsn = flow_sn[fid];
            pkt_desc_tmp.len = g_flow_rule_tab[fid].len;
            pkt_desc_tmp.pri = g_flow_rule_tab[fid].pri;
            pkt_desc_tmp.sport = g_flow_rule_tab[fid].sport;
            pkt_desc_tmp.dport = -1;
            pkt_desc_tmp.qid = -1;
            pkt_desc_tmp.vldl = -1;
            pkt_desc_tmp.csn = -1;
            pkt_desc_tmp.sop = false;
            pkt_desc_tmp.eop = false;

            if (flow_token_bucket[fid].read_token() >= pkt_desc_tmp.len) {
                flow_token_bucket[fid].sub_token(pkt_desc_tmp.len);
                if (port_fifo_inst[send_pkt_port].full == true) {
                    flow_dpd_pkts[fid]++;
                    flow_dpd_bytes[fid] = flow_dpd_bytes[fid] + pkt_desc_tmp.len;
                    port_dpd_pkts[send_pkt_port]++;
                    port_dpd_bytes[send_pkt_port] = port_dpd_bytes[send_pkt_port] + pkt_desc_tmp.len;
                } else {
                    port_fifo_inst[send_pkt_port].pkt_in(pkt_desc_tmp);
                    flow_sn[fid] = flow_sn[fid] + 1;
                    flow_sent_pkts[fid]++;
                    flow_sent_bytes[fid] = flow_sent_bytes[fid] + pkt_desc_tmp.len;
                    port_sent_pkts[send_pkt_port]++;
                    port_sent_bytes[send_pkt_port] = port_sent_bytes[send_pkt_port] + pkt_desc_tmp.len;
                }
                pkt_send_count++;
            }
            flow_sent_mbps[fid] = (flow_sent_bytes[fid] * G_FREQ_MHZ) / in_clk_cnt;
            port_sent_mbps[send_pkt_port] = (port_sent_bytes[send_pkt_port] * G_FREQ_MHZ) / in_clk_cnt;
        }
        //        }

        // output desc packet to each port
        for (int send_port = 0; send_port < G_INTER_NUM; send_port++) {
            //output pkt_data
            pkt_desc_tmp = port_fifo_inst[send_port].pkt_pre_val();
            if ((port_fifo_inst[send_port].empty == false)
                && (port_token_bucket[send_port].read_token() >= pkt_desc_tmp.len)) {
                pkt_desc_tmp = port_fifo_inst[send_port].pkt_out();
                port_token_bucket[send_port].sub_token(pkt_desc_tmp.len);
                out_pkt_stim[send_port].write(pkt_desc_tmp);
                cout << "@" << in_clk_cnt << "_clks stim sent =>:"
                     << "sport:" << send_port << pkt_desc_tmp << endl;
                pkt_sender_file << "@" << in_clk_cnt << "_clks stim sent =>:"
                                << "sport:" << send_port << pkt_desc_tmp;
            }
        }

        wait();
    }
}
