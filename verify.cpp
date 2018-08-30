#include <cstdlib>
#include <fstream>
#include <iostream>
#include <cmath>

using namespace std;

enum {
    ARG_SELF,
    ARG_INPUT_FILE,
    ARGC
};

struct value
{
    std::string as_string;
    double as_double;
};

istream &operator >>(istream &is, value &v)
{
    if (is >> v.as_string) {
        if (sscanf(v.as_string.c_str(), "%le", &v.as_double) != 1) {
            cerr << "Can't parse " << v.as_string << " as double" << std::endl;
            is.setstate(ios::failbit);
        }
    }
    return is;
}

int main(int argc, char *argv[])
{
    if (argc != ARGC) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[ARG_SELF]);
        return EXIT_FAILURE;
    }

    ifstream f(argv[ARG_INPUT_FILE]);
    value prev, v;
    bool prev_ok = false;
    while (f >> v) {
        if (isnan(v.as_double)) {
            continue;
        }
        if (prev_ok) {
            if (v.as_double < prev.as_double) {
                std::cerr << v.as_string << " is less than " << prev.as_string << std::endl;
                return EXIT_FAILURE;
            }
        } else {
            prev_ok = true;
        }
        prev = v;
    }
    if (!f.eof()) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
