import sys
import os
lst = os.listdir("./tb/tc")
for c in lst:
    if c.endswith('.tab'):
        fileName = os.path.splitext(c)[0] + '\n'  # 分割，不带后缀名
#        print(fileName)
        os.system('./carbon -c %s' % fileName)

if len(sys.argv) > 1:
    copy_dir = sys.argv[1]

    if os.path.exists('%s' % copy_dir) == False:
        os.system('mkdir /mnt/%s' % copy_dir)

    os.system('scp -r run_log/ /mnt/%s' % copy_dir)
    print('scp -r run_log/ /mnt/%s' % copy_dir)
