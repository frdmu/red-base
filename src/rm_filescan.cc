#include <cstring>
#include <cstdio>
#include "rm.h"
#include <cassert>
RM_FileScan::RM_FileScan() {
    scanOpened = FALSE;
}

RM_FileScan::~RM_FileScan() {}

// Initialize file scan
RC RM_FileScan::OpenScan(const RM_FileHandle &fileHandle, AttrType attrType, int attrLength, int attrOffset, CompOp compOp, void *value, ClientHint pinHint) {
    if (scanOpened) return RM_SCAN_NOT_CLOSED;

    this->fileHandle = &fileHandle;
    this->attrType = attrType;
    this->attrLength = attrLength;
    this->attrOffset = attrOffset;
    this->compOp = compOp;
    switch (attrType) {
        case INT:
            this->value.intVal = *((int *)value);
            break;
        case FLOAT:
            this->value.floatVal = *((float *)value);
            break;
        case STRING:
            if (value == NULL) this->value.stringVal = NULL;
            else {
                this->value.stringVal = new char[attrLength];
                memcpy(this->value.stringVal, value, (size_t)attrLength);
            }
            break;
    }

    char *data;
    PF_PageHandle pageHandle;
    scanOpened = true;
    TRY(fileHandle.pfHandle.GetFirstPage(pageHandle));
    TRY(pageHandle.GetData(data));
    recordSize = ((RM_FileHeader *)data)->recordSize;
    currentPageNum = 1; // ? Maybe page 0 just stands for "RM_FileHeader", and recording job starts from page 1
    currentSlotNum = 0;
    TRY(fileHandle.pfHandle.UnpinPage(0));

    return 0;
}

RC RM_FileScan::GetNextRec(RM_Record &rec) {
    if (!scanOpened) return RM_SCAN_NOT_OPENED;

    char *data;
    PF_PageHandle pageHandle;
    bool found = FALSE;
    TRY(fileHandle->pfHandle.GetThisPage(currentPageNum, pageHandle));
    // Retrieve all the records of certian page, all the pages of the file  util find the record that needed 
    while (1) {
        TRY(pageHandle.GetData(data));
        int cnt = ((RM_PageHeader *)data)->allocatedRecords;
        unsigned char *bitMap = ((RM_PageHeader *)data)->occupiedBitMap; 
        // ? 
        for (; currentSlotNum < cnt; ++currentSlotNum) {
            if (getBitMap(bitMap, currentSlotNum) == 0) continue;  // currentSlotNum record doesn't be allocated
            char *pData  = data + fileHandle->pageHeaderSize + recordSize * currentSlotNum;
            if (checkSatisfy(pData)) {
                found = TRUE;
                break;
            }
        }
        TRY(fileHandle->pfHandle.UnpinPage(currentPageNum));
        if (found) break;
        int rc = fileHandle->pfHandle.GetNextPage(currentPageNum, pageHandle);
        if (rc == PF_EOF) return RM_EOF;
        else if (rc != 0) return rc;
        TRY(pageHandle.GetPageNum(currentPageNum));
        currentSlotNum = 0;
    }

    rec.rid = RID(currentPageNum, currentSlotNum);
    rec.pData = new char[recordSize];
    memcpy(rec.pData, data + fileHandle->pageHeaderSize + recordSize * currentSlotNum, (size_t)recordSize);
    ++currentSlotNum;

    return 0;
}

// ??
RC RM_FileScan::CloseScan() {
	if (!scanOpened) return RM_SCAN_NOT_OPENED;
	if (attrType == STRING && value.stringVal != NULL)
		delete[] value.stringVal;
	return 0;
}

bool RM_FileScan::checkSatisfy(char *data) {
    switch(attrType) {
        case INT: {
            int curVal = *(int *)(data + attrOffset);    
            switch (compOp) {
                case NO_OP:
					return true;
				case EQ_OP:
					return curVal == value.intVal;
				case NE_OP:
					return curVal != value.intVal;
				case LT_OP:
					return curVal < value.intVal;
				case GT_OP:
					return curVal > value.intVal;
				case LE_OP:
					return curVal <= value.intVal;
				case GE_OP:
					return curVal >= value.intVal; 
            }
        }
        case FLOAT: {
            float curVal = *(float *)(data + attrOffset);
			switch (compOp) {
				case NO_OP:
					return true;
				case EQ_OP:
					return curVal == value.floatVal;
				case NE_OP:
					return curVal != value.floatVal;
				case LT_OP:
					return curVal < value.floatVal;
				case GT_OP:
					return curVal > value.floatVal;
				case LE_OP:
					return curVal <= value.floatVal;
				case GE_OP:
					return curVal >= value.floatVal;
			}         
                    
        }
        case STRING: {
            char *curVal = data + attrOffset;
			switch (compOp) {
				case NO_OP:
					return true;
				case EQ_OP:
					return strncmp(curVal, value.stringVal, (size_t)attrLength) == 0;
				case NE_OP:
					return strncmp(curVal, value.stringVal, (size_t)attrLength) != 0;
				case LT_OP:
					return strncmp(curVal, value.stringVal, (size_t)attrLength) < 0;
				case GT_OP:
					return strncmp(curVal, value.stringVal, (size_t)attrLength) > 0;
				case LE_OP:
					return strncmp(curVal, value.stringVal, (size_t)attrLength) <= 0;
				case GE_OP:
					return strncmp(curVal, value.stringVal, (size_t)attrLength) >= 0;
			}         
                     
        }
    }
    assert(0);
}
