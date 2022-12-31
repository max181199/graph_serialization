file = open('./input.txt', 'r')
file2 = open('./input2.txt', 'r')
data = file.read().split('\n')
data2 = file2.read().split('\n')
diff = []
for s in data:
    s2 = s.split('\t')
    s2[0], s2[1] = s2[1], s2[0]
    s2 = '\t'.join(s2)
    if (not((s in data2) or (s2 in data2)) ):
        diff.append([s,s2, (s in data2), (s2 in data2)])
if (len(diff) == 0):
    print('Ok')
else:
    for s in diff:
        print(s)
file.close()
file2.close()