#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <stdexcept>
#include <stdio.h>
#include <string>
#include <vector>

// #define PRINT_TESTS
// #define TEST_ERROR

/**
 * Index of stick positions to shape the numbers:
 *
 *   —    1
 *  | |  0 2
 *   —    3
 *  | |  4 6
 *   —    5
 *
 *  0: top left
 *  1: top middle
 *  2: top right
 *  3: middle
 *  4: bottom left
 *  5: bottom middle
 *  6: bottom right
 */
static constexpr struct stick_positions_t {
    //                   0b1000000 >> 0123456
    static constexpr int zero  = {  0b1110111  }; // zero
    static constexpr int one   = {  0b0010001  }; // one
    static constexpr int two   = {  0b0111110  }; // two
    static constexpr int three = {  0b0111011  }; // three
    static constexpr int four  = {  0b1011001  }; // four
    static constexpr int five  = {  0b1101011  }; // five
    static constexpr int six   = {  0b1101111  }; // six
    static constexpr int seven = {  0b0110001  }; // seven
    static constexpr int eight = {  0b1111111  }; // eight
    static constexpr int nine  = {  0b1111011  }; // nine

    static constexpr int get_val_pos(int _value) noexcept {
        switch (_value) {
            case 0: return zero;
            case 1: return one;
            case 2: return two;
            case 3: return three;
            case 4: return four;
            case 5: return five;
            case 6: return six;
            case 7: return seven;
            case 8: return eight;
            case 9: return nine;
        }

        return -1;
    }
} stick_positions;

class num_base_t {
    protected:
    uint64_t value;

    public:
    num_base_t(uint64_t _value) noexcept : value(_value) {}

    num_base_t &set_value(uint64_t _value) noexcept {
        value = _value;
        return *this;
    }

    uint64_t get_value() const noexcept {
        return value;
    }
};

class stick_num_t : public num_base_t {
    public:
    stick_num_t(uint64_t _value) noexcept : num_base_t(_value) {}

    stick_num_t &set_value(uint64_t _value) noexcept {
        if (_value > 9 || _value < 0) return *this;
        num_base_t::set_value(_value);
        return *this;
    }

    int get_pos() const noexcept {
        return stick_positions.get_val_pos(value);
    }

    std::string build_line_horizontal(int idx) const noexcept {
        constexpr char fmt[] = " %s ";

        int pos = get_pos();
        std::string buf;
        buf.resize(8);

        snprintf(buf.data(), buf.size(),
                fmt,
                longdash(cmpidx(pos,idx)));

        return buf;
    }

    std::string build_line_vertical(int idx_left, int idx_right) const noexcept {
        constexpr char fmt[] = "%s %s";

        int pos = get_pos();
        std::string buf;
        buf.resize(8);

        snprintf(buf.data(), buf.size(),
                fmt, 
                pipe(cmpidx(pos,idx_left)),
                pipe(cmpidx(pos,idx_right)));

        return buf;
    }

    std::vector<std::string> get_print_lines() const noexcept {
        std::vector<std::string> lines;
        lines.reserve(5);

        lines.emplace_back(build_line_horizontal(1));
        lines.emplace_back(build_line_vertical(0, 2));
        lines.emplace_back(build_line_horizontal(3));
        lines.emplace_back(build_line_vertical(4, 6));
        lines.emplace_back(build_line_horizontal(5));

        return lines;
    }

    std::string get_print_buf() const noexcept {
        int pos = get_pos();
        std::string buf;
        buf.resize(32);

        snprintf(buf.data(), buf.size(),
                " %s \n%s %s\n %s \n%s %s\n %s ",
                longdash(cmpidx(pos,1)),
                pipe(cmpidx(pos,0)),
                pipe(cmpidx(pos,2)),
                longdash(cmpidx(pos,3)),
                pipe(cmpidx(pos,4)),
                pipe(cmpidx(pos,6)),
                longdash(cmpidx(pos,5)));

        return buf;
    }

    const stick_num_t &print() const noexcept {
        std::string buf = get_print_buf();

        printf("%s\n", buf.c_str());

        return *this;
    }

    uint64_t diff_with(const stick_num_t &o) const noexcept {
        uint64_t c = 0;
        int val = get_pos();
        int oval = o.get_pos();

        for (int i = 0; i < 7; i++) {
            if (cmpidx(val, i) != cmpidx(oval, i)) c++;
        }

        return c;
    }

    uint64_t diff_count(const stick_num_t &o) const noexcept {
        uint64_t l = stick_count();
        uint64_t r = o.stick_count();
        return l > r ? l - r : r - l;
    }

    uint64_t stick_count() const noexcept {
        uint64_t c = 0;
        int val = get_pos();

        for (int i = 0; i < 7; i++) {
            if (cmpidx(val, i)) c++;
        }

        return c;
    }

    static bool cmpidx(int pos, int idx) noexcept {
        int val_true = (0b1000000 >> idx);
        return (pos & val_true) == val_true;
    }

    static constexpr const char *longdash(const bool v) noexcept {
        return char_or_space(v, "—");
    }

    static constexpr const char *pipe(const bool v) noexcept {
        return char_or_space(v, "|");
    }

    static constexpr const char *char_or_space(const bool v, const char *c) noexcept {
        return v ? c : " ";
    }
};

class stick_digit_t : public num_base_t {
    using digit_vec = std::vector<stick_num_t>;
    digit_vec digits;
    uint n_digit;

    public:
    stick_digit_t(uint64_t _value, uint _n_digit) : num_base_t(_value), n_digit(_n_digit) {
        parse_value(_value);
    }

    stick_digit_t &set_value(uint64_t _value, uint _n_digit) {
        num_base_t::set_value(_value);
        return parse_value(_value);
    }

    digit_vec get_digits() const noexcept {
        return digits;
    }

    stick_digit_t &parse_value(uint64_t _value) {
        const std::string s = std::to_string(_value);
        size_t s_len = s.length();

        if (n_digit < s_len) {
            std::string err = std::string("n_digit(") + std::to_string(n_digit) + ") < value(" + s + ").length()";
            throw std::logic_error(err);
        }

        digits.clear();

        if (int64_t prepend_zeroes = (int64_t)n_digit - s_len; prepend_zeroes > 0) {
            for (int64_t i = 0; i < prepend_zeroes; i++) {
                digits.emplace_back(0);
            }
        }

        for (const auto c : s) {
            const char v[2] = {c, '\0'};
            digits.emplace_back(std::atoi(v));
        }

        return *this;
    }

    uint64_t diff_with(const stick_digit_t &o) const noexcept {
        uint64_t c = 0;
        const digit_vec odigits = o.get_digits();
        const size_t digits_size = digits.size();
        const size_t odigits_size = odigits.size();

        bool mine_has_more_digit = digits_size >= odigits_size;
        const digit_vec &longer_digits = mine_has_more_digit ? digits : odigits;
        const digit_vec &shorter_digits = mine_has_more_digit ? odigits : digits;
        const size_t longer_digits_size = mine_has_more_digit ? digits_size : odigits_size;
        const size_t shorter_digits_size = mine_has_more_digit ? odigits_size : digits_size;

        for (size_t i = 0; i < longer_digits_size; i++) {
            const stick_num_t &vl = longer_digits.at(i);
            const bool shorter_has_current_digit = shorter_digits_size >= (i+1);
            if (!shorter_has_current_digit) {
                c+=vl.stick_count();
                continue;
            }
            const stick_num_t &vs = shorter_digits.at(i);
            c+=vl.diff_with(vs);
        }

        return c;
    }

    uint64_t diff_count(const stick_digit_t &o) const noexcept {
        uint64_t l = stick_count();
        uint64_t r = o.stick_count();
        return l > r ? l - r : r - l;
    }

    uint64_t stick_count() const noexcept {
        uint64_t c = 0;

        for (const auto &v : digits) {
            c+=v.stick_count();
        }

        return c;
    }

    const stick_digit_t &print() const noexcept {
        std::vector<std::vector<std::string>> bufs;
        for (const stick_num_t &n : digits) {
            bufs.emplace_back(n.get_print_lines());
        }

        for (int i = 0; i < 5; i++) {
            for (const auto &v : bufs) {
                printf("%s  ", v.at(i).c_str());
            }
            printf("\n");
        }

        return *this;
    }
};

int main() {
#ifdef PRINT_TESTS
    for (int i = 0; i < 10; i++) {
        printf(" ===== %d:\n", i);
        stick_num_t s(i);
        s.print();
        printf(" =====\n");
    }

    for (int i = 0; i < 10; i++) {
        printf(" ===== 0x%d: ", i);
        stick_num_t s(i);
        printf("%ld\n %d stick count: %ld\n =====\n", s.diff_with(0), i, s.stick_count());
    }

    for (int i = 1000; i < 1010; i++) {
        printf(" ===== digits 999x%d: ", i);
        stick_digit_t s(i, 4);
        printf("%ld\n %d stick count: %ld\n =====\n", s.diff_with({999, 4}), i, s.stick_count());
#ifdef TEST_ERROR
        stick_digit_t s2(i, 3);
#endif
    }
#endif

    printf("\n\n ========== BEGIN CHALLENGE ========== \n\n");
    uint64_t r = 995;
    uint64_t c = 994;

    const stick_digit_t top{995, 3};
    const uint64_t top_stick_count = top.stick_count();

    // 2 moves meaning 4 changes in the stick position
    const int N_CHANGES = 4;

    while (c > 0) {
        const stick_digit_t cs{c, 3};

        if (top_stick_count == cs.stick_count() && top.diff_with(cs) == N_CHANGES) r = c;

        c--;
    }

    printf("\n\n =============== RESULT =============== \n");
    printf("> %ld <\n", r);

    const stick_digit_t cs{r, 3};
    cs.print();

    return 0;
}
