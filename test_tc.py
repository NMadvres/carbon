import sys
import os
argv_list = [[777 for argv_split in range(2)] for num_argv in range(5)]

if len(sys.argv) > 1:
    for i in range(1, len(sys.argv)):
        argv_list[i] = sys.argv[i].split("=", 1)

        if argv_list[i][0] == "case":
            case_sel = 1
            case_name = argv_list[i][1]
            print(case_name)
        elif sys.argv[i].startswith("TC_"):
            case_sel = 1
            case_name = sys.argv[i]
        else:
            case_sel = 0
            lst = os.listdir("./tb/tc")
            for c in lst:
                if c.endswith('.tab'):
                    case_name = os.path.splitext(c)[0] + '\n'  # 分割，不带后缀名

        if argv_list[i][0] == "windir":
            dir_sel = 1
            case_dir = "/mnt/" + argv_list[i][1] + "/"
            print(case_dir)
        elif argv_list[i][0] == "dir":
            dir_sel = 1
            case_dir = argv_list[i][1] + "/"
            print(case_dir)
        else:
            dir_sel = 0
            case_dir = "./tb/tc" + "/"

        case_main = case_dir + case_name
        print(case_main)

    if (case_sel == 1) and (dir_sel == 1):   # case & &dir
        os.system('./carbon %s %s' % (case_name, case_dir))
    #    os.system('scp -r run_log/ %s' % case_dir)

    elif (case_sel == 1) and (dir_sel == 0):  # case
        print(case_main)
        os.system('./carbon %s %s' % (case_name, case_dir))

    elif (case_sel == 0) and (dir_sel == 1):  # dir
        case_main = case_dir + case_name
        print(case_main)
        os.system('./carbon %s %s' % (case_name, case_dir))


# if len(sys.argv) > 1:
# copy_dir = sys.argv[1]
# if os.path.exists('%s' % copy_dir) == False:
# os.system('mkdir /mnt/%s' % copy_dir)
##
# os.system('scp -r run_log/ /mnt/%s' % copy_dir)
# print('scp -r run_log/ /mnt/%s' % copy_dir)

