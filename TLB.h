/******************************************************************************************
 * Assignment 3
 * Zhiyu Li		    #821009794			cssc1433
 * 03/25/22
 ******************************************************************************************/
#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <sstream>
#include <iomanip>
#include <streambuf>
#include <vector>
#include <utility>
#include <map>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <stdio.h>
#include <sys/stat.h>
#include<math.h>
#include <time.h>
#include <bitset>
#include <unistd.h>
#include "pageTable.h"

#ifndef A_03_BACKUP_3_TLB_H
#define A_03_BACKUP_3_TLB_H

struct tlbCache{
    //key and value for mostRecentMap
    unsigned int vpn,accessTime;
    //pointer point to previous node or next node
    tlbCache *prev, *next;
    //constructor
    tlbCache(unsigned int key, unsigned int value){
        vpn = key;
        accessTime = value;
        prev = NULL;
        next = NULL;
    }
};

struct tlb{
    //tlb hit
    bool isTlbHit;
    // page hit
    bool isPageHit;
    //access time count
    unsigned int acc;
    //a array for track the least recent access time
    unsigned int *arr;
    //frame number
    unsigned int frame;
    //vpn
    unsigned int logicalNum;
    unsigned int cacheHit;
};
class TLB{

private:


public:
    //hash map of LRU
    unordered_map<unsigned int, tlbCache*> mostRecentMap;

    //hash map of tlb
    unordered_map<unsigned int, unsigned int> tlbMap;
    //the size of mostRecentMap
    int mostRecentUsed;
    //the size of tlbMap
    int tlbSize;
    //tlb pointer
    tlb *table;
    //head and tail of linked list
    tlbCache *head;
    tlbCache *tail;
    //constructor
    TLB(int size, int tlbmapsize, tlb *&table1, unsigned int start);
/*************************************************************************
 * Insert the vpn into TLB cache
 * @param vpn it is the virtual page number
 * @param page pointer point to pageTable
 *************************************************************************/
    void insertToTlbmap(unsigned int vpn, pageTable *page);
/*************************************************************************
 * Determine there is the tlb hit or not
 * @param vpn it is the virtual page number
 * @param page pointer point to pageTable
 *************************************************************************/
    unsigned int getTLB(unsigned int vpn, pageTable *page);
    /*************************************************************************
 * Keep update the mostRecentMap, delete the least used one if the size
 * reach out full
 * @param vpn it is the virtual page number
 * @param accessTime it is the access time from last visit
 *************************************************************************/
    void recentUse(unsigned int vpn, unsigned int accessTime);
    /*************************************************************************
 * Search the vpn in mostRecentMap hash map
 * @param vpn it is the virtual page number
 * @return the accesstime of vpn
 *************************************************************************/
    unsigned int searchMap(unsigned int vpn);
/*************************************************************************
 * Insert the recently used node into the linked list based on mostRecentMap hash map
 * @param currNode it is the pointer belong to value of mostRecentMap hash map
 *************************************************************************/
    void recentAccessInsert(tlbCache *currNode);
/*************************************************************************
 * delete the certain entry in linked list
 * @param currNode it is the pointer belong to value of mostRecentMap hash map
 *************************************************************************/
    void entryDelete(tlbCache *currNode);


};


#endif //A_03_BACKUP_3_TLB_H
