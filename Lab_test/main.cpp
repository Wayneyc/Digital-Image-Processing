//
//  main.cpp
//  Lab_10
//
//  Created by Wayne on 2022/05/20.
//

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <cstdlib>
#include "proto.h"

using namespace std;

/*
 PNM Method
 */
Image *CreateNewImage(Image *image, char *comment);
Image *ReadPNMImage(char *filename);
void SavePNMImage(Image *temp_image, char *filename);

/*
 Other Method
 */
void SequencePush(int new_data, int *sequence, int n, int *k);

/*
 Support Method
 */
Image *AverageMask(Image *image, int size);
Image *CreateImageByDouble(Image *targetImage, double *sourseData);
Image *otsuAlgorithm(Image *image);
Image *PartitionOTSU(Image *image, int row, int col);
Image *MovingAverageThreshold(Image *image, int n, int b);
Image *RegionGrowing(Image *image, int th);

int main()
{

    printf("Lab 10\n");

    printf("----------------- Question 3 ------------------\n");
    Image *inImage3, *outImage3;
    char inPath3[] = ".\\spot_shaded_text_image.pgm";
    inImage3 = ReadPNMImage(inPath3);
    outImage3 = MovingAverageThreshold(inImage3, 10, 0.5);
    char outPath4[] = ".\\3-MA.pgm";
    SavePNMImage(outImage3, outPath4);

    return 0;
}


Image *MovingAverageThreshold(Image *image, int n, int b)
{

    int width = image->Width;
    int height = image->Height;
    unsigned char *inData = image->data;

    /* Sequence */
    int k = 0;
    int *seq = (int *)malloc(sizeof(int) * n);

    int *outData = (int *)malloc(sizeof(int) * width * height);

    /*
     direction:
     1: go right    -->
     2: go left     <--
     */
    int direction = 1;

    for (int i = 0; i < height; i++)
    {

        if (direction == 1)
        {
            /* Go right */
            for (int j = 0; j < width; j++)
            {

                int sum = 0;
                double avg = 0;

                // Move to new pixel
                int newPixel = inData[i * width + j];
                SequencePush(newPixel, seq, n, &k);

                // Calculate the average of the sequence
                for (int m = 0; m < k; m++)
                {
                    sum += seq[m];
                }
                avg = sum * 1.0 / k;

                if (newPixel <= b * avg)
                {
                    outData[i * width + j] = 0;
                }
                else
                {
                    outData[i * width + j] = 255;
                }
            }

            direction++; // Change direction
        }
        else
        {
            /* Go left */
            for (int j = width - 1; j >= 0; j--)
            {

                int sum = 0;
                double avg = 0;

                // Move to new pixel
                int newPixel = inData[i * width + j];
                SequencePush(newPixel, seq, n, &k);

                // Calculate the average of the sequence
                for (int m = 0; m < k; m++)
                {
                    sum += seq[m];
                }
                avg = sum * 1.0 / k;

                if (newPixel <= b * avg)
                {
                    outData[i * width + j] = 0;
                }
                else
                {
                    outData[i * width + j] = 255;
                }
            }

            direction--; // Change direction
        }
    }

    char comments[] = "# Moving Average";
    Image *outImage = CreateNewImage(image, comments);
    unsigned char *outImageData = outImage->data;

    for (int k = 0; k < (width * height); k++) {
        outImageData[k] = inData[k];
    }

    return outImage;
}
void SequencePush(int new_data, int *sequence, int n, int *k)
{

    int length = *k;

    if (length + 1 < n)
    {
        for (int i = length; i >= 0; i--)
        {
            sequence[i + 1] = sequence[i]; // Move right
        }
        sequence[0] = new_data;
        (*k) += 1;
    }
    else
    {
        for (int i = n - 1; i >= 0; i--)
        {
            sequence[i + 1] = sequence[i]; // Move right
        }
    }
}

/* ------------------------------ Previous Functions ------------------------------ */

Image *ReadPNMImage(char *filename)
{
    char ch;
    int maxval, Width, Height;
    int size, num, j;
    FILE *fp;
    Image *image;
    int num_comment_lines = 0;

    image = (Image *)malloc(sizeof(Image));

    /**
     * @brief Construct a new if object
     *
     * Error fopen: rb
     */
    if ((fp = fopen(filename, "rb")) == NULL)
    {
        perror("Open failed");
        // printf("Cannot open %s\n", filename);
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
    printf("Complete reading of %d bytes \n", num);
    if (num != size)
    {
        printf("cannot read image data from file");
        exit(0);
    }

    // for(j=0;j<image->num_comment_lines;j++){
    //       printf("%s\n",image->comments[j]);
    //       }

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
    // char comment[100];

    printf("Saving Image %s\n", filename);
    fp = fopen(filename, "wb");
    if (!fp)
    {
        printf("cannot open file for writing");
        exit(0);
    }

    // strcpy(comment,"#Created by Dr Mohamed N. Ahmed");

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