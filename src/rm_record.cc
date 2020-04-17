#include "rm.h"

RM_Record::~RM_Record() {
    delete[] pData;
}
