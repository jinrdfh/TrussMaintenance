#include <vector>
#include <cstring>

#ifndef KTRUSS_V1_VARIANTBIN_H
#define KTRUSS_V1_VARIANTBIN_H

// A bin-sort variant data structure for maintaining top-k score/upperbound.
class VariantBin
{
public:
    VariantBin() {}
    VariantBin(int m, int maxScore) {
        init(m, maxScore);
    }
    void init(int m, int maxScore) {
        this->maxScore = maxScore;
        bin = new std::vector<int>[maxScore];
        binCount = 0;

        index = new int[m];
        memset(index, -1, m * sizeof(int));
    }
    ~VariantBin() {
        for (int i = 0; i < maxScore; i++)
            bin[i].clear();
        delete[] bin;
        delete[] index;
    }
    void insert(int id, int id_score) {
        bin[id_score].push_back(id);
        index[id] = bin[id_score].size() - 1;
        binCount++;
    }
    void update(int id, int id_score, int pre_score) {
        //First delete it, Second insert it.
        int pre_index = index[id];
        int final_index = bin[pre_score].size()-1;
        int final_id = bin[pre_score][final_index];

        if (final_index != pre_index) {
            bin[pre_score][pre_index] = final_id;
            index[final_id] = pre_index;
        }
        bin[pre_score].erase(bin[pre_score].begin()+final_index);

        bin[id_score].push_back(id);

        index[id] = bin[id_score].size() - 1;
    }
    const std::vector<int>& getBin(int score) const {
        return bin[score];
    }
    int getMaxScore() const {
        return maxScore;
    }
private:
    std::vector<int> *bin;
    int binCount, maxScore; //Score_index
    int *index;
};

#endif //KTRUSS_V1_VARIANTBIN_H
