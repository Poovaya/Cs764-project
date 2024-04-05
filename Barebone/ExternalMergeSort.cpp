#include <string.h>
#include <unistd.h>  // For getopt

#include <iostream>
#include <string>
using namespace std;

int main(int argc, char* argv[]) {
    int opt;
    int numRecords = 0;  // Initialize to default values
    int recordSize = 0;
    string trace_file = "trace";

    // Process command-line arguments using getopt
    while ((opt = getopt(argc, argv, "c:s:o:")) !=
           -1) {  // Added ':' for option 'o' to indicate it expects an argument
        switch (opt) {
            case 'c':
                numRecords = atoi(optarg);
                break;
            case 's':
                recordSize = atoi(optarg);
                break;
            case 'o':
                trace_file = optarg;
                break;
            default: /* '?' */
                abort();
        }
    }

    // Process non-option arguments here
    cout << numRecords << " " << recordSize << " " << trace_file << endl;

    return 0;
}
