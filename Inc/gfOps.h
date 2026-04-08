#include <iostream>
#include <vector>
#include <stdexcept>
// Forward declarations of Galois Field operations
int retrieveGFElement(int m, int exponent);
void performGFAddition(int m, int a, int b);
int retrieveGFExponent(int m, int value);
int powerGFElement(int m, int value, int power);
int multiplyGFElement(int m, int a, int b);
int inverseGFElement(int m, int value);
int divideGFElement(int m, int numerator, int denominator);

// Struct to represent Galois Field entries for different values of m
struct GFEntry {
    int exponent;
    int value;
};

/***
 * This function retrieves one Galois Field element for a given value of m where m = 2^k.
 * The exponent represents a power of alpha and the returned value is the polynomial bit pattern.
 */
int retrieveGFElement(int m, int exponent) {

    const GFEntry* gfTable = nullptr;
    int gfTableSize = 0;

    if (m == 8) {
        static const GFEntry table8[] = {
            {0, 0b100}, {1, 0b010}, {2, 0b001}, {3, 0b110},
            {4, 0b011}, {5, 0b111}, {6, 0b101}
        };
        gfTable = table8;
        gfTableSize = static_cast<int>(sizeof(table8) / sizeof(table8[0]));
    }
    else if (m == 16) {
        static const GFEntry table16[] = {
            {0,  0b1000}, {1,  0b0100}, {2,  0b0010}, {3,  0b0001},
            {4,  0b1100}, {5,  0b0110}, {6,  0b0011}, {7,  0b1101},
            {8,  0b1010}, {9,  0b0101}, {10, 0b1110}, {11, 0b0111},
            {12, 0b1111}, {13, 0b1011}, {14, 0b0101}
        };
        gfTable = table16;
        gfTableSize = static_cast<int>(sizeof(table16) / sizeof(table16[0]));
    }
    else if (m == 32) {
        static const GFEntry table32[] = {
            {0,  0b10000}, {1,  0b01000}, {2,  0b00100}, {3,  0b00010},
            {4,  0b00001}, {5,  0b10100}, {6,  0b01010}, {7,  0b00101},
            {8,  0b10110}, {9,  0b01011}, {10, 0b10001}, {11, 0b11100},
            {12, 0b01110}, {13, 0b00111}, {14, 0b10111}, {15, 0b11111},
            {16, 0b11011}, {17, 0b11001}, {18, 0b11000}, {19, 0b01100},
            {20, 0b00110}, {21, 0b00011}, {22, 0b10101}, {23, 0b11110},
            {24, 0b01111}, {25, 0b10011}, {26, 0b11101}, {27, 0b11010},
            {28, 0b01101}, {29, 0b10010}, {30, 0b01001}
        };
        gfTable = table32;
        gfTableSize = static_cast<int>(sizeof(table32) / sizeof(table32[0]));
    }
    else if (m == 64) {
        static const GFEntry table64[] = {
            {0,  0b100000}, {1,  0b010000}, {2,  0b001000}, {3,  0b000100},
            {4,  0b000010}, {5,  0b000001}, {6,  0b110000}, {7,  0b011000},
            {8,  0b001100}, {9,  0b000110}, {10, 0b000011}, {11, 0b110001},
            {12, 0b101000}, {13, 0b010100}, {14, 0b001010}, {15, 0b000101},
            {16, 0b110010}, {17, 0b011001}, {18, 0b111100}, {19, 0b011110},
            {20, 0b001111}, {21, 0b110111}, {22, 0b101011}, {23, 0b100101},
            {24, 0b100010}, {25, 0b010001}, {26, 0b111000}, {27, 0b011100},
            {28, 0b001110}, {29, 0b000111}, {30, 0b110011}, {31, 0b101001},
            {32, 0b100100}, {33, 0b010010}, {34, 0b001001}, {35, 0b110100},
            {36, 0b011010}, {37, 0b001101}, {38, 0b110110}, {39, 0b011011},
            {40, 0b111101}, {41, 0b101110}, {42, 0b010111}, {43, 0b111011},
            {44, 0b101101}, {45, 0b100110}, {46, 0b010011}, {47, 0b111001},
            {48, 0b101100}, {49, 0b010110}, {50, 0b001011}, {51, 0b110101},
            {52, 0b101010}, {53, 0b010101}, {54, 0b111010}, {55, 0b011101},
            {56, 0b111110}, {57, 0b011111}, {58, 0b111111}, {59, 0b101111},
            {60, 0b100111}, {61, 0b100011}, {62, 0b100001}
        };
        gfTable = table64;
        gfTableSize = static_cast<int>(sizeof(table64) / sizeof(table64[0]));
    }

    if (gfTable != nullptr) {
        for (int i = 0; i < gfTableSize; ++i) {
            if (gfTable[i].exponent == exponent) {
                return gfTable[i].value;
            }
        }
    }

    throw std::out_of_range("Invalid m or exponent for GF lookup");
}


/**
 * This function perfroms addition in GF
 */
void performGFAddition(int m, int a, int b) {
    int in1 = retrieveGFElement(m, a);
    int in2 = retrieveGFElement(m, b);
    int result = in1 ^ in2;
    (void)result;
}


/**
 * This function retrieves the exponent of a given value in the Galois Field.
 * It iterates through the GF table to find the exponent corresponding to the provided value.
 */
int retrieveGFExponent(int m, int value) {
    int fieldOrder = m - 1;
    for (int exponent = 0; exponent < fieldOrder; ++exponent) {
        if (retrieveGFElement(m, exponent) == value) {
            return exponent;
        }
    }
    throw std::out_of_range("Invalid value for GF exponent lookup");
}

/**
 * This function computes the power of a Galois Field element.
 * It retrieves the exponent of the input value, multiplies it by the power, 
 * and then retrieves the corresponding GF element for the resulting exponent.
 */
int powerGFElement(int m, int value, int power) {
    if (value == 0) return 0;
    int fieldOrder = m - 1;
    int exponent = retrieveGFExponent(m, value);
    int poweredExponent = (power * exponent) % fieldOrder;
    return retrieveGFElement(m, poweredExponent);
}

/**
 * This function multiplies two Galois Field elements.
 * It retrieves the exponents of both input values, adds them together, 
 * and then retrieves the corresponding GF element for the resulting exponent.
 */
int multiplyGFElement(int m, int a, int b) {
    if (a == 0 || b == 0) return 0;
    int fieldOrder = m - 1;
    int exponentA = retrieveGFExponent(m, a);
    int exponentB = retrieveGFExponent(m, b);
    int productExponent = (exponentA + exponentB) % fieldOrder;
    return retrieveGFElement(m, productExponent);
}
/**
 * This function computes the multiplicative inverse of a Galois Field element.
 * It retrieves the exponent of the input value, 
 * computes the inverse exponent by subtracting it from the field order, 
 * and then retrieves the corresponding GF element for the inverse exponent.
 */
int inverseGFElement(int m, int value) {
    int fieldOrder = m - 1;
    int exponent = retrieveGFExponent(m, value);
    int inverseExponent = (fieldOrder - exponent) % fieldOrder;
    return retrieveGFElement(m, inverseExponent);
}

/**
 * This function performs division of two Galois Field elements.
 * It computes the multiplicative inverse of the denominator and then multiplies it with the numerator.
 */
int divideGFElement(int m, int numerator, int denominator) {
    if (numerator == 0) return 0;
    if (denominator == 0) throw std::out_of_range("Cannot divide by zero in GF");
    return multiplyGFElement(m, numerator, inverseGFElement(m, denominator));
}



std::vector<std::vector<int>> generateCosets (int n) {
    std::vector<std::vector<int>> cosets;
    std::vector<bool> used(n, false);
    for (int i = 1; i < n; i++) {
        if (used[i]) continue;
        std::vector<int> coset;
        int current = i;
        do {
            coset.push_back(current);
            used[current] = true;
            current = (current * 2) % n;
        } while (current != i);
        cosets.push_back(coset);
    }
    return cosets;
}