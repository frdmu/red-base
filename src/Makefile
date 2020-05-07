#
# Makefile for the RedBase project.
#
# Author: Hyunjung Park (hyunjung@cs.stanford.edu)
#


#
# Global variables
#
CC             = g++
BUILD_DIR      = ../build/
LIB_DIR        = ../lib/
INC_DIRS       = -I.
AR             = ar -qc
RANLIB         = ranlib

# -g - Debugging information
# -O1 - Basic optimization
# -Wall - All warnings
# -DDEBUG_PF - This turns on the LOG file for lots of BufferMgr info
CFLAGS         = -g -O1 -Wall $(STATS_OPTION) $(INC_DIRS)

# The STATS_OPTION can be set to -DPF_STATS or to nothing to turn on and
# off buffer manager statistics.  The student should not modify this
# flag at all!
STATS_OPTION   = -DPF_STATS

#
# Students: Please modify SOURCES variables as needed.
#
PF_SOURCES     = pf_buffermgr.cc pf_error.cc pf_filehandle.cc \
                 pf_pagehandle.cc pf_hashtable.cc pf_manager.cc \
				 pf_statistics.cc statistics.cc
TESTER_SOURCES = pf_test1.cc pf_test2.cc pf_test3.cc 

PF_OBJECTS     = $(addprefix $(BUILD_DIR), $(PF_SOURCES:.cc=.o))
TESTER_OBJECTS = $(addprefix $(BUILD_DIR), $(TESTER_SOURCES:.cc=.o))

LIBRARY_PF     = $(LIB_DIR)libpf.a

TESTS          = $(TESTER_SOURCES:.cc=)

LIBS 			= -lpf

#
# Build targets
#

testers: clean $(TESTS)
	
clean:
	rm -f $(BUILD_DIR)*.o $(LIBRARY_PF)*.a $(TESTS)


$(TESTS): %: $(BUILD_DIR)%.o $(LIBRARY_PF)
	$(CC) $(CFLAGS) $< -o $@ -L$(LIB_DIR) $(LIBS)



$(TESTER_OBJECTS): $(BUILD_DIR)%.o: %.cc
	$(CC) -c $< -o $@
#
# Libraries
#
$(LIBRARY_PF): $(PF_OBJECTS)
	$(AR) $(LIBRARY_PF) $(PF_OBJECTS)
	$(RANLIB) $(LIBRARY_PF)

$(PF_OBJECTS): $(BUILD_DIR)%.o: %.cc
	$(CC) $(CFLAGS) -c $< -o $@
