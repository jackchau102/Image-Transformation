/*
 *
 *
 *    The implementation for uarray2.h (a 2D array)
 */

#include <stdio.h>
#include <stdlib.h>
#include <uarray.h>
#include <uarray2.h>
#include <math.h>
#include <assert.h>

/*        UArray2_T struct 
 *        
 * Purpose: storing information about the array and the array itself
 *          width stores an integer identifying the number of columns
 *          height stores an integer identifying the number of rows
 */

struct UArray2_T {
        int width;
        int height;
        UArray_T array;
};


extern UArray2_T UArray2_new(int col, int row, int size)
{
        // Plus 8 because UArray_T is just a pointer to the actual array
        size_t struct_size = (size_t)(sizeof(int) * 2) + 8;
        UArray2_T struct_to_return = 
            (UArray2_T) malloc(struct_size);

        /* Check for bad malloc */
        assert(struct_to_return != NULL); 
        /* Check for bad input */
        assert(col >= 0 && row >= 0 && size > 0);

        struct_to_return -> width = col;
        struct_to_return -> height = row;
        struct_to_return -> array = UArray_new(col * row, size);

        return struct_to_return;    
}

extern int UArray2_width(UArray2_T u2_array)
{
        assert(u2_array != NULL);
        return u2_array -> width;
}

extern int UArray2_height(UArray2_T u2_array)
{
        assert(u2_array != NULL);
        return u2_array -> height;
}

extern int UArray2_size(UArray2_T u2_array)
{
        assert(u2_array != NULL);
        return UArray_size(u2_array -> array);
}

extern void UArray2_free(UArray2_T *u2_array)
{
        assert(u2_array != NULL);
        assert((*u2_array) -> width > 0 && (*u2_array) -> height > 0);
        UArray_T to_free = (*u2_array) -> array;
        UArray_free(&to_free);
        free(*u2_array);
}

void *UArray2_at(UArray2_T u2_array, int col, int row)
{
        assert(u2_array != NULL);
        assert(col <= u2_array -> width && row <= u2_array -> height);
        int index = UArray2_width(u2_array) * row + col;

        return UArray_at(u2_array -> array, index);
}

extern void UArray2_map_col_major(UArray2_T u2_array, 
    void apply(int col, int row, UArray2_T u2_array, void *p1, void *p2), 
    void *cl)
{
        assert(u2_array != NULL);
        int width = UArray2_width(u2_array);
        int height = UArray2_height(u2_array);

        for (int i = 0; i < width; i++){
            for (int j = 0; j < height; j++){
                apply(i, j, u2_array, 
                    UArray2_at(u2_array, i, j),
                    cl);
            }
        }
}

extern void UArray2_map_row_major(UArray2_T u2_array, 
    void apply(int col, int row, UArray2_T u2_array, void *p1, void *p2), 
    void *cl)
{
        assert(u2_array != NULL);
        int table_col;
        int table_row;
        int width = UArray2_width(u2_array);

        for (int i = 0; i < UArray_length(u2_array -> array); i++){
        
            table_col = (i % width);
            table_row = i/width;

            apply(table_col, table_row, u2_array, 
                UArray_at(u2_array -> array, i), 
                cl);
        }
}
