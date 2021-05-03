# TODO documentation at different location

# input:
# input_dir: the directory where the tool was run and output was captured
# is_error_expected: bool if an error is expected
# error_specification: information read from error-classification.json

# return values:
# error_found count off errors found
# -1 means could not tell (e.g. output is missing or not helpful)
# correct_error_found wether or not exactely the expected error was found
# -1 means could not tell (e.g. output is missing or not helpful or error specification missing) 0 means false, 1 true

#  is_error_expected=True the output correct_error_found is ignored
def parse_output(input_dir, is_error_expected, error_specification):
    error_found = -1
    correct_error_found = -1

    fname = input_dir + "/output.txt"


    with open(fname, mode='r') as file:
        data = file.read().replace('\n', '').lower()

        if (data != "" and ("mpi-type-mismatch" in data
                            or "deadlock" in data
                            or "mpi-buffer-deref" in data
                            or "argument invalid" in data
                            or "no matching wait" in data
                            or "buffer misused" in data
                            or "missing commit" in data
                            or "clang-analyzer-optin.mpi.MPI-Checker" in data)):
            # found something
            error_found = 1

        else:
            # found nothing
            error_found = 0

    return error_found, correct_error_found


# debugging code:
if __name__ == '__main__':
    dir = "/home/tj75qeje/mpi-correctness-benchmarks/output/MUST/ArgError-MPISend-Count-2.c"
    error_found, correct_error_found = parse_output(dir, True, "Argument 5")
    print(error_found)
    print(correct_error_found)
