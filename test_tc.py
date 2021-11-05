import os
lst = os.listdir("./tb/tc")
for c in lst:
    if c.endswith('.tab'):
        fileName = os.path.splitext(c)[0] + '\n'  # 分割，不带后缀名
#        print(fileName)
        os.system('./carbon %s' % fileName)
