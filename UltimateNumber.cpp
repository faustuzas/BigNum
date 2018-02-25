#include "UltimateNumber.h"
#define PRECISION 100
using namespace std;

//constructors
UltimateNumber::UltimateNumber(string& num) {
    this->init(num);
}

UltimateNumber::UltimateNumber(string&& num) {
    this->init(num);
}

void UltimateNumber::init(std::string& num) {
    //validate is there any unaccepted symbols, if yes throws exception
    number_validation(num);

    if (num[0] == '-') {
        this->is_negative = true;
        num.erase(0, 1);
    }

    for (char& c : num) {
        if (c != '.' && c != ',' && c != '0') {
            this->is_zero = false;
            break;
        }
    }

    //trim number and check if number is float
    remove_zeroes_from_beginning(num);
    for (char& c : num) {
        if (c == '.' || c == ',' ) {
            this->is_float = true;
            break;
        }
    }
    remove_zeroes_from_end(num);


    size_t separator_place = 0;

    //find separator place
    if(this->is_float) {
        for (size_t i = num.length(); i >= 1; i--) {
            size_t j = i - 1;

            if (num[j] == '.' || num[j] == ',') {
                separator_place = num.length() - j - 1;
                break;
            }
        }
    }

    //according to separator place extract integer and remainder part
    if (this->is_float) {
        this->integer_part = num.substr(0, num.length() - separator_place - 1);
        if (this->integer_part.empty()) {
            this->integer_part = "0";
        }
        this->remainder_part = num.substr(num.length() - separator_place, num.length());
    } else {
        this->integer_part = num;
        this->remainder_part = "0";
    }
}


//arithmetic
UltimateNumber UltimateNumber::add(UltimateNumber& number) {
    if (number.is_zero) {
        UltimateNumber result = *this;
        return result;
    }

    if (this->is_zero) {
        UltimateNumber result = number;
        return result;
    }

    if (!this->is_negative && !number.is_negative) {
        UltimateNumber result = *this;
        result.add_two_positive_ultimate_numbers(number.integer_part, number.remainder_part);
        return result;
    } else {
        return this->sub(number);
    }
}

UltimateNumber UltimateNumber::sub(UltimateNumber& number) {
    if (number.is_zero) {
        return UltimateNumber(*this);
    }

    if (this->is_zero) {
        return UltimateNumber(number);
    }

    if (UltimateNumber::is_equal_despite_sign(*this, number) && this->is_negative != number.is_negative) {
        return UltimateNumber();
    }

    bool is_this_bigger = compare_numbers_despite_sign_strict(*this, number);

    if (!this->is_negative && !number.is_negative && is_this_bigger) {
        UltimateNumber result = *this;
        result.sub_bigger_from_smaller(number.integer_part, number.remainder_part);
        return result;
    }

    if (!this->is_negative && !number.is_negative && !is_this_bigger) {
        UltimateNumber result = number;
        result.sub_bigger_from_smaller(this->integer_part, this->remainder_part);
        result.is_negative = true;
        return result;
    }

    if (!this->is_negative && number.is_negative) {
        UltimateNumber result = *this;
        result.add_two_positive_ultimate_numbers(number.integer_part, number.remainder_part);
        return result;
    }

    if (this->is_negative && !number.is_negative) {
        UltimateNumber result = *this;
        result.add_two_positive_ultimate_numbers(number.integer_part, number.remainder_part);
        result.is_negative = true;
        return result;
    }

    if (this->is_negative && number.is_negative && is_this_bigger) {
        UltimateNumber result = *this;
        result.sub_bigger_from_smaller(number.integer_part, number.remainder_part);
        return result;
    }

    if (this->is_negative && number.is_negative && !is_this_bigger) {
        UltimateNumber result = number;
        result.sub_bigger_from_smaller(this->integer_part, this->remainder_part);
        result.is_negative = false;
        return result;
    }
}

UltimateNumber UltimateNumber::multiply(UltimateNumber& number) {
    string result = "0";

    if (this->is_zero || number.is_zero) {
        this->integer_part = "0";
        this->remainder_part = "0";
        this->is_zero = true;
        this->update_properties();
        return *this;
    }

    //make strings from numbers which contains all digits without any other symbols
    string full_number_1 = this->integer_part;
    if (this->remainder_part != "0") {
        full_number_1 += this->remainder_part;
    }
    this->remove_zeroes_from_beginning(full_number_1);

    string full_number_2 = number.integer_part;
    if (number.remainder_part != "0") {
        full_number_2 += number.remainder_part;
    }
    this->remove_zeroes_from_beginning(full_number_2);

    //find shorter number for easier multiplication
    if (full_number_1.length() < full_number_2.length()) {
        string tmp = full_number_2;
        full_number_2 = full_number_1;
        full_number_1 = tmp;
    }

    bool first_loop = true;

    //multiply first number by each digit in second number and sum up
    for (size_t i = full_number_2.length(); i >= 1; i--) {
        size_t j = i - 1;

        //multiply first number by ten if it isn't first loop
        if(first_loop){
            first_loop = false;
        } else {
            full_number_1.insert(full_number_1.end(), 1, '0');
        }

        //multiply that many times what digit is
        for (short k = 0; k < full_number_2[j] - '0'; k++) {
            result = add_two_integers(result, full_number_1);
        }
    }

    //find position of separator by summing up operands' separator places
    size_t pointPlace = result.length() - (this->separator_place() + number.separator_place() - 1) - 1;

    //if new number is float insert separator
    if (pointPlace != 0) {
        result.insert(pointPlace, ".");
    }

    if (this->is_negative != number.is_negative) {
        result.insert(0, "-");
    }

    return UltimateNumber(result);
}

UltimateNumber UltimateNumber::divide(UltimateNumber& number) {
    if (number.is_zero) {
        throw invalid_argument("Division by zero.");
    }

    if (this->is_zero) {
        return UltimateNumber(*this);
    }

    if (*this == number) {
        UltimateNumber result("1");
        result.is_negative = this->is_negative == number.is_negative;
        return result;
    }

    string result_integer   = "0";
    string temp_remainder   = "0";
    string one              = "1";
    string result_remainder;

    UltimateNumber result_num = *this;

    while (compare_numbers_despite_sign_loose(result_num, number)) {
        result_num.sub_bigger_from_smaller(number.integer_part, number.remainder_part);
        result_integer = add_two_integers(result_integer, one);
    }

    for (int i = 0; i < PRECISION; i++) {
        if (!result_num.is_zero) {
            result_num.multiply_by_ten();
            temp_remainder = "0";
            while (compare_numbers_despite_sign_loose(result_num, number)) {
                result_num.sub_bigger_from_smaller( number.integer_part, number.remainder_part);
                temp_remainder = add_two_integers(temp_remainder, one);
            }
            result_remainder.insert(result_remainder.end(), 1, '0');
            result_remainder = add_two_integers(result_remainder, temp_remainder);
        } else {
            break;
        }
    }

    result_num.integer_part   = result_integer;
    result_num.remainder_part = result_remainder;

    result_num.update_properties();

    return result_num;
}

UltimateNumber UltimateNumber::modulus(UltimateNumber& number) {
    if (number.is_zero) {
        throw invalid_argument("Modulus by zero.");
    }

    if (this->is_zero) {
        return UltimateNumber(*this);
    }

    if (*this == number) {
        return UltimateNumber("0");
    }

    UltimateNumber result_num = *this;

    while (compare_numbers_despite_sign_loose(result_num, number)) {
        result_num.sub_bigger_from_smaller(number.integer_part, number.remainder_part);
    }

    return result_num;
}


//comparison
bool UltimateNumber::compare_numbers_strict(UltimateNumber& number1, UltimateNumber& number2) {
    if ( number1.is_negative != number2.is_negative ) {
        return !number1.is_negative;
    }

    if (number1.integer_part.length() != number2.integer_part.length()) {
        return number1.integer_part.length() > number2.integer_part.length() == !number1.is_negative;
    }

    for (size_t i = 0; i < number1.integer_part.length(); i++) {
        if (number1.integer_part[i] != number2.integer_part[i]) {
            return number1.integer_part[i] > number2.integer_part[i] == !number1.is_negative;
        }
    }

    for (size_t i = 0; i < number1.remainder_part.length(); i++) {
        if (number1.remainder_part[i] != number2.remainder_part[i]) {
            return number1.remainder_part[i] > number2.remainder_part[i] == !number1.is_negative;
        }
    }

    return false;
}

bool UltimateNumber::compare_numbers_loose(UltimateNumber& number1, UltimateNumber& number2) {
    if (number1.value() == number2.value()) {
        return true;
    } else {
        return compare_numbers_strict(number1, number2);
    }
}

bool UltimateNumber::compare_numbers_despite_sign_strict(UltimateNumber& number1, UltimateNumber& number2) {
    if (number1.integer_part.length() != number2.integer_part.length()) {
        return number1.integer_part.length() > number2.integer_part.length();
    }

    for (size_t i = 0; i < number1.integer_part.length(); i++) {
        if (number1.integer_part[i] != number2.integer_part[i]) {
            return number1.integer_part[i] > number2.integer_part[i] ;
        }
    }

    for (size_t i = 0; i < number1.remainder_part.length(); i++) {
        if (number1.remainder_part[i] != number2.remainder_part[i]) {
            return number1.remainder_part[i] > number2.remainder_part[i];
        }
    }

    return false;
}

bool UltimateNumber::compare_numbers_despite_sign_loose(UltimateNumber& number1, UltimateNumber& number2) {
    if (is_equal_despite_sign(number1, number2)) {
        return true;
    } else {
        return compare_numbers_despite_sign_strict(number1, number2);
    }
}

bool UltimateNumber::is_equal(UltimateNumber& number1, UltimateNumber& number2) {
    return number1.value() == number2.value();
}

bool UltimateNumber::is_equal_despite_sign(UltimateNumber& number1, UltimateNumber& number2) {
    if (number1.integer_part.length() != number2.integer_part.length()) {
        return false;
    }

    if (number1.remainder_part.length() != number2.remainder_part.length()) {
        return false;
    }

    for (size_t i = 0; i < number1.integer_part.length(); i++) {
        if (number1.integer_part[i] != number2.integer_part[i]) {
            return false;
        }
    }

    for (size_t i = 0; i < number1.remainder_part.length(); i++) {
        if (number1.remainder_part[i] != number2.remainder_part[i]) {
            return false;
        }
    }

    return true;
}


//auxiliary functions
void UltimateNumber::sub_bigger_from_smaller(string smaller_integer, string smaller_remainder) {

    //filling empty spaces with zeros for easier operations
    if (this->remainder_part.length() > smaller_remainder.length()) {
        smaller_remainder.insert(smaller_remainder.end(), this->remainder_part.length() - smaller_remainder.length(), '0');
    } else {
        this->remainder_part.insert(this->remainder_part.end(), smaller_remainder.length() - this->remainder_part.length(), '0');
    }

    bool exceeded_ten = false;

    //subtract remainders
    for (size_t i = this->remainder_part.length(); i >= 1 ; i--) {
        size_t j = i - 1;

        int x = this->remainder_part[j] - smaller_remainder[j];

        if (exceeded_ten) {
            x--;
        }

        if (x < 0) {
            exceeded_ten = true;
            x += 10;
        } else {
            exceeded_ten = false;
        }

        this->remainder_part[j] = (char) (x + '0');
    }

    //filling empty spaces with zeros for easier operations
    if (this->integer_part.length() > smaller_integer.length()) {
        smaller_integer.insert(smaller_integer.begin(), this->integer_part.length() - smaller_integer.length(), '0');
    } else {
        this->integer_part.insert(this->integer_part.begin(), smaller_integer.length() - this->integer_part.length(), '0');
    }

    //subtract integers
    for (size_t i = this->integer_part.length(); i >= 1 ; i--) {
        size_t j = i - 1;

        int x = this->integer_part[j] - smaller_integer[j];

        if (exceeded_ten) {
            x--;
        }

        if (x < 0) {
            exceeded_ten = true;
            x += 10;
        } else {
            exceeded_ten = false;
        }

        this->integer_part[j] = (char) (x + '0');
    }

    this->update_properties();
}

void UltimateNumber::add_two_positive_ultimate_numbers(string &add_integer, string &add_remainder) {
    string result_remainder_part = this->remainder_part;

    //filling empty spaces with zeros for easier operations
    if (result_remainder_part.length() > add_remainder.length()) {
        add_remainder.insert(add_remainder.end(), result_remainder_part.length() - add_remainder.length(), '0');
    } else {
        result_remainder_part.insert(result_remainder_part.end(), add_remainder.length() - result_remainder_part.length(), '0');
    }

    //remainder addition
    bool moreThanTen = false;

    for (size_t i = result_remainder_part.length(); i >= 1; --i) {
        size_t j = i - 1;

        int x = result_remainder_part[j] + add_remainder[j] - 2 * '0';

        if (moreThanTen) {
            x++;
        }

        if (x >= 10) {
            x -= 10;
            moreThanTen = true;
        } else {
            moreThanTen = false;
        }

        result_remainder_part[j] = (char) (x + '0');
    }

    this->integer_part   = add_two_integers(this->integer_part, add_integer);
    this->remainder_part = result_remainder_part;

    this->update_properties();
}

void UltimateNumber::multiply_by_ten() {
    if (this->is_float) {
        this->integer_part.insert(this->integer_part.end(), 1, this->remainder_part[0]);
        this->remainder_part.erase(0, 1);
    } else {
        this->integer_part.insert(this->integer_part.end(), 1, '0');
    }
}

void UltimateNumber::update_properties() {
    remove_zeroes_from_beginning();
    remove_zeroes_from_end();

    this->remainder_part = this->remainder_part.empty() ? "0" : this->remainder_part;
    this->is_float       = this->remainder_part != "0";
    this->is_zero        = this->remainder_part == "0" && this->integer_part == "0";
}

void UltimateNumber::number_validation(string& str) {
    bool has_separator = false;

    for (char& c : str) {
        if (!((c >= '0' && c <= '9') || c == '-' || c == ',' || c == '.')) {
            throw invalid_argument("Only numbers, '-', ',' and '.' are allowed.");
        } else {
            if (c == ',' || c == '.') {
                if (has_separator) {
                    throw invalid_argument("Only one separator is allowed.");
                } else {
                    has_separator = true;
                }
            }
        }
    }
}

void UltimateNumber::remove_zeroes_from_beginning(string &str) {
    for (size_t i = 0; i < str.length(); i++) {
        if (str[i] != '0') {
            str = str.substr(i, str.length());
            break;
        }
    }
}

void UltimateNumber::remove_zeroes_from_end(string &str) {
    if (this->is_float) {

        bool has_non_zero_chars = false;

        for (size_t i = str.length(); i >= 1; i--) {
            if (str[i - 1] != '0') {
                str = str.substr(0, i);
                has_non_zero_chars = true;
                break;
            }
        }

        if (!has_non_zero_chars) {
            this->is_float = false;
            this->remainder_part = "0";
        }
    }
}

void UltimateNumber::remove_zeroes_from_beginning() {
    remove_zeroes_from_beginning(this->integer_part);
}

void UltimateNumber::remove_zeroes_from_end() {
    remove_zeroes_from_end(this->remainder_part);
}

string& UltimateNumber::value() {
    string result;

    if (this->is_negative) {
        result.append("-");
    }

    result.append(this->integer_part);

    if (this->is_float) {
        result.append(".");
        result.append(this->remainder_part);
    }

    return result;
}

string& UltimateNumber::add_two_integers(string &number1, string &number2) {

    //filling empty spaces with zeros for easier operations
    if (number1.length() > number2.length()) {
        number2.insert(number2.begin(), number1.length() - number2.length(), '0');
    } else {
        number1.insert(number1.begin(), number2.length() - number1.length(), '0');
    }

    bool more_than_ten = false;

    for (size_t i = number1.length(); i >= 1; i--) {
        size_t j = i - 1;

        int x = number1[j] + number2[j] - 2 * '0';

        if (more_than_ten) {
            x++;
        }

        if (x >= 10) {
            x -= 10;
            more_than_ten = true;
        } else {
            more_than_ten = false;
        }

        number1[j] = (char) (x + '0');
    }

    if (more_than_ten) {
        number1.insert(number1.begin(), '1');
    }

    return number1;
}

unsigned long long UltimateNumber::separator_place() {
    return this->remainder_part == "0" ? 0 : this->remainder_part.length();
}


//operators overloading
bool UltimateNumber::operator < (UltimateNumber& number)
{
    return UltimateNumber::compare_numbers_strict(number, *this);
}

bool UltimateNumber::operator <= (UltimateNumber& number)
{
    return UltimateNumber::compare_numbers_loose(number, *this);
}

bool UltimateNumber::operator > (UltimateNumber& number)
{
    return UltimateNumber::compare_numbers_strict(*this, number);
}

bool UltimateNumber::operator >= (UltimateNumber& number)
{
    return UltimateNumber::compare_numbers_loose(*this, number);
}

bool UltimateNumber::operator == (UltimateNumber& number)
{
    return UltimateNumber::is_equal(*this, number);
}

ostream& operator << (ostream& out, const UltimateNumber& number)
{
    if (number.is_zero) {
        return out << "0";
    }

    if (number.is_negative) {
        out << "-";
    }

    out << number.integer_part;

    if (number.is_float) {
        out << "." << number.remainder_part;
    }

    return out;
}



