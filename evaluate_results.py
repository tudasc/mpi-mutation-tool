#!/usr/bin/env python3

import pandas as pd
import glob
import sys


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
def parse_must_output(input_dir, is_error_expected, error_specification):
    error_found = -1
    correct_error_found = -1

    fname = input_dir + "/MUST_Output.html"

    try:
        # read must html report
        table = pd.read_html(fname)[0]
        #is_error = "MUST_ERROR" in table[1]
        is_error = table[1].str.contains("MUST_ERROR", na=False)
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
            #if row.lower().find(error_specification.lower()):
            #    correct_error_found = correct_error_found + 1

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

    except ValueError as e:
        #print(e)
        #print("could not read input file")
        #print(fname)
        pass

    return error_found, correct_error_found




# input:
# input_dir: the directory where the tool was run and output was captured
# is_error_expected: bool if an error is expected
# error_specification: information read from error-classification.json

# return values:
# error_found count off errors found
# -1 means could not tell (e.g. output is missing or not helpful)
# -2 means warning only, no error

#  is_error_expected=True the output correct_error_found is ignored
def parse_itac_output(input_dir, is_error_expected, error_specification):
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


if __name__ == '__main__':

    if sys.argv[1] == "ITAC":
        parser = parse_itac_output
    else:
        parser = parse_must_output

    if sys.argv[2] == "0":
        level = 0
    else:
        level = 2

    if level == 2:
        dirs = glob.glob("*.patch")
    else: 
        dirs = glob.glob("*")

    for d in dirs:
        num_errors, _  = parser(d, True, None)

        if level == 2:
            d_split = d.split('.')
            mutation = d_split[0]
            number = d_split[1]

            print(f"{mutation}, {number}, {num_errors > 0}")

        elif level == 0:
            with open(f'{d}/case_name') as f:
                case_name = f.read().strip()
                print(f"{case_name}, {num_errors > 0}")
