#include <stdio.h>
#include <stdlib.h>
#include <set>
#include <stdint.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <string>
#include <random>
#include "LSM.h"

#define CAPACITY (uint64_t)64*1024*1024*1024
#define LBA_RANGE (CAPACITY)/(4*1024)
#define WRITE_BUFFER (1*1024*1024)/(4*1024)

int main(int argc, char *argv[]){
    //LSM init

    std::string file_name(argv[1]);
    std::ifstream file_stream(file_name);

    bool cache_on=false;
    if(argc==3){
        cache_on=true;
    }
    LSM_param param(false, true, 10, LBA_RANGE, 5, true, cache_on);
    uint64_t cache_size=LBA_RANGE*4;
    cache_size=cache_size*30/100;
    LSM lsmtree(param, WRITE_BUFFER, cache_size);


    uint32_t line_cnt=0;
    uint32_t T_cnt=0;
    if(file_stream.is_open()){
        std::string line;
        while(std::getline(file_stream, line)){
            char type=0;
            uint64_t lba;
            line_cnt++;
            if(line_cnt %10000==0){
                printf("%u\n", line_cnt);
                /*
                if(T_cnt>=2){
                    fprintf(stderr, "%u\n", line_cnt);
                    lsmtree.print();
                    lsmtree.traffic_print();
                    lsmtree.print_final_memory_per_bit();
                    fflush(stderr);
                }*/
            }

            sscanf(line.c_str(), "%c%lu", &type, &lba);
            if(type=='W'){
                lsmtree.insert(lba);
            }
            else if(type=='T'){
                //lsmtree.print();
                //lsmtree.monitor_clear();
                T_cnt++;
                lsmtree.print();
                /*
                if(T_cnt<2){
                    lsmtree.traffic_monitor_clear();
                }*/
            }
            else{
                /*
                if(T_cnt>=2){
                    lsmtree.query(lba);
                }*/
            }
        }
    }
    else{
        printf("cannot open file!\n");
    }

    lsmtree.print();
    lsmtree.traffic_print();
    lsmtree.print_final_memory_per_bit();
    return 0;
}