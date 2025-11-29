#include "Octan.h"
#include <cstdlib>
void print_tree_structure(const Octan& node, int indent, int octant_index,
                          int* out_max_depth, std::vector<int>* out_level_counts) {
    // Create tracking variables if this is the root call
    bool is_root_call = (octant_index == -1);
    int local_max_depth = 0;
    std::vector<int> local_level_counts;

    int* max_depth = out_max_depth ? out_max_depth : &local_max_depth;
    std::vector<int>* level_counts = out_level_counts ? out_level_counts : &local_level_counts;

    // Track depth and width
    int current_depth = indent;
    *max_depth = std::max(*max_depth, current_depth);

    if (current_depth >= (int)level_counts->size()) {
        level_counts->resize(current_depth + 1, 0);
    }
    (*level_counts)[current_depth]++;

    // Print formatting
    std::string spaces(indent * 4, ' ');

    if (octant_index >= 0) {
        std::cout << spaces << "├── Octant " << octant_index << " ";
        std::cout << "(";
        std::cout << ((octant_index & 4) ? "+" : "-") << "x,";
        std::cout << ((octant_index & 2) ? "+" : "-") << "y,";
        std::cout << ((octant_index & 1) ? "+" : "-") << "z)";
    } else {
        std::cout << "ROOT";
    }

    if (node.is_leaf) {
        if (node.has_body) {
            std::cout << " LEAF with Body #" << node.body_index;
        } else {
            std::cout << " EMPTY LEAF";
        }
    } else {
        std::cout << " INTERNAL";
    }
    std::cout << "\n";

    if (!node.is_leaf) {
        for (int i = 0; i < 8; i++) {
            if (node.children[i] != nullptr) {
                print_tree_structure(*node.children[i], indent + 1, i, max_depth, level_counts);
            }
        }
    }

    // Print dimensions at the end of root call
    if (is_root_call) {
        int max_width = 0;
        for (int count : *level_counts) {
            max_width = std::max(max_width, count);
        }
        int height = *max_depth + 1;
        std::cout << "Tree dimensions: " << max_width << "x" << height << "\n";
    }
}