#include "compaction.h"
#include <set>

Run* compaction(Level &run_array, bool bf_on, bool indexing_on, std::vector<exact_mapping> &org_map, uint32_t max_lba_range){
    std::set<uint32_t> temp_set;
    for(uint32_t i=0; i<run_array.size(); i++){
        Run *temp_run=run_array[i];
        temp_run->init();
        while(!temp_run->done()){
            temp_set.insert(temp_run->now());
            temp_run->move_next();
        }
    }

    Run *res=new Run(temp_set.size(), bf_on, indexing_on, max_lba_range);
    std::set<uint32_t>::iterator iter;
    uint32_t idx=0;
    for(iter=temp_set.begin(); iter!= temp_set.end(); iter++){
        //printf("%u %u %u\n", *iter, idx, temp_set.size());
        org_map[*iter].ridx=res->now_run_idx;
        org_map[*iter].iidx=idx++;
        res->insert(*iter);
    }
    res->insert_finish();
    return res;
}