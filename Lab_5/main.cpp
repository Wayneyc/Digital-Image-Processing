//
//  main.cpp
//  Lab_5
//
//  Created by Wenyuanchun on 2022/3/22.
//

// for mac
// #include <sys/malloc.h>
// for windows
#include <malloc.h>

#include <complex.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <algorithm>
#include <cmath>
#include <iostream>
#include "proto.h"

#define PI 3.141592653589793

/*
 Define the structure "Complex"
 */
typedef struct Complex {
    double real;
    double imag;
} complex;

/*
 Basic Operation
 */
Image *ReadPNMImage(char *filename);
Image *CreateNewImage(int type, int width, int height, char *comment);
void SavePNMImage(Image *, char *);
/*
 Basic DFT & iDFT
 */
Complex *DFT(Image *image);
int *IDFT(Complex *complexArr, int width, int height);
Image *FourierSpectrum(Complex *complexArr, int width, int height, int enhance);
Image *PhaseAngle(Complex *complexArr, int width, int height);
/*
 Reconstruct from DFT
 */
Image *DFTResult(Image *image, int type); // type: real->0, imaginary->1
Image *DFTPhaseAngle(Image *image);
Image *DFTMagnitude(Image *image, int enhance);
/*
 Support functions
 */
Image *GenerateImage(int *imageDate, int width, int height);


Image *Test(Image *image) {
    int width = image->Width;
    int height = image->Height;
    unsigned char *data = image->data;
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            data[i * width + j] = int(round(data[i * width + j] * pow(-1, i+j) * pow(-1, i+j)));
        }
    }
    return image;
};


int main(int argc, const char * argv[]) {
    
    
    Image *image1, *imageLena;
    
    char lena[] = ".\\lena.pgm";
    imageLena = ReadPNMImage(lena);

    // image1 = DFTResult(imageLena, 0);
    // image1 = DFTPhaseAngle(imageLena);
    image1 = DFTMagnitude(imageLena, 100);
    // image1 = Test(imageLena);
    
    char savePath1[] = ".\\Test.pgm";
    
    SavePNMImage(image1, savePath1);
 
    return 0;
}

Image *DFTResult(Image *image, int type) {

    Image *outImage;
    
    Complex *complexArr;
    
    complexArr = DFT(image);

    int width = image->Width;
    int height = image->Height;

    int *resultArr = (int *)malloc(sizeof(int) * width * height);

    int enhance = 1;

    for (int i = 0; i < width*height; i++) {
        if (type == 0) {
            resultArr[i] = int(round(complexArr[i].real * enhance));
        } else {
            resultArr[i] = int(round(complexArr[i].imag * enhance));
        }
    }

    outImage = GenerateImage(resultArr, width, height);

    return outImage;
}

Image *DFTMagnitude(Image *image, int enhance) {
    
    Image *outImage;
    
    Complex *complexArr;
    
    complexArr = DFT(image);
    
    outImage = FourierSpectrum(complexArr, image->Width, image->Height, enhance);
    
    return outImage;
}

Image *DFTPhaseAngle(Image *image) {
    
    Image *outImage;
    
    Complex *complexArr;
    
    complexArr = DFT(image);
    
    outImage = PhaseAngle(complexArr, image->Width, image->Height);
    
    return outImage;
}

Image *FourierSpectrum(Complex *complexArr, int width, int height, int enhance) {
    
    // New image
    char comment[] = "# Fourier Spectrum";
    Image *image = CreateNewImage(GRAY, width, height, comment);
    unsigned char *imageData = image->data;
    
    // To reinforce the image
    int reinforce = enhance;
    
    for (int u = 0; u < width; u++) {
        for (int v = 0; v < height; v++) {
            double real = complexArr[u * width + v].real * reinforce;
            double imag = complexArr[u * width + v].imag * reinforce;
            imageData[u * width + v] = int(round(sqrt(real*real + imag*imag)));

            if (imageData[u * width + v] > 255) {
                imageData[u * width + v] = 255;
            } else if (imageData[u * width + v] < 0) {
                imageData[u * width + v] = 0;
            }
        }
    }
    
    return image;
}

Image *PhaseAngle(Complex *complexArr, int width, int height) {
    
    // New image
    char comment[] = "# Phase Angle";
    Image *image = CreateNewImage(GRAY, width, height, comment);
    unsigned char *imageData = image->data;
    
    for (int u = 0; u < width; u++) {
        for (int v = 0; v < height; v++) {
            double real = complexArr[u * width + v].real;
            double imag = complexArr[u * width + v].imag;
            imageData[u * width + v] = int(round(pow(tan(imag/real), -1)));

            if (imageData[u * width + v] > 255) {
                imageData[u * width + v] = 255;
            } else if (imageData[u * width + v] < 0) {
                imageData[u * width + v] = 0;
            }
        }
    }
    
    return image;
}

int *IDFT(Complex *complexArr, int width, int height) {
    int *outpurArr = (int*)malloc(sizeof(int) * width * height);

    double real, temp;

    for (int u = 0; u < width; u++) {
        for (int v = 0; v < height; v++) {
            real = 0;
            // IDFT Function
            for (int m = 0; m < width; m++) {
                for (int n = 0; n < height; n++) {
                    temp = (double)u * m / (double)width + (double)v * n / (double)height;

                    real += complexArr[m * width + n].real * cos(2 * PI * temp) - complexArr[m * width + n].imag * sin(2 * PI * temp);
                }
            }
            // Inject into outputArr
            outpurArr[u * width + v] = int(round(real));
            if (outpurArr[u * width + v] > 255) {
                outpurArr[u * width + v] = 255;
            } else if (outpurArr[u * width + v] < 0) {
                outpurArr[u * width + v] = 0;
            }
            
        }
    }
    
    return outpurArr;
}

Complex *DFT(Image *image) {
    
    // Original Image
    int width = image->Width;
    int height = image->Height;
    printf("Size: %d %d\n", width, height);
    unsigned char *imageData = image->data;
    
    // Malloc
    Complex *complexArr;
    complexArr = (Complex *)malloc(sizeof(Complex) * width * height);
    
    // Centralization
    int *tempImage = (int *)malloc(sizeof(int) * width * height);
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            tempImage[i * width + j] = imageData[i * width + j];
            tempImage[i * width + j] = int(round(tempImage[i * width + j] * pow(-1, i + j)));
        }
    }
    
    // DFT
    for (int u = 0; u < width; u++) {
        // Check the process
        printf("DFT: %d/%d\n", u, width-1);
        for (int v = 0; v < height; v++) {
            // DFT Function
            for (int m = 0; m < width; m++) {
                for (int n = 0; n < height; n++) {
                    // Real part
                    complexArr[u * width + v].real += tempImage[m * width + n] * \
                    cos(double(2)*PI*u*m/width + double(2)*PI*v*n/height) / (width*height);
                    // Imaginary part
                    complexArr[u * width + v].imag += tempImage[m * width + n] * \
                    sin(-(double(2)*PI*u*m/width + double(2)*PI*v*n/height)) / (width*height);
                }
            }
        }
    }
    
    return complexArr;
}

Image *GenerateImage(int *imageDate, int width, int height) {

    char comment[] = "# New Image";
    Image *image = CreateNewImage(GRAY, width, height, comment);
    unsigned char *data = image->data;
    
    for (int i = 0; i < (width*height); i++) {
        data[i] = imageDate[i];
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
