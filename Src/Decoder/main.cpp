#include <iostream>
#include <vector>
#include <stdexcept>
#include "../../Inc/gfOps.h"
#include "../../Inc/polyOps.h"  

// Forward declarations of functions
std::vector<int> computeSyndromes(int m, int t, const std::vector<int>& received);
std::vector<int> computeLambda(int m, int t, const std::vector<int>& syndromes);
std::vector<int> findRoots(int m, const std::vector<int>& lambda);
std::vector<int> correctErrors(const std::vector<int>& received, const std::vector<int>& errorPositions);

int main () {
    int m = 32;
    int t = 3;
    std::vector<int> received = {
        0, 0, 1, 0, 0, 0, 0, 1, 0, 0,
        0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
    // received = 001000011001100000000000000000
    // fixed =    001001011011100000000000000000
    std::vector<int> syndromes = computeSyndromes(m, t, received);
    std::vector<int> lambda = computeLambda(m, t, syndromes);
    std::vector<int> errorPositions = findRoots(m, lambda);
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
 * Only valid for t <= 4, for larger t or more complex cases Peterson's original algorithm is needed.
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
      int l2Numerator1 = multiplyGFElement(m, syndromes[0], (syndromes[6] ^ powerGFElement(m, syndromes[0], 7)));
      int l2Numerator2 = multiplyGFElement(m, syndromes[2], (syndromes[4] ^ powerGFElement(m, syndromes[0], 5)));
      int l2Denominator1 = multiplyGFElement(m, syndromes[2], (syndromes[2] ^ powerGFElement(m, syndromes[0], 3)));
      int l2Denominator2 = multiplyGFElement(m, syndromes[0], (syndromes[4] ^ powerGFElement(m, syndromes[0], 5)));
      lambda[1] = divideGFElement(m, l2Numerator1 ^ l2Numerator2, l2Denominator1 ^ l2Denominator2);
      lambda[2] = (powerGFElement(m, syndromes[0], 3) ^ syndromes[2]) ^ multiplyGFElement(m, syndromes[0], lambda[1]);
      int l3Numerator1 = multiplyGFElement(m, powerGFElement(m, syndromes[0], 2), syndromes[2]) ^ syndromes[4];
      int l3Numerator2 = multiplyGFElement(m, lambda[1], (syndromes[2] ^ powerGFElement(m, syndromes[0], 3)));
      lambda[3] = divideGFElement(m, l3Numerator1 ^ l3Numerator2, syndromes[0]);
    }
    return lambda;
}

/**
 * This function finds roots of Lambda(x) using Chien search.
 * Returned values are final error positions in the received codeword.
 * The roots are then inverted to get the error positions, 
 * since the roots correspond to alpha^(-i) where i is the error position.
 */
std::vector<int> findRoots(int m, const std::vector<int>& lambda) {
    std::vector<int> errorPositions;
    int fieldOrder = m - 1;
    int degree = static_cast<int>(lambda.size());

    for (int rootExponent = 0; rootExponent < fieldOrder; ++rootExponent) {
        int x = retrieveGFElement(m, rootExponent);

        // Lambda(x) = 1 + L1*x + L2*x^2 + ...
        int lambdaValue = retrieveGFElement(m, 0); // 1
        for (int i = 0; i < degree; ++i) {
            if (lambda[i] == 0) continue;
            int xPower = powerGFElement(m, x, i + 1);
            lambdaValue ^= multiplyGFElement(m, lambda[i], xPower);
        }

        if (lambdaValue == 0) {
            int location = (fieldOrder - rootExponent) % fieldOrder;
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
    for (int pos : errorPositions) {
        if (pos >= 0 && pos < static_cast<int>(corrected.size()))
            corrected[pos] ^= 1;
    }
    return corrected;
}