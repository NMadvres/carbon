#include "comm_def.h"
#include "systemc.h"
#include "top_carbon.h"
#include "../tb/src/top_tb.h"
#include <unistd.h>

#define DEFAULT_CASE_NAME "TC_LEN_001"
#define DEFAULT_CASE_DIR "./tb/tc/"

static void print_usage(const char *app_name)
{
    printf("Usage:\n"
           "  %s [options]\n"
           "Options:\n"
           "  -c <case name>   Run the case\n"
           "  -d               Print module debug log\n"
           "  -h               Print this help info\n",
           app_name);
}

////////////////////////////////////////////////////////
// Project： SystemC虚拟项目
// Module:   main
// Description: 主函数
// Group：预研组
// Author: Newton
// Date: 2021.10.14 第一版
// Hierarchy : 编号，索引公共库
////////////////////////////////////////////////////////
int sc_main(int argc, char *argv[])
{
    int opt;
    //全局计数器生成
    sc_clock clk("clk", 10, SC_NS); //10ns一个周期，100MHZ

    std::string glb_cfg_file;
    std::string print_file;
    std::string print_dir;
    std::string case_name(DEFAULT_CASE_NAME);
    std::string case_dir(DEFAULT_CASE_DIR);

    // 默认关闭模块日志打印
    mod_lg_inst.disable();

    // 命令行参数解析
    while ((opt = getopt(argc, argv, "c:dh")) != -1) {
        switch (opt) {
        case 'c':
            case_name = optarg;
            case_dir = argv[optind];
            break;
        case 'd':
            mod_lg_inst.enable();
            break;
        case 'h':
            print_usage(argv[0]);
            return 0;
        default:
            fprintf(stderr, "Unknown option: %c\n", opt);
            return -1;
        }
    }

    glb_cfg_file = case_dir + case_name + std::string(".tab");
    print_file = case_name + string(".stat");
    std::cout << "glb_cfg_file: " <<glb_cfg_file<< std::endl;
    std::cout << "case name: " << case_name << std::endl;
    glb_cfg_c glb_cfg(glb_cfg_file);

    //例化统计类
      func_stat *top_stat = new func_stat(print_file, Module_top);
    //子模块例化
    top_carbon top_carbon_mod("top_carbon", top_stat);
    top_tb top_tb_mod("top_tb", top_stat);
    //顶层绑定
    array<sc_signal<s_pkt_desc> *, G_INTER_NUM> tb_ing_sig;
    array<sc_signal<s_pkt_desc> *, G_INTER_NUM> egr_tb_sig;
    for (int i = 0; i < G_INTER_NUM; i++) {
        tb_ing_sig[i] = new sc_signal<s_pkt_desc>();
        egr_tb_sig[i] = new sc_signal<s_pkt_desc>();
        (*top_carbon_mod.in_ing_port[i])(*tb_ing_sig[i]);
        (*top_carbon_mod.out_egr_port[i])(*egr_tb_sig[i]);
        (*top_tb_mod.in_pkt_stat[i])(*egr_tb_sig[i]);
        (*top_tb_mod.out_pkt_stim[i])(*tb_ing_sig[i]);
    }
    //绑定入口主时钟
    top_tb_mod.in_glb_clk(clk);
    //绑定cnt计数
    sc_signal<int> cycle_cnt_sig;
    top_tb_mod.out_clk_cnt(cycle_cnt_sig); //一发多收
    top_tb_mod.in_clk_cnt(cycle_cnt_sig);  //一发多收
    top_carbon_mod.in_clk_cnt(cycle_cnt_sig);

    sc_start(1000, SC_US); //启动仿真
    top_tb_mod.stim_mod->~mod_stim();
    int simu_cycle = 1000 * 1000 / 10;
    top_stat->print_info(simu_cycle);
    return 0;
}
