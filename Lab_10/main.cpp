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

    printf("Hello Lab 10\n");

    printf("----------------- Question 1 -----------------\n");
    Image *inImage1, *outImage1;
    char inPath1[] = ".\\large_septagon_gaussian_noise_mean_0_std_50_added.pgm";
    inImage1 = ReadPNMImage(inPath1);
    outImage1 = otsuAlgorithm(inImage1);
    char outPath1[] = ".\\1-OTSU-WithoutSmoothed.pgm";
    SavePNMImage(outImage1, outPath1);
    char outPath2[] = ".\\1-OTSU-WithSmoothed.pgm";
    outImage1 = AverageMask(inImage1, 2);
    outImage1 = otsuAlgorithm(outImage1);
    SavePNMImage(outImage1, outPath2);

    printf("----------------- Question 2 -----------------\n");
    Image *inImage2, *outImage2;
    char inPath2[] = ".\\septagon_noisy_shaded.pgm";
    inImage2 = ReadPNMImage(inPath2);
    outImage2 = PartitionOTSU(inImage2, 2, 3);
    char outPath3[] = ".\\2-OTSU-Partition.pgm";
    SavePNMImage(outImage2, outPath3);

    printf("----------------- Question 3 -----------------\n");
    Image *inImage3, *outImage3;
    char inPath3[] = ".\\spot_shaded_text_image.pgm";
    inImage3 = ReadPNMImage(inPath3);
    outImage3 = MovingAverageThreshold(inImage3, 10, 0.5);
    char outPath4[] = ".\\3-MA.pgm";
    SavePNMImage(outImage3, outPath4);

    printf("----------------- Question 4 -----------------\n");
    Image *inImage4, *outImage4;
    char inPath4[] = ".\\defective_weld.pgm";
    char inPath5[] = ".\\noisy_region.pgm";
    inImage4 = ReadPNMImage(inPath4);
    outImage4 = RegionGrowing(inImage4, 180);
    char outPath5[] = ".\\4-RG-weld.pgm";
    SavePNMImage(outImage4, outPath5);
    inImage4 = ReadPNMImage(inPath5);
    outImage4 = RegionGrowing(inImage4, 170);
    char outPath6[] = ".\\4-RG-noisy.pgm";
    SavePNMImage(outImage4, outPath6);

    return 0;
}

Image *Erosion(Image *image)
{
    unsigned char *tempin, *tempout;
    int i, j, k, t, s, stop;
    Image *outimage;
    double mask[9] = {0, 1, 0, 1, 0, 1, 0, 1, 0};

    outimage = CreateNewImage(image, "#testing Swap");
    tempin = image->data;
    tempout = outimage->data;

    for (i = 0; i < image->Height; ++i)
    {
        for (j = 0; j < image->Width; ++j)
        {
            if (i == 0 || i == image->Height || j == 0 || j == image->Width)
            {
                tempout[image->Width * i + j] = tempin[image->Width * i + j];
            }
            else
            {
                s = 0;
                stop = 0;
                for (k = -1; k <= 1 && stop == 0; ++k)
                    for (t = -1; t <= 1 && stop == 0; ++t)
                    {
                        if (tempin[image->Width * (i + k) + (j + t)] == 0 && mask[s++] == 1)
                        {
                            tempout[image->Width * i + j] = 0;
                            stop = 1;
                        }
                    }
                if (stop == 0)
                    tempout[image->Width * i + j] = tempin[image->Width * i + j];
            }
        }
    }
    return (outimage);
}

Image *RegionGrowing(Image *image, int th)
{
    unsigned char *tempin, *tempout;
    int i, j, k, t, s, stop, count;
    Image *outimage;
    FILE *fp;
    int sign = 0;

    outimage = CreateNewImage(image, "# testing Swap");
    tempin = image->data;
    tempout = outimage->data;

    for (i = 0; i < image->Height; ++i)
    {
        for (j = 0; j < image->Width; ++j)
        {
            if (tempin[image->Width * i + j] > th)
                tempout[image->Width * i + j] = 255;
            else
                tempout[image->Width * i + j] = 0;
        }
    }

    outimage = Erosion(outimage);

    for (i = 0; i < image->Height; ++i)
    {
        for (j = 0; j < image->Width; ++j)
        {
            tempout[image->Width * i + j] = abs(tempout[image->Width * i + j] - tempin[image->Width * i + j]);
        }
    }

    for (i = 0; i < image->Height; ++i)
    {
        for (j = 0; j < image->Width; ++j)
        {
            if (abs(tempout[image->Width * i + j] - tempin[image->Width * i + j]) > 68)
            {
                tempout[image->Width * i + j] = 0;
            }
            else
            {
                tempout[image->Width * i + j] = 255;
            }
        }
    }

    return (outimage);
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
        outImageData[k] = outData[k];
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

Image *PartitionOTSU(Image *image, int row, int col)
{

    int width = image->Width;
    int height = image->Height;
    unsigned char *inData = image->data;

    /* Identify the partition */

    int *rowAnchor = (int *)malloc(sizeof(int) * (row + 1));
    int *colAnchor = (int *)malloc(sizeof(int) * (col + 1));

    // row
    rowAnchor[0] = 0;
    for (int k = 1; k < row; k++)
    {
        rowAnchor[k] = int(round((height * 1.0 / row) * k));
        // printf("rowAnchor: %d\n", rowAnchor[k]);
    }
    rowAnchor[row] = height;
    // col
    colAnchor[0] = 0;
    for (int k = 1; k < col; k++)
    {
        colAnchor[k] = int(round((width * 1.0 / col) * k));
        // printf("colAnchor: %d\n", colAnchor[k]);
    }
    colAnchor[col] = width;

    /* Create several array to store the result */

    /*
     histogram matrix:
     box0:                  [0, 1, ..., 255]
     box1:                  [0, 1, ..., 255]
     ...                                 ...
     box(row * col - 1):    [0, 1, ..., 255]
     */
    int *histogram = (int *)malloc(sizeof(int) * 256 * row * col);

    int *sum_1 = (int *)malloc(sizeof(int) * row * col);   // sum of box's left pixel's intensity
    int *sum_2 = (int *)malloc(sizeof(int) * row * col);   // sum of box's right pixel's intensity
    int *count_1 = (int *)malloc(sizeof(int) * row * col); // amount of box's left pixels
    int *count_2 = (int *)malloc(sizeof(int) * row * col); // amount of box's right pixels

    int *bestThreshold = (int *)malloc(sizeof(int) * row * col);
    double *bestSigma2 = (double *)malloc(sizeof(double) * row * col);

    /* Initialize */

    for (int k = 0; k < (256 * row * col); k++)
    {
        histogram[k] = 0;
    }
    for (int k = 0; k < (row * col); k++)
    {
        bestThreshold[k] = 0;
        bestSigma2[k] = 0;
    }

    /* Using the anchor to split image */

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {

            /* Find correct box */

            for (int row_box = 0; row_box < row; row_box++)
            {
                for (int col_box = 0; col_box < col; col_box++)
                {

                    // printf("(%d)-----------------\n\t%d\t%d\n", (row_box * col + col_box), colAnchor[col_box], colAnchor[col_box+1]);
                    // printf("%d\n%d\n", rowAnchor[row_box], rowAnchor[row_box+1]);

                    /* within the Row range & Col range */
                    if (((rowAnchor[row_box] <= i) && (i < rowAnchor[row_box + 1])) && ((colAnchor[col_box] <= j) && (j < colAnchor[col_box + 1])))
                    {
                        int box_index = row_box * col + col_box;
                        histogram[box_index * 256 + inData[i * width + j]] += 1;
                    }
                }
            }
        }
    }

    /* Calculate each box's best threshold */

    for (int row_box = 0; row_box < row; row_box++)
    {
        for (int col_box = 0; col_box < col; col_box++)
        {

            int box_index = row_box * col + col_box;

            // Try each intensity
            for (int t = 1; t < 256; t++)
            {
                // Initialize
                int sum_1 = 0;
                int sum_2 = 0;
                int count_1 = 0;
                int count_2 = 0;
                // Get info from histogram (split two parts)
                for (int k = 0; k < 256; k++)
                {
                    if (k < t)
                    {
                        sum_1 += histogram[box_index * 256 + k] * k;
                        count_1 += histogram[box_index * 256 + k];
                    }
                    else
                    {
                        sum_2 += histogram[box_index * 256 + k] * k;
                        count_2 += histogram[box_index * 256 + k];
                    }
                }
                // Calculate mean & probability
                double mean_1 = sum_1 * 1.0 / count_1;
                double mean_2 = sum_2 * 1.0 / count_2;
                double probability_1 = count_1 * 1.0 / (count_1 + count_2);
                double probability_2 = count_2 * 1.0 / (count_1 + count_2);
                double mean_g = (sum_1 + sum_2) * 1.0 / (count_1 + count_2);
                // Variance between classes
                double sigma2 = probability_1 * pow(mean_1 - mean_g, 2) + probability_2 * pow(mean_2 - mean_g, 2);
                if (sigma2 > bestSigma2[box_index])
                {
                    bestSigma2[box_index] = sigma2;
                    bestThreshold[box_index] = t;
                }
            }
        }
    }

    /* Apply to image */

    double *outData = (double *)malloc(sizeof(double) * width * height);

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {

            /* Find correct box */

            for (int row_box = 0; row_box < row; row_box++)
            {
                for (int col_box = 0; col_box < col; col_box++)
                {

                    int box_index = row_box * col + col_box;

                    /* within the Row range & Col range */
                    if (((rowAnchor[row_box] <= i) && (i < rowAnchor[row_box + 1])) && ((colAnchor[col_box] <= j) && (j < colAnchor[col_box + 1])))
                    {

                        int temp = inData[i * width + j];

                        /* Apply threshold */
                        if (temp < bestThreshold[box_index])
                        {
                            outData[i * width + j] = 0;
                        }
                        else
                        {
                            outData[i * width + j] = 255;
                        }
                    }
                }
            }
        }
    }

    Image *outImage = CreateImageByDouble(image, outData);

    return outImage;
}

Image *otsuAlgorithm(Image *image)
{

    int width = image->Width;
    int height = image->Height;
    unsigned char *inData = image->data;

    /* Create the histogram */
    int *histogramSet = (int *)malloc(sizeof(int) * 256);
    for (int k = 0; k < 256; k++)
    {
        histogramSet[k] = 0;
    }
    for (int k = 0; k < (width * height); k++)
    {
        histogramSet[inData[k]] += 1;
    }

    /* k -> threshold */
    double targetSigma2 = 0;
    double targetK = 0;
    for (int t = 1; t < 256; t++)
    {
        // Probability
        int sum_1 = 0;
        int sum_2 = 0;
        int count_1 = 0;
        int count_2 = 0;
        for (int k = 0; k < 256; k++)
        {
            if (k < t)
            {
                sum_1 += histogramSet[k] * k;
                count_1 += histogramSet[k];
            }
            else
            {
                sum_2 += histogramSet[k] * k;
                count_2 += histogramSet[k];
            }
        }
        double mean_1 = sum_1 * 1.0 / count_1;
        double mean_2 = sum_2 * 1.0 / count_2;
        double probability_1 = count_1 * 1.0 / (count_1 + count_2);
        double probability_2 = count_2 * 1.0 / (count_1 + count_2);
        double mean_g = (sum_1 + sum_2) * 1.0 / (count_1 + count_2);
        // Variance between classes
        double sigma2 = probability_1 * pow(mean_1 - mean_g, 2) + probability_2 * pow(mean_2 - mean_g, 2);
        if (sigma2 > targetSigma2)
        {
            targetSigma2 = sigma2;
            targetK = t;
        }
    }

    double *outData = (double *)malloc(sizeof(double) * width * height);
    for (int k = 0; k < (width * height); k++)
    {
        if (inData[k] < targetK)
        {
            outData[k] = 0;
        }
        else
        {
            outData[k] = 255;
        }
    }

    Image *outImage = CreateImageByDouble(image, outData);

    return outImage;
}

Image *AverageMask(Image *image, int size)
{

    int maskSize = size * 2 + 1;

    int width = image->Width;
    int height = image->Height;
    unsigned char *inData = image->data;

    double *outData = (double *)malloc(sizeof(double) * width * height);

    for (int k = 0; k < (width * height); k++)
    {
        outData[k] = 0;
    }

    for (int i = size; i < height - size; i++)
    {
        for (int j = size; j < width - size; j++)
        {

            int sumOfMask = 0;

            for (int u = 0; u < maskSize; u++)
            {
                for (int v = 0; v < maskSize; v++)
                {

                    int index = (i - size + u) * width + (j - size + v);

                    sumOfMask += inData[index];
                }
            }

            double avgOfMask = sumOfMask / (maskSize * maskSize) * 1.0;

            outData[i * width + j] = avgOfMask;
        }
    }

    Image *outImage = CreateImageByDouble(image, outData);

    return outImage;
}

Image *CreateImageByDouble(Image *targetImage, double *sourseData)
{

    char comments[] = "# Lab 10";
    Image *outImage = CreateNewImage(targetImage, comments);
    unsigned char *outData = outImage->data;

    int width = outImage->Width;
    int height = outImage->Height;

    for (int k = 0; k < (width * height); k++)
    {
        double avgOfMask = sourseData[k];

        int intOfAvg;
        if (avgOfMask > 255)
        {
            intOfAvg = 255;
        }
        else if (avgOfMask < 0)
        {
            intOfAvg = 0;
        }
        else
        {
            intOfAvg = int(round(avgOfMask));
        }

        outData[k] = intOfAvg;
    }

    return outImage;
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