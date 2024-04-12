#include <unistd.h>

#include "StorageDevice.h"

StorageDevice::StorageDevice(string device_path)
{
	this->device_path = device_path;
	this->last_run = 0;
	this->run_offset = map<int,int>();
    this->ssdSize = 10 * 1024 * 1024 * 1024;
}

void StorageDevice:: spillRecordsToDisk(bool ifNewFile, vector<DataRecord*> &records) {
    string runPath = this->device_path;
    if (ifNewFile) {
        runPath += "/sorted/sorted_run_" + to_string(this->getTotalRuns() + 1);
        
    } else {
        runPath += "/merged_runs";
    }

    fstream runfile;
    string str_records = "";

    runfile.open(runPath, ios_base::app);
    if (!runfile.is_open())
        return;

    for (uint ii = 0 ; ii < records.size() ; ii++) {
        DataRecord *record = records[ii];
        string str_record = record->getRecord();
        str_records += str_record + "|";
    }
    runfile << str_records;

    runfile.close();

    this->ssdSize -= records.size() * (4*4 + 3 + 1);
    return;
}

void StorageDevice::spillRecordListToDisk(vector<vector<DataRecord*> > record_lists)
{
	for (uint ii = 0 ; ii < record_lists.size() ; ii++) {		
		this->spillRecordsToDisk(true, record_lists[ii]);
	}
}

vector<vector<DataRecord*> >  StorageDevice::  getRecordsFromRunsOnDisk(int numRecords) {
	vector<vector<DataRecord*> > recordLists;

	for (uint ii = 1 ; ii <= this->getTotalRuns() ; ii++) {
		vector<DataRecord*> records;
        fstream runfile;	
        string runPath = this->device_path + "/sorted/sorted_run_" + to_string(ii);
        uint recordSize = 4*4 + 3 + 1;
        char *runs = new char[numRecords * recordSize + 1];

        runfile.open(runPath, ios::in);
        if (!runfile.is_open())
            return recordLists;

        runfile.seekg(this->run_offset[ii], ios::beg);

        runfile.get(runs, numRecords * recordSize + 1);
        runfile.close();
        this->run_offset[ii] += strlen(runs);

        if (strlen(runs) != (numRecords * recordSize)) {
            remove(runPath.c_str());
        }

        string s(runs);

        int start = 0;
        while (start < strlen(runs))
        {
            string record_str;

            record_str = s.substr(start, recordSize);
            string col_value1 = record_str.substr(0, 4);
            string col_value2 = record_str.substr(5, 4);
            string col_value3 = record_str.substr(4 * 2 + 2, 4);
            string col_value4 = record_str.substr(4 * 3 + 3, 4);

            DataRecord record = DataRecord(col_value1, col_value2, col_value3, col_value4);
            records.push_back(&record);
            start += recordSize;
        }

        recordLists.push_back(records);
	}

    return recordLists;
} 


int StorageDevice::getTotalRuns()
{
	uint n;
	struct dirent **namelist;
	uint count = 0;

	n = scandir((this->device_path + "/sorted").c_str(), &namelist, 0, alphasort);

	return n;
}

void StorageDevice::commitRun()
{
	int latestRun = this->getTotalRuns();
	string mergedRunPath = this->device_path + "/merged_runs";
	string newRunPath = this->device_path + "/sorted/sorted_run_" + to_string(last_run + 1);

	if (access(mergedRunPath.c_str(), F_OK) == 0) {
		rename(mergedRunPath.c_str(), newRunPath.c_str());
	}

	return;
}