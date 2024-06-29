#include <iostream>
#include <cstdint>
#include <sstream>
#include <iomanip>
#include <cstring>

template <typename T>
class bit_vector {
public:
    bit_vector(std::size_t size, T* initial_data) : bit_depth(size) {

        auto sz = bit_depth / (8 * sizeof(T));
        field = new T[sz];
        for (std::size_t i = 0; i < sz; ++i) {
            
            /*В соответствии с замечанием:
            "При этом нулевой разряд нулевого элемента массива соответствует нулевому разряду битвектора."*/
            field[i] = initial_data[sz - i - 1];
        }
    }

    auto get_bit_depth() const {
        return bit_depth;
    }

    auto get_bit(std::size_t digit) const {
        std::size_t index = bit_depth - digit - 1;
        auto elem_index = index / (8 * sizeof(T));
        auto bit_index_in_elem = index % (8 * sizeof(T));
        
        return (field[elem_index] >> ((8 * sizeof(T) - 1) - bit_index_in_elem)) & 0x1;
    }

    auto set_bit(std::size_t digit, T value) {
        std::size_t index = bit_depth - digit - 1;
        auto elem_index = index / (8 * sizeof(T));
        auto bit_index_in_elem = index % (8 * sizeof(T));

        if (value) {
            field[elem_index] |= (1ULL << (8 * sizeof(T) - 1) - bit_index_in_elem);
        } else {
            field[elem_index] &= ~(1ULL << (8 * sizeof(T) - 1) - bit_index_in_elem);
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
        oss << std::hex << std::setfill('0');
        for (std::size_t i = 0; i < (bit_depth + 8 * sizeof(T) - 1) / (8 * sizeof(T)); ++i) {
            oss << std::setw(2 * sizeof(T)) << static_cast<uint64_t>(field[i]);
        }
        return oss.str();
    }

private:
    std::size_t bit_depth;
    T* field; 
};