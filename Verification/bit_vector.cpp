#include <iostream>
#include <cstdint>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <vector>
#include <ranges>
#include <cmath>

class bit_vector {
public:
    template <typename T>
    bit_vector(std::size_t size, const T* initial_data) : bit_depth(size) {

        auto field_size = std::ceil(static_cast<double>(bit_depth) / (8 * sizeof(T)));
        field.reserve(field_size);

        std::uint64_t filled_element = 0;

        // счетчик считываемого элемента
        auto read_counter = 0;
        // счетчик элемента, в который записывают
        auto write_counter = 0;
        // счетчик итоговой последовательности
        auto seq_counter = 0;
        // позиция элемента исходного массива
        auto i_elem = 0;

        while (seq_counter < (bit_depth))
        {
                // случай, когда дошли до конца заполняемого элемента
                if (write_counter == 64)
                {
                    field.push_back(filled_element);

                    filled_element = 0;
                    write_counter = 0;
                }

                if (read_counter >= 8 * sizeof(T))
                {
                    read_counter = 0;
                    i_elem++;
                }
                
                auto i_bit = (initial_data[i_elem] >> read_counter) & 1;

                filled_element = (filled_element & ~(1ULL << write_counter)) | (i_bit << write_counter);

                read_counter++;
                write_counter++;
                seq_counter++;

            
        }

        if (write_counter > 0) 
        {
            field.push_back(filled_element);
            
        }

    }
    
    auto get_bit_depth() const {
        return bit_depth;
    }

    auto get_bit(std::size_t digit) const {
        
        auto view = std::views::all(field);
        auto elem_index = digit / (8 * sizeof(std::uint64_t));
        auto bit_index_in_elem = digit % (8 * sizeof(std::uint64_t));
        auto mask = 1 << bit_index_in_elem;
        return (view[elem_index] & mask) != 0;
        
    }

    auto set_bit(std::size_t digit, auto value) {

        auto elem_index = digit / (8 * sizeof(std::uint64_t));
        auto bit_index_in_elem = digit % (8 * sizeof(std::uint64_t));;

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
        auto sz = (field_size + 8 * sizeof(std::uint64_t) - 1) / (8 * sizeof(std::uint64_t));
        std::uint64_t* field_data = new std::uint64_t[sz];
        std::memset(field_data, 0, sz * sizeof(std::uint64_t));

        bit_vector result(field_size, field_data);
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
            for (int j = (2 * sizeof(std::uint64_t) - 1); j >= 0; --j) {
                auto nibble = (value >> (4 * j)) & 0xF;
                if (nibble || !leading_zero || (i == field.size() - 1 && j == 0)) {
                    oss << nibble;
                    leading_zero = false;
                }
            }
        }
        return oss.str();
    }

    auto significant_bits(auto n) {
        auto count = 0;
        while (n) {
            n >>= 1;
            count++;
        }
        return count;
    }

private:
    std::size_t bit_depth;
    std::vector<std::uint64_t> field; 
};

int main() {};