#include <cassert>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

using namespace std;

string omit_string(const string &code) {
    char *buffer = new char[code.length()];
    int length = code.length();
    bool in_str = false, in_chr = false;
    for (int i = 0, r = 0; i <= length; i++) {
        if (code[i] == '\\') {
            // omit next quote mark
            i++;
            continue;
        } else if (!in_chr && code[i] == '"')
            in_str = !in_str;
        else if (!in_str && code[i] == '\'')
            in_chr = !in_chr;

        if (!(in_str || in_chr))
            buffer[r++] = code[i];
    }
    string rt(buffer);
    delete[] buffer;
    return move(rt);
}

void analyze(ifstream &fin, const char *filename) {
    static const char empty_chars[] = " \t";
    static const char comment[] = "//";
    static int STACK_SIZE = 128;

    char stack[STACK_SIZE] = {0};
    string ss;
    int rows = 0, function_rows = 0, code_rows = 0, comment_rows = 0,
        empty_rows = 0;
    int function_cnt = 0;
    int current_func_rows = 0, current_func_pos = 0;
    int max_func_rows = -1, max_func_pos = 0;
    int stack_ptr = -1;    // empty stack
    char next_scope = 'f'; // function scope

    while (!fin.eof()) {
        // read a new line of code
        getline(fin, ss);
        // omit strings in code
        ss = omit_string(ss);
        int row_begin_pos = ss.find_first_not_of(empty_chars);
        if (row_begin_pos == string::npos) {
            // empty row
            empty_rows++;
        } else if (ss.find(comment) == row_begin_pos) {
            // comment
            comment_rows++;
        } else {
            // code
            // determine next scope
            if (stack_ptr >= 0 &&
                (stack[stack_ptr] == 'f' || stack[stack_ptr] == 'i'))
                next_scope = 'i'; // in function
            else if (ss.find("class") != string::npos)
                next_scope = 'c'; // class
            else if (ss.find("struct") != string::npos)
                next_scope = 's'; // struct
            else
                next_scope = 'f'; // function

            // new scope
            if (ss.find('{') != string::npos) {
                if (next_scope == 'f') {
                    // a new function
                    function_cnt++;
                    current_func_rows = 0;
                    current_func_pos = rows + 1;
                }
                // push scope stack
                stack[++stack_ptr] = next_scope;
                assert(stack_ptr < STACK_SIZE);
            }

            // count function rows
            if (stack_ptr >= 0 &&
                (stack[stack_ptr] == 'f' || stack[stack_ptr] == 'i')) {
                function_rows++;
                current_func_rows++;
            }

            // exit scope
            if (ss.find('}') != string::npos) {
                if (stack_ptr >= 0 && stack[stack_ptr] == 'f') {
                    // reset longest function
                    if (current_func_rows > max_func_rows) {
                        max_func_rows = current_func_rows;
                        max_func_pos = current_func_pos;
                    }
                }
                // pop scope stack
                stack_ptr--;
                assert(stack_ptr >= -1);
            }
            // count code row
            code_rows++;
        }
        // count row
        rows++;
    }

    // output answer
    float routine_size = float(function_rows) / function_cnt;
    int code_percentage = code_rows * 100 / rows,
        comment_percentage = comment_rows * 100 / rows,
        empty_percentage = empty_rows * 100 / rows;

    cout << "The results of analyzing program file \"" << filename
         << "\":" << endl;
    cout << "\tLines of code:\t\t" << code_rows << endl;
    cout << "\tLines of comments:\t" << comment_rows << endl;
    cout << "\tBlink lines:\t\t" << empty_rows << endl << endl;

    cout << "\tCode\tComments\tSpace\t" << endl;
    cout << '\t' << code_percentage << "%\t" << comment_percentage << "%\t"
         << empty_percentage << "%\t" << endl;
    cout << "\tThe program includes " << function_cnt << " functions." << endl;
    cout << "\tThe average length of a section of code is " << fixed
         << setprecision(1) << routine_size << " lines." << endl;
    cout << endl;

    static const char *grade_descriptions[] = {"Excellent", "Okay", "Poor",
                                               "Bad"};

    // routine size grade
    int routine_style_level, comment_style_level, space_style_level;
    if (routine_size >= 10 && routine_size <= 15)
        routine_style_level = 0;
    else if (routine_size == 8 || routine_size == 9 ||
             routine_size >= 16 && routine_size <= 20)
        routine_style_level = 1;
    else if (routine_size >= 5 && routine_size <= 7 ||
             routine_size >= 21 && routine_size <= 24)
        routine_style_level = 2;
    else
        routine_style_level = 3;

    if (comment_percentage >= 15 && comment_percentage <= 25)
        comment_style_level = 0;
    else if (comment_percentage >= 10 && comment_percentage <= 14 ||
             comment_percentage >= 26 && comment_percentage <= 30)
        comment_style_level = 1;
    else if (comment_percentage >= 5 && comment_percentage <= 9 ||
             comment_percentage >= 31 && comment_percentage <= 35)
        comment_style_level = 2;
    else
        comment_style_level = 3;

    if (empty_percentage >= 15 && empty_percentage <= 25)
        space_style_level = 0;
    else if (empty_percentage >= 10 && empty_percentage <= 14 ||
             empty_percentage >= 26 && empty_percentage <= 30)
        space_style_level = 1;
    else if (empty_percentage >= 5 && empty_percentage <= 9 ||
             empty_percentage >= 31 && empty_percentage <= 35)
        space_style_level = 2;
    else
        space_style_level = 3;

    cout << "\tGrade " << char('A' + routine_style_level) << ": "
         << grade_descriptions[routine_style_level] << " routine size style."
         << endl;
    cout << "\tGrade " << char('A' + comment_style_level) << ": "
         << grade_descriptions[comment_style_level] << " comments style."
         << endl;
    cout << "\tGrade " << char('A' + space_style_level) << ": "
         << grade_descriptions[space_style_level] << " white space style."
         << endl;

    cout << endl;
    cout << "\tLongest function @ line " << max_func_pos << endl;
    cout << "\tLength: " << max_func_rows << endl;
}

int main(int argc, const char *argv[]) {
    ios::sync_with_stdio(false);
    const char *filename;
    if (argc < 2) {
        cout << "Please enter filename:" << endl;
        char *fn = new char[64];
        cin >> fn;
        ifstream fin(fn);
        if (fin) {
            analyze(fin, fn);
            delete fn;
            return 0;
        }
    } else {
        ifstream fin(argv[1]);
        if (fin) {
            analyze(fin, argv[1]);
            return 0;
        }
    }

    cout << "Error: Failed to open file " << argv[1] << endl;
    return 1;
}
