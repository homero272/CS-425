/////////////////////////////////////////////////////////////////////////////
//
//  Number.h
//
//  A class storing a natural number (an unsigned integer value) represented
//    by its digits (as compared to computer binary representation).  The
//    implementation leverages the C++ standard library's deque (double-ended
//    queue) class for storing the digits (which are represented as their
//    numeric value stored in an unsigned char).
//
//  The std::deque forms the base class of the Number class so that we can
//    easily access all of deque's methods.

#include <algorithm>
#include <deque>
#include <iostream>
#include <iterator>

using Digit = unsigned char;

struct Number : public std::deque<Digit> {
    //
    // --- Constructors ---
    //

    // Default constructor; empty initializes the deque
    Number() = default;

    // Populate the number using a string of digits.  The assign() method
    //   copies the input data directly into the deque.
    Number(const char* digits, size_t len) {
        resize(len);
        assign(digits, digits + len);
    }

    // Populate the number using a character representation of the input
    //   number.  Each ASCII digit is converted into its value representation
    //   by subtracting the ASCII code for zero.
    Number(const std::string& s) {
        std::transform(s.begin(), s.end(), std::back_inserter(*this),
            [](auto c){ return (c - '0'); } 
        );
    }

    // Populate the number using a binary integer representation.
    Number(unsigned long long v) {
        while (v) {
            push_front(v % 10);
            v /= 10;
        }
    }

    // Initialize an empty number but with allocated memory for storing
    //   <n> digits.
    explicit Number(size_t n) 
        { resize(n); }

    // Copy and reverse the digits of the stored value, returning a new
    //   Number value 
    Number reverse() const {
        Number n(size());
        std::reverse_copy(begin(), end(), n.begin());
        return n;
    }

    // Determine if the stored value is a palindrome
    bool is_palindrome() const {
        auto midpoint = begin() + size() / 2;
        return std::equal(begin(), midpoint, rbegin());
    }

    // The less-than operator, useful for sorting a list of Numbers
    bool operator < (const Number& n) const {
        if (size() == n.size()) {
            for (auto i = 0; i < size(); ++i) {
                auto lhs = at(i);
                auto rhs = n.at(i);
                if (lhs != rhs) { return lhs < rhs; }
            }
        }

        return size() < n.size();
    }

    // The output stream insertion operator.  This will print the Number
    //   converting its binary digits to their ASCII equivalents.
    friend std::ostream& operator << (std::ostream& os, const Number& n) {
        std::for_each(n.begin(), n.end(), 
            [&](auto d) { os << char('0' + d); }
        );
        return os;
    }
};