def check_sorted(filename):
    with open(filename, 'r') as file:
        for line in file:
            elements = line.strip().split('|')
            for i in range(len(elements) - 1):
                if elements[i] > elements[i + 1] and  elements[i + 1] != "":
                    print(elements[i],"  |  ", elements[i+1])
                    return False
    return True

filename = '/home/poovaya/project764/Cs764-project/Barebone/HDD/merged_runs'  # Replace 'your_file.txt' with the actual filename
if check_sorted(filename):
    print("All elements are alphanumerically sorted.")
else:
    print("Elements are not alphanumerically sorted.")