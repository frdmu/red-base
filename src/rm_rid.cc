#include "rm.h"
//#include "rm_rid.h"

static const int uninitializeRidNums = -1;

RID::RID() {
	pageNum = uninitializeRidNums;
	slotNum = uninitializeRidNums;
}

RID::RID(PageNum pageNum, SlotNum slotNum) {
	this->pageNum = pageNum;
	this->slotNum = slotNum;	
}

RID::~RID() {}

RC RID::GetPageNum(PageNum &pageNum) const {
	pageNum = this->pageNum;
	return pageNum == uninitializeRidNums ? RM_UNINITIALIZED_RID : 0;
}

RC RID::GetSlotNum(SlotNum &slotNum) const {
	slotNum = this->slotNum;
	return slotNum == uninitializeRidNums ? RM_UNINITIALIZED_RID: 0;
}

