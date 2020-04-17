#include "pf.h"
#include "rm.h"

/* RM Manager */
RM_Manager::RM_Manager(PF_Manager &pfm) {
	this->pfm = pfm;	
}


RM_Manager::~RM_Manager() {}


RC RM_Manager::CreateFile(const char *fileName, int recordSize) {
	if (recordSize > PF_PAGE_SIZE) {
		return RM_RECORDSIZE_TOO_LARGE;	
	}	
	pfm->CreateFile(fileName);
	// initialize header
	PF_FileHandle fileHandle;
	PF_PageHandle pageHandle;
	char *data;
	TRY(pfm->OpenFile(fileName, fileHandle));
	TRY(fileHandle.AllocatePage(pageHandle));
	TRY(pageHandle.GetData(data));

	short recordsPerPage = (PF_PAGE_SIZE - sizeof(RM_PageHeader)) / (recordSize + 1);
	if (((recordPerPage + 3) & (~0x3)) + sizeof(RM_PageHeader) +
					    recordSize * recordsPerPage > PF_PAGE_SIZE)
		--recordsPerPage;
	*(RM_FileHeader *)data = {(short)recordSize, recordsPerPage, kLastFreePage};

	TRY(fileHandle.MarkDirty(0));
	TRY(fileHandle.UnpinPage(0));
	TRY(pfm->CloseFile(fileHandle));
	return 0;
}


RC RM_Manager::DestoryFile(const char *fileName) {
	return pfm->DestoryFile(fileName);
}


RC RM_Manager::OpenFile(const char *fileName, RM_FileHandle &fileHandle) {
	PF_FileHandle pfHandle;
	TRY(pfm->OpenFile(fileName, pfHandle));
	fileHandle.pfHandle = pfHandle;
	PF_PageHandle pageHandle;
	char *data;
	TRY(pfHandle.GetFirstPage(pageHandle));
	TRY(pageHandle.GetData(data));

	fileHandle.recordSize = ((RM_FileHeader *)data)->recordSize;
	fileHandle.recordPerPage = ((RM_FileHeader *)data)->recordPerPage;
	fileHandle.firstFreePage = ((RM_FileHeader *)data)->firstFreePage;
	fileHandle.isHeaderDirty = false;
	fileHandle.pageHeaderSize = sizeof(RM_PageHeader) + ((fileHandle.recordsPerPage + 3) & (~0x3));

	TRY(pfHandle.UnpinPage(0));
	return 0;
}
    

RC RM_Manager::CloseFile(RM_FileHandle &fileHandle) {
	if (fileHandle.isHeaderDirty) {
		PF_PageHandle pageHandle;
		char *data;
		TRY(fileHandle.pfHandle.GetFirstPage(pageHandle));
		TRY(pageHandle.GetData(data));

		*(RM_FileHeader *)data = {fileHandle.recordSize,
				fileHandle.recordsPerPage,
				fileHandle.firstFreePage};	
		TRY(fileHandle.pfHandle.MarkDirty(0));
		TRY(fileHandle.pfHandle.UnpinPage(0));
	}
	TRY(pfm->CloseFile(fileHandle.pfHandle));
	return 0;
}
