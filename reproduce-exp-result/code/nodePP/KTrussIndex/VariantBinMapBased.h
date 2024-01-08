#include <vector>
#include <unordered_map>
#include <cassert>

#ifndef KTRUSS_V1_VARIANTBIN_H
#define KTRUSS_V1_VARIANTBIN_H

// A bin-sort variant data structure for maintaining top-k score/upperbound.
class VariantBinMapBased {
public:
    VariantBinMapBased() {
    }

    ~VariantBinMapBased() {
    }

    void insert(int id, int id_score) {
        bin[id_score].push_back(id);
        index[id] = bin[id_score].size() - 1;
    }

    void update(int id, int id_score, int pre_score) {
        // First delete it, Second insert it.
        assert(index.count(id));
        int pre_index = index[id];
        int final_index = (int) bin[pre_score].size() - 1;
        int final_id = bin[pre_score][final_index];

        if (final_index != pre_index) {
            bin[pre_score][pre_index] = final_id;
            index[final_id] = pre_index;
        }
        bin[pre_score].erase(bin[pre_score].begin() + final_index);

        bin[id_score].push_back(id);

        index[id] = bin[id_score].size() - 1;
    }

    bool hasAnyBin(int score) const {
        return bin.count(score);
    }

    const std::vector<int> &getBin(int score) const {
        return bin.at(score);
    }

private:
    std::unordered_map<int, std::vector<int> > bin;
    std::unordered_map<int, int> index;
};

#endif //KTRUSS_V1_VARIANTBIN_H
