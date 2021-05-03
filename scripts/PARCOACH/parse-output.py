import re


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

    try:
        with open(fname, mode='r') as file:
            data = file.read().replace('\n', ' ').lower()

            warns = re.findall(r'\d+ warning\(s\) issued', data)
            if warns:
                found = 0
                for warn in warns:
                    n = int(warn.split()[0])

                    found += n

                if found > 0:
                    # found something
                    error_found = 1
                else:
                    # meaning the text 0 warnings issued was present
                    assert found == 0
                    # found nothing
                    error_found = 0

    except FileNotFoundError:
        print("Error: FileNotFoundError for file %s (ignoring case)" % (fname))

    return error_found, correct_error_found
