#ifndef ULTIMATENUMBER_ULTIMATENUMBER_H
#define ULTIMATENUMBER_ULTIMATENUMBER_H

#include <string>
#include <iostream>

class UltimateNumber {
private:

    //properties
    std::string integer_part = "0";
    std::string remainder_part = "0";
    bool is_float = false;
    bool is_negative = false;
    bool is_zero = true;

    //methods
    void init(std::string&);
    static void number_validation(std::string&);
    static std::string& add_two_integers(std::string&, std::string&);
    void sub_bigger_from_smaller(std::string, std::string);
    void static remove_zeroes_from_beginning(std::string&);
    void remove_zeroes_from_beginning();
    void remove_zeroes_from_end(std::string&);
    void remove_zeroes_from_end();
    void update_properties();
    void declare_sign(UltimateNumber&);
    void add_two_positive_ultimate_numbers(std::string&, std::string&);
    void multiply_by_ten();
    unsigned long long separator_place();
    static bool compare_numbers_strict(UltimateNumber&, UltimateNumber&);
    static bool compare_numbers_loose(UltimateNumber&, UltimateNumber&);
    static bool compare_numbers_despite_sign_strict(UltimateNumber&, UltimateNumber&);
    static bool compare_numbers_despite_sign_loose(UltimateNumber&, UltimateNumber&);
    static bool is_equal(UltimateNumber&, UltimateNumber&);
    static bool is_equal_despite_sign(UltimateNumber&, UltimateNumber&);


public:
    //constructors
    explicit UltimateNumber(std::string&);
    explicit UltimateNumber(std::string&&);
    explicit UltimateNumber() = default;


    //addition variations
    UltimateNumber add(UltimateNumber&);

    //subtraction variations
    UltimateNumber sub(UltimateNumber&);

    //multiplication variations
    UltimateNumber multiply(UltimateNumber&);

    //division variations
    UltimateNumber divide(UltimateNumber&);

    //modulus variations
    UltimateNumber modulus(UltimateNumber&);

    //correct string value of number
    std::string& value();

    //operator overloading
    //compare operators
    bool operator > (UltimateNumber&);
    bool operator >= (UltimateNumber&);
    bool operator < (UltimateNumber&);
    bool operator <= (UltimateNumber&);
    bool operator == (UltimateNumber&);

    //IO operators
    friend std::ostream& operator << (std::ostream&, const UltimateNumber&);
};

#endif //ULTIMATENUMBER_ULTIMATENUMBER_H
