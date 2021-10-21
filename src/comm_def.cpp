////////////////////////////////////////////////////////
// Project： SystemC虚拟项目
// Module:   commdef
// Description: 全局定义文件
// Group：预研组
// Author: Newton
// Date: 2021.10.14 第一版
// Hierarchy : 编号，索引公共库
////////////////////////////////////////////////////////

#include "comm_def.h"

// 用于实现全局cycle_cnt计数

std::vector<s_flow_rule> g_flow_rule_tab;
std::unordered_map<s_hash_rule_key, int, has_rule_key_hash> g_hash_rule_tab;
std::vector<s_tab_que> g_que_rule_tab;
std::vector<s_tab_port> g_port_rule_tab;

////////////////////////////////////////////////////////
// Project： SystemC虚拟项目
// Module:   glb_cfg_c
// Description: glb_cfg_c 生成配置表格
// Group：预研组
// Author: Newton
// Date: 2021.10.14 第一版
// Hierarchy : 编号，索引公共库
////////////////////////////////////////////////////////
glb_cfg_c::glb_cfg_c(std::string file_name)
{
    read_cfg_file(file_name);
    gen_cfg_table();
}

////////////////////////////////////////////////////////
// Project： SystemC虚拟项目
// Module:   mod_ing
// Description: read_cfg_file
// Group：预研组
// Author: Newton
// Date: 2021.10.14 第一版
// Hierarchy : 编号，索引公共库
////////////////////////////////////////////////////////
void glb_cfg_c::read_cfg_file(std::string file_name)
{
    ifstream in_file(file_name.c_str());
    std::string read_line;
    int file_pos_status = -255; // 如果status =1,读取流表 status =2,读取HASH表  status =3,读取队列权重表  status =4,读取Port端口表
    while (getline(in_file, read_line)) {
        if (read_line.find("g_flow_rule_tab") != read_line.npos) // 标识符不写入
        {
            file_pos_status = 1;
            continue;
        } else if (read_line.find("g_hash_rule_tab") != read_line.npos) {
            file_pos_status = 2;
            continue;
        } else if (read_line.find("g_que_rule_tab") != read_line.npos) {
            file_pos_status = 3;
            continue;
        } else if (read_line.find("g_port_rule_tab") != read_line.npos) {
            file_pos_status = 4;
            continue;
        }
        // 根据状态填充表格
        std::string tmp_value;
        int index = 0;
        char *split_char;
        std::vector<int> tmp_value_vc;
        const char *sep = " ";
        char *line_str = new char[read_line.size() + 1];
        strcpy(line_str, read_line.c_str());
        // 获取第一个子字符串
        split_char = strtok(line_str, sep);
        while (split_char != NULL) {
            //cout << split_char <<endl;
            if ((index != 0) && (*split_char != '\r')) // 全面的标识符sid /did 等不写入
            {
                std::string tmp_str = std::string(split_char);
                tmp_value_vc.push_back(stoi(tmp_str));
            }
            index++;
            split_char = strtok(NULL, sep);
        }
        delete[] line_str;
        if (file_pos_status == 1) {
            g_flow_tab_relate.push_back(tmp_value_vc);
        }
        if (file_pos_status == 2) {
            g_hash_tab_relate.push_back(tmp_value_vc);
        }
        if (file_pos_status == 3) {
            g_que_tab_relate.push_back(tmp_value_vc);
        }
        if (file_pos_status == 4) {
            g_port_tab_relate.push_back(tmp_value_vc);
        }
    }
}

////////////////////////////////////////////////////////
// Project： SystemC虚拟项目
// Module:   mod_ing
// Description: gen_cfg_table
// Group：预研组
// Author: Newton
// Date: 2021.10.14 第一版
// Hierarchy : 编号，索引公共库
////////////////////////////////////////////////////////
void glb_cfg_c::gen_cfg_table()
{
    // 填充flow表格
    if (g_flow_tab_relate.size() != 0) {
        int flow_num = g_flow_tab_relate[0].size(); // index0对应fid号
        // 填充真实flow表
        for (int i = 0; i < flow_num; i++) {
            s_flow_rule fill_flow_rule;
            fill_flow_rule.sid = g_flow_tab_relate[1][i];        // index1对应sid号
            fill_flow_rule.did = g_flow_tab_relate[2][i];        // index2对应did号
            fill_flow_rule.pri = g_flow_tab_relate[3][i];        // index3对应pri
            fill_flow_rule.len = g_flow_tab_relate[4][i];        // index4对应len
            fill_flow_rule.sport = g_flow_tab_relate[5][i];      // index5对应sport号
            fill_flow_rule.dport = g_flow_tab_relate[6][i];      // index6对应dport号
            fill_flow_rule.qid = g_flow_tab_relate[7][i];        // index7对应qid号
            fill_flow_rule.len2add = g_flow_tab_relate[8][i];    // index8对应len2add
            fill_flow_rule.flow_speed = g_flow_tab_relate[9][i]; // index9对应flow_speed
            g_flow_rule_tab.push_back(fill_flow_rule);
        }
    }

    // 填充HASH表格
    if (g_hash_tab_relate.size() != 0) {
        int hash_rule_num = g_hash_tab_relate[0].size(); // index0对应key_sid
        // 填充真实HASH表
        for (int i = 0; i < hash_rule_num; i++) {
            s_hash_rule_key fill_hash_key;
            fill_hash_key.sid = g_hash_tab_relate[0][i]; // index0对应key_sid
            fill_hash_key.did = g_hash_tab_relate[1][i]; // index0对应key_sid
            fill_hash_key.pri = g_hash_tab_relate[2][i]; // index0对应key_sid
            int fill_fid = g_hash_tab_relate[3][i];
            g_hash_rule_tab.insert({fill_hash_key, fill_fid});
        }
    }

    // 填充que表格
    if (g_que_tab_relate.size() != 0) {
        int que_num = g_que_tab_relate[0].size(); // index0对应que_id
        // 填充真实que表
        for (int i = 0; i < que_num; i++) {
            int fill_weight = g_que_tab_relate[2][i]; // index2对应weight
            g_que_rule_tab.push_back(fill_weight);
        }
    }

    // 填充port表格
    if (g_port_tab_relate.size() != 0) {
        int port_num = g_port_tab_relate[0].size(); // index0对应port_id
        // 填充真实port表
        for (int i = 0; i < port_num; i++) {
            int fill_speed = g_port_tab_relate[1][i]; // index1对应port speed
            g_port_rule_tab.push_back(fill_speed);
        }
    }
}
