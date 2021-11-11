import subprocess
import sys
import os
case_sel = 0
dir_sel = 0
argv_list = [[777 for argv_split in range(2)] for num_argv in range(5)]

if len(sys.argv) > 1:
    for i in range(1, len(sys.argv)):
        argv_list[i] = sys.argv[i].split("=", 1)

    for i in range(1, len(sys.argv)):
        if (dir_sel == 0):
            if argv_list[i][0] == "windir":
                dir_sel = 1
                case_dir = "/mnt/" + argv_list[i][1]
                break
            elif argv_list[i][0] == "dir":
                dir_sel = 1
                case_dir = argv_list[i][1]
                break
            else:
                dir_sel = 0
                case_dir = "./tb/tc"

    for i in range(1, len(sys.argv)):
        if (case_sel == 0):
            if argv_list[i][0] == "case":
                case_sel = 1
                case_name = argv_list[i][1]
                break
            elif sys.argv[i].startswith("TC_"):
                case_sel = 1
                case_name = sys.argv[i]
                break
            else:
                case_sel = 0

else:
    dir_sel = 0
    case_sel = 0
    case_dir = "./tb/tc"

case_dir_all = case_dir + "/"

if (case_sel == 1):
    os.system('./carbon -c %s %s' % (case_name, case_dir_all))
    if (dir_sel == 1):
        os.system('scp -r run_log/ %s' % case_dir_all)
else:
    lst = os.listdir(case_dir)
    for c in lst:
        if c.endswith('.tab'):
            case_name = os.path.splitext(c)[0]  # 分割，不带后缀名
            print('./carbon -c %s %s' % (case_name, case_dir_all))            
            os.system('./carbon -c %s %s' % (case_name, case_dir_all))
    if (dir_sel == 1):
        os.system('scp -r run_log/ %s' % case_dir_all)

# os.system('scp -r run_log/ %s' % case_dir_all)
# if (case_sel == 1) and (dir_sel == 1):   # case & &dir
# os.system('./carbon %s %s' % (case_name, case_dir))
# os.system('scp -r run_log/ %s' % case_dir)
##
# elif (case_sel == 1) and (dir_sel == 0):  # case
# print(case_main)
# os.system('./carbon %s %s' % (case_name, case_dir))
##
# elif (case_sel == 0) and (dir_sel == 1):  # dir
# case_main = case_dir + case_name
# print(case_main)
# os.system('./carbon %s %s' % (case_name, case_dir))
##