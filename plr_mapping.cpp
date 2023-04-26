#include "mapping.h"
#include <algorithm>

void PLR::insert(uint32_t lba, uint32_t psa){
#ifdef OPTIMAL_PLR
    data.push_back(std::pair<double, double>(lba, psa));
#else
    if(prepare==false){
        prepare=true;
        this->data.clear();
    }
    Point p;
    p.x=lba;
    p.y=psa;
    data.push_back(p);
#endif
}

int lower_bound(std::vector<int>& vec, int target) {
    int left = 0, right = vec.size() - 1;
    int result = -1;

    while (left <= right) {
        int mid = left + (right - left) / 2;

        if (vec[mid] >= target) {
            result = mid;
            right = mid - 1;
        } else {
            left = mid + 1;
        }
    }

    return result;
}

uint32_t PLR::query(uint32_t lba){
#ifdef OPTIMAL_PLR
    auto segments=results.get_results();
    int left = 0, right = segments.size() - 1;
    int result = -1;

    /*found upper bound*/
    while (left <= right) {
        int mid = left + (right - left) / 2;

        if (segments[mid].get_first_x() > lba) {
            result = mid;
            right = mid - 1;
        } else {
            left = mid + 1;
        }
    }

    if(result==-1){
        return UINT32_MAX;
    }
    else if(result==0){
        result=-1;
    }
    else{
        result--;
    }

    auto target_segment=segments[result];
    double res;
    try{
        res=target_segment.get_y(lba);
    }
    catch(std::out_of_range a){
        return UINT32_MAX;
    }
    return (uint32_t)res;
#else
    abort();
    return 1;
#endif
}

uint64_t PLR::get_memory(uint32_t lba_size){
#ifdef OPTIMAL_PLR
    return results.get_results().size() * (8+8+4+4);
#else
    return results.size() * (8+8+4+4);
#endif
}

void PLR::make_done(){
#ifdef OPTIMAL_PLR
    results.perform_segmentation(data.size(), FPR, [&](size_t i) { return data[i]; });
#else
    results=slidingWindowPiecewiseLinearRegression(data, FPR);
#endif
    data.clear();
    data.shrink_to_fit();
}