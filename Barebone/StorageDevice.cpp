#include "StorageDevice.h"

#include <unistd.h>

#include <filesystem>

#include "DeviceConstants.h"
#include "SortTrace.h"
extern int recordSize;
extern int ON_DISK_RECORD_SIZE;
extern SortTrace trace;
StorageDevice::StorageDevice() {
    this->device_path = "";
    this->last_run = 0;
    this->run_offset = map<long long int, long long int>();
    this->total_reads = 0;
    this->total_writes = 0;
    this->lastValidString = "";
}

StorageDevice::StorageDevice(string device_path) {
    this->device_path = device_path;
    this->last_run = 0;
    this->run_offset = map<long long int, long long int>();
    if (device_path == "SSD") {
        this->pageSize = SSD_PAGE_SIZE;
    } else {
        this->pageSize = HDD_PAGE_SIZE;
    }
    this->lastValidString = "";
}

void StorageDevice::spillRecordsToDisk(bool ifNewFile,
                                       vector<DataRecord *> &records,
                                       int specificFile) {
    string delim = "|";
    std::filesystem::path currentDir = std::filesystem::current_path();

    string runPath = currentDir.string() + "/" + this->device_path;
    if (specificFile >= 1) {
        runPath += "/sorted/sorted_run_" + to_string(specificFile);
    } else if (ifNewFile) {
        runPath += "/sorted/sorted_run_" + to_string(this->getTotalRuns() + 1);

    } else {
        runPath += "/output";
        delim = "\n";
    }

    fstream runfile;
    string str_records = "";

    runfile.open(runPath, fstream::out | fstream::app);
    if (!runfile.is_open()) return;
    string str_record;
    for (uint ii = 0; ii < records.size(); ii++) {
        DataRecord *record = records[ii];

        if (record == NULL) {
            break;
        } else {
            str_record = record->getRecord();
            this->lastValidString = str_record;
            if (delim == "\n") {
                if (str_record[str_record.size() - 1] == '\n') {
                    str_record[str_record.size() - 1] = '\n';
                } else if (str_record[str_record.size() - 1] == '|') {
                    str_record[str_record.size() - 1] = '\n';
                } else {
                    str_record += '\n';
                }
            } else {
                if (str_record[str_record.size() - 1] == '|') {
                    str_record[str_record.size() - 1] = '|';
                } else if (str_record[str_record.size() - 1] == '\n') {
                    str_record[str_record.size() - 1] = '|';
                } else {
                    str_record += '|';
                }
            }
        }
        str_records += str_record;
        delete record;
    }
    runfile << str_records;

    this->total_writes += 1;

    runfile.close();

    return;
}

// void StorageDevice::spillRecordListToDisk(
//     vector<vector<DataRecord *> > record_lists) {
//     for (uint ii = 0; ii < record_lists.size(); ii++) {
//         this->spillRecordsToDisk(true, record_lists[ii], -1);
//     }
// }

vector<RecordDetails *> StorageDevice::getRecordsFromRunsOnDisk(
    int numRecords) {
    vector<RecordDetails *> recordDetailsLists;

    for (uint ii = 1; ii <= this->getTotalRuns(); ii++) {
        vector<DataRecord *> records;
        RecordDetails *recordDetails = new RecordDetails;
        fstream runfile;
        std::filesystem::path currentDir = std::filesystem::current_path();

        string runPath = currentDir.string() + "/" + this->device_path +
                         "/sorted/sorted_run_" + to_string(ii);
        recordDetails->runPath = runPath;

        if (this->device_path == "SSD") {
            recordDetails->deviceType = Type::SSD;
        } else {
            recordDetails->deviceType = Type::HDD;
        }

        char *runs = new char[numRecords * ON_DISK_RECORD_SIZE + 1];
        clock_t begin_time = clock();

        runfile.open(runPath, fstream::in);
        if (!runfile.is_open()) return recordDetailsLists;

        runfile.seekg(this->run_offset[ii], fstream::beg);

        this->total_reads += 1;

        runfile.get(runs, numRecords * ON_DISK_RECORD_SIZE + 1);
        runfile.close();
        long long int time_spent_us =
            float(clock() - begin_time) * 1000 * 1000 / CLOCKS_PER_SEC;

        this->run_offset[ii] += strlen(runs);

        // if (strlen(runs) != (numRecords * ON_DISK_RECORD_SIZE)) {
        //     remove(runPath.c_str());
        // }

        string s(runs);
        string trace_str = "ACCESS -> A read from ./" + this->device_path +
                           " was made with size " + to_string(s.size()) +
                           " bytes and latency " + to_string(time_spent_us) +
                           " us";

        trace.append_trace(trace_str);
        int start = 0;
        while (start < strlen(runs)) {
            string record_str;

            record_str = s.substr(start, ON_DISK_RECORD_SIZE);
            record_str.pop_back();

            DataRecord *record = new DataRecord(record_str);
            records.push_back(record);
            start += ON_DISK_RECORD_SIZE;
        }

        recordDetails->recordLists = records;
        delete runs;
        recordDetailsLists.push_back(recordDetails);
    }

    return recordDetailsLists;
}

int StorageDevice::getTotalRuns() {
    uint n;
    struct dirent **namelist;
    uint count = 0;

    std::filesystem::path currentDir = std::filesystem::current_path();
    const std::string directory_path =
        currentDir.string() + "/" + this->device_path +
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
    std::filesystem::path currentDir = std::filesystem::current_path();

    string mergedRunPath =
        currentDir.string() + "/" + this->device_path + "/output";
    string newRunPath = currentDir.string() + "/" + this->device_path +
                        "/sorted/sorted_run_" + to_string(last_run + 1);

    if (access(mergedRunPath.c_str(), F_OK) == 0) {
        rename(mergedRunPath.c_str(), newRunPath.c_str());
    }

    return;
}

void StorageDevice::get_device_access_stats() {
    cout << "Number of reads on the device: " << this->total_reads << endl;
    cout << "Number of writes on the device: " << this->total_writes << endl;
}

StorageDevice::StorageDevice(const StorageDevice &other) {
    // Copy the device path
    this->device_path = other.device_path;

    // Copy other member variables
    this->last_run = other.last_run;
    this->ssdSize = other.ssdSize;
    this->pageSize = other.pageSize;

    // Copy the run_offset map
    this->run_offset = other.run_offset;
}