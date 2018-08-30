#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

using namespace std;

enum {
    ARG_SELF,
    ARG_INPUT_FILE,
    ARG_OUTPUT_FILE,
    ARGC
};

struct value
{
    std::string as_string;
    double as_double;
};

bool operator <(const value &a, const value &b)
{
    if (isnan(a.as_double) && isnan(b.as_double)) {
        return a.as_string < b.as_string;
    }
    if (isnan(a.as_double)) {
        return true;
    }
    if (isnan(b.as_double)) {
        return false;
    }
    return a.as_double < b.as_double;
}

istream &operator >>(istream &is, value &v)
{
    if (is >> v.as_string) {
        if (sscanf(v.as_string.c_str(), "%le", &v.as_double) != 1) {
            is.setstate(ios::failbit);
        }
    }
    return is;
}

std::string gen_fname()
{
    static unsigned long long id = 0;
    std::ostringstream name;
    name << "tmp" << id << ".tmp";
    id++;
    return name.str();
}

int main(int argc, char *argv[])
{
    if (argc != ARGC) {
        fprintf(stderr, "Usage: %s <input_file> <output_file>\n", argv[ARG_SELF]);
        return EXIT_FAILURE;
    }

    vector<string> prev_step_files, new_step_files;

    {
        ifstream input(argv[ARG_INPUT_FILE]);
        vector<value> values;
        for (;;) {
            values.clear();
            values.reserve(1024 * 1024);

            value v;
            while (values.size() < values.capacity() && input >> v) {
                values.push_back(v);
            }

            if (values.empty()) {
                break;
            }

            sort(values.begin(), values.end());

            auto tmp_name = gen_fname();
            ofstream tmp(tmp_name);
            for (auto &v : values) {
                tmp << v.as_string << "\n";
            }
            new_step_files.push_back(tmp_name);
        }
    }

    while (new_step_files.size() > 1) {
        new_step_files.swap(prev_step_files);
        new_step_files.clear();

        for (size_t i = 0; i < prev_step_files.size(); i += 2) {
            if (i + 1 == prev_step_files.size()) {
                new_step_files.push_back(prev_step_files[i]);
                continue;
            }

            auto merged_file = gen_fname();
            new_step_files.push_back(merged_file);
            {
                ofstream merged(merged_file);
                ifstream in1(prev_step_files[i]);
                ifstream in2(prev_step_files[i + 1]);
                value in1_value, in2_value;
                bool has_in1_value = false, has_in2_value = false;

                while (in1 || in2) {
                    if (in1 && !has_in1_value) {
                        has_in1_value = bool(in1 >> in1_value);
                    }
                    if (in2 && !has_in2_value) {
                        has_in2_value = bool(in2 >> in2_value);
                    }

                    if (!has_in1_value || !has_in2_value) {
                        if (has_in1_value) {
                            merged << in1_value.as_string << "\n";
                            has_in1_value = false;
                        } else if (has_in2_value) {
                            merged << in2_value.as_string << "\n";
                            has_in2_value = false;
                        }
                        continue;
                    }

                    if (in2_value < in1_value) {
                        merged << in2_value.as_string << "\n";
                        has_in2_value = false;
                    } else {
                        merged << in1_value.as_string << "\n";
                        has_in1_value = false;
                    }
                }
            }

            remove(prev_step_files[i].c_str());
            remove(prev_step_files[i + 1].c_str());
        }
    }

    rename(new_step_files[0].c_str(), argv[ARG_OUTPUT_FILE]);

    return EXIT_SUCCESS;
}
