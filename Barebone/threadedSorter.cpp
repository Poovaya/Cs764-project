#include <fstream>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "DataRecord.h"

// Define a mutex to protect shared resources
std::mutex mtx;

std::vector<std::string> splitString(const std::string& input) {
    std::istringstream iss(input);
    std::vector<std::string> result;
    std::string token;
    while (iss >> token) {
        result.push_back(token);
    }
    return result;
}

void processChunk(const std::string& chunk) {
    std::vector<std::string> result = splitString(chunk);
    DataRecord currRec(result[0], result[1], result[2], result[3]);
    DataRecord nextRec;
    for (size_t i = 20; i < chunk.size(); i += 20) {
        result = splitString(chunk.substr(i, 20));
        nextRec = DataRecord(result[0], result[1], result[2], result[3]);
        if (!DataRecord::compareDataRecords(currRec, nextRec)) {
            // Lock the mutex before printing
            std::lock_guard<std::mutex> lock(mtx);
            currRec.show();
            nextRec.show();
            std::cout << "BAD ORDER" << std::endl;
            return;
        }
        currRec = nextRec;
    }
}

int main() {
    std::ifstream file(
        "/home/poovaya/project764/Cs764-project/Barebone/HDD/output",
        std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
        return 1;
    }

    const int chunkSize = 100000;
    char buffer[chunkSize];
    std::vector<std::thread> threads;

    while (file.read(buffer, chunkSize)) {
        // Launch a thread to process each chunk
        std::string chunk(buffer, file.gcount());

        threads.push_back(std::thread(processChunk, chunk));
    }

    for (auto& th : threads) th.join();

    if (file.bad()) {
        std::cerr << "Error reading file!" << std::endl;
        return 1;
    }

    file.close();
    std::cout << "GOOD ORDER" << std::endl;

    return 0;
}