#
# MAIN CONFIGURATION
#

#sequential execution
EXEC = stats benchmark visualization sudoku_remake
OBJECTS = utils.o sudoku.o colors.o functions.o window.o
#parallel mpi execution
EXEC_MPI = stats benchmark sudoku_mpi
OBJECTS_MPI = utils.o sudoku.o
#parallel openmp version
EXEC_OMP = stats benchmark sudoku_omp
OBJECTS_OMP = utils.o sudoku.o

PROJECT_NAME = SUDOKU_SOLVER

SRC_DIR = src
OBJECTS_DIR = obj
INCLUDE_DIR = includes
BIN_DIR = bin

#
# SUFFIXES (must not change it)
#

.SUFFIXES: .c .o

#
# OBJECTS (must not change it)
#

EXEC_O = $(EXEC:=.o)
OBJECTS_O = $(OBJECTS) $(EXEC_O)

EXEC_MPI_O = $(EXEC_MPI:=.o)
OBJECTS_MPI_O = $(OBJECTS_MPI) $(EXEC_MPI_O)

EXEC_OMP_O = $(EXEC_OMP:=.o)
OBJECTS_OMP_O = $(OBJECTS_OMP) $(EXEC_OMP_O)

#
# ARGUMENTS AND COMPILER (to configure)
#

CC = gcc
CCFLAGS_STD = -Wall -O3 -fopenmp 
CCFLAGS_DEBUG = -D _DEBUG_
CCFLAGS_SHOW = -D _SHOW_
CCFLAGS_PARALLEL = -D _PARALLEL_
CCFLAGS = $(CCFLAGS_STD)
CCLIBS = -fopenmp -lm -lncurses

#
# RULES (must not change it)
#

all: msg $(addprefix $(OBJECTS_DIR)/,$(OBJECTS)) $(addprefix $(OBJECTS_DIR)/,$(EXEC_O))
	@echo "Create executables..."
	@for i in $(EXEC); do \
	$(CC) -o $(addprefix $(BIN_DIR)/,$$i) $(addprefix $(OBJECTS_DIR)/,$$i.o) $(addprefix $(OBJECTS_DIR)/,$(OBJECTS)) $(CCLIBS) -I../$(INCLUDE_DIR)/; \
	done
	@echo "Done."

msg:
	@echo "Create objects..."

debug: CCFLAGS = $(CCFLAGS_STD) $(CCFLAGS_DEBUG)
debug: all

show: CCFLAGS = $(CCFLAGS_STD) $(CCFLAGS_SHOW)
show: all

parallel: CCFLAGS = $(CCFLAGS_STD) $(CCFLAGS_PARALLEL)
parallel: all

omp: msg $(addprefix $(OBJECTS_DIR)/,$(OBJECTS_OMP)) $(addprefix $(OBJECTS_DIR)/,$(EXEC_OMP_O))
	@echo "Create executables..."
	@for i in $(EXEC_OMP); do \
	$(CC) -o $(addprefix $(BIN_DIR)/,$$i) $(addprefix $(OBJECTS_DIR)/,$$i.o) $(addprefix $(OBJECTS_DIR)/,$(OBJECTS_OMP)) $(CCLIBS) -I../$(INCLUDE_DIR)/; \
	done
	@echo "Done."

mpi: CC = mpicc
mpi: msg $(addprefix $(OBJECTS_DIR)/,$(OBJECTS_MPI)) $(addprefix $(OBJECTS_DIR)/,$(EXEC_MPI_O))
	@echo "Create executables..."
	@for i in $(EXEC_MPI); do \
	$(CC) -o $(addprefix $(BIN_DIR)/,$$i) $(addprefix $(OBJECTS_DIR)/,$$i.o) $(addprefix $(OBJECTS_DIR)/,$(OBJECTS_MPI)) $(CCLIBS) -I../$(INCLUDE_DIR)/; \
	done
	@echo "Done."

omp_mpi: CCFLAGS = $(CCFLAGS_STD) $(CCFLAGS_PARALLEL)
	CC = mpicc
omp_mpi: msg $(addprefix $(OBJECTS_DIR)/,$(OBJECTS_MPI)) $(addprefix $(OBJECTS_DIR)/,$(EXEC_MPI_O))
	@echo "Create executables..."
	@for i in $(EXEC_MPI); do \
	$(CC) -o $(addprefix $(BIN_DIR)/,$$i) $(addprefix $(OBJECTS_DIR)/,$$i.o) $(addprefix $(OBJECTS_DIR)/,$(OBJECTS_MPI)) $(CCLIBS) -I../$(INCLUDE_DIR)/; \
	done
	@echo "Done."
$(info $$CCFLAGS is [${CCFLAGS}])
$(info $$CC is [${CC}])
#
# DEFAULT RULES (must not change it)
#

$(addprefix obj/,%.o) : $(addprefix src/,%.c)
	@${CC} ${CCFLAGS} -c $< -o $@ -I$(INCLUDE_DIR)/

#
# MAIN RULES (must not change it)
#

# You can add your own commands
clean:
	@echo "Delete objects, temporary files..."
	@rm -f $(addprefix $(OBJECTS_DIR)/,$(OBJECTS_O))
	@rm -f $(addprefix $(OBJECTS_DIR)/,$(EXEC_O))
	@rm -f $(addprefix $(OBJECTS_DIR)/,$(OBJECTS_MPI_O))
	@rm -f $(addprefix $(OBJECTS_DIR)/,$(EXEC_MPI_O))
	@rm -f $(addprefix $(OBJECTS_DIR)/,$(OBJECTS_OMP_O))
	@rm -f $(addprefix $(OBJECTS_DIR)/,$(EXEC_OMP_O))
	@rm -f $(addprefix $(OBJECTS_DIR)/,*~) $(addprefix $(OBJECTS_DIR)/,*#)
	@rm -f $(addprefix $(INCLUDE_DIR)/,*~) $(addprefix $(INCLUDE_DIR)/,*#)
	@rm -f $(addprefix $(BIN_DIR)/,$(EXEC))
	@rm -f $(addprefix $(BIN_DIR)/,$(EXEC_MPI))
	@rm -f $(addprefix $(BIN_DIR)/,$(EXEC_OMP))
	@rm -f dependancies
	@echo "Done."

depend:
	@echo "Create dependancies..."
	@sed -e "/^# DEPENDANCIES/,$$ d" makefile > dependancies
	@echo "# DEPENDANCIES" >> dependancies
	@for i in $(OBJECTS_O); do \
	$(CC) -MM -MT $(addprefix $(OBJECTS_DIR)/,$$i) $(CCFLAGS) $(SRC_DIR)/`echo $$i | sed "s/\(.*\)\\.o$$/\1.c/"` -I$(INCLUDE_DIR) >> dependancies; \
	done
	@cat dependancies > makefile
	@rm dependancies
	@echo "Done."

#
# CREATE ARCHIVE (must not modify)
#

ARCHIVE_FILES = *

archive: clean
	@echo "Create archive $(PROJECT_NAME)_$(shell date '+%y%m%d.tar.gz')..."
	@REP=`basename "$$PWD"`; cd .. && tar zcf $(PROJECT_NAME)_$(shell date '+%y%m%d.tar.gz') $(addprefix "$$REP"/,$(ARCHIVE_FILES))
	@echo "Done."# DEPENDANCIES