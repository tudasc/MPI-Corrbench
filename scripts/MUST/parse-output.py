import pandas as pd


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
# -2 means warning only, no error

#  is_error_expected=True the output correct_error_found is ignored
def parse_output(input_dir, is_error_expected, error_specification):
    error_found = -1
    correct_error_found = -1

    fname = input_dir + "/MUST_Output.html"

    try:
        # read must html report
        table = pd.read_html(fname)[0]
        is_error = table[1] == "Error"
        errors_only = table[is_error]
        # if no exeption was thrown, the report was read (it my be empty though)
        # index 2 is the short error message
        for row in errors_only[2]:
            if error_found == - 1:
                # start counting errors at 0
                error_found = 0
                correct_error_found = 0
            error_found = error_found + 1
            # search for the specificated string in the error message
            if row.lower().find(error_specification.lower()):
                correct_error_found = correct_error_found + 1

        # check if must has completed with warnings
        if error_found == -1:
            # MUST has completed successfully
            is_warning = table[1] == "Warning"
            warns_only = table[is_warning]

            if len(warns_only) > 0:
                error_found = -2

        # check if must has completed without detecting errors
        if error_found == -1:
            # MUST has completed successfully
            is_info = table[1] == "Information"
            infos_only = table[is_info]

            for row in infos_only[2]:
                if "MUST detected no MPI usage errors" in row:
                    error_found = 0

    except ValueError:
        print("could not read input file")
        print(fname)

    return error_found, correct_error_found


# debugging code:
if __name__ == '__main__':
    dir = "/home/tj75qeje/mpi-correctness-benchmarks/output/MUST/ArgError-MPISend-Count-2.c"
    error_found, correct_error_found = parse_output(dir, True, "Argument 5")
    print(error_found)
    print(correct_error_found)
