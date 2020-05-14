#include "ix.h"

#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cassert>
#include <unistd.h>


const char* kFileName = "ixt";

//
// Function declarations
//
RC Test1(void);
RC Test2(void);
RC Test3(void);

int (*tests[])() =                      // RC doesn't work on some compilers
{
    Test1,
    Test2,
	Test3,
};
#define NUM_TESTS       ((int)((sizeof(tests)) / sizeof(tests[0])))    // number of tests

PF_Manager pfm;
IX_Manager ixm(pfm);

static void Test_PrintError(RC rc)
{
    if (abs(rc) <= END_PF_WARN)
        PF_PrintError(rc);
    else if (abs(rc) <= END_RM_WARN)
        IX_PrintError(rc);
    else
       	printf("Error code out of range: %d\n", rc); 
}

int main(int argc, char *argv[]) {
    FLAGS_logtostderr = true;
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    google::InitGoogleLogging(argv[0]);

    RC   rc;
    char *progName = argv[0];   // since we will be changing argv
    int  testNum;

    // Delete files from last time
    system((std::string("rm ") + kFileName + ".*").c_str());

    // If no argument given, do all tests
    if (argc == 1) {
        for (testNum = 0; testNum < NUM_TESTS; testNum++)
            if ((rc = (tests[testNum])())) {

                // Print the error and exit
                Test_PrintError(rc);
                return (1);
            }
    }
    else {

        // Otherwise, perform specific tests
        while (*++argv != NULL) {

            // Make sure it's a number
            if (sscanf(*argv, "%d", &testNum) != 1) {
               	printf("%s: %s is not a number\n", progName, *argv); 
				//LOG(INFO) << progName << ": " << *argv << " is not a number\n";
                continue;
            }

            // Make sure it's in range
            if (testNum < 1 || testNum > NUM_TESTS) {
               	printf("Valid test numbers are between 1 and %d\n", NUM_TESTS); 
                continue;
            }

            // Perform the test
            if ((rc = (tests[testNum - 1])())) {

                // Print the error and exit
                Test_PrintError(rc);
                return (1);
            }
        }
    }

    // Write ending message and exit
 	printf("Ending IX component test.\n\n"); 

    return 0;
}

inline RC check_rid_eq(const RID &lhs, const RID &rhs) {
    PageNum a, b;
    SlotNum u, v;
    TRY(lhs.GetPageNum(a));
    TRY(rhs.GetPageNum(b));
    TRY(lhs.GetSlotNum(u));
    TRY(rhs.GetSlotNum(v));
    CHECK(a == b);
    CHECK(u == v);
    return 0;
}

RC Test1() {
   	printf("test1\n"); 
	IX_IndexHandle ih;
    // CHECK(ixm.CloseIndex(ih) != 0);
    TRY(ixm.CreateIndex(kFileName, 0, INT, 4));
    TRY(ixm.OpenIndex(kFileName, 0, ih));
    // CHECK(ixm.DestroyIndex(kFileName, 1) != 0);
    TRY(ixm.CloseIndex(ih));
    TRY(ixm.DestroyIndex(kFileName, 0));
    return 0;
}

RC Test2() {
   	printf("test2\n"); 
	IX_IndexHandle ih;
    TRY(ixm.CreateIndex(kFileName, 1, INT, 4));
    TRY(ixm.OpenIndex(kFileName, 1, ih));
    RID rid0(0, 0), rid1(1, 1);
    int n0 = 4, n1 = 1;
    TRY(ih.InsertEntry(&n0, rid0));
    TRY(ih.InsertEntry(&n1, rid1));
    // now the index contains (1, (1, 1)), (4, (0, 0))

    IX_IndexScan sc;
    TRY(sc.OpenScan(ih, NO_OP, NULL));
    RID rid;

    TRY(sc.GetNextEntry(rid));
    TRY(check_rid_eq(rid, rid1));
    TRY(sc.GetNextEntry(rid));
    TRY(check_rid_eq(rid, rid0));
    CHECK(sc.GetNextEntry(rid) == IX_EOF);
    TRY(sc.CloseScan());

    int eqcmpi = 1;
    TRY(sc.OpenScan(ih, EQ_OP, &eqcmpi));
    TRY(sc.GetNextEntry(rid));
    TRY(check_rid_eq(rid, rid1));
    CHECK(sc.GetNextEntry(rid) == IX_EOF);
    TRY(sc.CloseScan());

    int necmpi = 1;
    TRY(sc.OpenScan(ih, NE_OP, &necmpi));
    TRY(sc.GetNextEntry(rid));
    TRY(check_rid_eq(rid, rid0));
    CHECK(sc.GetNextEntry(rid) == IX_EOF);
    TRY(sc.CloseScan());

    int gecmpi = 4;
    TRY(sc.OpenScan(ih, GE_OP, &gecmpi));
    TRY(sc.GetNextEntry(rid));
    TRY(check_rid_eq(rid, rid0));
    CHECK(sc.GetNextEntry(rid) == IX_EOF);
    TRY(sc.CloseScan());

    int gtcmpi = 1;
    TRY(sc.OpenScan(ih, GT_OP, &gtcmpi));
    TRY(sc.GetNextEntry(rid));
    TRY(check_rid_eq(rid, rid0));
    CHECK(sc.GetNextEntry(rid) == IX_EOF);
    TRY(sc.CloseScan());

    int ltcmpi = 4;
    TRY(sc.OpenScan(ih, LT_OP, &ltcmpi));
    TRY(sc.GetNextEntry(rid));
    TRY(check_rid_eq(rid, rid1));
    CHECK(sc.GetNextEntry(rid) == IX_EOF);
    TRY(sc.CloseScan());

    int lecmpi = 1;
    TRY(sc.OpenScan(ih, LE_OP, &lecmpi));
    TRY(sc.GetNextEntry(rid));
    TRY(check_rid_eq(rid, rid1));
    CHECK(sc.GetNextEntry(rid) == IX_EOF);
    TRY(sc.CloseScan());

    int lecmpi2 = 4;
    TRY(sc.OpenScan(ih, LE_OP, &lecmpi2));
    TRY(sc.GetNextEntry(rid));
    TRY(check_rid_eq(rid, rid1));
    TRY(sc.GetNextEntry(rid));
    TRY(check_rid_eq(rid, rid0));
    CHECK(sc.GetNextEntry(rid) == IX_EOF);
    TRY(sc.CloseScan());

    TRY(ixm.CloseIndex(ih));
    TRY(ixm.DestroyIndex(kFileName, 1));
    return 0;
}

RC Test3() {
   	printf("test3\n"); 
	IX_IndexHandle ih;
    TRY(ixm.CreateIndex(kFileName, 1, INT, 4));
    TRY(ixm.OpenIndex(kFileName, 1, ih));

    auto rid_gen = [](int n) {
        return RID(n, n);
    };

    const int n = 9;

    for (int i = 0; i < n; ++i) {
       	printf("insertentry i = %d\n", i); 
        TRY(ih.InsertEntry(&i, rid_gen(i)));
    }

    IX_IndexScan sc;
    TRY(sc.OpenScan(ih, NO_OP, NULL));
    RID rid;

    for (int i = 0; i < n; ++i) {
       	printf("checkentry i = %d\n", i); 
        TRY(sc.GetNextEntry(rid));
        TRY(check_rid_eq(rid, rid_gen(i)));
    }
    CHECK(sc.GetNextEntry(rid) == IX_EOF);
    TRY(sc.CloseScan());

    TRY(ixm.CloseIndex(ih));
    TRY(ixm.DestroyIndex(kFileName, 1));
    return 0;
}
