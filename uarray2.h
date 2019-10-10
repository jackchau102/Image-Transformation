/*
 *    uarray2.h
 *    by Nghi Chau (nchau01), Jacqueline Chin (jlin04), 27 Sep. 2019
 *    HW2 - file
 *
 *    The interface for the 2D array
 */

#ifndef UARRAY2_H_INCLUDED
#define UARRAY2_H_INCLUDED

#define U UArray2_T

#include <uarray.h>
#include <stdio.h>

typedef struct U *U;

/*
 * Purpose: UArray2_new allocates, initializes, and returns a new array of 
 *          length = row(height)* column(width). With bounds zero to length-1.
 *          Each element occupies size bytes. The bytes in each element are 
 *          initialized to zero.
 *
 *Parameters: It takes in the int number of row and columns that they want the 
 *            new array to have. And it also takes in the int size which is
 *            how many bytes they want each element in the array to have.
 *
 * Returns: Returns a UArray2_T
 *
 * Errors: If the row * columns is negative it is check runtime error for it
 *         to be negative or for size to be non positive, and UArray2 can raise
 *         Mem_Failed.
 */
extern U UArray2_new(int col, int row,  int size);

/*
 * Purpose: This function will deallocate and clear the *u2_array
 *
 *Parameters: Takes in  a UArray2_T type pointer to the array
 *
 *Returns: nothing 
 *
 * Errors/Note: It is checked runtime error for u2_array or *u2array to be 
 *              null.
 */

extern void UArray2_free(U *u2_array);

/*
 * Purpose: This function will give the width of the 2-D Uarray. Which should
 *          also return the number of columns in the 2-D Uarray.
 *
 * Parameters: takes in a UArray2_T uarray
 *
 * Returns: returns an integer that represents the width of the table.
 *
 * Errors/Note: Will return 0 if the Uarray is empty.
 */

extern int UArray2_width(U u2_array);

/*
 * Purpose: This function will give the height of the 2-D Uarray
 *
 *Parameters: a UArray2_T uarray
 *
 *Returns:returns an integer that represents the height of the table
 *
 * Errors/Note: Will return 0 is the Uarray is empty.
 */

extern int UArray2_height(U u2_array);

/*
 * Purpose: This function will return the size in bytes of the elements in the 
 * array
 *
 * Parameters: in a UArray2_T uarray
 *
 * Returns: returns an integer that represents the size of the element in bytes 
 *          that space was originally allocated for 
 *
 * Errors/Note: Even if the Uarray is empty it will return the size in bytes
 *              that was originally allocated when UArray2_new was called.
 */

extern int UArray2_size(U u2_array);

/*
 * Purpose:this function will return a pointer to the element in the 
 *              corresponding (column, row) position.
 *
 * Parameters:in a UArray2_T Uarray, and integer values that represent the 
 *               row and the colum of the 2-D Uarray.
 *
 * Returns: a void*
 *
 * Errors: It is checked runtime error for (row * column) to be greater than
 *         or equal to the length of our Uarray, which is row * column. 
 */

void *UArray2_at(U u2_array, int col, int row);

/*
 * Purpose: This function will call the function pointed to by apply for
 *          every element in the 2-D array. Clients can pass an 
 *          application specific pointer, cl, to this function and this 
 *          pointer is passed along to apply in each call.
 *
 *Parameters: It takes in a UArray2_T, a function, and a void* that will act 
 *            as closure. The *cl will be passed into the apply function as 
 *            the p2 pointer.
 *
 * Returns: nothing
 *
 * Errors/Note: This function will call the function for each element in
 *              the array, but the way that it differs from the 
 *              UArray2_map_column_major is the order in which it goes through 
 *              the 2-D array. For this function the row varies more 
 *              rapidly. So it looks as is we are traversing the columns
 *              vertically from top to bottom before moving on to the 
 *              next column.
 */

extern void UArray2_map_col_major(U u2_array, 
    void apply(int col, int row, U u2_array, void *p1,void *p2), 
    void *cl);

/*
 * Purpose: This function will call the function pointed to by apply for
 *          every element in the 2-D array. Clients can pass an 
 *          application specific pointer, cl, to this function and this 
 *          pointer is passed along to apply in each call.
 *
 *Parameters: It takes in a UArray2_T, a function, and a void* that will act 
 *            as closure. The *cl will be passed into the apply function as 
 *            the p2 pointer.
 *
 * Returns: nothing
 *
 * Errors/Note: This function will call the function for each element in
 *              the array, but the way that it differs from the 
 *              UArray2_map_row_major is the order in which it goes through 
 *              the 2-D array. For this function the column varies more 
 *              rapidly. So it looks as if we are traversing the rows 
 *              horizontally from left to right and then moving down
 *              to the next row.
 */

extern void UArray2_map_row_major(U u2_array, 
    void apply(int col, int row, U u2_array, void *p1, void *p2), 
    void *cl);

void UArray2_map_block_major(UArray2_T u2_array,
    void apply(int col, int row, UArray2_T u2_array, void *p1, void *p2),
    void *cl);

#undef U
#endif
