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
#include "tracereader.h"
#include "output.h"
#include "TLB.h"

#define NUMBEROFFRAMEINIT 0
#define BADFLAG 0
#define TOTALBIT 32
#define NUMOFADDR 0
#define DEFULTTLBSIZE 0
#define INITACCTIME 0
#define LEVEL 0
#define INITDEPTH 0
#define CACHEHIT 0
#define THESIZEOFLRU 10
using namespace std;




#define STANDALONE  /* #define to use this as a program */
int main(int argc, char **argv) {
    //initialize the default total bits in the program
    int shiftTmp = TOTALBIT;
    //the default number of total address need to implement from optional argument
    int num = NUMOFADDR;
    //to determine which mode need to implement
    string mode;
    //the default size of tlb
    int numTlb = DEFULTTLBSIZE;
    //for get the optional argument
    int Option;
    //magic number
    //

    int numOfBits = 0;

    while ( (Option = getopt(argc, argv, "n:o:c:")) != -1) {
        switch (Option) {
            case 'n': /* Number of addresses to process */
                // optarg will contain the string following -n
                // Process appropriately (e.g. convert to integer atoi(optarg))
                num = (int) atoi(optarg);
                break;
            case 'o': /* produce map of pages */
                // optarg contains the output method…
                mode = optarg;
                break;

            case 'c':
                numTlb = (int) atoi(optarg);

                break;
            default:
                // print something about the usage and die…
                exit(BADFLAG); // BADFLAG is an error # defined in a header
        } }

    int idx = optind;


    //if the cache of tlb is out of range
    if(numTlb < 0){
        fprintf(stderr,"Cache capacity must be a number, greater than or equal to 0\n");
        exit(BADFLAG);
    }


    //page table allocate
    pageTable *pagetable = new pageTable();

    //for initialize the array from each level
    //start from the index 0
    int levelTrace = LEVEL;
    if(idx < argc){
        //initialize the level
        pagetable->LevelCount = (argc-1) - (idx);
        pagetable->isPagehit = false;
        //allocate the dynamic memory for below array by level count
        pagetable->BitmaskAry = new unsigned int[pagetable->LevelCount];
        pagetable->EntryCount = new unsigned int[pagetable->LevelCount];
        pagetable->ShiftAry = new unsigned int[pagetable->LevelCount];
        pagetable->pageNum = new unsigned int[pagetable->LevelCount];
        pagetable->numOfNullENtries = new unsigned int[pagetable->LevelCount];
        pagetable->byteTotal = new unsigned int[pagetable->LevelCount];
        for(int i = idx+1; i <= argc-1; ++i ){
            if(atoi(argv[i]) < 1){
                fprintf(stderr,"Level %d page table must be at least 1 bit\n",levelTrace);
                exit(BADFLAG);
            }
            numOfBits += atoi(argv[i]);
            if(numOfBits >28){
                fprintf(stderr,"Too many bits used in page tables\n");
                exit(BADFLAG);
            }
            //get the shift number
            shiftTmp = shiftTmp - atoi(argv[i]);
            pagetable->EntryCount[levelTrace] = pow(2,atoi(argv[i]));
            pagetable->BitmaskAry[levelTrace] = getMask(atoi(argv[i]))<<shiftTmp;
            pagetable->ShiftAry[levelTrace] = shiftTmp;
            levelTrace++;
        }
    }

    //total page size
    unsigned int page_size = pow(2, pagetable->ShiftAry[pagetable->LevelCount-1]);

    //initialize the address counter
    unsigned int addrProcessed = NUMOFADDR;
    //Level allocate
    Level *levelT = new Level();
    levelT->PageTablePtr = pagetable;
    //the start of depth is 0
    levelT->Depth = INITDEPTH;
    levelT->map = NULL;

    //allocate the memory for nextLevel
    levelT->NextLevelPtr = (Level **)calloc(pagetable->EntryCount[levelT->Depth], sizeof(Level));

    //initialize the root node
    pagetable->RootNodePtr = levelT;
    //tlb struct pointer from the TLB.h
    tlb *tlbC = new tlb();

    //initialize the array with tlb size
    tlbC->arr = new unsigned int[numTlb];
    tlbC->cacheHit = CACHEHIT;


    int sizeOfLRUmap = THESIZEOFLRU;
    TLB *testTlb = new TLB(sizeOfLRUmap, numTlb,tlbC, 0 );


    FILE *ifp;
    p2AddrTr *trace = new p2AddrTr ();
    uint32_t t;
    uint32_t offset;

    //get the mask for offset
    unsigned int offsetMask = getMask(pagetable->ShiftAry[pagetable->LevelCount-1]);
    if ((ifp = fopen(argv[idx],"rb")) == NULL) {
        fprintf(stderr,"Unable to open <<%s>>\n",argv[idx]);
        exit(1);
    }

    if(mode == "bitmasks"){
        //print out the bit masks
        report_bitmasks(pagetable->LevelCount,pagetable->BitmaskAry);
    } else{
        while (!feof(ifp)){

            if (NextAddress(ifp, trace)) {

                t = trace->addr;
                addrProcessed++;

                //get the offset
                offset = t & getMask(pagetable->ShiftAry[pagetable->LevelCount-1]);
                if(mode == "offset"){
                    hexnum(offset);
                }else if( mode == "vpn2pfn"){
                    //walk the pagetable
                    pageLookup(pagetable,t);
                    report_pagemap(pagetable->LevelCount,pagetable->pageNum, pageLookup(pagetable,t)->frame);
                }else if(mode == "virtual2physical"){
                    //walk the pagetable
                    pageLookup(pagetable,t);
                    //get the PFN by pagetable map
                    uint32_t PFN = pageLookup(pagetable,t)->frame << pagetable->ShiftAry[pagetable->LevelCount-1];
                    //get the physical address
                    PFN = PFN + offset;
                    report_virtual2physical(t, PFN);
                }else if( mode == "v2p_tlb_pt"){


                    //get the frame by tlb
                    uint32_t fr = testTlb->getTLB(t, pagetable) << pagetable->ShiftAry[pagetable->LevelCount-1];
                    uint32_t PA = fr + offset;

                    report_v2pUsingTLB_PTwalk(t,PA, tlbC->isTlbHit,tlbC->isPageHit);
                    //printf("test the cache hit:  %d\n", tlbC->cacheHit);
                    tlbC->isTlbHit = false;
                    tlbC->isPageHit = false;
                }else if (numTlb != 0){
                    //for count cache hit
                    testTlb->getTLB(t, pagetable);
                }else{

                    pageLookup(pagetable,t);
                }


            }

            //break out the loop if address reach out the -n
            if(addrProcessed == num && num != NUMOFADDR){
                break;
            }

        }
    }

    // summary or default print out
    if(mode == "summary" || mode == ""){
        uint32_t byteUsed = 0;
        //count the all non-null entries
        countNonNullEntries(pagetable,0,pagetable->RootNodePtr);
        //traverse the level
        for(int i = 0; i < pagetable->LevelCount; i++){
            //add the total byte used of level0 and non-null entries * the next level byte used
            if(i == 0){
                byteUsed = pagetable->byteTotal[i];
            } else{
                byteUsed += pagetable->numOfNullENtries[i-1] * pagetable->byteTotal[i];
            }
        }


        report_summary(page_size,tlbC->cacheHit,pagetable->hitCount,addrProcessed,pagetable->frameCount,byteUsed);
    }
    delete(tlbC);
    delete(testTlb);
    delete(pagetable);
    delete(levelT);

    fclose(ifp);
    return (0);
}
