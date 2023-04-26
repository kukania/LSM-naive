#pragma once
#include <iostream>
#include <list>
#include <unordered_map>
#include <stdint.h>
using namespace std;
class cache_entry{
public:
    uint32_t entry_size;
    bool is_cache;
};


class LRUCache {
public:
    LRUCache(int capacity) : capacity(capacity) {
        now_capacity=0;
    }

    int get(int key) {
        if (cacheMap.find(key) == cacheMap.end()) {
            return -1;
        }
        cacheList.splice(cacheList.begin(), cacheList, cacheMap[key]);
        return cacheMap[key]->second;
    }

    void put(int key, uint32_t entry_size) {
        if (cacheMap.find(key) != cacheMap.end()) {
            cacheList.splice(cacheList.begin(), cacheList, cacheMap[key]);
            now_capacity-=cacheMap[key]->second;
            cacheMap[key]->second = entry_size;
        } else {
            while(now_capacity+entry_size > capacity){
                int keyToRemove = cacheList.back().first;
                cacheList.pop_back();
                now_capacity-=cacheMap[keyToRemove]->second;
                cacheMap.erase(keyToRemove);
            }
            cacheList.emplace_front(key, entry_size);
            cacheMap[key] = cacheList.begin();
            now_capacity+=entry_size;
        }
    }

private:
    uint32_t now_capacity;
    int capacity;
    list<pair<int, uint32_t> > cacheList;
    unordered_map<int, list<pair<int, uint32_t>>::iterator> cacheMap;
};
