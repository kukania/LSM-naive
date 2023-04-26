#include "LSM.h"
#include "compaction.h"
#include <string.h>

void Level_clear(Level &level){
    for(uint32_t i=0; i<level.size(); i++){
        delete level[i];
    }
}

void LSM::monitoring(uint32_t level_idx, Run *new_run){
    while(monitor.size() <level_idx+1){
        PLR_memory temp={1024,0,0,0};
        monitor.push_back(temp);
    }

    PLR_memory &target=monitor[level_idx];
    uint64_t member;
    uint64_t memory=new_run->get_memory(member);

    target.num+=member;
    target.sum+=memory;

    double bits_per_memory=(double)memory*8/member;
    if(target.min > bits_per_memory){
        target.min=bits_per_memory;
    }
    
    if(target.max < bits_per_memory){
        target.max=bits_per_memory;
    }
}

void LSM::print(){
    static int cnt=0;
    fprintf(stderr, "%u\n", cnt++);
    for(uint32_t i=0; i<monitor.size(); i++){
        PLR_memory &target=monitor[i];
        fprintf(stderr, "%u %lf %lf %lf\n",i, target.max, target.min, (double)target.sum*8/target.num);
    }
}

void LSM::traffic_print(){
    fprintf(stderr, "WAF,%lf,RAF,%lf,%lf,%lf\n", (double)t_monitor.total_insert_IO/t_monitor.insert_op, (double)t_monitor.total_read_IO/t_monitor.read_op, (double)t_monitor.error_read_IO/t_monitor.read_op, (double)t_monitor.cache_read_IO/t_monitor.read_op);
}

void LSM::monitor_clear(){
    monitor.clear();
}

void LSM::traffic_monitor_clear(){
    memset(&t_monitor, 0, sizeof(Traffic));
}

void LSM::insert(uint32_t lba){
    t_monitor.insert_op++;
    if(table.size() < memtable_size){
        table.insert(lba);
        return;
    }


    t_monitor.total_insert_IO+=table.size();
    /*flush*/
    Run *flush_run=new Run(table.size(), param.bf_on, param.indexing_on, param.total_LBA_number);
    std::set<uint32_t>::iterator iter;
    uint32_t iidx=0;
    for(iter=table.begin(); iter!=table.end(); iter++){
        map[*iter].ridx=flush_run->now_run_idx;
        map[*iter].iidx=iidx++;
        flush_run->insert(*iter);
    }
    flush_run->insert_finish();
    if(param.cache_on){
        uint64_t member_num;
        uint32_t entry_size=flush_run->get_memory(member_num);
        cache->put(flush_run->now_run_idx, entry_size);
    }

    monitoring(0, flush_run);
    table.clear();

    if(level_list.size()==0){
        Level temp_level;
        level_list.push_back(temp_level);
    }

    /*compaction*/
    level_list[0].push_back(flush_run);
    for(uint32_t i=0; i<level_list.size(); i++){
        Level& temp=level_list[i];
        if(temp.size()==param.size_factor){
            Run *new_run=compaction(temp, param.bf_on, param.indexing_on, map, param.total_LBA_number);

            if(param.cache_on){
                uint64_t member_num;
                uint32_t entry_size=new_run->get_memory(member_num);
                cache->put(new_run->now_run_idx, entry_size);
            }

            t_monitor.total_insert_IO+=new_run->data.size();
            Level_clear(temp);
            temp.clear();

            if(i==param.max_level){
                temp.push_back(new_run);
                monitoring(i+1, new_run);
                continue;
            }
            else if(i==level_list.size()-1){
                Level temp_new_level;
                level_list.push_back(temp_new_level);
            }

            level_list[i+1].push_back(new_run);
            monitoring(i+1, new_run);
        }
    }
}

void LSM::query(uint32_t lba){
    t_monitor.read_op++;
    for(uint32_t i=0; i<level_list.size(); i++){
        Level& temp=level_list[i];
        if(temp.size()==0) continue;
        for(int32_t j=temp.size()-1; j>=0; j--){
            Run *r_temp=temp[j];
            uint32_t psa;
            //check run

            if(param.bf_simulation){
                if(map[lba].ridx == r_temp->now_run_idx){
                    //t_monitor.total_read_IO++;
                    //always success
                }
                else if(rand()%100 < 10){
                    //t_monitor.total_read_IO++;
                }
                else{
                    continue;
                }
            }
            else if(r_temp->check(lba)==Run::QUERY_RETURN::RUN_NO_DATA){
                continue;
            }

            if(param.cache_on){
                if(cache->get(r_temp->now_run_idx)==-1){
                    t_monitor.total_read_IO++;
                    t_monitor.cache_read_IO++;
                    uint64_t member;
                    uint32_t entry_size=r_temp->get_memory(member);
                    cache->put(r_temp->now_run_idx, entry_size);
                }
            }

            uint32_t loop=0;
            switch(r_temp->indexing->type){
                case Mapping::map_type::FP_INDEX:
                    if(map[lba].ridx==r_temp->now_run_idx){
                        if(r_temp->query(lba, &psa) == Run::QUERY_RETURN::RUN_FOUND_DATA){
                            //one time found
                            t_monitor.total_read_IO++;
                        }
                        else{
                            //two time found
                            t_monitor.total_read_IO+=2;
                            t_monitor.error_read_IO++;
                        }
                    }
                    else{
                        for(loop=0; loop<2; loop++){
                            if (r_temp->query(lba, &psa) == Run::QUERY_RETURN::RUN_FOUND_DATA){
                                t_monitor.total_read_IO++;
                                t_monitor.error_read_IO++;
                            }
                        }
                    }
                    break;
                case Mapping::map_type::PLR_INDEX:
                    if(map[lba].ridx==r_temp->now_run_idx){
                        r_temp->query(lba, &psa);
                        if (map[lba].iidx != psa){
                            //two time found
                            t_monitor.total_read_IO+=2;
                            t_monitor.error_read_IO++;
                        }
                        else{
                            //one time found
                            t_monitor.total_read_IO++;
                        }
                    }
                    else{
                        if(r_temp->query(lba, &psa) == Run::QUERY_RETURN::RUN_FOUND_DATA){
                            //two time fail;
                            t_monitor.total_read_IO+=2;
                            t_monitor.error_read_IO+=2;
                        }
                    }
                    break;
            }
        }
    }
}

void LSM::print_final_memory_per_bit(){
    uint64_t FP_data=0;
    uint64_t FP_memory=0;

    uint64_t PLR_data=0;
    uint64_t PLR_memory=0;

    for(uint32_t level_idx=0; level_idx<level_list.size(); level_idx++){
        Level &temp=level_list[level_idx];
        for(uint32_t run_idx=0; run_idx<temp.size(); run_idx++){
            Run *r=temp[run_idx];
            switch(r->indexing->type){
                case Mapping::map_type::FP_INDEX:
                FP_data+=r->data.size();
                FP_memory+=r->indexing->get_memory(32);
                break;
                case Mapping::map_type::PLR_INDEX:
                PLR_data+=r->data.size();
                PLR_memory+=r->indexing->get_memory(32);
                break;
            }
        }
    } 

    fprintf(stderr, "TYPE, BITS-PER-ENTRY, TOTAL-BYTE, DATA-NUM\n");
    fprintf(stderr, "FP, %lf, %lu, %lu\n", (double)FP_memory*8/FP_data, FP_memory, FP_data);
    fprintf(stderr, "PLR, %lf, %lu, %lu\n", (double)PLR_memory*8/PLR_data, PLR_memory, PLR_data);
    fprintf(stderr, "ALL, %lf, %lu, %lu\n", (double)(FP_memory+PLR_memory)*8/(FP_data+PLR_data), FP_memory+PLR_memory, FP_data+PLR_memory);
}