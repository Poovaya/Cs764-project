import os

def check_sorted(filename):
    j = 0
    prev_record = None

    ans = True
    with open(filename, 'r') as file:
        for line in file:
            record = line.strip()       
            j += 1
            if prev_record is not None and prev_record > record:
                print(prev_record,"  |  ", record, "   ", j)
                ans =  False
            prev_record = record
    return ans, j

filename = '/home/poovaya/project764/Cs764-project/Barebone/HDD/output'  # Replace 'your_file.txt' with the actual filename
val, i = check_sorted(filename)
if val:
    print("All elements are alphanumerically sorted.")
    print("Num elements scanned are " + str(i))
    # os.remove(filename)
else:
    print("Elements are not alphanumerically sorted.")
    print("Num elements scanned are " + str(i))