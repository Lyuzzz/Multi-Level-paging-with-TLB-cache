/******************************************************************************************
 * Assignment 3
 * Zhiyu Li		    #821009794			cssc1433
 * 03/25/22
 ******************************************************************************************/
#include "pageTable.h"

// helper function to split the virtual address in certain level
uint32_t virtualAddressToPageNum (uint32_t virtualAddress, unsigned int mask, unsigned int shift) {

    uint32_t Page;

    Page = mask & virtualAddress;

    Page = Page >> shift;

    return Page;



}

//shirft the mask as the @param request
unsigned int getMask(unsigned int bit) {
    return (1 << bit) - 1 ;
}

/*************************************************************************
 * insert virtual address into page table
 * @param levelIn pointer point to Level struct
 * @param virtualAddress is the virtual address used to split in multi-level
 * @param frame the pfn for each virtualAddress
 *************************************************************************/
void pageInsertLevels(Level *levelIn, uint32_t virtualAddress, unsigned int frame){

    //track the depth of each level
    int levelCout = levelIn->Depth;
    pageTable *pageLe = levelIn->PageTablePtr;

    //split it into multi-level
    uint32_t pagN = virtualAddressToPageNum(virtualAddress,pageLe->BitmaskAry[levelCout],
                                            pageLe->ShiftAry[levelCout] );


    //allocate for next Level pointer
    if(levelIn->NextLevelPtr == NULL){
        levelIn->NextLevelPtr = (Level **)calloc(pageLe->EntryCount[levelIn->Depth], sizeof(Level));

    }

    if(levelIn->NextLevelPtr[pagN] == NULL && levelCout < pageLe->LevelCount){
        //allocate for next Level pointer's entry
        levelIn->NextLevelPtr[pagN] = (Level *)calloc(pageLe->EntryCount[levelIn->Depth], sizeof(Level));
        //track the depth
        levelIn->NextLevelPtr[pagN]->Depth = levelIn->Depth + 1;
        levelIn->NextLevelPtr[pagN]->PageTablePtr = pageLe;
        //determine if it the leaf node or not
        // if it is not the leaf node get to the next level
        if( levelIn->NextLevelPtr[pagN]->Depth == pageLe->LevelCount ){
            levelIn = levelIn->NextLevelPtr[pagN];
        }else{
            pageInsertLevels(levelIn->NextLevelPtr[pagN], virtualAddress,frame);
        }


        // if the current depth is less than the total level number
    }else if(levelCout < pageLe->LevelCount){
        //track the depth
        levelIn->NextLevelPtr[pagN]->Depth = levelIn->Depth + 1;
        //if the current number is equal to the level number after track the depth
        //if it is not, then get to the next level
        if( levelIn->NextLevelPtr[pagN]->Depth == pageLe->LevelCount ){
            levelIn = levelIn->NextLevelPtr[pagN];
        }else{
            pageInsertLevels(levelIn->NextLevelPtr[pagN], virtualAddress,frame);
        }
    }

    // check if current depth is equal to level number
    // which means it is leaf node
    if(levelIn->Depth == pageLe->LevelCount){
        //if the map of leaf node is null
        if(levelIn->map == NULL){

            // allocate a new map
            levelIn->map = new Map();


            //insert the frame and mak is true
            levelIn->map->frame = frame;
            levelIn->map->isPage = true;

        }




    }









}

/*************************************************************************
 * insert the virtual address into page table
 * @param pagetable pointer point to pageTable struct
 * @param virtualAddress is the virtual address used to split in multi-level
 * @param frame the pfn for each virtualAddress
 *************************************************************************/
void pageInsert(pageTable *pagetable, uint32_t virtualAddress, unsigned int frame){
    pageInsertLevels(pagetable->RootNodePtr, virtualAddress,frame);
}

/*************************************************************************
 * to count how many non null entries in each level
 * @param pagetable pointer point to pageTable struct
 * @param pos is the position to track the entries
 * @param levelIn the pointer point to Level struct
 *************************************************************************/
void countNonNullEntries(pageTable *pagetable, int pos, Level *levelIn){

    //get the size of each entry
    pagetable->byteTotal[pos] = sizeof(Level) * pagetable->EntryCount[pos];
    if(pos < pagetable->LevelCount){
        for(int i = 0; i < pagetable->EntryCount[pos]; i++){
            if(levelIn->NextLevelPtr[i] != NULL){
                pagetable->numOfNullENtries[pos] += 1;
            }
        }
        countNonNullEntries(pagetable,pos+1,levelIn->NextLevelPtr[pos]);
    }

}


/*************************************************************************
 * walk the page table see if there is a page exist; if not ,add it to the page table
 * @param pagetable pointer point to pageTable struct
 * @param virtualAddress is the virtual address used to split in multi-level
 * @return the page table map, or NULL if there this not the page in page table yet
 *************************************************************************/

Map *pageLookup(pageTable *pagetable, uint32_t virtualAddress) {

    // point to the level struct
    Level *node = pagetable->RootNodePtr;
    int levels = pagetable->LevelCount;
    unsigned int page;
    //traverse each level to determine it is a page in the table or not
    for(int i = 0; i <= levels; i++){
        //split the virtualAddress in to multi-level
        page = virtualAddressToPageNum(virtualAddress,pagetable->BitmaskAry[i],
                                       pagetable->ShiftAry[i] );

        pagetable->pageNum[i] = page;
        //if the page of this level found in the page table, go to next level
        if( node != NULL && node->map == NULL ){
            node = node->NextLevelPtr[page];

            //if it found the map which mean it is the leaf node
        }else if(node != NULL && node->map->isPage){
            pagetable->hitCount++;

            return node->map;
        }else{

            // it means not found the page in page table, then need to insert it into the page table
            pagetable->pageNum[levels - 1] = virtualAddressToPageNum(virtualAddress,pagetable->BitmaskAry[levels - 1],
                                                                     pagetable->ShiftAry[levels - 1] );
            pageInsert(pagetable,virtualAddress, pagetable->frameCount);
            pagetable->frameCount++;
            pagetable->missCount++;
            return NULL;
        }


    }


    return NULL;


}
