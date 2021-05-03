# MPI Checker
TIDY_OBJS = ${FILES:.c=.ct}

$(TIDY_OBJS): %.ct: %.c
	clang-tidy $< -checks='*mpi*' -- ${CFLAGS} $(mpicc --showme:compile)

tidy-target: $(TIDY_OBJS)
	@echo "tidy target"
	@echo "Running tidy on $(TIDY_OBJS)"


