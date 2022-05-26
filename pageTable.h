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

#ifndef A_03_PAGE_H
#define A_03_PAGE_H


using namespace std;
#define NUMBEROFFRAMEINIT 0
#define NUMOFINITBYTES 0
#define NUMBEROFHITS 0
#define NUMBEROFMISS 0

struct Map{


    bool isPage;
    unsigned int frame;


};


struct Level{

    //point to pageTable
    struct pageTable *PageTablePtr;

    int Depth;
    //point to the map
    struct Map  *map;
    //double pointer, for point to next level
    struct Level **NextLevelPtr;


};

struct pageTable{


    //number of levels
    int LevelCount;
    //contain the bit mask for each level
    unsigned int* BitmaskAry;
    //contain the shift number for each level
    unsigned int* ShiftAry;
    //count the entries
    unsigned int* EntryCount;
    //total byte used for each level
    unsigned int* byteTotal;
    //the number of non-null entries
    unsigned int* numOfNullENtries;
    //contain the vpn
    uint32_t *pageNum;
    //the root node of Level
    Level *RootNodePtr;
    //page hit
    bool isPagehit;

    //total frame collected
    unsigned int frameCount = NUMBEROFFRAMEINIT;
    //total hit count
    unsigned int hitCount = NUMBEROFHITS;
    //miss count
    unsigned int missCount = NUMBEROFMISS;
};

// helper function to split the virtual address in certain level
uint32_t virtualAddressToPageNum (uint32_t virtualAddress, unsigned int mask, unsigned int shift);
//shirft the mask as the @param request
unsigned int getMask(unsigned int bit);
/*************************************************************************
 * insert virtual address into page table
 * @param levelIn pointer point to Level struct
 * @param virtualAddress is the virtual address used to split in multi-level
 * @param frame the pfn for each virtualAddress
 *************************************************************************/
void pageInsertLevels(Level *levelIn, uint32_t virtualAddress,  unsigned int frame);
/*************************************************************************
 * insert the virtual address into page table
 * @param pagetable pointer point to pageTable struct
 * @param virtualAddress is the virtual address used to split in multi-level
 * @param frame the pfn for each virtualAddress
 *************************************************************************/
void pageInsert(pageTable *pagetable, uint32_t virtualAddress, unsigned int frame);
/*************************************************************************
 * walk the page table see if there is a page exist; if not ,add it to the page table
 * @param pagetable pointer point to pageTable struct
 * @param virtualAddress is the virtual address used to split in multi-level
 * @return the page table map, or NULL if there this not the page in page table yet
 *************************************************************************/
Map * pageLookup(pageTable *pagetable, uint32_t virtualAddress);
/*************************************************************************
 * to count how many non null entries in each level
 * @param pagetable pointer point to pageTable struct
 * @param pos is the position to track the entries
 * @param levelIn the pointer point to Level struct
 *************************************************************************/
void countNonNullEntries(pageTable *pagetable, int pos, Level *levelIn);

#endif //A_03_PAGE_H
