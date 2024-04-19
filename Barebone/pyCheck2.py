def is_sorted(file_path):
    with open(file_path, 'r') as file:
        previous_line = None
        for line in file:
            line = line.strip()
            if previous_line is not None:
                if previous_line > line:
                    print(previous_line, line)
                    return False
            previous_line = line
    return True

file_path = "/home/poovaya/project764/Cs764-project/Barebone/test.txt"  # Replace this with the path to your file
if is_sorted(file_path):
    print("The file is sorted.")
else:
    print("The file is not sorted.")
