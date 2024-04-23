#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "DataRecord.h"

std::vector<std::string> splitString(char* input) {
    std::istringstream iss(input);
    std::vector<std::string> result;
    std::string token;
    while (iss >> token) {
        result.push_back(token);
    }
    return result;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <integer>" << std::endl;
        return 1;
    }

    int record_size = std::atoi(argv[1]);
    int onDiskSize = record_size + 1;
    std::ifstream file(
        "/home/poovaya/project764/Cs764-project/Barebone/HDD/output",
        std::ios::binary);  // Open file in binary mode

    if (!file.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
        return 1;
    }

    // const int numBytesToRead = 20;
    int numBytesToRead = (10000 / onDiskSize);
    numBytesToRead = numBytesToRead * onDiskSize;
    char buffer[numBytesToRead];  // Create a buffer to store the read bytes
    DataRecord currRec;
    DataRecord nextRec;
    // Read and process the file in chunks of numBytesToRead
    while (file.read(buffer, numBytesToRead)) {
        // Process the data in the buffer
        int start = 0;
        char dataRecordBytes[onDiskSize];
        for (int curr = 0; curr < onDiskSize; curr++) {
            dataRecordBytes[curr] = buffer[start];
            start++;
        }
        dataRecordBytes[onDiskSize - 1] = '\0';
        currRec = DataRecord(dataRecordBytes);

        while (start < numBytesToRead) {
            for (int curr = 0; curr < onDiskSize; curr++) {
                dataRecordBytes[curr] = buffer[start];
                start++;
            }
            dataRecordBytes[onDiskSize - 1] = '\0';

            nextRec = DataRecord(dataRecordBytes);
            if (!(DataRecord::compareDataRecords(currRec, nextRec))) {
                currRec.show();
                nextRec.show();
                std::cout << "BAD ORDER" << std::endl;
                return 0;
            }
            currRec = nextRec;

            //
        }
    }

    // Check if there was an error or if there are remaining bytes to be read
    if (file.bad()) {
        std::cerr << "Error reading file!" << std::endl;
        return 1;
    } else if (file.eof()) {
        // If the end of the file is reached, but there are remaining bytes in
        // the buffer, process them
        const int numBytesToRead = file.gcount();
        while (file.read(buffer, numBytesToRead)) {
            // Process the data in the buffer
            int start = 0;
            char dataRecordBytes[onDiskSize];
            for (int curr = 0; curr < onDiskSize; curr++) {
                dataRecordBytes[curr] = buffer[start];
                start++;
            }
            dataRecordBytes[onDiskSize - 1] = '\0';
            currRec = DataRecord(dataRecordBytes);

            while (start < numBytesToRead) {
                for (int curr = 0; curr < onDiskSize; curr++) {
                    dataRecordBytes[curr] = buffer[start];
                    start++;
                }
                dataRecordBytes[onDiskSize - 1] = '\0';

                nextRec = DataRecord(dataRecordBytes);
                if (!(DataRecord::compareDataRecords(currRec, nextRec))) {
                    std::cout << "BAD ORDER1" << std::endl;
                    return 0;
                }
                currRec = nextRec;
            }
        }
    }

    file.close();  // Close the file
    std::cout << "GOOD ORDER" << std::endl;

    return 0;
}