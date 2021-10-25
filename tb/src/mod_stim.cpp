

#include <stdlib.h>
#include <time.h>
#include "mod_stim.h"
#include "comm_def.h"

void port_fifo::pkt_in(const s_pkt_desc& data_pkt)
{
    regs[pntr++] = data_pkt;
    empty = false;
    if (pntr == FLOW_RULE_TAB_SIZE)
        full = true;      
}

s_pkt_desc port_fifo::pkt_out()
{
    s_pkt_desc temp;
    temp = regs[0];
    if (--pntr == 0) 
        empty = true;
    else
        for(int i=0; i<FLOW_RULE_TAB_SIZE; i++)
            regs[i] = regs[i+1];
  return(temp);  
}

s_pkt_desc port_fifo::pkt_pre_val()
{
    s_pkt_desc temp;
    temp = regs[0];
  return(temp);  
}

void token_bucket::add_token(const int& add_token_val)
{
    token = token + add_token_val;
}

void token_bucket::sub_token(const int& sub_token_val)
{
    token = token - sub_token_val;
}

int token_bucket::read_token()
{
    int temp;
    temp = token;
    return(temp);
}

void mod_stim :: stim_prc()
{
    int pkt_send_count;
    int send_pkt_port;
    int drop_count;
    int token_count;
    array<port_fifo,G_INTER_NUM> port_fifo_inst;
    array<token_bucket,G_INTER_NUM> port_token_bucket;
    array<token_bucket,FLOW_RULE_TAB_SIZE> flow_token_bucket;
    array<int,FLOW_RULE_TAB_SIZE> flow_sn;

    for(int i=0; i<G_INTER_NUM; i++)
    {
        port_fifo_inst[i].pntr = 0;
        port_fifo_inst[i].full = false;
        port_fifo_inst[i].empty = true;
    }

    for(int i=0; i<G_INTER_NUM; i++)
    {
        port_token_bucket[i].token = 0;
    }

    for(int i=0; i<FLOW_RULE_TAB_SIZE; i++)
    {
        flow_token_bucket[i].token = 0;
    }

    for(int i=0; i<FLOW_RULE_TAB_SIZE; i++)
    {
        flow_sn[i] = 0;
    }

 
    ofstream pkt_sender_file;
//    pkt_sender_file.open("pkt_sender_file.log");
    pkt_sender_file.open(pkt_sender_filename);

    pkt_send_count = 0;
    token_count = 0;

    srand((unsigned)time(NULL));
    wait(8);
    while(1)
//    while(pkt_send_count < SEND_FILE_CYCLE)
    {
        if(token_count < G_FREQ)
        {
            token_count++;
        }
        else
        {
            token_count = 0;
            // add token to port token bucket
            for(int i=0; i<G_INTER_NUM; i++)
            {
                port_token_bucket[i].add_token(125);   // 1000Mbps=125MBPS
            }
            // add token to flow token bucket
            for(int i=0; i<FLOW_RULE_TAB_SIZE; i++)
            {
                flow_token_bucket[i].add_token(g_flow_rule_tab[i].flow_speed);
            }
        }

        // generate desc packet per flow
        if (pkt_send_count < SEND_FILE_NUM)
        {
            for(int fid=0; fid < FLOW_RULE_TAB_SIZE; fid++)
            {
                send_pkt_port = g_flow_rule_tab[fid].sport;

                pkt_desc_tmp.type = 0;
                pkt_desc_tmp.fid  = fid;
                pkt_desc_tmp.sid  = g_flow_rule_tab[fid].sid;
                pkt_desc_tmp.did  = g_flow_rule_tab[fid].did;
                pkt_desc_tmp.fsn  = flow_sn[fid];
                pkt_desc_tmp.len  = g_flow_rule_tab[fid].len;
                pkt_desc_tmp.pri  = g_flow_rule_tab[fid].pri;
                pkt_desc_tmp.sport= g_flow_rule_tab[fid].sport;
                pkt_desc_tmp.dport= -1;
                pkt_desc_tmp.qid  = -1;
                pkt_desc_tmp.vldl = -1;
                pkt_desc_tmp.csn  = -1;
                pkt_desc_tmp.sop  = false;
                pkt_desc_tmp.eop  = false;

                if(flow_token_bucket[fid].read_token() >= pkt_desc_tmp.len)
                {
                    flow_token_bucket[fid].sub_token(pkt_desc_tmp.len);
                    if(port_fifo_inst[send_pkt_port].full == true) 
                    {
                        drop_count++;
                        cout << "****Port" << send_pkt_port 
                            << " dropped packets: " << dec << drop_count << endl;
                    }
                    else
                    {
                        port_fifo_inst[send_pkt_port].pkt_in(pkt_desc_tmp);
                        flow_sn[fid] = flow_sn[fid] + 1;
                    }
                pkt_send_count++;
                }
            }
        }

        // output desc packet to each port
        for(int send_port=0; send_port<G_INTER_NUM; send_port++)
        {
            //output pkt_data
            pkt_desc_tmp = port_fifo_inst[send_port].pkt_pre_val();
            if((port_fifo_inst[send_port].empty == false)
                && (port_token_bucket[send_port].read_token() >= pkt_desc_tmp.len))
            {
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

        // wait for 1 to 3 clock periods
        //wait(1+(rand()%3));

    }

    pkt_sender_file.close();
}

#if 0
#include "packet_gen.h"
//激励包长应从顶层获取，先配置固定256B，稍后再改
packet_gen_module::packet_gen_module(string name, global_config_c *glb_cfg):sc_module(name)
{

    m_cycle_cnt =0;
    m_packet_id =0;
    m_cfg = glb_cfg;
    
    output.resize(g_m_inter_num);
    for(int i=0; i < g_m_inter_num; i++)
    {
        output[i] =new sc_fifo_out<TRANS>;
    }
    //init shape
    packet_shape.resize(g_m_inter_num);
    for(int i=0; i < g_m_inter_num; i++)
    {
        packet_shape[i] = new comm_shape_func(1000, 9620, 13, 10);
    }

    //stat
    string debug_file = name + string("_debug.log");
    m_bw_stat =new comm_stat_bw(m_cfg, debug_file,g_m_inter_num);
    
    SC_METHOD(packet_gen_process);
    sensitive << clk.pos();

    

}
void packet_gen_module::packet_gen_process()
{
   m_cycle_cnt++;   
   //stat
   if((m_cycle_cnt !=0) && (m_cycle_cnt % (m_cfg->stat_period *100) ==0))
   {
       m_bw_stat->print_bw_info(m_cycle_cnt);
   }
   
   //填桶
    if((m_cycle_cnt !=0) &&(m_cycle_cnt % 10 ==0))
    {
        for(int i=0; i < g_m_inter_num; i++)
        {
            packet_shape[i]->add_token(13);  //10cc填充13个
        }    
    }
    
   //4端口轮询
   for(int i=0; i < g_m_inter_num; i++)
   {
        if(packet_shape[i]->shape_status(256)) //包长是否够
        {
            m_packet_id++;  
            TRANS new_trans(new trans_type);
            new_trans->packet_id = m_packet_id;
            new_trans->port_id =i;
            new_trans->packet_len = 256;
            new_trans->valid_len =  256;
            new_trans->is_sop = true;
            new_trans->is_eop = true;
            output[i]->nb_write(new_trans);
            //stat
            m_bw_stat->record_bw_info(i, new_trans->valid_len, true);
        }
   }   
}
#endif//if 0