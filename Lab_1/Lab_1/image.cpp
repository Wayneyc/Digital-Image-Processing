//
//  image.cpp
//  DIP_Space
//
//  Created by Wenyuanchun on 2022/2/23.
//

#include <stdio.h>

// for mac
#include <sys/malloc.h>
// for windows
//#include <malloc.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <algorithm>
#include "proto.h"

using namespace std;

void SavePNMImage(Image *, char *);
Image *ReadPNMImage(char *);
Image *CreateNewImage(Image *, char *comment);
int TestReadImage(char *, char *);

int AverageFilter(char *filename, char *outfilename);
int MedianFilter(char *filename, char *outfilename);
bool CheckMargin(int index, int width, int height, int margin);
Image *AverageOperator(Image *image);
Image *MedianOperator(Image *image);

/*
int main(int argc, const char * argv[]) {
    printf("Hello, DIP!\n");
 
    char input1[] = "/Users/wenyuanchun/Desktop/DIP/DIP_Space/DIP_Space/Resource/images/noise.pgm";
    char input2[] = "/Users/wenyuanchun/Desktop/DIP/DIP_Space/DIP_Space/Resource/images/lena.pgm";
    char output1[] = "/Users/wenyuanchun/Desktop/DIP/DIP_Space/DIP_Space/noise_average.pgm";
    char output2[] = "/Users/wenyuanchun/Desktop/DIP/DIP_Space/DIP_Space/noise_median.pgm";
    char output3[] = "/Users/wenyuanchun/Desktop/DIP/DIP_Space/DIP_Space/lena_average.pgm";
    char output4[] = "/Users/wenyuanchun/Desktop/DIP/DIP_Space/DIP_Space/lena_median.pgm";
    
    AverageFilter(input1, output1);
    MedianFilter(input1, output2);
    AverageFilter(input2, output3);
    MedianFilter(input2, output4);
    
    return 0;
}
*/

int AverageFilter(char *filename, char *outfilename) {
    Image *image;
    Image *outimage;

    image = ReadPNMImage(filename);
    outimage = AverageOperator(image);
    SavePNMImage(outimage, outfilename);
    
    return 0;
}

int MedianFilter(char *filename, char *outfilename) {
    Image *image;
    Image *outimage;

    image = ReadPNMImage(filename);
    outimage = MedianOperator(image);
    SavePNMImage(outimage, outfilename);
    
    return 0;
}

/*
 To ignorn all around the margins.
 index: the index of each pixel
 width: width of image
 height: height of image
 margin: size of filter (eg. 3x3 -> margin = 1; 5x5 -> margin = 2)
 */
bool CheckMargin(int index, int width, int height, int margin) {
    
    /* filter area is too large */
    if (((margin*2 + 1) > width) || ((margin*2 + 1) > height)) {
        printf("Error: check image or filter again!\n");
        exit(0);
    }
    
    int size = width * height; // total size
    margin = margin - 1; // for computing
    bool ret = true; // Is the margin or not
    
    /* Check top & bottom margin */
    if ((((index % width) >= 0) && ((index % width) <= margin)) || (((index % width) >= (width - 1 - margin)) && ((index % width) <= (width - 1)))) {
        ret = false;
    }
    /* Check left & right margin */
    if (((index >= 0) && (index <= (width * (margin + 1) - 1))) || ((index <= size - 1) && (index >= (size - (width * (margin + 1)))))) {
        ret = false;
    }
    /*
     true: not the margin
     false: is the margin
     */
    return ret;
}

/*
 For average filter
 */
Image *AverageOperator(Image *image) {
    /* Grab image info */
    int width = image->Width;
    int height = image->Height;
    
    unsigned char *tempin, *tempout;
    int size;
    Image *outimage;
    
    /* Create a host image */
    char comment[] = "#Average-Filter"; // '#' is necessary!
    outimage = CreateNewImage(image, comment);
    tempin = image->data;
    tempout = outimage->data;
    /*
     Ignore margin (1 pixel)
     */
    int margin = 1;
    size = width * height;
    for (int i = 0; i < size; i++) {
        if (CheckMargin(i, width, height, margin)) {
            int sum = 0; // sum of intensity of the pixel in filter area
            for (int j = -margin; j <= margin; j++) { // each line in filter area
                for (int k = -margin; k <= margin; k++) { // each item in the line
                    sum = sum + *(tempin + (j * width) + k);
                }
            }
            sum /= (margin * 2 + 1) * (margin * 2 + 1); // as the average
            *tempout = sum; // Update
        } else {
            *tempout = *tempin;
        }
        tempin++;
        tempout++;
    }
    return outimage;
}


Image *MedianOperator(Image *image) {
    
    /* Grab image info */
    int width = image->Width;
    int height = image->Height;
    
    unsigned char *tempin, *tempout;
    int size;
    Image *outimage;
    
    /* Create a host image */
    char comment[] = "#Median-Filter"; // '#' is necessary!
    outimage = CreateNewImage(image, comment);
    tempin = image->data;
    tempout = outimage->data;
    
    /*
     Ignore margin (1 pixel)
     */
    int margin = 1;
    size = width * height;
    for (int i = 0; i < size; i++) {
        if (CheckMargin(i, width, height, margin)) {
            int arr[(margin * 2 + 1) * (margin * 2 + 1)]; // to store the pixel in filter area
            int count = 0; // as the index in arr
            for (int j = -margin; j <= margin; j++) { // each line
                for (int k = -margin; k <= margin; k++) { // each item in the line
                    arr[count] = *(tempin + (j * width) + k); // add to array
                    count++;
                }
            }
            sort(arr, arr + (margin * 2 + 1) * (margin * 2 + 1)); // sort the pixel
            *tempout = arr[((margin * 2 + 1) * (margin * 2 + 1) / 2)]; // Update
        } else {
            *tempout = *tempin;
        }
        tempin++;
        tempout++;
    }
    
    return outimage;
}

/* ------------ Given functions ------------ */

Image *ReadPNMImage(char *filename)
{
    char ch;
    int maxval, Width, Height;
    int size, num, j;
    FILE *fp;
    Image *image;
    int num_comment_lines = 0;

    image = (Image *)malloc(sizeof(Image));

    if ((fp = fopen(filename, "rb")) == NULL)
    {
        printf("Cannot open %s\n", filename);
        exit(0);
    }

    printf("Loading %s ...", filename);

    if (fscanf(fp, "P%c\n", &ch) != 1)
    {
        printf("File is not in ppm/pgm raw format; cannot read\n");
        exit(0);
    }
    if (ch != '6' && ch != '5')
    {
        printf("File is not in ppm/pgm raw format; cannot read\n");
        exit(0);
    }

    if (ch == '5')
    {
        image->Type = GRAY; // Gray (pgm)
    }
    else if (ch == '6')
    {
        image->Type = COLOR; // Color (ppm)
    }

    /* skip comments */
    ch = getc(fp);
    j = 0;
    while (ch == '#')
    {
        image->comments[num_comment_lines][j] = ch;
        j++;
        do
        {
            ch = getc(fp);
            image->comments[num_comment_lines][j] = ch;
            j++;
        } while (ch != '\n'); /* read to the end of the line */
        image->comments[num_comment_lines][j - 1] = '\0';
        j = 0;
        num_comment_lines++;
        ch = getc(fp); /* thanks, Elliot */
    }

    if (!isdigit((int)ch))
    {
        printf("Cannot read header information from ppm file");
        exit(0);
    }

    ungetc(ch, fp); /* put that digit back */

    /* read the width, height, and maximum value for a pixel */
    fscanf(fp, "%d%d%d\n", &Width, &Height, &maxval);

    /*
  if (maxval != 255){
    printf("image is not true-color (24 bit); read failed");
    exit(0);
  }
  */

    if (image->Type == GRAY)
        size = Width * Height;
    else if (image->Type == COLOR)
        size = Width * Height * 3;
    image->data = (unsigned char *)malloc(size);
    image->Width = Width;
    image->Height = Height;
    image->num_comment_lines = num_comment_lines;

    if (!image->data)
    {
        printf("cannot allocate memory for new image");
        exit(0);
    }

    num = fread((void *)image->data, 1, (size_t)size, fp);
    //printf("Complete reading of %d bytes \n", num);
    if (num != size)
    {
        printf("cannot read image data from file");
        exit(0);
    }

    //for(j=0;j<image->num_comment_lines;j++){
    //      printf("%s\n",image->comments[j]);
    //      }

    fclose(fp);

    /*-----  Debug  ------*/

    if (image->Type == GRAY)
        printf("..Image Type PGM\n");
    else
        printf("..Image Type PPM Color\n");
    /*
  printf("Width %d\n", Width);
  printf("Height %d\n",Height);
  printf("Size of image %d bytes\n",size);
  printf("maxvalue %d\n", maxval);
  */
    return (image);
}

void SavePNMImage(Image *temp_image, char *filename)
{
    int num, j;
    int size;
    FILE *fp;
    //char comment[100];

    printf("Saving Image %s\n", filename);
    fp = fopen(filename, "w");
    if (!fp)
    {
        printf("cannot open file for writing");
        exit(0);
    }

    //strcpy(comment,"#Created by Dr Mohamed N. Ahmed");

    if (temp_image->Type == GRAY)
    { // Gray (pgm)
        fprintf(fp, "P5\n");
        size = temp_image->Width * temp_image->Height;
    }
    else if (temp_image->Type == COLOR)
    { // Color (ppm)
        fprintf(fp, "P6\n");
        size = temp_image->Width * temp_image->Height * 3;
    }

    for (j = 0; j < temp_image->num_comment_lines; j++)
        fprintf(fp, "%s\n", temp_image->comments[j]);

    fprintf(fp, "%d %d\n%d\n", temp_image->Width, temp_image->Height, 255);

    num = fwrite((void *)temp_image->data, 1, (size_t)size, fp);

    if (num != size)
    {
        printf("cannot write image data to file");
        exit(0);
    }

    fclose(fp);
}

/*************************************************************************/
/*Create a New Image with same dimensions as input image                 */
/*************************************************************************/

Image *CreateNewImage(Image *image, char *comment)
{
    Image *outimage;
    int size, j;

    outimage = (Image *)malloc(sizeof(Image));

    outimage->Type = image->Type;
    if (outimage->Type == GRAY)
        size = image->Width * image->Height;
    else if (outimage->Type == COLOR)
        size = image->Width * image->Height * 3;

    outimage->Width = image->Width;
    outimage->Height = image->Height;
    outimage->num_comment_lines = image->num_comment_lines;

    /*--------------------------------------------------------*/
    /* Copy Comments for Original Image      */
    for (j = 0; j < outimage->num_comment_lines; j++)
        strcpy(outimage->comments[j], image->comments[j]);

    /*----------- Add New Comment  ---------------------------*/
    strcpy(outimage->comments[outimage->num_comment_lines], comment);
    outimage->num_comment_lines++;

    outimage->data = (unsigned char *)malloc(size);
    if (!outimage->data)
    {
        printf("cannot allocate memory for new image");
        exit(0);
    }
    return (outimage);
}
