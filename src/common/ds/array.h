/// \file array_t.h
/// \internal
/// \brief similar to std::vector
/// \details Usually the Queue class is used instead,
/// since it has all the same functionality and is only worse at random access.
/// 顺序线性表
///
#ifndef __COMMON_DS_ARRARY_LIST_H
#define __COMMON_DS_ARRARY_LIST_H

#include "../geco-engine-export.h"
#include "../ultils/geco-malloc.h"
namespace geco
{
    namespace ds
    {
        // Maximum unsigned long
        const unsigned int MAX_UNSIGNED_LONG = 4294967295U;
        const unsigned int MAX_COUNT_ELEMENTS_CLEAR = 521;

        /**
         * @brief  slow to insert and remove middle element,
         * but fast to random get the element by index,
         * replace and remove unordered element
         * @note ONLY USE THIS FOR SHALLOW COPIES.
         *  I don't bother with operator= to improve performance.
         */
        template<class ElementType, unsigned int QUEUE_INIT_SIZE = 32>
        class GECO_EXPORT array_t
        {
            private:
                /// An array of user values
                ElementType* listArray;
                /// Number of elements in the list
                unsigned int list_size;
                /// Size of @a array
                unsigned int allocation_size;

            public:
                array_t() :
                        allocation_size(0), listArray(0), list_size(0)
                {
                }
                ~array_t()
                {
                    if (allocation_size > 0)
                    {
                        geco_delete_array<ElementType>(listArray,
                                FILE_AND_LINE);
                        list_size = allocation_size = 0;
                    }
                }
                array_t(const array_t& original_copy)
                {
                    /// allocate memory for copying the elements from @param original_copy
                    if (original_copy.list_size == 0)
                    {
                        list_size = allocation_size = 0;
                        return;
                    }
                    listArray = geco_new_array<ElementType>(
                            original_copy.list_size, FILE_AND_LINE);

                    for (int Index = 0; Index < original_copy.list_size;
                            Index++)
                    {
                        listArray[Index] = original_copy.listArray[Index];
                    }

                    list_size = allocation_size = original_copy.list_size;
                }
                array_t& operator=(const array_t& original_copy)
                {
                    if (this == &original_copy)
                        return *this;

                    /// allocate memory for copying the elements from @param original_copy
                    if (original_copy.list_size == 0)
                    {
                        Clear();
                        return *this;
                    }

                    if (original_copy.list_size > allocation_size)
                    {
                        Clear();
                        listArray = geco_new_array<ElementType>(
                                original_copy.list_size, FILE_AND_LINE);
                        allocation_size = original_copy.list_size;
                    }

                    for (int Index = 0; Index < original_copy.list_size;
                            Index++)
                    {
                        listArray[Index] = original_copy.listArray[Index];
                    }

                    list_size = original_copy.list_size;
                    return *this;
                }
                inline ElementType& operator[](
                        const unsigned int position) const
                {
                    assert(position < list_size);
                    return listArray[position];
                }
                void Clear(bool doNotDeallocateSmallBlocks = false)
                {
                    if (allocation_size == 0)
                        return;

                    if (allocation_size > MAX_COUNT_ELEMENTS_CLEAR
                            || doNotDeallocateSmallBlocks == false)
                    {
                        geco_delete_array(listArray, FILE_AND_LINE);
                        allocation_size = 0;
                        listArray = 0;
                    }
                    list_size = 0;
                }

                /** @brief similar to std::vector's reverse() to preallocate elements. */
                void reserve(unsigned int countNeeded)
                {
                    unsigned int amount2Alloc = allocation_size;
                    if (allocation_size == 0)
                        amount2Alloc = 16;
                    while (amount2Alloc < countNeeded)
                    {
                        amount2Alloc <<= 1;
                    }
                    if (allocation_size < amount2Alloc)
                    {
                        // allocate twice the currently allocated memory
                        ElementType* new_array;
                        allocation_size = amount2Alloc;
                        new_array = geco_new_array<ElementType>(allocation_size,
                                FILE_AND_LINE);
                        if (listArray != 0)
                        {
                            // copy old array over
                            for (unsigned int counter = 0; counter < list_size;
                                    ++counter)
                                new_array[counter] = listArray[counter];
                            geco_delete_array<ElementType>(listArray,
                                    FILE_AND_LINE);
                        }
                        listArray = new_array;
                    }
                }
                void Shrink2MiniSzie(void)
                {
                    if (allocation_size == 0)
                        return;
                    unsigned int mini_size = 1;
                    while (mini_size < list_size)
                        mini_size <<= 1;

                    ElementType * new_array = geco_new_array<ElementType>(
                            mini_size, FILE_AND_LINE);
                    // copy old array over
                    for (unsigned int counter = 0; counter < list_size;
                            ++counter)
                        new_array[counter] = listArray[counter];
                    // set old array to point to the newly allocated array
                    geco_delete_array<ElementType>(listArray, FILE_AND_LINE);
                    listArray = new_array;
                    allocation_size = mini_size;
                }

                /// @brief Pop an element from the end of the stack,
                /// combination of operations of [size()-1] and remove_back()
                /// @pre Size()>0
                /// @return The element at the end.
                inline ElementType& pop_back(void)
                {
#ifdef _DEBUG
                    assert(list_size > 0);
#endif
                    list_size--;
                    return listArray[list_size];
                }

                /// @brief Insert at the end of the list.
                /// @param[in] input The new element.
                void push_back(const ElementType &input)
                {
                    // Reallocate list if necessary
                    if (list_size == allocation_size)
                    {
                        // allocate twice the currently allocated memory
                        if (allocation_size == 0)
                            allocation_size = QUEUE_INIT_SIZE;
                        else
                            allocation_size <<= 1;

                        ElementType * new_array = geco_new_array<ElementType>(
                                allocation_size, FILE_AND_LINE);

                        if (listArray != 0)
                        {
                            // copy old array over
                            for (unsigned int counter = 0; counter < list_size;
                                    ++counter)
                                new_array[counter] = listArray[counter];
                            // set old array to point to the newly allocated and twice as large array
                            geco_delete_array<ElementType>(listArray,
                                    FILE_AND_LINE);
                        }
                        listArray = new_array;
                    }

                    // Insert the new item at the correct spot
                    listArray[list_size] = input;
                    ++list_size;
                }

                /// @brief Insert an element at position @a position in the list.
                /// @param[in] input The new element.
                /// @param[in] position The position of the new element.
                /// @Remarks Move the elements in the list to make room and so is slow operation
                void push(const ElementType &input, const unsigned int position)
                {
#ifdef _DEBUG
                    if (position > list_size)
                    {
                        assert(position <= list_size);
                    }
#endif
                    // Reallocate list if necessary
                    if (list_size == allocation_size)
                    {
                        // allocate twice the currently allocated memory
                        if (allocation_size == 0)
                            allocation_size = QUEUE_INIT_SIZE;
                        else
                            allocation_size <<= 1;

                        ElementType * new_array = geco_new_array<ElementType>(
                                allocation_size, FILE_AND_LINE);

                        if (listArray != 0)
                        {
                            // copy old array over
                            for (unsigned int counter = 0; counter < list_size;
                                    ++counter)
                                new_array[counter] = listArray[counter];
                            // set old array to point to the newly allocated and twice as large array
                            geco_delete_array<ElementType>(listArray,
                                    FILE_AND_LINE);
                        }
                        listArray = new_array;
                    }

                    // Move the elements in the list to make room
                    for (unsigned int counter = list_size; counter != position;
                            counter--)
                        listArray[counter] = listArray[counter - 1];

                    // Insert the new item at the correct spot
                    listArray[position] = input;
                    ++list_size;
                }

                /// @brief Replace the value at @a position by @a input.
                /// @details If the size of the list is less than @em position, it increase the capacity of
                /// the list and fill slot with @em filler.
                /// @param[in] input The element to replace at position @em position.
                /// @param[in] filler The element use to fill new allocated capacity.
                /// @param[in] position The position of input in the list.
                void replace(const ElementType &input, const ElementType filler,
                        const unsigned int position)
                {
                    if (list_size > 0 && position < list_size)
                    {
                        // Direct replacement
                        listArray[position] = input;
                    }
                    else
                    {
                        if (position >= allocation_size)
                        {
                            // Reallocate the list to size position and fill in blanks with filler
                            allocation_size = position + 1;

                            ElementType * new_array =
                                    geco_new_array<ElementType>(allocation_size,
                                            FILE_AND_LINE);

                            // copy old array over
                            for (unsigned int counter = 0; counter < list_size;
                                    ++counter)
                                new_array[counter] = listArray[counter];

                            // set old array to point to the newly allocated array
                            geco_delete_array<ElementType>(listArray,
                                    FILE_AND_LINE);
                            listArray = new_array;
                        }

                        // Fill in holes with filler
                        while (list_size < position)
                            listArray[list_size++] = filler;

                        // Fill in the last element with the new item
                        listArray[list_size++] = input;

#ifdef _DEBUG
                        assert(list_size == position + 1);
#endif
                    }
                }

                /// @brief Replace the last element of the list by @a input.
                /// @param[in] input The element used to replace the last element.
                inline void replace_back(const ElementType &input)
                {
                    if (list_size > 0)
                        listArray[list_size - 1] = input;
                }

                /// @brief Delete the element at position @a position.
                /// will move all elements forward to keep all the elements still in order
                /// @param[in] position The index of the element to delete
                /// @eg. [1,2,3,5]->remove_at_idx_of(2)->[1,2,5]
                void remove_slow(const unsigned int position)
                {
#ifdef _DEBUG
                    if (position >= list_size)
                    {
                        assert(position < list_size);
                        return;
                    }
#endif
                    if (position < list_size)
                    {
                        // move all elements forward to keep it in order
                        for (unsigned int counter = position;
                                counter < list_size - 1; ++counter)
                            listArray[counter] = listArray[counter + 1];

                        // Delete the last elements on the list.  No compression needed
#ifdef _DEBUG
                        assert(list_size >= 1);
#endif
                        list_size--;

                    }
                }

                /// @brief Delete the element at position @a position.
                /// by swaping it with last element only used when order does not matter
                /// @param[in] position The index of the element to delete
                /// @eg. [1,2,3,5]->remove_at_idx_of(0)->[5,2,3]
                void remove_fast(const unsigned int position)
                {
#ifdef _DEBUG
                    if (position >= list_size)
                    {
                        assert(position < list_size);
                        return;
                    }
#endif
                    --list_size;
                    if (list_size > position)
                        listArray[position] = listArray[list_size];
                }

                /// @brief Delete the element at the end of the list.
                inline void remove_back(const unsigned num = 1)
                {
                    // Delete the last elements on the list.  No compression needed
#ifdef _DEBUG
                    assert(list_size >= num);
#endif
                    list_size -= num;
                }

                /// @brief Returns the index of the specified item or MAX_UNSIGNED_LONG if not found.
                /// @param[in] input The element to check for
                /// @return The index or position of @em input in the list.
                /// @retval MAX_UNSIGNED_LONG The object is not in the list
                /// @retval [Integer] The index of the element in the list
                unsigned int getindexof(const ElementType &input) const
                {
                    for (unsigned int i = 0; i < list_size; ++i)
                    {
                        if (listArray[i] == input)
                            return i;
                    }
                    return MAX_UNSIGNED_LONG;
                }

                /// @return The number of elements in the list.
                inline unsigned int size(void) const
                {
                    return list_size;
                }
                unsigned int capacity() const
                {
                    return allocation_size;
                }
        };
    }
}
#endif // !__ARRARY_LIST_H__
