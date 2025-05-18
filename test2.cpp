#include <iostream>

int main() {
    int n;

    std::cout << "Enter the number of terms: ";
    std::cin >> n;

    if (n <= 0) {
        std::cout << "Please enter a positive integer.\n";
        return 1;
    }

    long long a = 0, b = 1;

    std::cout << "Fibonacci Series: ";
    for (int i = 1; i <= n; ++i) {
        std::cout << a << " ";
        long long next = a + b;
        a = b;
        b = next;
    }

    std::cout << std::endl;
    return 0;
}
