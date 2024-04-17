#include "DataRecord.h"
#include "StorageDevice.h"
#include "defs.h"

using namespace std;
class Tree {
   private:
    /* data */
   public:
    Tree();
    Tree(vector<vector<DataRecord *>> &, int, bool, StorageDevice &, bool);
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
};

struct Node {
    // Only for non leaf nodes
    DataRecord *dataRecord;
    int dataIndex;

    // For leaf nodes
    vector<DataRecord *> sortedRun;
    int sortedRunIndex;
    bool isLeaf;
    bool isEmpty;
};