#include <iostream>
#include <vector>
#include <stdexcept>

int retrieveGFElement(int m, int exponent);
void performGFAddition(int m, int a, int b);
int retrieveGFExponent(int m, int value);
int powerGFElement(int m, int value, int power);
int multiplyGFElement(int m, int a, int b);
int inverseGFElement(int m, int value);
int divideGFElement(int m, int numerator, int denominator);
std::vector<int> computeSyndromes(int m, int t, const std::vector<int>& received);
std::vector<int> computeLambda(int m, int t, const std::vector<int>& syndromes);
std::vector<int> findRoots(int m, const std::vector<int>& lambda, int codewordLength);
std::vector<int> correctErrors(const std::vector<int>& received, const std::vector<int>& errorPositions);
struct GFEntry {
    int exponent;
    int value;
};




int main () {
    int m = 32;
    int t = 2;
    std::vector<int> received = {
        0, 0, 1, 0, 0, 0, 0, 1, 1, 0,
        0, 1, 1, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };

    std::vector<int> syndromes = computeSyndromes(m, t, received);
    std::vector<int> lambda = computeLambda(m, t, syndromes);
    std::vector<int> errorPositions = findRoots(m, lambda, static_cast<int>(received.size()));
    std::vector<int> corrected = correctErrors(received, errorPositions);

    std::cout << "Syndromes: ";
    for (int i = 0; i < static_cast<int>(syndromes.size()); ++i) {
        if (syndromes[i] == 0) {
            std::cout << "S" << (i + 1) << "=0 ";
        } else {
            std::cout << "S" << (i + 1) << "=" << retrieveGFExponent(m, syndromes[i]) << " ";
        }
    }
    std::cout << std::endl;

    std::cout << "Lambda coefficients: ";
    for (int i = 0; i < static_cast<int>(lambda.size()); ++i) {
        if (lambda[i] == 0) {
            std::cout << "0 ";
        } else {
            std::cout << retrieveGFExponent(m, lambda[i]) << " ";
        }
    }
    std::cout << std::endl;

    std::cout << "Error positions: ";
    for (int pos : errorPositions) {
        std::cout << pos << " ";
    }
    std::cout << std::endl;

    std::cout << "Corrected word: ";
    for (int bit : corrected) {
        std::cout << bit;
    }
    std::cout << std::endl;

    return 0;
}

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

int powerGFElement(int m, int value, int power) {
    if (value == 0) return 0;
    int fieldOrder = m - 1;
    int exponent = retrieveGFExponent(m, value);
    int poweredExponent = (power * exponent) % fieldOrder;
    return retrieveGFElement(m, poweredExponent);
}

int multiplyGFElement(int m, int a, int b) {
    if (a == 0 || b == 0) return 0;
    int fieldOrder = m - 1;
    int exponentA = retrieveGFExponent(m, a);
    int exponentB = retrieveGFExponent(m, b);
    int productExponent = (exponentA + exponentB) % fieldOrder;
    return retrieveGFElement(m, productExponent);
}

int inverseGFElement(int m, int value) {
    int fieldOrder = m - 1;
    int exponent = retrieveGFExponent(m, value);
    int inverseExponent = (fieldOrder - exponent) % fieldOrder;
    return retrieveGFElement(m, inverseExponent);
}

int divideGFElement(int m, int numerator, int denominator) {
    if (numerator == 0) return 0;
    if (denominator == 0) throw std::out_of_range("Cannot divide by zero in GF");

    return multiplyGFElement(m, numerator, inverseGFElement(m, denominator));
}

/**
 * This function computes the syndromes for a received codeword in a BCH code.
 * This will compute 2t syndromes, where t is the error correction capability of the code. 
 * The syndromes are computed using the received codeword 
 * and the Galois Field elements corresponding to the powers of alpha.
 */
std::vector<int> computeSyndromes(int m, int t, const std::vector<int>& received) {
    std::vector<int> syndromes(2 * t, 0);
    int maxSyndrome = 2 * t;
    int fieldOrder = m - 1;

    for (int s = 1; s <= maxSyndrome; s += 2) {
        int syndromeValue = 0;

        // Evaluate r(alpha^s) for odd s using received bits as polynomial coefficients.
        for (int i = 0; i < static_cast<int>(received.size()); ++i) {
            if (received[i] == 0) {
                continue;
            }

            int exponent = (s * i) % fieldOrder;
            syndromeValue ^= retrieveGFElement(m, exponent);
        }

        syndromes[s - 1] = syndromeValue;
    }
    for (int i = 1; i <= t; ++i) {
        int evenIndex = 2 * i;
        if (evenIndex <= maxSyndrome) {
            syndromes[evenIndex - 1] = powerGFElement(m, syndromes[i - 1], 2);
        }
    }

    return syndromes;
}


/**
 * This function computes the error locator polynomial (lambda)
 * using Peterson's technique for simpler cases 
 * Only valid for t <= 4, for larger t or more complex cases Peterson's original algorithm is needed
 */
std::vector<int> computeLambda(int m, int t, const std::vector<int>& syndromes) {
    std::vector<int> lambda(t, 0);
    if (t == 1) lambda[0] = syndromes[0];
    else if (t == 2) {
        lambda[0] = syndromes[0];
        int s1Cubed = powerGFElement(m, syndromes[0], 3);
        int numerator = syndromes[2] ^ s1Cubed;
        lambda[1] = divideGFElement(m, numerator, syndromes[0]);
    }
    else if (t == 3) { 
        lambda[0] = syndromes[0];
        int s1Squared = powerGFElement(m, syndromes[0], 2);
        int s1Cubed = powerGFElement(m, syndromes[0], 3);
        int numerator = (multiplyGFElement(m, s1Squared, syndromes[2]) ^ syndromes[4]);
        int denuminator = s1Cubed ^ syndromes[2];
        lambda[1] = divideGFElement(m, numerator, denuminator);
        lambda[2] = s1Cubed ^ syndromes[2] ^ multiplyGFElement(m, syndromes[0], lambda[1]);
    }
    else if (t == 4) {
      lambda[0] = syndromes[0];
    }
    return lambda;
}

/**
 * This function finds roots of Lambda(x) using Chien search.
 * Returned values are final error positions in the received codeword.
 * The roots are then inverted to get the error positions, 
 * since the roots correspond to alpha^(-i) where i is the error position.
 */
std::vector<int> findRoots(int m, const std::vector<int>& lambda, int codewordLength) {
    std::vector<int> errorPositions;
    int fieldOrder = m - 1;
    int degree = static_cast<int>(lambda.size());

    for (int rootExponent = 0; rootExponent < fieldOrder; ++rootExponent) {
        int x = retrieveGFElement(m, rootExponent);

        // Monic form: x^degree + lambda[0]x^(degree-1) + ... + lambda[degree-1].
        int lambdaValue = powerGFElement(m, x, degree);
        for (int i = 0; i < degree; ++i) {
            int coeff = lambda[degree - 1 - i];
            if (coeff == 0) continue;
            int xPower = powerGFElement(m, x, i);
            int term = multiplyGFElement(m, coeff, xPower);
            lambdaValue ^= term;
        }

        if (lambdaValue == 0) {
            int location = rootExponent;
            if (codewordLength > 0) location %= codewordLength;
            errorPositions.push_back(location);
        }
    }
    return errorPositions;
}

/**
 * This function corrects the errors in the received code word
 * the bits at the error positions are flipped.
 */
std::vector<int> correctErrors(const std::vector<int>& received, const std::vector<int>& errorPositions) {
    std::vector<int> corrected = received;
    int codewordLength = static_cast<int>(received.size());
    if (codewordLength == 0) return corrected;

    for (int errorPosition : errorPositions) {
        errorPosition = ((errorPosition % codewordLength) + codewordLength) % codewordLength;
        if (errorPosition >= 0 && errorPosition < codewordLength) corrected[errorPosition] ^= 1;
    }
    return corrected;
}
