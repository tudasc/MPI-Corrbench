#!/usr/bin/env python

# populates the error-cases.json
# by collecting error specifications from the *.c files in ./micro-benches

import os
import json

ARG_ERROR = 'ArgError-'
ARG_MISMATCH = 'ArgMismatch-'
MISPLACED_CALL = 'MisplacedCall-'
MISSING_CALL = 'MissingCall-'

def main():
    dir_name = "./micro-benches"
    levels = ["0-level", "1-level", "2-level"]
    categories = ["coll", "pt2pt", "rma", "usertypes"]
    all_files = []
    notes_str= '< Currently none >'
    total_errs = 0
    total_corrs = 0
    the_total = 0
    total_tcs = {}
    for c in categories:
      total_tcs[c] = {}

    # Print overview of the number of test cases for the different categories
    base_dir = dir_name + '/' + levels[0]
    for cat in categories:
      tcs = {ARG_ERROR: 0, ARG_MISMATCH: 0, MISPLACED_CALL: 0, MISSING_CALL: 0}
      file_names = os.listdir(base_dir + '/' + cat)
      file_names.sort()

      for f in file_names:
        all_files.append(base_dir + '/' + cat + '/' + f)
        found = False
        if ARG_ERROR in f:
          tcs[ARG_ERROR] = tcs[ARG_ERROR] + 1
          found = True
        if ARG_MISMATCH in f:
          tcs[ARG_MISMATCH] = tcs[ARG_MISMATCH] + 1
          found = True
        if MISPLACED_CALL in f:
          tcs[MISPLACED_CALL] = tcs[MISPLACED_CALL] + 1
          found = True
        if MISSING_CALL in f:
          tcs[MISSING_CALL] = tcs[MISSING_CALL] + 1
          found = True

        if not found:
          print('Did not find: ' + base_dir + '/' + cat + '/' + f)

      total_tcs[cat][ARG_ERROR] = tcs[ARG_ERROR]
      total_tcs[cat][ARG_MISMATCH] = tcs[ARG_MISMATCH]
      total_tcs[cat][MISPLACED_CALL] = tcs[MISPLACED_CALL]
      total_tcs[cat][MISSING_CALL] = tcs[MISSING_CALL]
      print('=== Category: {cat} ===\n{the_dict}\n >> All cases should statically determinable.'.format(cat=cat,the_dict=tcs))
   
    # Code Duplication // FIXME
    for cat in categories:
      tcs = {ARG_ERROR: 0, ARG_MISMATCH: 0, MISPLACED_CALL: 0, MISSING_CALL: 0}
      file_names = os.listdir(base_dir + '/conflo/' + cat)
      file_names.sort()

      for f in file_names:
        found = False
        if ARG_ERROR in f:
          tcs[ARG_ERROR] = tcs[ARG_ERROR] + 1
          total_tcs[cat][ARG_ERROR] = total_tcs[cat][ARG_ERROR] + 1
          found = True
        if ARG_MISMATCH in f:
          tcs[ARG_MISMATCH] = tcs[ARG_MISMATCH] + 1
          total_tcs[cat][ARG_MISMATCH] = total_tcs[cat][ARG_MISMATCH] + 1
          found = True
        if MISPLACED_CALL in f:
          tcs[MISPLACED_CALL] = tcs[MISPLACED_CALL] + 1
          total_tcs[cat][MISPLACED_CALL] = total_tcs[cat][MISPLACED_CALL] + 1
          found = True
        if MISSING_CALL in f:
          tcs[MISSING_CALL] = tcs[MISSING_CALL] + 1
          total_tcs[cat][MISSING_CALL] = total_tcs[cat][MISSING_CALL] + 1
          found = True
        
        if not found:
          print('Did not find: ' + base_dir + '/conflo/' + cat + '/' + f)

      print('=== Category: {cat} ===\n{the_dict}\n >> The erroneous state depends on the value of argv[1].'.format(cat=cat,the_dict=tcs))

    # Code Duplication // FIXME
    for cat in ['coll', 'datatype', 'pt2pt', 'rma']:
      file_names = os.listdir(base_dir + '/correct/' + cat)
      file_names.sort()
      total_corrs = total_corrs + len(file_names)
      print('=== Category: {cat} ===\nCorrect cases: {num_tcs}'.format(cat=cat,num_tcs=len(file_names)))

    sum_str = ''
    for cd in total_tcs:
      sum_str = sum_str + '[' + cd + ']\t' + str(total_tcs[cd]) + '\n'
      for k in total_tcs[cd]:
        total_errs = total_errs + total_tcs[cd][k]
    the_total = total_corrs + total_errs
    print('\n\n>>> ==== SUMMARY of MPI CorrBench Test cases ==== <<<\n\n{summary}\n[NOTES]:\n{notes}\n\n===== >>> Total number of test cases: {total} | Errors/Correct: {errs}/{corrs} <<< ====\n'.format(summary=sum_str, total=the_total, notes=notes_str, errs=total_errs, corrs=total_corrs))


    # Obtain the error descriptions // FIXME
    file_names = os.listdir(dir_name)
    file_names.sort()

    print("Found " + str(len(file_names)) + " error case files in " + dir_name)

    error_cases = {}

    for file_name in all_files:
        print('The current file ' + file_name)
        error_cases[file_name] = {}
        error_cases[file_name]["ITAC"] = ""
        error_cases[file_name]["MPI-Checker"] = ""
        error_cases[file_name]["MUST"] = ""

        # read the file to discover error specification from comment
        file_path = os.path.join(dir_name, file_name)
        with open(file_name, "r") as file:
            lines = file.readlines()
            read_spec = False
            error_specification = ""
            for line in lines:
                if not read_spec and "/*" in line:
                    read_spec = True
                elif read_spec:
                    if "*/" in line:
                        break
                    error_specification = error_specification + line
            error_specification = error_specification.replace("*", "")
            error_specification = error_specification.replace("\n", "")
            error_cases[file_name]["Error_Specification"] = error_specification.strip()

    with open("error-cases.json", "w") as error_cases_file:
        json.dump(error_cases, error_cases_file, indent=4)

if __name__ == '__main__':
        main()
