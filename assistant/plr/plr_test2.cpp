#include "plr.h"
//#include "zipfian.h"
#include <random>
#include <stdio.h>
#include <stdlib.h>
#include <set>

//#define LIMIT_LBA_NUM 1000000
#define RANGE 64*1024*1024/4
//#define RANGE 
#define LIMIT_ROUND 1

int main(){
	
	srand(time(NULL));
	std::set<uint32_t>* lba_set=new std::set<uint32_t>();

	PLR temp_plr(7,5);
	temp_plr.insert(1,0);
	temp_plr.insert_end();

	double prev_merged_entry=0;
	for(uint32_t i=1; i<=LIMIT_ROUND; i++){
		uint64_t LIMIT_LBA_NUM=32*1024*1024/4/1024;
		while(lba_set->size()<LIMIT_LBA_NUM){
			uint32_t lba=rand();
			lba%=RANGE;
			lba_set->insert(lba);
		}

		PLR *plr=new PLR(7, 5);
		
		std::set<uint32_t>::iterator it;
		uint32_t ppa=0;
	
		for(it=lba_set->begin(); it!=lba_set->end(); it++){
			plr->insert(*it, ppa++/4);
		}
		
		plr->insert_end();
		printf("memory:%lf\n", (double)plr->get_line_cnt()*8/LIMIT_LBA_NUM);
		delete plr;
	}
	delete lba_set;

//	printf("memroy usage: %lf\n", (double)plr.memory_usage(32)/LIMIT_LBA_NUM);
}
