#include "mapping.h"

void SIDX::insert(uint32_t lba, uint32_t psa){
    data.push_back(lba);
}


void SIDX::make_done(){
    uint32_t prev=0;
    for(uint32_t i=0; i<data.size(); i++){
        if(i==0){
            header_list.push_back(data[i]);
            prev=data[i];
            bitmap.push_back(true);
            continue;
        }

        if(prev+1!=data[i]){
            header_list.push_back(data[i]);
            bitmap.push_back(true);
        }
        else{
            bitmap.push_back(false);   
        }
        prev=data[i];
    }

    data.clear();
    data.shrink_to_fit();
}

uint32_t SIDX::query(uint32_t lba){
    return 0;
}

uint64_t SIDX::get_memory(uint32_t lba_size){
    return header_list.size()*4+bitmap.size()/8;
}
