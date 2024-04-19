#include "DataRecord.h"
#include "RecordDetails.h"
#include "StorageDevice.h"
#include "defs.h"

using namespace std;
class Tree {
   private:
    /* data */
   public:
    Tree();
    Tree(vector<RecordDetails *> &, int, bool, StorageDevice &, bool);
    Tree(vector<DataRecord> &, bool);
    ~Tree();
    void generateSortedRun();
    void run_tournament(int);
    int numRuns;
    struct Node *heap;
    void printHeap();
    int numLeaves;
    int numNodes;
    bool ramTree;
    int numInnerNodes;
    int numRecords;
    bool removeDuplicate;
    StorageDevice runDevice;
    vector<DataRecord *> generated_run;
    DataRecord *popRecordFromLeafList(Node *);
    DataRecord *getTopRecordFromLeafList(Node *);
    void checkforEmptyNode(Node *);
    //  vector<DataRecord *> getGeneratedRun();

    void printSortedRun(vector<DataRecord *>);
    void fetchNextPage(Node *);
};

struct Node {
    // Only for non leaf nodes
    DataRecord *dataRecord;
    int dataIndex;

    // For leaf nodes
    vector<DataRecord *> sortedRun;
    int sortedRunIndex;
    string runPath;
    Type deviceType;
    bool isLeaf;
    bool isEmpty;
    int getDevicePageSize() {
        if (deviceType == Type::SSD) {
            return 20972;
        } else {
            return 524288;
        }
    };
};