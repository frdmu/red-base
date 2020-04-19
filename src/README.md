<h1> TEST  </h1>

<h2>Test PF Component...</h2>
<div>
// build pf_test1 <br>
$ g++ -c pf_test1.cc -o pf_test1.o <br>
$ g++ -c pf_manager.cc -o pf_manager.o <br>
$ g++ -c pf_filehandle.cc -o pf_filehandle.o <br>
$ g++ -c pf_pagehandle.cc -o pf_pagehandle.o <br>
$ g++ -c pf_buffermgr.cc -o pf_buffermgr.o <br>
$ g++ -c pf_hashtable.cc -o pf_hashtable.o <br>
$ g++ -c pf_error.cc -o pf_error.o <br>
$ g++ pf_test1.o pf_manager.o pf_filehandle.o pf_pagehandle.o pf_buffermgr.o pf_hashtable.o pf_error.o -o test1 <br>
// run test1 <br>
$ ./test1<br>
</div>

<div>
// build pf_test2 <br>
$ g++ -c pf_test2.cc -o pf_test2.o <br>
$ g++ pf_test2.o pf_manager.o pf_filehandle.o pf_pagehandle.o pf_buffermgr.o pf_hashtable.o pf_error.o -o test2 <br>
// run test2 <br>
$ ./test2<br>
</div>

<div>
// build pf_test3 <br>
$ g++ -c pf_test3.cc -o pf_test3.o <br>
$ g++ pf_test3.o pf_manager.o pf_filehandle.o pf_pagehandle.o pf_buffermgr.o pf_hashtable.o pf_error.o -o test3 <br>
// run test3 <br>
$ ./test3<br>
</div>

<h2>Test RM Component ...</h2>
<div>
// build rm_test <br>
$g++ -c rm_test.cc -o rm_test.o <br>
$g++ -c rm_rid.cc -o rm_rid.o <br>
$g++ -c rm_record.cc -o rm_record.o<br>
$g++ -c rm_filehandle.cc -o rm_filehandle.o <br>
$g++ -c rm_filescan.cc -o rm_filescan.o <br>
$g++ -c rm_manager.cc -o rm_manager.o <br>
$g++ -c rm_error.cc -o rm_error.o <br>
$g++ rm_test.o rm_filescan.o rm_manager.o rm_filehandle.o rm_record.o  <br>
 rm_rid.o rm_error.o pf_manager.o pf_filehandle.o pf_pagehandle.o pf_buffermgr.o  <br>
 pf_hashtable.o pf_error.o -o rm_test <br>
</div>
<div>
// run rm_test <br>
$./rm_test          // test all the sample<br>
$./rm_test 1        // test the sample 1<br>
$./rm_test 2        // test the sample 2<br>
$./rm_test 3        // test the sample 3<br>
$./rm_test 4        // test the sample 4<br>
$./rm_test 5        // test the sample 5<br>
$./rm_test 6        // test the sample 6<br>
</div>
