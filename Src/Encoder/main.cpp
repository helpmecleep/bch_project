#include <iostream>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include "gfOps.h"


int main(){
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
    for (int root = b; root <= b + delta - 2; ++root) {
        for (const auto& coset : cosets) {
            if (std::find(coset.begin(), coset.end(), root) == coset.end()) continue;

            uint32_t mp = retrieveMinimalPolynomial(m, coset.front());

            if (std::find(result.begin(), result.end(), mp) == result.end()) result.push_back(mp);
            break;
        }
    }
    return result;
}

uint32_t generatorPolynomial(int m, int b, int delta) {
    std::vector<uint32_t> minimalPolynomials = matchCosetMP(m, b, delta);
    uint32_t generatorPoly = 1;
    for (uint32_t mp : minimalPolynomials) {
        generatorPoly = multiplyGFElement(m, generatorPoly, mp);
    }
    return generatorPoly;
}