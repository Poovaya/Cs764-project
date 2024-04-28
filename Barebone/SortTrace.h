#pragma once

#include <string>

#include "defs.h"
using namespace std;

class SortTrace {
   public:
    SortTrace(string trace_file);
    void append_trace(string trace);
    void truncate_trace();

   private:
    char trace_file[256 + 1];
};