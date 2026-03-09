#pragma once
#include <vector>
#include <sstream>
#include <chrono>
#include <algorithm>

namespace numberTheory {



    inline bool factor(long int a, long int b) {
        return b % a == 0;
    }


    inline std::vector<long int> factors(unsigned long long int a, unsigned long long int b) {
        std::vector<long int> gcd;
        for (long int i = 1; i <= std::min(a, b); i++) {
            if (factor(i, a) && factor(i, b)) {
                gcd.push_back(i);
            }
        }
        return gcd;
    }
    inline bool isPrime(long int a) {
        if (a <= 1) return false;
        for (long int i = 2; i * i <= a; ++i) {
            if (a % i == 0) return false;
        }
        return true;
    }

    
    
inline std::vector<unsigned long long> prime_factors(unsigned long long a) {
    std::vector<unsigned long long> factors;
    if (a < 2) return factors;

    for (unsigned long long i = 2; i * i <= a; ++i) {
        while (a % i == 0) {
            factors.push_back(i);
            a /= i;
        }
    }
    if (a > 1) {
        factors.push_back(a);
    }

    return factors;
}

    inline std::string gcd(bool time, unsigned long long int x, unsigned long long int y) {
        std::vector<long int> set;
        decltype(std::chrono::high_resolution_clock::now()) start;
        decltype(std::chrono::high_resolution_clock::now()) end;

        if (time) {
            start = std::chrono::high_resolution_clock::now();
            set = factors(x, y);
            end = std::chrono::high_resolution_clock::now();
        }
        else {
            set = factors(x, y);
        }

        std::stringstream ss;

        ss << "D(a) = {";
        for (const auto& i : set) {
            ss << "i = " << i << ",";
        }
        ss << "}\n";
        ss << "max D(a): " << *std::max_element(set.begin(), set.end()) << "\n";
        if (time) {
            ss << "Tempo: "
                << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
                << " ms\n";
        }

        return ss.str();

    }


}