import os

def check_sorted(filename):
    j = 0
    ans = True
    with open(filename, 'r') as file:
        for line in file:
            elements = line.strip().split('|')            
            for i in range(len(elements) - 1):
                j += 1
                if elements[i] > elements[i + 1] and  elements[i + 1] != "":
                    print(elements[i],"  |  ", elements[i+1], "   ", i)
                    ans =  False
    return ans, j

filename = '/home/poovaya/project764/Cs764-project/Barebone/HDD/merged_runs'  # Replace 'your_file.txt' with the actual filename
val, i = check_sorted(filename)
if val:
    print("All elements are alphanumerically sorted.")
    print("Num elements scanned are " + str(i))
    os.remove(filename)
else:
    print("Elements are not alphanumerically sorted.")
    print("Num elements scanned are " + str(i))