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

/*
 Basic Method
 */
Complex *DFT(double *imageArr, int width, int height);
double *IDFT(Complex *imageArr, int width, int height);
double *FourierSpectrum(Complex *imageArr, int width, int height);
/*
 Advance Method
 */
Complex *ILPF(Complex *imageArr, int width, int height, int d);
Complex *BLPF(Complex *imageArr, int width, int height, int d, int n);
Complex *GLPF(Complex *imageArr, int width, int height, int d);
Complex *IHPF(Complex *imageArr, int width, int height, int d);
Complex *BHPF(Complex *imageArr, int width, int height, int d, int n);
Complex *GHPF(Complex *imageArr, int width, int height, int d);
/*
 Support Method
 */
double *CenterTranslation(double *image, int width, int height);
double *Image2Double(Image *image, int width, int height);
Image *GenerateImage(double *imageArr, int width, int height); 
Image *Thresholding(Image *image, int threshold);
/*
 PNM Method
 */
Image *CreateNewImage(int type, int width, int height, char *comment);
Image *ReadPNMImage(char *filename);
void SavePNMImage(Image *temp_image, char *filename);



Image *ShowDFT(Image *image) {

    int width = image->Width;
    int height = image->Height;
    
    double *imageData = Image2Double(image, width, height);

    imageData = CenterTranslation(imageData, width, height);

    Complex *imageArr = DFT(imageData, width, height);

    double *retArr = FourierSpectrum(imageArr, width, height);

    Image *outputImage = GenerateImage(retArr, width, height);

    return outputImage;
}

/*
 Ideal Low Pass Filter
 */
void ShowILPF(Image *image, int d) {

    char savePath[] = "/Users/wenyuanchun/Desktop/DIP/Digital-Image-Processing/Lab_6/lena_ILPF.pgm";

    int width = image->Width;
    int height = image->Height;

    double *imageData = Image2Double(image, width, height);
    imageData = CenterTranslation(imageData, width, height);
    Complex *imageArr = DFT(imageData, width, height);
    imageArr = ILPF(imageArr, width, height, d); // Pass Filter
    double *retArr = IDFT(imageArr, width, height);
    retArr = CenterTranslation(retArr, width, height);
    Image *outputImage = GenerateImage(retArr, width, height);
    SavePNMImage(outputImage, savePath);
}

/*
 Butterworth Low Pass Filter
 */
void ShowBLPF(Image *image, int d, int n) {

    char savePath[] = "/Users/wenyuanchun/Desktop/DIP/Digital-Image-Processing/Lab_6/lena_BLPF.pgm";

    int width = image->Width;
    int height = image->Height;

    double *imageData = Image2Double(image, width, height);
    imageData = CenterTranslation(imageData, width, height);
    Complex *imageArr = DFT(imageData, width, height);
    imageArr = BLPF(imageArr, width, height, d, n); // Pass Filter
    double *retArr = IDFT(imageArr, width, height);
    retArr = CenterTranslation(retArr, width, height);
    Image *outputImage = GenerateImage(retArr, width, height);
    SavePNMImage(outputImage, savePath);
}

/*
 Gaussian Low Pass Filter
 */
void ShowGLPF(Image *image, int d) {

    char savePath[] = "/Users/wenyuanchun/Desktop/DIP/Digital-Image-Processing/Lab_6/lena_GLPF.pgm";

    int width = image->Width;
    int height = image->Height;

    double *imageData = Image2Double(image, width, height);
    imageData = CenterTranslation(imageData, width, height);
    Complex *imageArr = DFT(imageData, width, height);
    imageArr = GLPF(imageArr, width, height, d); // Pass Filter
    double *retArr = IDFT(imageArr, width, height);
    retArr = CenterTranslation(retArr, width, height);
    Image *outputImage = GenerateImage(retArr, width, height);
    SavePNMImage(outputImage, savePath);
}

/*
 Ideal High Pass Filter
 */
void ShowIHPF(Image *image, int d) {

    char savePath[] = "/Users/wenyuanchun/Desktop/DIP/Digital-Image-Processing/Lab_6/lena_IHPF.pgm";

    int width = image->Width;
    int height = image->Height;

    double *imageData = Image2Double(image, width, height);
    imageData = CenterTranslation(imageData, width, height);
    Complex *imageArr = DFT(imageData, width, height);
    imageArr = IHPF(imageArr, width, height, d); // Pass Filter
    double *retArr = IDFT(imageArr, width, height);
    retArr = CenterTranslation(retArr, width, height);
    Image *outputImage = GenerateImage(retArr, width, height);
    SavePNMImage(outputImage, savePath);
}

/*
 Butterworth High Pass Filter
 */
void ShowBHPF(Image *image, int d, int n) {

    char savePath[] = "/Users/wenyuanchun/Desktop/DIP/Digital-Image-Processing/Lab_6/lena_BHPF.pgm";

    int width = image->Width;
    int height = image->Height;

    double *imageData = Image2Double(image, width, height);
    imageData = CenterTranslation(imageData, width, height);
    Complex *imageArr = DFT(imageData, width, height);
    imageArr = BHPF(imageArr, width, height, d, n); // Pass Filter
    double *retArr = IDFT(imageArr, width, height);
    retArr = CenterTranslation(retArr, width, height);
    Image *outputImage = GenerateImage(retArr, width, height);
    SavePNMImage(outputImage, savePath);
}

/*
 Gaussian High Pass Filter
 */
void ShowGHPF(Image *image, int d) {

    char savePath[] = "/Users/wenyuanchun/Desktop/DIP/Digital-Image-Processing/Lab_6/lena_GHPF.pgm";

    int width = image->Width;
    int height = image->Height;

    double *imageData = Image2Double(image, width, height);
    imageData = CenterTranslation(imageData, width, height);
    Complex *imageArr = DFT(imageData, width, height);
    imageArr = GHPF(imageArr, width, height, d); // Pass Filter
    double *retArr = IDFT(imageArr, width, height);
    retArr = CenterTranslation(retArr, width, height);
    Image *outputImage = GenerateImage(retArr, width, height);
    SavePNMImage(outputImage, savePath);
}

/*
 Thresholding
 */
void ShowThreshold(Image *image) {

    char savePath[] = "/Users/wenyuanchun/Desktop/DIP/Digital-Image-Processing/Lab_6/lena_Threshold2.pgm";

    int width = image->Width;
    int height = image->Height;

    int d = 30;
    int threshold = 15;

    double *imageData = Image2Double(image, width, height);
    imageData = CenterTranslation(imageData, width, height);
    Complex *imageArr = DFT(imageData, width, height);
    imageArr = GHPF(imageArr, width, height, d); // Pass Filter
    double *retArr = IDFT(imageArr, width, height);
    retArr = CenterTranslation(retArr, width, height);
    Image *outputImage = GenerateImage(retArr, width, height);
    outputImage = Thresholding(outputImage, threshold);
    SavePNMImage(outputImage, savePath);
}

int main(int argc, const char * argv[]) {
    
    printf("Hello DFT ...\n");

    Image *lenaImage;

    char lena[] = ".\\lena.pgm";

    lenaImage = ReadPNMImage(lena);

    char savePath[] = ".\\lena_testsss.pgm";

    SavePNMImage(lenaImage, savePath);
    
    // // Testing
    // ShowDFT(lenaImage);
    // // Low Filter
    // ShowILPF(lenaImage, 160);
    // ShowBLPF(lenaImage, 160, 2);
    // ShowGLPF(lenaImage, 160);
    // // High Filter
    // ShowIHPF(lenaImage, 30);
    // ShowBHPF(lenaImage, 30, 2);
    // ShowGHPF(lenaImage, 30);
    
    // // Thresholding
    // ShowThreshold(lenaImage);
 
    return 0;
}

Image *Thresholding(Image *image, int threshold) {

    int width = image->Width;
    int height = image->Height;
    unsigned char *imageData = image->data;

    for (int k = 0; k < (width * height); k++) {
        if (imageData[k] > threshold) {
            imageData[k] = 0;
        } else {
            imageData[k] = 255;
        }
    }

    return image;
}

Complex *GHPF(Complex *imageArr, int width, int height, int d) {

    double e = 2.718281828;

    double center_x = width / double(2);
    double center_y = height / double(2);

    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            double D = sqrt(pow(center_x-i, 2) + pow(center_y-j, 2));
            double H = pow(e, (-1) * (pow(D, 2) / (2 * pow(d, 2))));
            H = 1 - H;
            imageArr[i * width + j].real *= H;
            imageArr[i * width + j].imag *= H;
        }
    }
    
    return imageArr;
}

Complex *BHPF(Complex *imageArr, int width, int height, int d, int n) {

    double center_x = width / double(2);
    double center_y = height / double(2);

    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            double D = sqrt(pow(center_x-i, 2) + pow(center_y-j, 2));
            double H = 1 / (1 + pow(D / d, 2 * n));
            H = 1 - H;
            imageArr[i * width + j].real *= H;
            imageArr[i * width + j].imag *= H;
        }
    }
    
    return imageArr;
}

Complex *IHPF(Complex *imageArr, int width, int height, int d) {

    double center_x = width / double(2);
    double center_y = height / double(2);

    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            double D = sqrt(pow(center_x-i, 2) + pow(center_y-j, 2));
            if (D <= d) {
                imageArr[i * width + j].real = 0;
                imageArr[i * width + j].imag = 0;
            }
        }
    }

    return imageArr;
}

Complex *GLPF(Complex *imageArr, int width, int height, int d) {

    double e = 2.718281828;

    double center_x = width / double(2);
    double center_y = height / double(2);

    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            double D = sqrt(pow(center_x-i, 2) + pow(center_y-j, 2));
            double H = pow(e, (-1) * (pow(D, 2) / (2 * pow(d, 2))));
            imageArr[i * width + j].real *= H;
            imageArr[i * width + j].imag *= H;
        }
    }
    
    return imageArr;
}

Complex *BLPF(Complex *imageArr, int width, int height, int d, int n) {

    double center_x = width / double(2);
    double center_y = height / double(2);

    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            double D = sqrt(pow(center_x-i, 2) + pow(center_y-j, 2));
            double H = 1 / (1 + pow(D / d, 2 * n));
            imageArr[i * width + j].real *= H;
            imageArr[i * width + j].imag *= H;
        }
    }
    
    return imageArr;
}

Complex *ILPF(Complex *imageArr, int width, int height, int d) {

    double center_x = width / double(2);
    double center_y = height / double(2);

    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            double D = sqrt(pow(center_x-i, 2) + pow(center_y-j, 2));
            if (D > d) {
                imageArr[i * width + j].real = 0;
                imageArr[i * width + j].imag = 0;
            }
        }
    }

    return imageArr;
}

Complex *DFT(double *imageArr, int width, int height) {

    Complex *t = (Complex *)malloc(sizeof(Complex) * width * height);
    Complex *g = (Complex *)malloc(sizeof(Complex) * width * height);

    int rows = height;
    int cols = width;

    printf("Starting DFT ...\n");

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

    printf("DFT finished\n");

    return g;
}

double *IDFT(Complex *imageArr, int width, int height) {

    Complex *t = (Complex *)malloc(sizeof(Complex) * width * height);
    double *g = (double *)malloc(sizeof(double) * width * height);

    int rows = height;
    int cols = width;

    printf("Starting IDFT ...\n");

    // IDFT for cols
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            for (int m = 0; m < rows; m++) {
                double theta = double(2) * PI * (i * m) / rows;
                t[i * rows + j].real += imageArr[m * rows + j].real * cos(theta) - imageArr[m * rows + j].imag * sin(theta);
                t[i * rows + j].imag += imageArr[m * rows + j].real * sin(theta) + imageArr[m * rows + j].imag * cos(theta);
            }
        }
    }

    // IDFT for rows
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            for (int n = 0; n < cols; n++) {
                double theta = double(2) * PI * (j * n) / cols;
                g[i * rows + j] += t[i * rows + n].real * cos(theta) - t[i * rows + n].imag * sin(theta);
            }
        }
    }

    printf("IDFT finished\n");

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
        }
    }

    return outputArr;   
}

double *CenterTranslation(double *imageArr, int width, int height) {
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            imageArr[i * width + j] = round(imageArr[i * width + j] * pow(-1, i + j));
        }
    }
    return imageArr;
}

double *Image2Double(Image *image, int width, int height) {
    unsigned char *imageData = image->data;
    double *retArr = (double *)malloc(sizeof(double) * width * height);
    for (int i = 0; i < (width * height); i++) {
        retArr[i] = double(imageData[i]);
    }
    return retArr;
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