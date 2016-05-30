#ifndef MEMORY_POOL_H_
#define MEMORY_POOL_H_

#include <cassert>
#ifndef __APPLE__
// Use stdlib and not malloc for compatibility
#include <stdlib.h>
#endif
#include "geco-export.h"
#include "geco-malloc-interface.h"
#include "geco-basic-type.h"

///====================================================
/// Very fast memory pool for allocating and deallocating structures that don't have 
/// constructors or destructors (in other words, there no pointer type of 
/// variables that nneed to be deleted). 
///=======================================================
GECO_NET_BEGIN_NSPACE
template <typename Type,
uint CELLS_SIZE_PER_PAGE = 256,
/* if all cells in an usable page are reclaimed,
 * MAX_FREE_PAGES determins whether to free this page or not */
 uint MAX_FREE_PAGES = 4>
class GECO_EXPORT JackieMemoryPool
{
    public:
    struct Cell;
    struct Page
    {
        Cell** freeCells; /// array that stores all the @Cell pointers
        int freeCellsSize; /// the size of un-allocated cells
        Cell* cell; /// array that stores @Cell itself
        Page *next;
        Page *prev;
    };
    struct Cell
    {
        Type blockType;
        Page *parentPage;
    };

    private:
    /// @Brief Contains pages which have free cell to allocate
    Page *usablePage;
    /// @Brief contains pages which have no free cell to allocate
    Page *unUsablePage;
    uint mUsablePagesSize;
    uint mUnUsablePagesSize;
    uint mPerPageSizeByte;
    uint mCellSizePerPage;


    ///========================================
    /// @Function InitPage 
    /// @Brief
    /// @Access  private  
    /// @Param [in] [Page * page]  
    /// @Param [in] [Page * prev]  
    /// @Returns [bool]
    /// @Remarks
    /// @Notice
    /// @Author mengdi[Jackie]
    ///========================================
    bool InitPage(Page *page, Page *prev)
    {
        uint i = 0;

        ///  allocate @Cell Array
        if ((page->cell = (Cell*)gMallocEx(mPerPageSizeByte, TRACKE_MALLOC))
            == 0) return false;

        /// allocate @Cell Pointers Array
        if ((page->freeCells = (Cell**)gMallocEx(sizeof(Cell*)*mCellSizePerPage, TRACKE_MALLOC)) == 0)
        {
            gFreeEx(page->cell, TRACKE_MALLOC);
            return false;
        }

        /// @freeCells stores all the pointers to the @Cell
        Cell *currCell = page->cell;
        Cell **currFreeCells = page->freeCells;
        while (i < mCellSizePerPage)
        {
            currCell->parentPage = page;
            currFreeCells[i] = currCell++;
            i++;
        }
        page->freeCellsSize = mCellSizePerPage;
        page->next = usablePage;
        page->prev = prev;
        return true;
    }

    public:
    JackieMemoryPool()
    {
#if _DISABLE_MEMORY_POOL == 0
        mUsablePagesSize = mUnUsablePagesSize = 0;
        mPerPageSizeByte = CELLS_SIZE_PER_PAGE* sizeof(Cell);
        mCellSizePerPage = CELLS_SIZE_PER_PAGE;
        usablePage = unUsablePage = 0;
#endif
    }
    ~JackieMemoryPool() { Clear(); }

    Type* Allocate(void)
    {
#if _DISABLE_MEMORY_POOL != 0
        return(Type*) gMallocEx(sizeof(Type), TRACKE_MALLOC);
#endif

        if (mUsablePagesSize > 0)
        {
            Page *currentPage = usablePage;
            Type *retValue = (Type*)currentPage->freeCells[--(currentPage->freeCellsSize)];
            if (currentPage->freeCellsSize == 0)
            {
                --mUsablePagesSize;
                usablePage = currentPage->next;
                assert(mUsablePagesSize == 0 || usablePage->freeCellsSize > 0);
                currentPage->next->prev = currentPage->prev;
                currentPage->prev->next = currentPage->next;
                if (mUnUsablePagesSize++ == 0)
                {
                    unUsablePage = currentPage;
                    currentPage->next = currentPage;
                    currentPage->prev = currentPage;
                }
                else
                {
                    currentPage->next = unUsablePage;
                    currentPage->prev = unUsablePage->prev;
                    unUsablePage->prev->next = currentPage;
                    unUsablePage->prev = currentPage;
                }
            }
            assert(mUsablePagesSize == 0 || usablePage->freeCellsSize > 0);
            return retValue;
        }

        if ((usablePage = (Page *)gMallocEx(sizeof(Page), TRACKE_MALLOC)) == 0) return 0;
        mUsablePagesSize = 1;
        if (!InitPage(usablePage, usablePage)) return 0;
        assert(usablePage->freeCellsSize > 1);
        return (Type *)usablePage->freeCells[--usablePage->freeCellsSize];
    }
    void Reclaim(Type *m)
    {
#if _DISABLE_MEMORY_POOL != 0
        gFreeEx(m, TRACKE_MALLOC);
        return;
#endif
        /// find the page where @m is in and return it
        Cell *currCell = (Cell*)m;
        Page *currPage = currCell->parentPage;

        /// this is an unavaiable page
        if (currPage->freeCellsSize == 0)
        {
            /// firstly reclaim @currCell to currentPage
            currPage->freeCells[currPage->freeCellsSize++] = currCell;

            // then remove @currentPage from unavailable page list
            currPage->next->prev = currPage->prev;
            currPage->prev->next = currPage->next;
            mUnUsablePagesSize--;

            /// then update the @unavailablePage  
            if (mUnUsablePagesSize > 0 && currPage == unUsablePage)
            {
                unUsablePage = unUsablePage->next;
            }

            /// then insert currentPage to the head of available page list
            if (mUsablePagesSize++ == 0)
            {
                usablePage = currPage;
                currPage->next = currPage;
                currPage->prev = currPage;
            }
            else
            {
                currPage->next = usablePage;
                currPage->prev = usablePage->prev;
                usablePage->prev->next = currPage;
                usablePage->prev = currPage;
            }

        }
        else 	/// this is an avaiable page
        {
            // first reclaim @m to currentPage
            currPage->freeCells[currPage->freeCellsSize++] = currCell;

            // all cells in @currentPage are reclaimed and becomes empty
            if (currPage->freeCellsSize == mCellSizePerPage &&
                mUsablePagesSize > MAX_FREE_PAGES)
            {
                /// After a certain point, just deallocate empty pages
                /// rather than keep them around
                if (currPage == usablePage)
                {
                    usablePage = currPage->next;
                    assert(usablePage->freeCellsSize > 0);
                }
                currPage->prev->next = currPage->next;
                currPage->next->prev = currPage->prev;
                mUsablePagesSize--;
                gFreeEx(currPage->freeCells, TRACKE_MALLOC);
                gFreeEx(currPage->cell, TRACKE_MALLOC);
                gFreeEx(currPage, TRACKE_MALLOC);
            }
        }
    }
    void Clear(void)
    {
#if _DISABLE_MEMORY_POOL != 0
        return;
#endif
        Page *currentPage, *freedPage;

        if (mUsablePagesSize > 0)
        {
            currentPage = usablePage;
            while (true)
            {
                gFreeEx(currentPage->freeCells, TRACKE_MALLOC);
                gFreeEx(currentPage->cell, TRACKE_MALLOC);
                freedPage = currentPage;
                currentPage = currentPage->next;
                if (currentPage == usablePage)
                {
                    gFreeEx(freedPage, TRACKE_MALLOC);
                    mUsablePagesSize = 0;
                    break;
                }
                gFreeEx(freedPage, TRACKE_MALLOC);
            }
        }

        if (mUnUsablePagesSize > 0)
        {
            currentPage = unUsablePage;
            while (true)
            {
                gFreeEx(currentPage->freeCells, TRACKE_MALLOC);
                gFreeEx(currentPage->cell, TRACKE_MALLOC);
                freedPage = currentPage;
                currentPage = currentPage->next;
                if (currentPage == unUsablePage)
                {
                    gFreeEx(freedPage, TRACKE_MALLOC);
                    mUnUsablePagesSize = 0;
                    break;
                }
                gFreeEx(freedPage, TRACKE_MALLOC);
            }
        }
    }
};
GECO_NET_END_NSPACE
#endif
/*
#include "DS_MemoryPool.h"
#include "DS_List.h"

struct TestMemoryPool
{
int allocationId;
};

int main(void)
{
DataStructures::MemoryPool<TestMemoryPool> memoryPool;
DataStructures::List<TestMemoryPool*> returnList;

for (int i=0; i < 100000; i++)
returnList.Push(memoryPool.Allocate(_FILE_AND_LINE_), _FILE_AND_LINE_);
for (int i=0; i < returnList.Size(); i+=2)
{
memoryPool.Release(returnList[i], _FILE_AND_LINE_);
returnList.RemoveAtIndexFast(i);
}
for (int i=0; i < 100000; i++)
returnList.Push(memoryPool.Allocate(_FILE_AND_LINE_), _FILE_AND_LINE_);
while (returnList.Size())
{
memoryPool.Release(returnList[returnList.Size()-1], _FILE_AND_LINE_);
returnList.RemoveAtIndex(returnList.Size()-1);
}
for (int i=0; i < 100000; i++)
returnList.Push(memoryPool.Allocate(_FILE_AND_LINE_), _FILE_AND_LINE_);
while (returnList.Size())
{
memoryPool.Release(returnList[returnList.Size()-1], _FILE_AND_LINE_);
returnList.RemoveAtIndex(returnList.Size()-1);
}
for (int i=0; i < 100000; i++)
returnList.Push(memoryPool.Allocate(_FILE_AND_LINE_), _FILE_AND_LINE_);
for (int i=100000-1; i <= 0; i-=2)
{
memoryPool.Release(returnList[i], _FILE_AND_LINE_);
returnList.RemoveAtIndexFast(i);
}
for (int i=0; i < 100000; i++)
returnList.Push(memoryPool.Allocate(_FILE_AND_LINE_), _FILE_AND_LINE_);
while (returnList.Size())
{
memoryPool.Release(returnList[returnList.Size()-1], _FILE_AND_LINE_);
returnList.RemoveAtIndex(returnList.Size()-1);
}

return 0;
}
*/