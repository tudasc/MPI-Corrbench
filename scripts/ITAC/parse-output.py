# TODO documentation at different location

# input:
# input_dir: the directory where the tool was run and output was captured
# is_error_expected: bool if an error is expected
# error_specification: information read from error-classification.json

# return values:
# error_found count off errors found
# -1 means could not tell (e.g. output is missing or not helpful)
# -2 means warning only, no error

#  is_error_expected=True the output correct_error_found is ignored
def parse_output(input_dir, is_error_expected, error_specification):
    error_found = -1
    correct_error_found = -1

    fname = input_dir + "/output.txt"
    try:
        with open(fname, mode='r') as file:
            data = file.read().replace('\n', '')

            if (data != "" and "ERROR:" in data):
                # found something
                # explicitly not include an ITAC segfault
                if not "ERROR: Signal 11 caught in ITC code section." in data:
                    error_found = 1
            elif (data != "" and "WARNING:" in data):
                # found warning opnly
                error_found =-2
            elif (
                    data != "" and "INFO: Error checking completed without finding any problems." in data):
                # found nothing
                error_found = 0
    except UnicodeDecodeError:
        print("Error: UnicodeDecodeError while reading file %s (ignoring case)" % (fname))

    return error_found, correct_error_found
