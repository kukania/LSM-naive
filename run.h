#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <vector>
#include <set>
#include "mapping.h"
#include "assistant/BF/bloomfilter.h"

class Iterator{
public:
    enum STATE{
        INIT, HAS_DATA, NO_DATA,
    };
    STATE state;
    uint32_t idx;
    Iterator(){
        state=INIT;
    }
    virtual void init(){
        idx=0;
        state=HAS_DATA;
    }
    virtual uint32_t now()=0;
    virtual STATE move_next()=0;
    bool done(){
        return state==NO_DATA;
    }
};

class Run: public Iterator{
private:
    const float BF_FPR=0.1;
    uint32_t max_size;
    BF *bf;
    static uint32_t run_idx;

public:
    Mapping *indexing;
    enum QUERY_RETURN {
        RUN_NO_DATA, RUN_FOUND_DATA
    };
    std::vector<uint32_t> data;
    uint32_t now_run_idx;
    Run(uint32_t max_size, bool bf_on, bool indexing_on, uint64_t max_lba_range){
        this->max_size=max_size;
        data.clear();

        if(bf_on){
            bf=bf_init(max_size, BF_FPR);   
        }
        else{
            bf=NULL;
        }

        if(indexing_on){
            if((double)max_size/max_lba_range > 0.1){
                indexing=new PLR();
            }
            else{
                indexing=new FP();
            }
        }
        else{
            indexing=NULL;
        }

        now_run_idx=run_idx++;
    }

    bool isfull(){  
        if(data.size() > max_size){
            abort();
        }
        return data.size() == max_size;
    }

    void insert(uint32_t lba){
        if(data.size()!=0 && data.back() >= lba){
            printf("not sorted data!\n");
            abort();
        }

        if(bf){
            bf_put(bf, lba);
        }

        if(indexing){
            indexing->insert(lba, data.size());
        }

        data.push_back(lba);
    }

    void insert_finish(){
        if(indexing){
            indexing->make_done();
        }
    }

    uint64_t get_memory(uint64_t &member_num){
        member_num=data.size();
        uint64_t res=0;
        if(bf){
            res+=bf_bits(member_num, BF_FPR);
        }

        if(indexing){
            res+=indexing->get_memory(32);
        }
        return res;
    }

    QUERY_RETURN check(uint32_t lba){
        if(bf){
            if(bf_check(bf, lba)==false){
                return RUN_NO_DATA;
            }
        }
        return RUN_FOUND_DATA;
    }

    QUERY_RETURN query(uint32_t lba, uint32_t *psa){

        
        if(indexing){
            (*psa)=indexing->query(lba);
            if((*psa)==UINT32_MAX){
                return RUN_NO_DATA;
            }
            return RUN_FOUND_DATA;
        }
        else{
            return RUN_FOUND_DATA;
        }
    }

    void init(){
        idx=0;
        if(data.size() == idx){
            state=NO_DATA;
        }
    }

    uint32_t now(){
        return data[idx];
    }

    STATE move_next(){
        if(idx < data.size()-1){
            idx++;
            state=HAS_DATA;
            return HAS_DATA;
        }
        else{   
            state=NO_DATA;
            return NO_DATA;
        }
    }
    ~Run(){
        if(bf){
            bf_free(bf);
        }

        if(indexing){
            delete indexing;
        }
    }
};

typedef std::vector<Run*> Level;