#include <stdint.h>

#include <bitset>
#include <cmath>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <limits>

using namespace std;

#define NUM_TESTS 10
#define MAX_VALUE 100
#define MIN_VALUE -100
uint8_t const table_width[] = {12, 12, 35, 12};

// IEEE 754 single-precision float constants
uint8_t const width = 32U;
uint8_t const exp_width = 8U;
uint8_t const mantissa_width = width - exp_width - 1;
uint8_t const bias = 127U;

/*
 * *** STUDENTS SHOULD WRITE CODE FOR THIS FUNCTION ***
 * Students should create or add any data structures needed.
 * Students should create or add any functions or classes they may need.
 */
float ieee_754(uint32_t const data) {

    // Extract the sign bit.
    uint32_t sign = (data >> 31U) & 0b1U;

    // Extract the 8-bit exponent.
    uint32_t exponent = (data >> 23U) & 0b11111111U;

    // Extract the 23-bit mantissa.
    uint32_t mantissa = data & 0b01111111111111111111111U;

    // Handle zero and denormalized numbers.
    if (exponent == 0U) {

        float value = 0.0f;
        float bit_value = 0.5f;

        for (uint32_t i = 0U; i < 23U; i++) {
            if ((mantissa >> (22U - i)) & 0b1U) {
                value += bit_value;
            }
            bit_value *= 0.5f;
        }

        // Denormalized numbers use an exponent of -126.
        for (int i = 0; i < 126; i++) {
            value *= 0.5f;
        }

        if (sign == 1U) {
            value = -value;
        }

        return value;
    }

    // Handle infinity and NaN.
    if (exponent == 255U) {
        if (mantissa == 0U) {
            if (sign == 1U) {
                return -numeric_limits<float>::infinity();
            }
            return numeric_limits<float>::infinity();
        }

        return numeric_limits<float>::quiet_NaN();
    }

    // Normalized number.
    float value = 1.0f;
    float bit_value = 0.5f;

    for (uint32_t i = 0U; i < 23U; i++) {
        if ((mantissa >> (22U - i)) & 0b1U) {
            value += bit_value;
        }
        bit_value *= 0.5f;
    }

    int actual_exponent = exponent - bias;

    if (actual_exponent > 0) {
        for (int i = 0; i < actual_exponent; i++) {
            value *= 2.0f;
        }
    }
    else if (actual_exponent < 0) {
        for (int i = 0; i > actual_exponent; i--) {
            value *= 0.5f;
        }
    }

    if (sign == 1U) {
        value = -value;
    }

    return value;
}

/*
 * *** STUDENTS SHOULD NOT NEED TO CHANGE THE CODE BELOW. IT IS A CUSTOM TEST HARNESS. ***
 */

void header() {
    cout << left << setw(table_width[0]) << setfill(' ') << "pass/fail";
    cout << left << setw(table_width[1]) << setfill(' ') << "value";
    cout << left << setw(table_width[2]) << setfill(' ') << "bits";
    cout << left << setw(table_width[3]) << setfill(' ') << "IEEE-754" << endl;

    cout << left << setw(table_width[0]) << setfill(' ') << "--------";
    cout << left << setw(table_width[1]) << setfill(' ') << "--------";
    cout << left << setw(table_width[2]) << setfill(' ') << "--------";
    cout << left << setw(table_width[3]) << setfill(' ') << "--------" << endl;
}

void print_row(bool const test_success, float const rand_val, uint32_t const val_int, float const ieee_754_value) {
    // print results
    string const pass_fail = test_success ? "PASS" : "FAIL";
    cout << left << setw(table_width[0]) << setfill(' ') << pass_fail;
    cout << left << setw(table_width[1]) << setfill(' ') << rand_val;
    cout << left << setw(table_width[2]) << setfill(' ') << bitset<width>(val_int);
    cout << left << setw(table_width[3]) << setfill(' ') << ieee_754_value << endl;
}

template <typename T>
T rand_min_max(T const min, T const max) {
    T const rand_val =
        min + static_cast<double>(static_cast<double>(rand())) / (static_cast<double>(RAND_MAX / (max - min)));
    return rand_val;
}

bool test() {
    // the union
    union float_uint {
        float val_float;
        uint32_t val_int;
    } data;

    // print header
    header();

    // seed the random number generator
    srand(time(NULL));

    bool success = true;
    uint16_t pass = 0;
    for (size_t i = 0; i < NUM_TESTS; i++) {
        // random value
        float const rand_val = rand_min_max<float>(MIN_VALUE, MAX_VALUE);

        data.val_float = rand_val;

        // calculate using ieee_754 function
        float ieee_754_value = ieee_754(data.val_int);

        // test the results
        float const epsilon = std::numeric_limits<float>::epsilon();
        bool test_success = (abs(ieee_754_value - rand_val) < epsilon);
        if (test_success) {
            pass += 1;
        }

        // print row
        print_row(test_success, rand_val, data.val_int, ieee_754_value);
    }

    // summarize results
    cout << "-------------------------------------------" << endl;
    if (pass == NUM_TESTS) {
        cout << "SUCCESS ";
    } else {
        cout << "FAILURE ";
    }
    cout << pass << "/" << NUM_TESTS << " passed" << endl;
    cout << "-------------------------------------------" << endl;

    return success;
}

int main() {
    if (!test()) {
        return -1;
    }
    return 0;
}
