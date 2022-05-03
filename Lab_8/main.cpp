//
//  main.cpp
//  Lab_5
//
//  Created by Wayne on 2022/04/04.
//

// for mac
// #include <sys/malloc.h>
// for windows
#include <malloc.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <algorithm>
#include <cmath>
#include <iostream>
#include "proto.h"

using namespace std;

/*
 PNM Method
 */
Image *CreateNewImage(int type, int width, int height, char *comment);
Image *ReadPNMImage(char *filename);
void SavePNMImage(Image *temp_image, char *filename);

/*
 Support Method
 */
int WhiteOrBlack(int x) {
    if (x >= 128) {
        return 1; // White
    } else {
        return 0; // Black
    }
}

/*
 Question functions
 */
Image *DilationAndErosion(Image *inImage, int operation, int kernel);
Image *OpenAndClose(Image *inImage, int operation, int kernel);
Image *ExtractBoundaries(Image *inImage, int size);
void CountNumberOfPixel(Image *inImage);
Image *FindParticles(Image *inImage, int method);

int main() {

    printf("Hello Lab 8\n");

    /* ---- Question 1 ---- */
    Image *image11, *image12;
    char image11Path[] = ".\\noisy_fingerprint.pgm";
    char image12Path[] = ".\\noisy_rectangle.pgm";
    image11 = ReadPNMImage(image11Path);
    image12 = ReadPNMImage(image12Path);

    Image *out11, *out12, *out13, *out14, *out15, *out16, *out17, *out18;
    out11 = DilationAndErosion(image11, 0, 0); // operation (Dilation, Erosion) , kernelType ('+', 'x')
    out12 = DilationAndErosion(image11, 0, 1);
    out13 = DilationAndErosion(image11, 1, 0);
    out14 = DilationAndErosion(image11, 1, 1);
    out15 = OpenAndClose(image11, 0, 0); // operation (Open, Close) , kernelType ('+', 'x')
    out16 = OpenAndClose(image11, 0, 1);
    out17 = OpenAndClose(image11, 1, 0);
    out18 = OpenAndClose(image11, 1, 1);

    char out11Path[] = ".\\out11.pgm";
    char out12Path[] = ".\\out12.pgm";
    char out13Path[] = ".\\out13.pgm";
    char out14Path[] = ".\\out14.pgm";
    char out15Path[] = ".\\out15.pgm";
    char out16Path[] = ".\\out16.pgm";
    char out17Path[] = ".\\out17.pgm";
    char out18Path[] = ".\\out18.pgm";
    SavePNMImage(out11, out11Path);
    SavePNMImage(out12, out12Path);
    SavePNMImage(out13, out13Path);
    SavePNMImage(out14, out14Path);
    SavePNMImage(out15, out15Path);
    SavePNMImage(out16, out16Path);
    SavePNMImage(out17, out17Path);
    SavePNMImage(out18, out18Path);

    /* ---- Question 2 ---- */
    Image *image21, *image22;
    char image21Path[] = ".\\licoln.pgm";
    char image22Path[] = ".\\U.pgm";
    image21 = ReadPNMImage(image21Path);
    image22 = ReadPNMImage(image22Path);

    Image *out21, *out22;
    out21 = ExtractBoundaries(image21, 1);
    out22 = ExtractBoundaries(image22, 1);

    char out21Path[] = ".\\out21.pgm";
    char out22Path[] = ".\\out22.pgm";    
    SavePNMImage(out21, out21Path);
    SavePNMImage(out22, out22Path);

    /* ---- Question 3 ---- */
    Image *image31;
    char image31Path[] = ".\\connected.pgm";
    image31 = ReadPNMImage(image31Path);

    CountNumberOfPixel(image31);

    /* ---- Question 4 ---- */
    Image *image41;
    char image41Path[] = ".\\bubbles_on_black_background.pgm";
    image41 = ReadPNMImage(image41Path);

    Image *out41, *out42, *out43;
    out41 = FindParticles(image41, 0);
    out42 = FindParticles(image41, 1);
    out43 = FindParticles(image41, 2);

    char out41Path[] = ".\\out41.pgm";
    char out42Path[] = ".\\out42.pgm";
    char out43Path[] = ".\\out43.pgm";
    SavePNMImage(out41, out41Path);
    SavePNMImage(out42, out42Path);
    SavePNMImage(out43, out43Path);

    return 0;
}

int *FindBoundary(Image *image) {
    int width = image->Width;
    int height = image->Height;
    unsigned char *dataset = image->data;
    /*
     0 -> Top
     1 -> Bottom
     2 -> Left
     3 -> Right
     */
    int index = 0;
    int *boundaries = (int *)malloc(sizeof(int) * 4);
    /* Find Top & Bottom */
    int *rows = (int *)malloc(sizeof(int) * height);
    for (int i = 0; i < height; i++) {
        int sum = 0;
        for (int j = 0; j < width; j++) {
            if (WhiteOrBlack(dataset[i * width + j]) == 1) {
                sum++;
            }
        }
        if (sum > 0.95 * width) {
            rows[i] = 1;
        } else {
            rows[i] = 0;
        }
    }
    for (int i = 0; i < height - 1; i++) {
        if (rows[i] != rows[i+1]) {
            boundaries[index++] = i;
        }
    }
    /* Find Left & Right */
    int *cols = (int *)malloc(sizeof(int) * width);
    for (int j = 0; j < width; j++) {
        int sum = 0;
        for (int i = 0; i < height; i++) {
            if (WhiteOrBlack(dataset[i * width + j]) == 1) {
                sum++;
            }
        }
        if (sum > 0.90 * height) {
            cols[j] = 1;
        } else {
            cols[j] = 0;
        }
    }
    for (int j = 0; j < width - 1; j++) {
        if (cols[j] != cols[j+1]) {
            boundaries[index++] = j;
        }
    }
    /* Do some small changes and Return */
    boundaries[0] += 1; // Top
    boundaries[2] += 1; // Left
    return boundaries;
}
void SendSeed(Image *image, int *seeds, int i, int j, int index) {
    int width = image->Width;
    int height = image->Height;
    unsigned char *dataset = image->data;

    if (WhiteOrBlack(dataset[i * width + j]) == 1) {
        dataset[i * width + j] = 0; // Set to black
        seeds[i * width + j] = index; // Set the index of seed

        /* Send to four direction */
        if (i > 0) {
            SendSeed(image, seeds, i - 1, j, index); // Go top
        }
        if (i < height - 1) {
            SendSeed(image, seeds, i + 1, j, index); // Go bottom
        }
        if (j > 0) {
            SendSeed(image, seeds, i, j - 1, index); // Go left
        }
        if (j < width - 1) {
            SendSeed(image, seeds, i, j + 1, index); // Go right
        }  

    }
    
}
Image *SeedForBoundaries(Image *image) {
    int width = image->Width;
    int height = image->Height;
    unsigned char *dataset = image->data;

    /* Create a set to store seeds */
    int *seeds = (int *)malloc(sizeof(int) * width * height);
    for (int k = 0; k < (width * height); k++) {
        seeds[k] = -1;
    }

    int index = 0;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            /* If it's the four boundaries */
            if ((i == 0) || (i == height - 1) || (j == 0) || (j == width - 1)) {
                /* If find the bubble */
                if (WhiteOrBlack(dataset[i * width + j]) == 1) {
                    SendSeed(image, seeds, i, j, index);
                    index++; // Next connected component
                    // printf("Index: %d\n", index);
                }
            }
        }
    }

    int threshold = 0;
    for (int k = 0; k < (width * height); k++) {
        if (seeds[k] == 9) {
            threshold++;
        }
    }
    printf("threshold (circle size) is about %d\n", threshold); // 383

    /* Create a new image */
    char comments[] = "# Boundaries image";
    Image *outImage = CreateNewImage(image->Type, width, height, comments);
    unsigned char *outSet = outImage->data;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (seeds[i * width + j] != -1) {
                outSet[i * width + j] = 255;
            } else {
                outSet[i * width + j] = 0;
            }
        }
    }

    return outImage;
}
/* type: 0 -> Overlapping; 1 -> non-Overlapping */
Image *SeedForOverlapping(Image *image, int type) {
    int width = image->Width;
    int height = image->Height;
    unsigned char *dataset = image->data;

    /* Create a set to store seeds */
    int *seeds = (int *)malloc(sizeof(int) * width * height);
    for (int k = 0; k < (width * height); k++) {
        seeds[k] = -1;
    }

    int index = 0;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            /* If find the bubble */
            if (WhiteOrBlack(dataset[i * width + j]) == 1) {
                SendSeed(image, seeds, i, j, index);
                index++; // Next connected component
                // printf("Index: %d\n", index);
            }
        }
    }

    /* Create a new image */
    char comments[] = "# (non)Overlapping image";
    Image *outImage = CreateNewImage(image->Type, width, height, comments);
    unsigned char *outSet = outImage->data;
    /* Initialization */
    for (int i = 0; i < (width * height); i++) {
        outSet[i] = 0;
    }
    /* Find target */
    int threshold = 400;
    for (int k = 0; k < index; k++) {
        int sum = 0;
        for (int i = 0; i < (width * height); i++) {
            if (seeds[i] == k) {
                sum++;
            }
        }
        /* If it's the target */
        if (type == 0) {
            if (sum < threshold) {
                for (int i = 0; i < height; i++) {
                    for (int j = 0; j < width; j++) {
                        if (seeds[i * width + j] == k) {
                            outSet[i * width + j] = 255;
                        }
                    }
                }
            }
        } else {
            if (sum >= threshold) {
                for (int i = 0; i < height; i++) {
                    for (int j = 0; j < width; j++) {
                        if (seeds[i * width + j] == k) {
                            outSet[i * width + j] = 255;
                        }
                    }
                }
            }
        }
    }
    
    return outImage;
}
Image *FindParticles(Image *inImage, int method) {

    int width = inImage->Width;
    int height = inImage->Height;
    unsigned char *inData = inImage->data;

    int *boundaries = FindBoundary(inImage);
    int top = boundaries[0];
    int bottom = boundaries[1];
    int left = boundaries[2];
    int right = boundaries[3];

    /* Create a new image which is cropped */
    char comments[] = "# Cropped image";
    Image *tempImage = CreateNewImage(inImage->Type, right - left + 1, bottom - top + 1, comments);
    unsigned char *tempData = tempImage->data;
    for (int i = top; i < bottom + 1; i++) { // rows
        for (int j = left; j < right + 1; j++) { // cols
            tempData[(i-top) * (tempImage->Width) + (j-left)] = inData[i * width + j];
        }
    }

    Image *outImage;
    if (method == 0) {
        /* Question a - Only of particles that have merged with the boundary of the image */
        outImage = SeedForBoundaries(tempImage);
    } else if (method == 1) {
        /* Question b - Only overlapping particles */
        outImage = SeedForOverlapping(tempImage, 1);
    } else if (method == 2) {
        /* Question c - Only nonoverlapping particles*/
        outImage = SeedForOverlapping(tempImage, 0);
    }
    
    return outImage;
}


int CountingSubFunction(Image *image, int i, int j, int i0, int j0, int count) {

    int width = image->Width;
    int height = image->Height;
    unsigned char *dataset = image->data;

    /* Two situations */
    int newline = 0;
    int bottom = 0;

    // It need newline
    if (j + 1 >= width) {
        newline = 1;
    } else if (WhiteOrBlack(dataset[i * width + j + 1]) == 0) {
        newline = 1;
    }
    // It will be bottom
    if (i + 1 >= height) {
        bottom = 1;
    } else if (WhiteOrBlack(dataset[(i + 1) * width + j]) == 0) {
        bottom = 1;
    }
    // Clear white connected component
    if (WhiteOrBlack(dataset[i * width + j]) == 1) {
        count++;
        dataset[i * width + j] = 0;
    }

    /* Recursion */
    if (bottom == 1) {
        return count;
    } else if (newline == 1) {
        return CountingSubFunction(image, i+1, j0, i0, j0, count);
    } else {
        return CountingSubFunction(image, i, j+1, i0, j0, count);
    }
}
void CountNumberOfPixel(Image *inImage) {

    printf("Counting...\n");

    unsigned char *inData, *outData;
    int width = inImage->Width;
    int height = inImage->Height;
    char comments[] = "# Temporary image";
    Image *outImage = CreateNewImage(inImage->Type, width, height, comments);
    inData = inImage->data;
    outData = outImage->data;

    int *countTable = (int *)malloc(sizeof(int) * width * height);
    int index = 0;

    for (int k = 0; k < (width * height); k++) {
        countTable[k] = 0;
        outData[k] = inData[k];
    }
    
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (WhiteOrBlack(outData[i * width + j]) == 1) {
                /* It's white */
                countTable[index] = CountingSubFunction(outImage, i, j, i, j, 0);
                index++;
            }
        }
    }

    /* Write to file */
    FILE *fp;
    char filePath[] = "CountPixels.txt";
    fp = fopen(filePath, "w+");
    if (fp == NULL) {
        printf("File Error!\n");
        exit(0);
    } else {
        printf("Start writing...\n");
    }

    fprintf(fp, "Index\t\tCount\n");
    for (int k = 0; k < index; k++) {
        fprintf(fp, "%d\t\t\t%d\n", k, countTable[k]);
    }
    fclose(fp);
    printf("Writed\n");
}

/*
 @ size
    1 -> 3x3
    2 -> 5x5
 */
Image *ExtractBoundaries(Image *inImage, int size) {

    unsigned char *inData, *outData;
    int width = inImage->Width;
    int height = inImage->Height;
    char comments[] = "# Extract Boundaries";
    Image *outImage = CreateNewImage(inImage->Type, width, height, comments);
    inData = inImage->data;
    outData = outImage->data;

    /* Initialize the image */
    for (int k = 0; k < (width * height); k++) {
        outData[k] = 0;
    }

    for (int i = size; i < height - size; i++) {
        for (int j = size; j < width - size; j++) {
            int white = 0;
            int black = 0;
            for (int u = i - 1; u < (i + 2 * size + 1); u++) {
                for (int v = j - 1; v < (j + 2 * size + 1); v++) {
                    if (WhiteOrBlack(inData[u * width + v]) == 0) {
                        black++;
                    } else {
                        white++;
                    }
                }
            }
            /* The Boundary */
            if ((white > 0) && (black > 0)) {
                outData[i * width + j] = 255;
            }
        }
    }

    return outImage;
}

/*
 @ operation
    Open -> 0
    Close  -> 1
 */
Image *OpenAndClose(Image *inImage, int operation, int kernel) {
    Image *outImage;
    /* Choose operation */
    if (operation == 0) {
        /* Open = Erosion + Dilation */
        outImage = DilationAndErosion(inImage, 1, kernel);
        outImage = DilationAndErosion(outImage, 0, kernel);
    } else {
        /* Close = Dilation + Erosion */
        outImage = DilationAndErosion(inImage, 0, kernel);
        outImage = DilationAndErosion(outImage, 1, kernel);
    }
    return outImage;
}

/*
 @ operation
    Dilation -> 0
    Erosion  -> 1
 */
Image *DilationAndErosion(Image *inImage, int operation, int kernelType) {

    unsigned char *inData, *outData;
    int width = inImage->Width;
    int height = inImage->Height;
    char comments[] = "# DilationOrErosion";
    Image *outImage = CreateNewImage(inImage->Type, width, height, comments);
    inData = inImage->data;
    outData = outImage->data;

    /* Copy the image firstly*/
    for (int k = 0; k < (width * height); k++) {
        outData[k] = inData[k];
    }

    /* Define the kernal (3x3) */
    int *pkernel;
    if (kernelType == 0) {
        /* '+' pattern kernel */
        int kernel[3][3] = {{0, 1, 0}, {1, 1, 1},{0, 1, 0}};
        pkernel = &kernel[0][0];
    } else if (kernelType == 1) {
        /* 'x' pattern kernel */
        int kernel[3][3] = {{1, 0, 1}, {0, 1, 0},{1, 0, 1}};
        pkernel = &kernel[0][0];
    } else if (kernelType == 2) {
        /* 'all' pattern kernel */
        int kernel[3][3] = {{1, 1, 1}, {1, 1, 1},{1, 1, 1}};
        pkernel = &kernel[0][0];
    } else {
        /* Not defined kernel */
        printf("Error! Please input correct kernel type.\n");
    }

    int filterSize = 1;
    /* Operating the image */
    for (int i = filterSize; i < height - filterSize; i++) {
        for (int j = filterSize; j < width - filterSize; j++) {
            /* In the structural element */
            for (int u = 0; u < filterSize * 2 + 1; u++) {
                for (int v = 0; v < filterSize * 2 + 1; v++) {
                    /* Relative location*/
                    int relative = u * (filterSize * 2 + 1) + v;
                    /* Absolute location */
                    int absolute = (i-1) * width + (j-1) + relative;
                    /* Choose operation */
                    if (operation == 0) {
                        /* dilation */
                        if ((*(pkernel + relative) == 1) && (WhiteOrBlack(inData[absolute]) == 1)) {
                            outData[i * width + j] = 255;
                        }
                    } else if (operation == 1) {
                        /* erosion */
                        if ((*(pkernel + relative) == 1) && (WhiteOrBlack(inData[absolute]) == 0)) {
                            outData[i * width + j] = 0;
                        }
                    } else {
                        /* Not defined operation */
                        printf("Error! Please input correct operation type.\n");
                    }
                }
            }
        }
    }

    return outImage;
}

/* ------------------------------ Previous Functions ------------------------------ */

Image *CreateNewImage(int type, int width, int height, char *comment)
{
    Image *outimage;
    int size;
    
    // Check the input legality
    if ((type != GRAY) && (type != COLOR)) {
        printf("Type inlegal!\n");
        exit(0);
    } else if ((width <= 0) || (height <= 0)) {
        printf("Width or height inlegal!\n");
        exit(0);
    }

    outimage = (Image *)malloc(sizeof(Image));

    outimage->Type = type;
    if (outimage->Type == GRAY)
        size = width * height;
    else if (outimage->Type == COLOR)
        size = width * height * 3;

    outimage->Width = width;
    outimage->Height = height;
    outimage->num_comment_lines = 0;

    strcpy(outimage->comments[outimage->num_comment_lines], comment);
    outimage->num_comment_lines++;

    outimage->data = (unsigned char *)malloc(sizeof(unsigned char) * size);
    if (!outimage->data)
    {
        printf("cannot allocate memory for new image");
        exit(0);
    }
    return (outimage);
}

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
        printf(".. Image Type PGM\n");
    else
        printf(".. Image Type PPM Color\n");
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