#include "rm.h"

RM_Record::RM_Record() {
    pData = NULL;
}

RM_Record::~RM_Record() {
    if (pData != NULL)    
        delete[] pData;
}

void RM_Record::SetData(char *data, size_t size) {
    if (pData != NULL)
        delete [] pData;
    pData = new char[size];
    memcpy(pData, data, size);
}
RC RM_Record::GetData(char *&pData) const {
    pData = this->pData;
    return pData == NULL ? RM_UNINITIALIZED_RECORD : 0;
}

RC RM_Record::GetRid(RID &rid) const {
    rid = this->rid;
    return pData == NULL ? RM_UNINITIALIZED_RECORD : 0;
}
