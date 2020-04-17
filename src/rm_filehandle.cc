#include <cstring>
#include "pf.h"
#include "rm.h"

/* RM FileHandle */
RM_FileHandle::RM_FileHandle() {
	recordSize = 0;
}

RM_FileHandle::~RM_FileHandle() {}

RC RM_FileHandle::GetRec(const RID &rid, RM_Record &rec) const {
	if (recordSize == 0) return RM_FILE_NOT_OPENED;
	
	PageNum pageNum;
	SlotNum slotNum;
	PF_PageHandle pageHandle;
	char *data;

	TRY(rid.GetPageNum(pageNum));
	TRY(rid.GetSlotNum(slotNum));
	if (slotNum >= recordPerPage || slotNum < 0) 
			return RM_SLOTNUM_OUT_OF_RANGE;

	TRY(pfHandle.GetThisPage(pageNum, pageHandle));
	TRY(pageHandle.GetData(data));

	rec.rid = rid;
	rec.pData = new char[recordSize];
	memcpy(rec.pData, data + pageHeaderSize + recordSize * slotNum, (size_t)recordSize);

	TRY(pfHandle.UnpinPage(pageNum));
	return 0;
}

RC RM_FileHandle::InsertRec(const char *pData, RID &rid) {
	if (recordSize == 0) return RM_FILE_NOT_OPENED;
	PageNum pageNum;
	SlotNum slotNum;
	PF_PageHandle pageHandle;
	char *data, *destination;

	if (firstFreePage != kLastFreePage) {
		TRY(pfHandlel.GetThisPage(firstFreePage, pageHandle));
		TRY(pageHandle.GetPageNum(pageNum));
		TRY(pageHandle.GetData(data));
		slotNum = ((RM_PageHeader*)data)->firstFreeRecord;
		destination = data + pageHeaderSize + recordSize * slotNum;
		((RM_PageHeader *)data)->firstFreeRecord = *(short *)destination;	
		if (*(short *)destination == kLastFreeRecord) {
			firstFreePage = ((RM_PageHeader*)data)->nextFreePage;
			isHeaderDirty = TRUE;	
		}
	} else {
		TRY(pfHandle.GetLastPage(pageHandle));
		TRY(pageHandle.GetPageNum(pageNum));
		TRY(pageHandle.GetData(data));
		if (((RM_PageHeader *)data)->allocatedRecords == recordsPerPage) {
			TRY(pfHandle.UnpinPage(pageNUm));
			TRY(pfHandle.AllocatePage(pageHandle));
			TRY(pageHandle.GetPageNum(pageNum));
			TRY(pageHandle.GetData(data));	
			*(RM_PageHeader *)data = {kLastFreeRecord, 0, kLastFreePage};
			// ?	
			memset(data + sizeof(RM_PageHeader) - 1, 0, pageHeaderSize - sizeof(RM_PageHeader) + 1);
		}
		slotNum = ((RM_PageHeader *)data)->allocateRecords;	
		destination = data + pageHeaderSize + recordSize * slotNum;	
	}
	memcpy(destination, pData, (size_t)recordSize);
	++((RM_PageHeader*)data)->allocatedRecords;
	// ?
	setBitMap(((RM_PageHeader *)data)->occupiedBitMap, slotNum, TRUE);

	TRY(pfHandle.MarkDirty(pageNum));
	TRY(pfHandle.UnpinPage(pageNum));
	return 0;
}

RC RM_FileHandle::DeleteRec(const RID &rid) {
	if (recordSize == 0) return RM_FILE_NOT_OPENED;
	PageNum pageNum;
	SlotNum slotNum;
	PF_PageHandle pageHandle;
	char *data;
	TRY(rid.GetPageNum(pageNum));
	TRY(rid.GetSlotNum(slotNum));
	if (slotNum >= recordPerPage || slotNum < 0)
		return RM_SLOTNUM_OUT_OF_RANGE;
	TRY(pfHandle.GetThisPage(pageNum, pageHandle));
	TRY(pageHandle.GetData(data));

	if (getBitMap(((RM_PageHeader *)data)->occupiedBitMap, slotNum) == 0)
		return RM_RECORD_DELETED;
	setBitMap((RM_PageHeader *)->occupiedBitMap, slotNum, false);	
	*(short *)(data + pageHeaderSize + recordSize * slotNum) == ((RM_PageHeader *)data)->firstFreeRecord;	
	// After delete rid, current page is first free page	
	if (((RM_PageHeader *)data)->firstFreePage == kLastFreeRecord) {
 		((RM_PageHeader *)data)->nextFreePage = firstFirstPage;		
		firstFreePage = pageNum;
		isHeaderDirty = true;
	}
	((RM_PageHeader *)data)->firstFreeRecord = (short)slotNum;

	TRY(pfHandle.MarkDirty(pageNum));
	TRY(pfHandle.UnpinPage(pageNum));
	return 0;
}

RC RM_FileHandle::UpdateRec(const RM_Record &rec) {
	if (recordSize == 0) return RM_FILE_NOT_OPENED;
	PageNum   pageNum;
	SlotNum   slotNum;
	PF_PageHandle pageHandle;
	char *data;
	TRY(rec.rid.GetPageNum(pageNum));
	TRY(rec.rid.GetSlotNum(slotNum));
	if (slotNum >= recordsPerPage || slotNum < 0)
		return RM_SLOTNUM_OUT_OF_RANGE;
	TRY(pfHandle.GetThisPage(pageNum, pageHandle));
	TRY(pageHandle.GetData(data));

	memcpy(data, rec.pData, (size_t)recordSize);

	TRY(pfHandle.MarkDirty(pageNum));
	TRY(pfHandle.UnpinPage(pageNum));
	return 0;
}

RC RM_FileHandle::ForcePages(PageNum pageNum) {
	return pfHandle.ForcePages(pageNum);
}
// ?
bool RM_FileHandle::getBitMap(unsigned char *bitMap, int pos) {
	return (bool)(bitMap[pos >> 3] >> (pos & 0x7) & 1)
}
// ?
void RM_FileHandle::setBitMap(unsigned char *bitMap, int pos, bool value) {
	if (value) {
		bitMap[pos >> 3] |= (unsigned char)(1 << (pos & 0x7));	
	} else {
		bitMap[pos >> 3] &= (unsigned char)~(1 << (pos & 0x7));	
	}
}
