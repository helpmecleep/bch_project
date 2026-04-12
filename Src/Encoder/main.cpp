#include <iostream>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include "../../Inc/gfOps.h"
#include "../../Inc/polyOps.h"  

std::vector<uint32_t> matchCosetMP(int m, int b, int delta);
uint32_t generatorPolynomial(int m, int b, int delta);
uint32_t generateCode(uint32_t message, uint32_t generator);

int main(){ 
    const int m = 8;
    const int t = 1;
    const int b = 0;
    const int delta = 2 * t;

    uint32_t g = generatorPolynomial(m, b, delta);
    int degree = polyDegree(g);
    uint32_t message = 0b101; // Example 3-bit message
    uint32_t codeword = generateCode(message, g);

    std::cout << "GF(" << m << "), t=" << t << ", b=" << b << ", delta=" << delta << '\n';
    std::cout << "g(x) bits: ";
    if (degree < 0) {
        std::cout << "0";
    }
    else {
        for (int i = degree; i >= 0; --i) {
            std::cout << (((g >> i) & 1U) ? '1' : '0');
        }
    }
    std::cout << '\n';

    std::cout << "Message bits: ";
    for (int i = 2; i >= 0; --i) {
        std::cout << (((message >> i) & 1U) ? '1' : '0');
    }
    std::cout << '\n';

    int cwDegree = polyDegree(codeword);
    std::cout << "Codeword bits: ";
    for (int i = cwDegree; i >= 0; --i) {
        std::cout << (((codeword >> i) & 1U) ? '1' : '0');
    }
    return 0;
}


/**
 * This function matches the cosests generated to their respective Minimal Polynomials.
 * The matching is done through the first element of the coset, which corresponds to the respective
 * exponent of the minimal polynomial
 * C1 = {1, 2, 4, 8} -> MP1 
 * C3 = {3, 6, 12, 24} -> MP3
 */
std::vector<uint32_t> matchCosetMP(int m, int b, int delta) {
    std::vector<std::vector<int>> cosets = generateCosets(m - 1);
    std::vector<uint32_t> result;
    // BCH roots are alpha^b through alpha^(b + delta - 1), inclusive.
    for (int root = b; root <= b + delta - 1; ++root) {
        for (const auto& coset : cosets) {
            if (std::find(coset.begin(), coset.end(), root) == coset.end()) continue;

            uint32_t mp = (coset.front() == 0)
                ? 0b11U
                : static_cast<uint32_t>(retrieveMinimalPolynomial(m, coset.front()));

            if (std::find(result.begin(), result.end(), mp) == result.end()) result.push_back(mp);
            break;
        }
    }
    return result;
}

/**
 * This function computes the generator polynomial for the BCH code 
 * based on the specified parameters m, b, and delta.
 */
uint32_t generatorPolynomial(int m, int b, int delta) {
    std::vector<uint32_t> minimalPolynomials = matchCosetMP(m, b, delta);
    uint32_t generatorPoly = 1U;
    for (uint32_t mp : minimalPolynomials) {
        generatorPoly = multiplyPolyElement(generatorPoly, mp);
    }
    return generatorPoly;
}   


uint32_t generateCode(uint32_t message, uint32_t generator) {
    int r = polyDegree(generator);
    uint32_t shiftedMessage = (message << r);
    uint32_t remainder = polyMod(shiftedMessage, generator);
    return shiftedMessage ^ remainder;
}