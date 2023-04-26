#include <stdio.h>
#include <stdlib.h>
#include <set>
#include <stdint.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <string>
#include <random>
#include "mapping.h"

#define CAPACITY (uint64_t)4*1024*1024*1024
#define LBA_RANGE (CAPACITY)/(4*1024)
#define WRITE_BUFFER (1*1024*1024)/(4*1024)

int main(){
    PLR target_mapping;
    std::set<uint32_t> temp_set;

    while(temp_set.size() < LBA_RANGE/4){
        temp_set.insert(rand()%LBA_RANGE);
    }

    PLR my_plr;
    std::set<uint32_t>::iterator iter=temp_set.begin();
    for(uint32_t i=0; iter!=temp_set.end(); iter++, i++){
        my_plr.insert(*iter,i);
    }
    my_plr.make_done();

    iter=temp_set.begin();
    uint32_t correct=0;
    uint32_t incorrect=0;
    for(uint32_t i=0; iter!=temp_set.end(); iter++, i++){
        uint32_t res=my_plr.query(*iter);
        if(res==i){
            correct++;
        }
        else{
            incorrect++;
        }
    }

    printf("%lf %u %u\n", (double)correct/(correct+incorrect), correct, incorrect);
}