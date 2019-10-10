#include <stdio.h>
#include <stdlib.h>
#include "uarray2b.h"
#include "uarray2.h"
#include "uarray.h"

#include <math.h>
#include <assert.h>

struct UArray2b_T {
        int block_size;
        int width;
        int height;
        UArray2_T array;
};

// * UArray2b_new
// * Param: Takes in the width, height, size of each element, and blocksize 
// * for the UArray2b
// * Purpose: Creates and returns a new UArray2b which is a UArray2 where each
// * element is a UArray that represents a block and holds elements 
// * of the array
// * Returns: A Uarray2b_T object

extern UArray2b_T UArray2b_new (int width, int height, int size, int blocksize)
{
                assert(width > 0);
                assert(height > 0);
                assert(blocksize > 0);
                assert(size > 0);

                long size_a;
                int length = blocksize * blocksize;

                int width_u = width / blocksize;
                int height_u = height / blocksize;
                int width_r, height_r;
                
                if (width % blocksize != 0){
                        width_u++;
                        width_r = width + (width % blocksize);
                }
                if (height % blocksize != 0){
                        height_u++;
                        height_r = height + (height % blocksize);
                }

                size_a = size * width_r * height_r;

                UArray2b_T big_array = (UArray2b_T) malloc(12 + size_a);

                big_array -> block_size = blocksize;
                big_array -> width = width;
                big_array -> height = height;
                big_array -> array = UArray2_new(width_u, 
                                        height_u, length * size);

                for (int i = 0; i < width_u; i++){
                        for (int j = 0; j < height_u; j++){
                                /* Creating a new UArray */
                                UArray_T block = UArray_new(length, size);
                                /* Retrieve elem from UArray2 */
                                UArray_T *placeholder = 
                                        UArray2_at(big_array -> array, i,j);
                                *placeholder = block;
                        }
                }

                return big_array;
}

/*
* UArray2b_width
* Param: The UArray2b 
* Purpose: Return the width of the UArray2b 
* Return: The width of the UArray2b
*/
extern int UArray2b_width (UArray2b_T array2b)
{
        assert(array2b != NULL);
        return array2b -> width;
}

/*
* UArray2b_height
* Param: The UArray2b 
* Purpose: Return the height of the UArray2b 
* Return: The height of the UArray2b
*/
extern int UArray2b_height (UArray2b_T array2b)
{
        assert(array2b != NULL);
        return array2b -> height;
}

/*
* UArray2b_size
* Param: The UArray2b 
* Purpose: Return the size of the UArray2b 
* Return: The size of the UArray2b
*/
extern int UArray2b_size(UArray2b_T array2b)
{
        assert(array2b != NULL);
        int width = array2b -> width;
        int height = array2b -> height;
        return width * height;
}

/*
* UArray2b_blocksize
* Param: The UArray2b 
* Purpose: Return the blocksize of the UArray2b 
* Return: The blocksize of the UArray2b
*/
extern int UArray2b_blocksize(UArray2b_T array2b)
{
        assert(array2b != NULL);
        return array2b -> block_size;
}

/*
* UArray2b_free
* Param: The UArray2b 
* Purpose: free the allocated memory in the UArray2b 
* Return: Nothing
*/
extern void UArray2b_free (UArray2b_T *array2b)
{
        assert(array2b != NULL);
        UArray2b_T deref_array = *array2b;
        assert(deref_array != NULL);

        int width_u = (deref_array -> width) / (deref_array -> block_size);
        int height_u = (deref_array -> height) / (deref_array -> block_size);
                
        if ((deref_array -> width) % (deref_array -> block_size) != 0)
                width_u++;
        if ((deref_array -> height) % (deref_array -> block_size) != 0)
                height_u++;

        for (int i = 0; i < width_u; i++){
                for (int j = 0; j < height_u; j++){
                        UArray_T *array_at = UArray2_at(deref_array -> array, 
                                                        i, j);
                        UArray_free(array_at);
                }
        }

        UArray2_free(&(deref_array -> array));
        free(deref_array);
}

/*
* UArray2b_new_64K_block
* Param: The width and height of the UArray2b and the size of each element  
* Purpose: Similar to "new" this function creates new UArray2b however this 
* function defaults the blocksize so that it can fit the max number of elements
* less than 64kb 
* Return: The UArray2b
*/
extern UArray2b_T UArray2b_new_64K_block(int width, int height, int size)
{

        assert(size > 0);
        int total_bytes = 64 * 1024;
        int total_cells = total_bytes / size;
        int blocksize = (int) sqrt(total_cells);
        long size_a;
        int length = blocksize * blocksize;

        if (total_cells == 0){
                blocksize = 1;
                length = 1;
        }

        int width_u = width / blocksize;
        int height_u = height / blocksize;
        int width_r, height_r;
                
        if (width % blocksize != 0){
                width_u++;
                width_r = width + (width % blocksize);
        }
        if (height % blocksize != 0){
                height_u++;
                height_r = height + (height % blocksize);
        }

        size_a = size * width_r * height_r;

        assert(size_a > 0);

        UArray2b_T big_array = (UArray2b_T) malloc(12 + size_a);

        big_array -> width = width;
        big_array -> height = height;
        big_array -> block_size = blocksize;
        big_array -> array = UArray2_new(width_u, height_u, length * size);

        for (int i = 0; i < width_u; i++){
                for (int j = 0; j < height_u; j++){
                        UArray_T block = UArray_new(length, size);
                        UArray_T *placeholder = UArray2_at(big_array -> array, 
                                                        i,j);
                        *placeholder = block;
                }
        }

        return big_array;
}

/*
* UArray2b_at
* Param: The UArray2b, the column and row of the element 
* Purpose: Retrieve the element at position(column, row)
* Return: A void pointer to the object in the UArray2b
*/
extern void *UArray2b_at(UArray2b_T array2b, int column, int row)
{
        assert(array2b != NULL);
        assert(column <= UArray2b_width(array2b) - 1);
        assert(row <= UArray2b_height(array2b) - 1);
        int bl_size = array2b -> block_size;

        UArray_T *temp = 
        UArray2_at(array2b -> array, 
                column / bl_size, 
                row / bl_size);

        return UArray_at(*temp, 
                (column % bl_size) + bl_size * (row % bl_size));
}

/*
* UArray2b_map
* Param: The UArray2b, the apply function, a void pointer to the closure 
*        argument 
* Purpose: Map through each element of the array and call the apply function on
* each element
* Return: Nothing
*/
extern void  UArray2b_map(UArray2b_T array2b,
        void apply(int col, int row, UArray2b_T array2b, void *elem, void *cl),
        void *cl)
{
        assert(array2b != NULL);
        int height = UArray2b_height(array2b);
        int width = UArray2b_width(array2b);

        int blocksize = UArray2b_blocksize(array2b);

        for(int i = 0; i < height; i = i + blocksize){
                for(int j = 0; j < width; j = j + blocksize){
                        for(int k = 0; k < blocksize; k++){
                                for(int m = 0; m < blocksize; m++){
                                        if (j + m < width && i + k < height)
                                                apply(j + m, i + k, array2b, 
                                                        UArray2b_at(array2b, 
                                                                j + m, i + k), 
                                                                cl);
                                }
                        }
                }
        }
}