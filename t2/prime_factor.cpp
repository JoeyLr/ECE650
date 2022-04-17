#include <iostream>
#include <sstream>
#include <vector>
#include <cmath>

//using namespace std;


std::string prime_factor(unsigned x) {
    std::stringstream out;

    // YOUR CODE GOES HERE
    std::vector<unsigned> resList;
    std::vector<std::string> outputList;

    // if x == 2 or 3, then return the number itself
    if (x <= 3) {
        resList.push_back(x);
    } else {
        // if x > 3,  divide the number from 2 to the maximum iteration and check whether the input number can be divisible by the number.
        int maxIteration = (int) sqrt(x);
        for (int num = 2; num <= maxIteration; num++) {
            bool hasPrime = false;
            while (x % num == 0) {
                x /= num;
                resList.push_back(num);
                hasPrime = true;
            }
            if (hasPrime) {
                maxIteration = (int) sqrt(x);
            }
        }
        if (x > 1) {
            resList.push_back(x);
        }
    }
    int count = 1;

    // output the result to the string stream
    for (unsigned i = 0; i < resList.size(); i++) {
        if (resList[i] == resList[i + 1]) {
            count = count + 1;
        } else {
            std::string output;
            output += std::to_string(resList[i]);
            if (count > 1) {
                output += "^";
                output += std::to_string(count);
                count = 1;
            }
            if (i < resList.size() - 1) {
                output += " x ";
            }
            out << output;
            outputList.push_back(output);
        }
    }

    return out.str();
}


int main() {

    auto input = 60;
    auto output = prime_factor(input);

    std::cout << "Prime factorization of " << input << " is " << output
              << std::endl;
    // expected output: 3^2


    input = 4;
    output = prime_factor(input);

    std::cout << "Prime factorization of " << input << " is " << output
              << std::endl;
    // expected output: 2^0


    input = 8320;
    output = prime_factor(input);

    std::cout << "Prime factorization of " << input << " is " << output
              << std::endl;
    // expected output: 2^4 x 3 x 5
    return 0;
}
