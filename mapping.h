#pragma once
#include <stdint.h>
#include <vector>
#include "org_plr.h"
#include "optimal_PLR.h"
struct exact_mapping{
    uint32_t ridx;
    uint32_t iidx;
};

class Mapping{
public:
    enum map_type{
        FP_INDEX, PLR_INDEX
    };
    map_type type;
    Mapping(map_type _type): type(_type){
    }
    virtual void insert(uint32_t lba, uint32_t psa)=0;
    virtual uint32_t query(uint32_t lba)=0;
    virtual uint64_t get_memory(uint32_t lba_size)=0;
    virtual void make_done(){
        return;
    }
};

#ifdef OPTIMAL_PLR
template <typename X, typename Y>
class SegmentationResults {
public:
    using CanonicalSegment = typename OptimalPiecewiseLinearModel<X, Y>::CanonicalSegment;

    // Constructor
    SegmentationResults() {}

    // Perform segmentation and store the results
    void perform_segmentation(size_t n, size_t epsilon, std::function<std::pair<X, Y>(size_t)> in) {
        auto out_fun = [this](const CanonicalSegment& cs) { this->segments.push_back(cs); };
        make_segmentation(n, epsilon, in, out_fun);
    }

    // Get the stored segmentation results
    const std::vector<CanonicalSegment>& get_results() const {
        return segments;
    }

private:
    std::vector<CanonicalSegment> segments;
};
#endif

class PLR: public Mapping{
private:
    const double FPR=0.55;
    bool prepare;
#ifdef OPTIMAL_PLR
    using canonical_segment = typename OptimalPiecewiseLinearModel<double, size_t>::CanonicalSegment;
    std::vector<std::pair<double,double> > data;
    SegmentationResults<double, double> results;
#else   
    std::vector<Point> data;
    std::vector<LineSegment> results;
#endif
public:
    PLR(): Mapping(Mapping::map_type::PLR_INDEX){
        prepare=false;
    }
    void insert(uint32_t lba, uint32_t psa);
    uint32_t query(uint32_t lba);
    uint64_t get_memory(uint32_t lba_size);
    void make_done();
};

class FP :public Mapping{
private:
    uint32_t data_size;
public:
    const double FPR=0.1;
    FP():Mapping(Mapping::map_type::FP_INDEX){
        data_size=0;
    }
    void insert(uint32_t lba, uint32_t psa){
        data_size++;
        return;
    }
    uint32_t query(uint32_t lba){
        if(rand()%100 <= FPR*100){
            //fail case   
            return UINT32_MAX;
        }
        else{
            //success case
            return 0;
        }
    }
    uint64_t get_memory(uint32_t lba_size){
        return data_size*40/8;
    }
};