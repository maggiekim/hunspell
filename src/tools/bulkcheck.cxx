/* Copyright 2017 Sander van Geloven
 *
 * This file is part of Hunspell.
 *
 * Hunspell is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Hunspell is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Hunspell.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <cstring>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iomanip>

#include "../hunspell/hunspell.hxx"

using namespace std;

int main(int argc, char** argv) {

  /* first parse the command line options */

  if (argc < 4) {
    // Note: refactor to use a library for this
    fprintf(stderr, "bulkcheck (now it works with more dictionary files):\n");
    fprintf(stderr,
            "bulkcheck affix_file dictionary_file(s) file_of_words_to_check result_file\n");
    exit(1);
  }

  /* open the words to check list, word is expected on each line */
  ifstream input_file(argv[argc - 1], ios_base::in);
  if (!input_file.is_open()) {
    fprintf(stderr, "Error - could not open file of words to check %s\n", argv[argc - 1]);
    exit(1);
  }

  Hunspell* hunspell = new Hunspell(argv[1], argv[2]);

  // load extra dictionaries, such as medical dictionaries or personal dictionaries that do not have affix file
  // Note: this should go into the documentation
  if (argc > 3)
    for (int k = 3; k < argc - 1; ++k)
      hunspell->add_dic(argv[k]);

//  /* open output file */
//  string filename = string(argv[argc - 1]) + "-bulkcheck.tsv";
//  ofstream output_file(filename, ios_base::out);
//  if (!output_file.is_open()) {
//    fprintf(stderr, "Error - could not open result file\n");
//    exit(1);
//  }

  /* declare variables for iteration */
  string word;
  int num = 0;
  int num_space = 0;
  int num_correct = 0;
  int num_correct_space = 0;

  /* iterate all lines in input file */
  while (getline(input_file, word)) {
    /* count number of words and words with space */
    num++;
    bool has_space = false;
    if (count(word.begin(), word.end(), ' ') > 0 ) {
      has_space = true;
      num_space++;
    }

    bool is_correct = hunspell->spell(word);
//    output_file << is_success << "\t" << is_correct << "\t" << has_space << "\t" << has_suggest
//                << "\t" << word.c_str() << "\t" << expect.c_str() << "\t";
    cout << is_correct << "\t" << word.c_str() << "\t";

    bool is_first = true;
    if (is_correct) {
      num_correct++;
      if (has_space) {
        num_correct_space++;
      }
    } else {
      vector<string> suggestions = hunspell->suggest(word.c_str());
      for (size_t i = 0; i < suggestions.size(); ++i) {
        if (is_first) {
          cout << suggestions[i];
          is_first = false;
        } else {
          cout << ";" << suggestions[i];
        }
      }
    }

    cout << endl;
  }

  input_file.close();

  if (num == 0) {
    fprintf(stderr, "ERROR: No words to check in file %s:\n", argv[argc - 2]);
    exit(1);
  }

  int num_nospace = num - num_space;
  int num_incorrect = num - num_correct;
  int num_incorrect_space = num_space - num_correct_space;
  int num_correct_nospace = num_correct - num_correct_space;
  int num_incorrect_nospace = num_nospace - num_correct_nospace;

  float per_nospace = 100.0 * num_nospace / num;
  float per_correct = 100.0 * num_correct / num;
  float per_correct_space = 100.0;
  if (num_space != 0) {
    per_correct_space = 100.0 * num_correct_space / num_space;
  }
  float per_correct_nospace = 100.0;
  if (num_nospace != 0) {
    per_correct_nospace = 100.0 * num_correct_nospace / num_nospace;
  }

  float per_space = 100.0 - per_nospace;
  float per_incorrect = 100.0 - per_correct;
  float per_incorrect_space = 100.0 - per_correct_space;
  float per_incorrect_nospace = 100.0 - per_correct_nospace;

  //cerr << "Hunspell version\t" << VERSION << endl;
  cerr << "Hunspell affix\t" << argv[1] << endl;
  cerr << "Hunspell dict\t" << argv[2] << endl;

  cerr << "wordlist\t" << argv[argc - 1] << endl;
  cerr << "percentage of words without space\t" << fixed << setw(6)
              << setprecision(2) << setfill('0') << per_nospace << endl;
  cerr << "percentage of words with space\t" << fixed << setw(6)
              << setprecision(2) << setfill('0') << per_space << endl;

  cerr << "number of words\t" << num << endl;
  cerr << "number of correct words\t" << num_correct << endl;
  cerr << "number of incorect words\t" << num_incorrect << endl;
  cerr << "percentage of correct words\t" << fixed << setw(6)
              << setprecision(2) << setfill('0') << per_correct << endl;
  cerr << "percentage of incorrect words\t" << fixed << setw(6)
              << setprecision(2) << setfill('0') << per_incorrect << endl;

  cerr << "number of words without space\t" << num_nospace << endl;
  cerr << "number of correct words without space\t" << num_correct_nospace << endl;
  cerr << "number of incorrect words without space\t" << num_incorrect_nospace << endl;
  cerr << "percentage of correct words without space\t" << fixed << setw(6)
              << setprecision(2) << setfill('0') << per_correct_nospace << endl;
  cerr << "percentage of incorrect words without space\t" << fixed << setw(6)
              << setprecision(2) << setfill('0') << per_incorrect_nospace << endl;

  cerr << "number of words with space\t" << num_space << endl;
  cerr << "number of correct words with space\t" << num_correct_space << endl;
  cerr << "number of incorrect words with space\t" << num_incorrect_space << endl;
  cerr << "percentage of correct words with space\t" << fixed << setw(6)
              << setprecision(2) << setfill('0') << per_correct_space << endl;
  cerr << "percentage of incorrect words with space\t" << fixed << setw(6)
              << setprecision(2) << setfill('0') << per_incorrect_space << endl;

//  output_file.close();

  delete hunspell;

  return 0;
}
