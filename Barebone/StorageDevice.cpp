#include "StorageDevice.h"

#include <unistd.h>

#include <filesystem>

StorageDevice::StorageDevice() {
    this->device_path = "";
    this->last_run = 0;
    this->run_offset = map<int, int>();
    this->ssdSize = 1e11;
}

StorageDevice::StorageDevice(string device_path) {
    this->device_path = device_path;
    this->last_run = 0;
    this->run_offset = map<int, int>();
    this->ssdSize = 1e11;
}

void StorageDevice::spillRecordsToDisk(bool ifNewFile,
                                       vector<DataRecord *> &records) {
    string runPath =
        "/home/poovaya/project764/Cs764-project/Barebone/" + this->device_path;
    if (ifNewFile) {
        runPath += "/sorted/sorted_run_" + to_string(this->getTotalRuns() + 1);

    } else {
        runPath += "/merged_runs";
    }

    fstream runfile;
    string str_records = "";

    runfile.open(runPath, fstream::out | fstream::app);
    if (!runfile.is_open()) return;

    for (uint ii = 0; ii < records.size(); ii++) {
        DataRecord *record = records[ii];
        string str_record = record->getRecord();
        str_records += str_record + "|";
    }
    runfile << str_records;

    runfile.close();

    this->ssdSize -= records.size() * (4 * 4 + 3 + 1);
    return;
}

void StorageDevice::spillRecordListToDisk(
    vector<vector<DataRecord *> > record_lists) {
    for (uint ii = 0; ii < record_lists.size(); ii++) {
        this->spillRecordsToDisk(true, record_lists[ii]);
    }
}

vector<vector<DataRecord *> > StorageDevice::getRecordsFromRunsOnDisk(
    int numRecords) {
    vector<vector<DataRecord *> > recordLists;

    for (uint ii = 1; ii <= this->getTotalRuns(); ii++) {
        vector<DataRecord *> records;
        fstream runfile;
        string runPath = "/home/poovaya/project764/Cs764-project/Barebone/" +
                         this->device_path + "/sorted/sorted_run_" +
                         to_string(ii);
        uint recordSize = 4 * 4 + 3 + 1;
        char *runs = new char[numRecords * recordSize + 1];

        runfile.open(runPath, fstream::in);
        if (!runfile.is_open()) return recordLists;

        runfile.seekg(this->run_offset[ii], fstream::beg);

        runfile.get(runs, numRecords * recordSize + 1);
        runfile.close();
        this->run_offset[ii] += strlen(runs);

        if (strlen(runs) != (numRecords * recordSize)) {
            remove(runPath.c_str());
        }

        string s(runs);

        int start = 0;
        while (start < strlen(runs)) {
            string record_str;

            record_str = s.substr(start, recordSize);
            string col_value1 = record_str.substr(0, 4);
            string col_value2 = record_str.substr(5, 4);
            string col_value3 = record_str.substr(4 * 2 + 2, 4);
            string col_value4 = record_str.substr(4 * 3 + 3, 4);

            DataRecord *record =
                new DataRecord(col_value1, col_value2, col_value3, col_value4);
            records.push_back(record);
            start += recordSize;
        }

        recordLists.push_back(records);
    }

    return recordLists;
}

int StorageDevice::getTotalRuns() {
    uint n;
    struct dirent **namelist;
    uint count = 0;

    const std::string directory_path =
        "/home/poovaya/project764/Cs764-project/Barebone/" + this->device_path +
        "/sorted";  // Replace this with your directory path
    int file_count = 0;

    for (const auto &entry :
         std::filesystem::directory_iterator(directory_path)) {
        // if (entry.is_regular_file()) {
        ++file_count;
        // }
    }
    return file_count;
}

void StorageDevice::commitRun() {
    int latestRun = this->getTotalRuns();
    string mergedRunPath = "/home/poovaya/project764/Cs764-project/Barebone" +
                           this->device_path + "/merged_runs";
    string newRunPath = "/home/poovaya/project764/Cs764-project/Barebone" +
                        this->device_path + "/sorted/sorted_run_" +
                        to_string(last_run + 1);

    if (access(mergedRunPath.c_str(), F_OK) == 0) {
        rename(mergedRunPath.c_str(), newRunPath.c_str());
    }

    return;
}