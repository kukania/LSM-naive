all: compaction.cpp LSM.cpp main.cpp org_plr.cpp plr_mapping.cpp run.cpp
	g++ -std=c++17 -O3 -g -o naive_lsm compaction.cpp LSM.cpp main.cpp org_plr.cpp plr_mapping.cpp assistant//BF/bloomfilter.cpp run.cpp


plr_test:
	g++ -std=c++17 -O3 -g -o plr_test_main compaction.cpp LSM.cpp plr_test_main.cpp org_plr.cpp plr_mapping.cpp assistant//BF/bloomfilter.cpp run.cpp
