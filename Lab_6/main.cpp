//
//  main.cpp
//  Lab_5
//
//  Created by Wenyuanchun on 2022/3/22.
//

// for mac
#include <sys/malloc.h>
// for windows
// #include <malloc.h>

#include <complex.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <algorithm>
#include <cmath>
#include <iostream>
#include "proto.h"

using namespace std;

#define PI 3.141592653589793

/*
 Basic Method
 */
Complex *DFT(int *imageArr, int width, int height);
Complex *IDFT(int *imageArr, int width, int height);
double *FourierSpectrum(Complex *imageArr, int width, int height);
/*
 Support Method
 */
int *CenterTranslation(int *image, int width, int height);
Image *GenerateImage(double *imageArr, int width, int height);
/*
 Image Method
 */
Image *CreateNewImage(int type, int width, int height, char *comment);
Image *ReadPNMImage(char *filename);
void SavePNMImage(Image *temp_image, char *filename);
/*
 Functions
 */
void ShowDFT(Image *image);


int main(int argc, const char * argv[]) {
    
    printf("Hello DFT ...\n");

    Image *lenaImage;
    char lena[] = "/Users/wenyuanchun/Desktop/DIP/Digital-Image-Processing/Lab_6/lena.pgm";
    lenaImage = ReadPNMImage(lena);

    ShowDFT(lenaImage);
 
    return 0;
}

void ShowDFT(Image *image) {

    char savePath[] = "/Users/wenyuanchun/Desktop/DIP/Digital-Image-Processing/Lab_6/lena_save.pgm";

    int width = image->Width;
    int height = image->Height;
    unsigned char *imageData = image->data;

    // Read image into int array
    int *intImageData = (int *)malloc(sizeof(int) * width * height);
    for (int k = 0; k < (width * height); k++) {
        intImageData[k] = imageData[k];
    }

    int *centeredArr = CenterTranslation(intImageData, width, height);

    Complex *imageArr = DFT(intImageData, width, height);

    double *spectrumArr = FourierSpectrum(imageArr, width, height);

    Image *outputImage = GenerateImage(spectrumArr, width, height);

    SavePNMImage(outputImage, savePath);
}

Complex *DFT(int *imageArr, int width, int height) {

    Complex *t = (Complex *)malloc(sizeof(Complex) * width * height);
    Complex *g = (Complex *)malloc(sizeof(Complex) * width * height);

    int rows = height;
    int cols = width;

    // DFT for cols
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            for (int m = 0; m < rows; m++) {
                double theta = double(-2) * PI * (i * m) / rows;
                t[i * rows + j].real += imageArr[m * rows + j] * cos(theta);
                t[i * rows + j].imag += imageArr[m * rows + j] * sin(theta);
            }
        }
    }

    // DFT for rows
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            for (int n = 0; n < cols; n++) {
                double theta = double(-2) * PI * (j * n) / cols;
                g[i * rows + j].real += t[i * rows + n].real * cos(theta) - t[i * rows + n].imag * sin(theta);
                g[i * rows + j].imag += t[i * rows + n].real * sin(theta) + t[i * rows + n].imag * cos(theta);
            }
        }
    }

    // Divide by image size
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            g[i * rows + j].real /= rows * cols;
            g[i * rows + j].imag /= rows * cols;
        }
    }

    return g;
}

Complex *IDFT(int *imageArr, int width, int height) {

    Complex *t = (Complex *)malloc(sizeof(Complex) * width * height);
    Complex *g = (Complex *)malloc(sizeof(Complex) * width * height);

    int rows = height;
    int cols = width;

    return g;
}

double *FourierSpectrum(Complex *imageArr, int width, int height) {
    
    double *outputArr = (double *)malloc(sizeof(double) * width * height);

    int rows = height;
    int cols = width;

    int enhance = 100;

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            double real = imageArr[i * rows + j].real;
            double imag = imageArr[i * rows + j].imag;
            outputArr[i * rows + j] = sqrt(real * real + imag * imag) * enhance;
            // outputArr[i * rows + j] = real * enhance;
        }
    }

    return outputArr;   
}

int *CenterTranslation(int *imageArr, int width, int height) {
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            imageArr[i * width + j] = int(round(imageArr[i * width + j] * pow(-1, i + j)));
        }
    }
    return imageArr;
}
    
Image *GenerateImage(double *imageArr, int width, int height) {
    // New image
    char comment[] = "# New Image";
    Image *image = CreateNewImage(GRAY, width, height, comment);
    unsigned char *imageData = image->data;
    // Mapping to 0 ~ 255
    for (int k = 0; k < (width * height); k++) {
        int intensity = int(round(imageArr[k]));
        if (intensity > 255) {
            intensity = 255;
        } else if (intensity < 0) {
            intensity = 0;
        }
        imageData[k] = intensity;
    }

    return image;
}


/* --------------- Previous Functions --------------- */

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

    /*--------------------------------------------------------*/
    /* Copy Comments for Original Image      */
//    for (j = 0; j < outimage->num_comment_lines; j++)
//        strcpy(outimage->comments[j], image->comments[j]);

    /*----------- Add New Comment  ---------------------------*/
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