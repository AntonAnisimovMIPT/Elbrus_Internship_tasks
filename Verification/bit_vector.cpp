#include <iostream>
#include <cstdint>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <vector>
#include <ranges>

template <typename T>
class bit_vector {
public:
    bit_vector(std::size_t size, T* initial_data) : bit_depth(size) {

        auto sz = (bit_depth + 8 * sizeof(T) - 1) / (8 * sizeof(T));
        field.resize(sz);
        for (std::size_t i = 0; i < sz; ++i) {
            
            if (i != sz - 1)
            {
                field[i] = initial_data[i];
            }

            else
            {
                auto number_digits_last_number = bit_depth - 8 * sizeof(T) * (sz - 1);
                if (number_digits_last_number >= 8 * sizeof(T))
                {
                    field[i] = initial_data[i];
                }
                else
                {
                    T mask = (1ULL << number_digits_last_number) - 1;
                    field[i] = initial_data[i] & mask;
                }
            }
        }
    }

    auto get_bit_depth() const {
        return bit_depth;
    }

    auto get_bit(std::size_t digit) const {
        
        auto view = std::views::all(field);
        auto elem_index = digit / (8 * sizeof(T));
        auto bit_index_in_elem = digit % (8 * sizeof(T));
        auto mask = 1 << bit_index_in_elem;
        return (view[elem_index] & mask) != 0;
        
    }

    auto set_bit(std::size_t digit, T value) {

        auto elem_index = digit / (8 * sizeof(T));
        auto bit_index_in_elem = digit % (8 * sizeof(T));;

        if (value) {
            field[elem_index] |= (1ULL << bit_index_in_elem);
        } else {
            field[elem_index] &= ~(1ULL << bit_index_in_elem);
        }
    }

    // считал (т.к. в условии не обговорено в какую сторону), что смещение задается влево, т.к. нумерация разрядов идет справа налево
    auto set_field(std::size_t offset, const bit_vector& bv) {
        for (std::size_t i = 0; i < bv.get_bit_depth(); ++i) {
            set_bit(offset + i, bv.get_bit(i));
        }
    }

    auto get_field(std::size_t offset, std::size_t field_size) const {
        auto sz = (field_size + 8 * sizeof(T) - 1) / (8 * sizeof(T));
        T* field_data = new T[sz];
        std::memset(field_data, 0, sz * sizeof(T));

        bit_vector<T> result(field_size, field_data);
        delete[] field_data;

        for (std::size_t i = 0; i < field_size; ++i) {
            result.set_bit(i, get_bit(offset + i));
        }
        return result;
    }

    // аналогично стандартной библиотеке промжуток вида [ , )
    auto to_integral_type(std::size_t offset, std::size_t field_size) const {
        uint64_t result = 0;
        for (std::size_t i = 0; i < field_size; ++i) {
            uint64_t bit_value = get_bit(offset + i);
            if (bit_value) {
                result |= (1ULL << i);
            }
        }
        return result;
    }

    auto hexadecimal() const {
        std::ostringstream oss;
        oss << std::hex;
        
        bool leading_zero = true;
        for (std::size_t i = 0; i < field.size(); ++i) {
            auto value = static_cast<uint64_t>(field[field.size() - i - 1]);
            for (int j = (2 * sizeof(T) - 1); j >= 0; --j) {
                auto nibble = (value >> (4 * j)) & 0xF;
                if (nibble || !leading_zero || (i == field.size() - 1 && j == 0)) {
                    oss << nibble;
                    leading_zero = false;
                }
            }
        }
        return oss.str();
    }

private:
    std::size_t bit_depth;
    std::vector<T> field; 
};