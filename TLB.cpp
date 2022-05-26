/******************************************************************************************
 * Assignment 3
 * Zhiyu Li		    #821009794			cssc1433
 * 03/25/22
 ******************************************************************************************/

#include "TLB.h"

//constructor
TLB::TLB(int size, int tlbmapsize, tlb *&table1, unsigned int start){

    //size of LRU map
    mostRecentUsed = size;

    // tlb pointer point to tlb struct
    table = table1;
    //access time
    table->acc = start;
    //tlb map size
    tlbSize = tlbmapsize;

    //head initialize
    head = new tlbCache(-1,-1);

    //tail initialize
    tail = new tlbCache(-1,-1);

    head->next = tail;

    tail->prev = head;

}

/*************************************************************************
 * Insert the vpn into TLB cache
 * @param vpn it is the virtual page number
 * @param page pointer point to pageTable
 *************************************************************************/
void TLB::insertToTlbmap(unsigned int vpn, pageTable *page){
    //integer to track the tlbMap traverse
    //initialize it with 0 as start
    int j = 0;
    //shrift virtual address to vpn only
    uint32_t tmpVPN = vpn >> page->ShiftAry[page->LevelCount-1];
    // boolean indicate the vpn is on most recent map or not
    bool isOutofMost10 = false;
    //get the key
    uint32_t tmpKey;
    //if the tlbmap is full
    if(tlbMap.size() == tlbSize ){
        table->isTlbHit = false;
        //tlbMap traverse
        for(auto i = tlbMap.begin(); i != tlbMap.end(); i++){
            if(searchMap(i->first) == -1){
                isOutofMost10 = true;
                tmpKey = i->first;
                break;
            }else{
                //add each accesstime into the array
                table->arr[j] = searchMap(i->first);
                j++;
            }

        }
        //if there is a vpn is not on most recent map
        //erase it from the map
        if(isOutofMost10){
            tlbMap.erase(tmpKey);
        } else{
            //sort the array to an ascending array
            int s = sizeof( table->arr)/sizeof( table->arr[0]);
            sort( table->arr,  table->arr + s);

            // get the least used accesstime
            unsigned int value =  table->arr[0];
            //mostRecentMap traverse
            for(auto k = mostRecentMap.begin(); k != mostRecentMap.end(); k++){

                //if the least used accesstime matched, delete it from mostRecentMap, linked list
                // and tlbMap
                if(k->second->accessTime == value){
                    auto deleteKey = mostRecentMap[k->first];
                    unsigned int keyOftlb = k->first;

                    tlbMap.erase(keyOftlb);
                    mostRecentMap.erase(deleteKey->vpn);
                    entryDelete(deleteKey);


                    delete(deleteKey);
                    break;
                }
            }
        }


        //check if the page is already exist in the table or not
        if((pageLookup(page, vpn)) != NULL){
            table->isPageHit = true;
            tlbMap[tmpVPN] = (pageLookup(page, vpn))->frame;
        } else{
            table->isPageHit = false;
            //pageLookup(page,vpn);
            tlbMap[tmpVPN] = (pageLookup(page, vpn))->frame;
        }
        //if the size is not full and cannot found the vpn inside the map
        //check if the page is already exist in the table or not
    }else if(tlbMap.count(tmpVPN) != 1 && tlbMap.size() != tlbSize){
        table->isTlbHit = false;
        if((pageLookup(page, vpn)) != NULL){

            table->isPageHit = true;
            tlbMap[tmpVPN] = (pageLookup(page, vpn))->frame;
        } else{

            table->isPageHit = false;
            //pageLookup(page,vpn);
            tlbMap[tmpVPN] = (pageLookup(page, vpn))->frame;
        }
    }

}
/*************************************************************************
 * Determine there is the tlb hit or not
 * @param vpn it is the virtual page number
 * @param page pointer point to pageTable
 *************************************************************************/
unsigned int TLB::getTLB(unsigned int vpn, pageTable *page){

    uint32_t tmpVPN = vpn >> page->ShiftAry[page->LevelCount-1];
    //if there is a match in the map, tlb hit
    if(tlbMap.count(tmpVPN)){
        table->cacheHit++;
        table->isTlbHit = true;
        recentUse(tmpVPN, table->acc);
        return tlbMap[tmpVPN];

    }else{
        recentUse(tmpVPN, table->acc);
        insertToTlbmap(vpn, page);
        return tlbMap[tmpVPN];
    }
}

/*************************************************************************
 * Keep update the mostRecentMap, delete the least used one if the size
 * reach out full
 * @param vpn it is the virtual page number
 * @param accessTime it is the access time from last visit
 *************************************************************************/
void TLB::recentUse(unsigned int vpn, unsigned int accessTime){

    // if the vpn is already exist in the map
    // add the accesstime
    if(mostRecentMap.count(vpn)){

        auto access = mostRecentMap[vpn];

        access->accessTime = accessTime+1;
        table->acc = access->accessTime ;
        entryDelete(access);

        recentAccessInsert(access);

    }else{
        //initial size is 0
        int inCasesize = 0;
        //determine if the LRU track the TLB size
        if(tlbMap.size() < mostRecentUsed && tlbMap.size() != 0 ){
            inCasesize = tlbMap.size();
        }else{
            inCasesize = mostRecentUsed;
        }
        //if it is full, delete the least access one
        if(mostRecentMap.size() == inCasesize){

            auto access = tail->prev;

            entryDelete(access);

            mostRecentMap.erase(access->vpn);

            delete(access);
        }
        //insert the vpn into the mostRecentMap
        auto tmp = new tlbCache(vpn, accessTime+1);
        table->acc = accessTime+1;
        mostRecentMap[vpn] = tmp;

        recentAccessInsert(tmp);
    }

}
/*************************************************************************
 * Search the vpn in mostRecentMap hash map
 * @param vpn it is the virtual page number
 * @return the accesstime of vpn
 *************************************************************************/
unsigned int TLB::searchMap(unsigned int vpn){

    if(mostRecentMap.count(vpn) == 0){
        return -1;

    }else{

        auto access = mostRecentMap[vpn];


        return access->accessTime;
        delete(access);
    }
}
/*************************************************************************
 * Insert the recently used node into the linked list based on mostRecentMap hash map
 * @param currNode it is the pointer belong to value of mostRecentMap hash map
 *************************************************************************/
void TLB::recentAccessInsert(tlbCache *currNode){

    //add the node in the front of the linked list
    currNode->next = head->next;

    head->next->prev = currNode;

    currNode->prev = head;

    head->next = currNode;
    head->next = currNode;
}
/*************************************************************************
 * delete the certain entry in linked list
 * @param currNode it is the pointer belong to value of mostRecentMap hash map
 *************************************************************************/
void TLB::entryDelete(tlbCache *currNode){

    //unlink both previous node and next
    currNode->next->prev = currNode->prev;

    currNode->prev->next = currNode->next;
}