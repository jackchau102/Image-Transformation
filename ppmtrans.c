/*
 * Name: Nghi Chau, Jake Owen
 * UTLN: nchau01, jowen03
 * 
 * Program to perform image transformation using client's data structure
 * 
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "assert.h"
#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "pnm.h"
#include "cputiming.h"

#define SET_METHODS(METHODS, MAP, WHAT) do {                    \
        methods = (METHODS);                                    \
        assert(methods != NULL);                                \
        map = methods->MAP;                                     \
        if (map == NULL) {                                      \
                fprintf(stderr, "%s does not support "          \
                                WHAT "mapping\n",               \
                                argv[0]);                       \
                exit(1);                                        \
        }                                                       \
} while (0)

static A2Methods_T methods;
const int RGB_STRUCT_SIZE = 12;

static void
usage(const char *progname)
{
        fprintf(stderr, "Usage: %s [-rotate <angle>] "
                        "[-{row,col,block}-major] [filename]\n",
                        progname);
        exit(1);
}

static void rotate_0(int i, int j, 
    A2Methods_UArray2 array, A2Methods_Object *elem, void *cl);

static void rotate_90(int i, int j, 
    A2Methods_UArray2 array, A2Methods_Object *elem, void *cl);

static void rotate_180(int i, int j, 
    A2Methods_UArray2 array, A2Methods_Object *elem, void *cl);

static void rotate_270(int i, int j, 
    A2Methods_UArray2 array, A2Methods_Object *elem, void *cl);

static void flip_horiz(int i, int j, 
    A2Methods_UArray2 array, A2Methods_Object *elem, void *cl);

static void flip_verti(int i, int j, 
    A2Methods_UArray2 array, A2Methods_Object *elem, void *cl);

static void transpose(int i, int j, 
    A2Methods_UArray2 array, A2Methods_Object *elem, void *cl);

int main(int argc, char *argv[]) 
{
        char *time_file_name = NULL;
        int   rotation       = 0;
        int   i;
        char *image          = NULL; /* Image is in same directory*/
        FILE *fptr;
        char *flip           = NULL;
        int transpose_bool   = 0;
        
        /* Time variables */
        FILE *fptr_time;
        CPUTime_T timer;
        double time_used;

        int counter = 1;

        int width, height;
        int size = RGB_STRUCT_SIZE;

        timer = CPUTime_New();

        /* Copy over array */
        A2Methods_UArray2 image_copy;

        /* Reader */
        Pnm_ppm reader;

        /* default to UArray2 methods */
        methods = uarray2_methods_plain; 
        assert(methods);

        /* default to best map */
        A2Methods_mapfun *map = methods->map_default; 
        assert(map);

        for (i = 1; i < argc; i++) {
                if (strcmp(argv[i], "-row-major") == 0) {
                        SET_METHODS(uarray2_methods_plain, map_row_major, 
                                    "row-major");
                        counter++;
                } else if (strcmp(argv[i], "-col-major") == 0) {
                        SET_METHODS(uarray2_methods_plain, map_col_major, 
                                    "column-major");
                        counter++;
                } else if (strcmp(argv[i], "-block-major") == 0) {
                        SET_METHODS(uarray2_methods_blocked, map_block_major,
                                    "block-major");
                        counter++;
                } else if (strcmp(argv[i], "-transpose") == 0){
                        transpose_bool = 1;
                        counter++;
                } else if (strcmp(argv[i], "-flip") == 0) {
                        if (!(i + 1 < argc)) {      /* no rotate value */
                                usage(argv[0]);
                        }
                        flip = argv[++i];
                        if (!(strcmp(flip, "horizontal") == 0 
                            || strcmp(flip, "vertical") == 0)) {
                                fprintf(stderr, 
                                    "Flip must be horizontal or vertical\n");
                                usage(argv[0]);
                        }
                        counter += 2;
                } else if (strcmp(argv[i], "-rotate") == 0) {
                        if (!(i + 1 < argc)) {      /* no rotate value */
                                usage(argv[0]);
                        }
                        char *endptr;
                        rotation = strtol(argv[++i], &endptr, 10);
                        if (!(rotation == 0 || rotation == 90 ||
                            rotation == 180 || rotation == 270)) {
                                fprintf(stderr, 
                                        "Rotation must be 0, 90 180 or 270\n");
                                usage(argv[0]);
                        }
                        if (!(*endptr == '\0')) {    /* Not a number */
                                usage(argv[0]);
                        }
                        counter += 2;
                } else if (strcmp(argv[i], "-time") == 0) {
                        time_file_name = argv[++i];
                        counter += 2;
                } else if (*argv[i] == '-') {
                        fprintf(stderr, "%s: unknown option '%s'\n", argv[0],
                                argv[i]);
                } else if (argc - i > 1) {
                        fprintf(stderr, "Too many arguments\n");
                        usage(argv[0]);
                } else {
                        break;
                }
        }

        /* Opening file */

            if(counter == argc){
                fptr = stdin;
            }
            else{
                image = argv[argc - 1];
                fptr = fopen(image, "rb+");

                if(fptr == NULL){
                    fprintf(stderr, 
                        "%s: does not have valid input. Exit with Failure\n",
                        argv[0]);
                    exit(EXIT_FAILURE); 
                }
            }

        reader = Pnm_ppmread(fptr, methods);

        width = reader -> width;
        height = reader -> height;
        size = RGB_STRUCT_SIZE;


        if (rotation == 90){
            image_copy = methods -> new(height, width, size);
            CPUTime_Start(timer);
            map(reader -> pixels, rotate_90, &image_copy);
            time_used = CPUTime_Stop(timer);
        }
        else if (rotation == 180){
            image_copy = methods -> new(width, height, size);
            CPUTime_Start(timer);
            map(reader -> pixels, rotate_180, &image_copy);
            time_used = CPUTime_Stop(timer);
        }
        else if(rotation == 0){
            image_copy = methods -> new(width, height, size);
            CPUTime_Start(timer);
            map(reader -> pixels, rotate_0, &image_copy);
            time_used = CPUTime_Stop(timer);
        }
        else if (rotation == 270){
            image_copy = methods -> new(height, width, size);
            CPUTime_Start(timer);
            map(reader -> pixels, rotate_270, &image_copy);
            time_used = CPUTime_Stop(timer);
        }

        if (flip != NULL && strcmp(flip, "horizontal") == 0){
            image_copy = methods -> new(width, height, size);
            CPUTime_Start(timer);
            map(reader -> pixels, flip_horiz, &image_copy);
            time_used = CPUTime_Stop(timer);
        }
        else if (flip != NULL && strcmp(flip, "vertical") == 0){
            image_copy = methods -> new(width, height, size);
            CPUTime_Start(timer);
            map(reader -> pixels, flip_verti, &image_copy);
            time_used = CPUTime_Stop(timer);
        }

        if (transpose_bool == 1){
            image_copy = methods -> new(height, width, size);
            CPUTime_Start(timer);
            map(reader -> pixels, transpose, &image_copy);
            time_used = CPUTime_Stop(timer);
        }

        int total_pixels = 
        (methods -> width(image_copy) * methods -> height(image_copy));

        time_used = time_used / total_pixels;

        if (time_file_name != NULL){
            fptr_time = fopen(time_file_name, "a");
            if(fptr_time == NULL){
                fprintf(stderr, 
                    "-time does not have valid input. Exit with Failure\n");
                exit(EXIT_FAILURE); 
            }
            fprintf(fptr_time, 
                "Rotation %d %s %d %s %d takes %.0f %s\n",
                    rotation, 
                    "for image with width of",
                    methods -> width(image_copy),
                    "and height of",
                    methods -> height(image_copy),
                    time_used,
                    "nanoseconds per pixels");
            fclose(fptr_time);
        }

        /* Rotate 0 if 0 */
        /* Rotate 90 */
        /* Rotate 180 */
        /* Rotate 270 */

        reader -> width = methods -> width(image_copy);

        reader -> height = methods -> height(image_copy);

        reader -> methods -> free(&(reader -> pixels));

        reader -> pixels = image_copy;

        Pnm_ppmwrite(stdout, reader);

        /* Free timer */
        CPUTime_Free(&timer);

        /* Free timer */
        Pnm_ppmfree(&reader);

        fclose(fptr);
        return 0;
}

/* 
 * rotate_0
 * Purpose: rotate the image 0 degree
 * Param: (width, height) coordinates 
 *         the array itself
 *         the element passed down from the map function
 *         a void pointer: the array used to store copy-over pixels
 * Return: go through the array and re-assign each index to the new index
 *         the array passed in as void pointer should show the transformation
 *
 */

static void rotate_0(int i, int j, 
    A2Methods_UArray2 array, A2Methods_Object *elem, void *cl) 
{
    (void) array;
    A2Methods_UArray2 copy_array = *(A2Methods_UArray2*) cl;

    Pnm_rgb position = methods -> at(copy_array, i, j);
    *position = *(Pnm_rgb) elem;
}

/* 
 * rotate_90
 * Purpose: rotate the image 90 degree
 * Param: (width, height) coordinates 
 *         the array itself
 *         the element passed down from the map function
 *         a void pointer: the array used to store copy-over pixels
 * Return: go through the array and re-assign each index to the new index
 *         the array passed in as void pointer should show the transformation
 *
 */

static void rotate_90(int i, int j, 
    A2Methods_UArray2 array, A2Methods_Object *elem, void *cl) 
{
    A2Methods_UArray2 copy_array = *(A2Methods_UArray2*) cl;

    /* Retrieving from copy_array, NOT original array */
    int height = methods -> height(array);

    Pnm_rgb position = methods -> at(copy_array, height - j - 1, i);
    *position = *(Pnm_rgb) elem;
}

/* 
 * rotate_180
 * Purpose: rotate the image 180 degree
 * Param: (width, height) coordinates 
 *         the array itself
 *         the element passed down from the map function
 *         a void pointer: the array used to store copy-over pixels
 * Return: go through the array and re-assign each index to the new index
 *         the array passed in as void pointer should show the transformation
 *
 */

static void rotate_180(int i, int j, 
    A2Methods_UArray2 array, A2Methods_Object *elem, void *cl) 
{
    A2Methods_UArray2 copy_array = *(A2Methods_UArray2*) cl;

    /* Retrieving from copy_array, NOT original array */
    int height = methods -> height(array);
    int width = methods -> width(array);

    Pnm_rgb position = 
        methods -> at(copy_array, width - i - 1, height - j - 1);
    *position = *(Pnm_rgb) elem;
}


/* 
 * rotate_270
 * Purpose: rotate the image 270 degree
 * Param: (width, height) coordinates 
 *         the array itself
 *         the element passed down from the map function
 *         a void pointer: the array used to store copy-over pixels
 * Return: go through the array and re-assign each index to the new index
 *         the array passed in as void pointer should show the transformation
 *
 */

static void rotate_270(int i, int j, 
    A2Methods_UArray2 array, A2Methods_Object *elem, void *cl) 
{
    A2Methods_UArray2 copy_array = *(A2Methods_UArray2*) cl;

    /* Retrieving from copy_array, NOT original array */
    int width = methods -> width(array);

    Pnm_rgb position = methods -> at(copy_array, j, width - 1 - i);
    *position = *(Pnm_rgb) elem;
}

/* 
 * flip_horiz
 * Purpose: flip the image horizontally
 * Param: (width, height) coordinates 
 *         the array itself
 *         the element passed down from the map function
 *         a void pointer: the array used to store copy-over pixels
 * Return: go through the array and re-assign each index to the new index
 *         the array passed in as void pointer should show the transformation
 *
 */

static void flip_horiz(int i, int j, 
    A2Methods_UArray2 array, A2Methods_Object *elem, void *cl)
{
    A2Methods_UArray2 copy_array = *(A2Methods_UArray2*) cl;

    /* Retrieving from copy_array, NOT original array */
    int width = methods -> width(array);

    Pnm_rgb position = 
        methods -> at(copy_array, width - i - 1, j);
    *position = *(Pnm_rgb) elem;
}

/* 
 * flip_verti
 * Purpose: flip the image vertically
 * Param: (width, height) coordinates 
 *         the array itself
 *         the element passed down from the map function
 *         a void pointer: the array used to store copy-over pixels
 * Return: go through the array and re-assign each index to the new index
 *         the array passed in as void pointer should show the transformation
 *
 */

static void flip_verti(int i, int j, 
    A2Methods_UArray2 array, A2Methods_Object *elem, void *cl)
{
    A2Methods_UArray2 copy_array = *(A2Methods_UArray2*) cl;

    /* Retrieving from copy_array, NOT original array */
    int height = methods -> height(array);

    Pnm_rgb position = 
        methods -> at(copy_array, i, height - 1 - j);
    *position = *(Pnm_rgb) elem;
}

/* 
 * tranpose
 * Purpose: tranpose the image
 * Param: (width, height) coordinates 
 *         the array itself
 *         the element passed down from the map function
 *         a void pointer: the array used to store copy-over pixels
 * Return: go through the array and re-assign each index to the new index
 *         the array passed in as void pointer should show the transformation
 *
 */

static void transpose(int i, int j, 
    A2Methods_UArray2 array, A2Methods_Object *elem, void *cl) 
{
    (void) array;
    A2Methods_UArray2 copy_array = *(A2Methods_UArray2*) cl;

    /* Retrieving from copy_array, NOT original array */

    Pnm_rgb position = methods -> at(copy_array, j, i);
    *position = *(Pnm_rgb) elem;
}
