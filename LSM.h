#pragma once
#include "run.h"
#include <set>

typedef std::set<uint32_t> memtable;

class LSM_param{
public:
    bool bf_on;
    bool indexing_on;
    uint32_t size_factor;
    uint64_t total_LBA_number;
    uint32_t max_level;
    bool bf_simulation;

    LSM_param(bool _bf_on, bool _indexing_on, uint32_t _size_factor, uint64_t _total_LBA_number, uint32_t _max_level, bool _bf_simulation): 
    bf_on(_bf_on), indexing_on(_indexing_on), size_factor(_size_factor), total_LBA_number(_total_LBA_number), max_level(_max_level), bf_simulation(_bf_simulation)
    {}
};

struct PLR_memory{
    double min;
    double max;
    uint64_t sum;
    uint64_t num;
};

struct Traffic{
    uint64_t read_op;
    uint64_t total_read_IO;
    uint64_t error_read_IO;
    uint64_t insert_op;
    uint64_t total_insert_IO;
};

class LSM{
private:
    LSM_param param;
    memtable table;
    std::vector<exact_mapping> map;
    std::vector<Level> level_list;
    std::vector<PLR_memory> monitor;
    std::vector<uint32_t> level_size;
    uint32_t memtable_size;
    Traffic t_monitor;
    void monitoring(uint32_t level_idx, Run *new_run);
public:
    LSM(LSM_param _param, uint32_t _memtable_size): param(_param), memtable_size(_memtable_size){
        map.reserve(param.total_LBA_number);
        t_monitor={0,0,0,0,0};
    }
    void insert(uint32_t lba);
    void query(uint32_t lba); 
    void print();
    void monitor_clear();
    void traffic_monitor_clear();
    void traffic_print();
    void print_final_memory_per_bit();
};

typedef std::vector<Level>::iterator Level_iter;