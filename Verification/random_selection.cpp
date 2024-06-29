#include <vector>
#include <random>
#include <algorithm>

std::vector<int> random_selection(const std::vector<int>& input_sequence, std::size_t n) {
    
    std::random_device device; 
    std::mt19937 engine(device());
    
    std::vector<int> mixed = input_sequence;
    std::shuffle(mixed.begin(), mixed.end(), engine);

    std::vector<int> selection(mixed.begin(), std::next(mixed.begin(), n));
    return selection;
}
