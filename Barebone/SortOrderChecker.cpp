#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;
#include "DataRecord.h"
#include "DeviceConstants.h"
#include "SortOrderChecker.h"

vector<std::string> splitString(char* input) {
    istringstream iss(input);
    vector<std::string> result;
    string token;
    while (iss >> token) {
        result.push_back(token);
    }
    return result;
}

void verifySortOrder(int record_size, long long int num_record) {
    bool hdd = false;
    if ((long long int)record_size * num_record > SSD_SIZE) {
        hdd = true;
    }
    int onDiskSize = record_size + 1;
    filesystem::path currentDir = std::filesystem::current_path();
    std::ifstream file(currentDir.string() + "/HDD/output",
                       std::ios::binary);  // Open file in binary mode

    if (!file.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
        return;
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
            if (!(DataRecord::compareDataRecords(currRec, nextRec)) && !hdd) {
                currRec.show();
                nextRec.show();
                std::cout << "Scanned " + to_string(num_record) +
                                 " records, records are not sorted"
                          << std::endl;
                return;
            }
            currRec = nextRec;
        }
    }

    // Check if there was an error or if there are remaining bytes to be read
    if (file.bad()) {
        std::cerr << "Error reading file!" << std::endl;
        return;
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
                if (!(DataRecord::compareDataRecords(currRec, nextRec)) &&
                    !hdd) {
                    std::cout << "Scanned " + to_string(num_record) +
                                     " records, records are not sorted"
                              << std::endl;
                    return;
                }
                currRec = nextRec;
            }
        }
    }

    file.close();  // Close the file
    std::cout << "Scanned " + to_string(num_record) +
                     " records, all records are sorted"
              << std::endl;
}