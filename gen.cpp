#include <cstdio>
#include <cstdlib>

#include <random>

using namespace std;

enum {
    ARG_SELF,
    ARG_OUTPUT_FILE,
    ARG_OUTPUT_SIZE,
    ARGC
};


union ieee754_double
{
    double d;

    struct
    {
        unsigned long long mantissa:52;
        unsigned int exponent:11;
        unsigned int negative:1;
    } ieee;
};

int main(int argc, char *argv[])
{
    if (argc != ARGC) {
        fprintf(stderr, "Usage: %s <output_file> <size>\n", argv[ARG_SELF]);
        return EXIT_FAILURE;
    }

    long long fsize = 0;
    long long target_size = atoll(argv[ARG_OUTPUT_SIZE]);

    std::mt19937 gen;
    std::uniform_int_distribution<unsigned long long> mantissa_dist(0, (1ULL << 52) - 1);
    std::uniform_int_distribution<unsigned int> exponent_dist(0, (1ULL << 11) - 1);
    std::uniform_int_distribution<unsigned int> negative_dist(0, 1);
    std::bernoulli_distribution inf_dist;

    auto f = fopen(argv[ARG_OUTPUT_FILE], "w");
    while (fsize < target_size) {
        ieee754_double value;
        value.ieee.negative = negative_dist(gen);
        value.ieee.exponent = exponent_dist(gen);

        if (value.ieee.exponent == 0x7ff && inf_dist(gen)) {
            value.ieee.mantissa = 0;
        } else {
            value.ieee.mantissa = mantissa_dist(gen);
        }

        int n;
        fprintf(f, "%0.5e\n%n", value.d, &n);
        fsize += n;
    }

    fclose(f);
    return EXIT_SUCCESS;
}
