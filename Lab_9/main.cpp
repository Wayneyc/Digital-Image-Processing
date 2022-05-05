//
//  main.cpp
//  Lab_9
//
//  Created by Wayne on 2022/05/03.
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
 Support Method
 */
int Threshold(int x, int threshold);
int *GetGradientKernel(int type, int direction);
int *GradientOperator(Image *image, int type, int direction);
Image *ShowGLPF(Image *image, int d);
Complex *GHPF(Complex *imageArr, int width, int height, int d);
Complex *GLPF(Complex *imageArr, int width, int height, int d);
double *Image2Double(Image *image, int width, int height);
double *CenterTranslation(double *imageArr, int width, int height);
Image *GenerateImage(Image *image, double *imageArr, int width, int height);
Complex *DFT(double *imageArr, int width, int height);
double *IDFT(Complex *imageArr, int width, int height);
double *GradientMagnitude(Image *image);
double *AngleImage(Image *image);

/*
 Main Method
 */
Image *GetGradientImage(Image *image, int type);
Image *CannyAlgorithm(Image *image);


int main() {

    printf("Hello Lab 9\n");
    
    char inPath_1[] = ".\\headCT-Vandy.pgm";
    char inPath_2[] = ".\\building_original.pgm";
    char inPath_3[] = ".\\noisy_fingerprint.pgm";
    char inPath_4[] = ".\\polymersomes.pgm";

    /* Question 1 */
    Image *inImage, *outImage;
    inImage = ReadPNMImage(inPath_1);
    outImage = GetGradientImage(inImage, 0);
    char outPath1[] = ".\\q1\\Gradient-0.pgm";
    SavePNMImage(outImage, outPath1);

    /* Question 2 */
    inImage = ReadPNMImage(inPath_1);
    outImage = CannyAlgorithm(inImage);
    char outPath2[] = ".\\q2\\Canny.pgm";
    SavePNMImage(outImage, outPath2);

    return 0;
}


Image *CannyAlgorithm(Image *image) {

    Image *GaussianImage = ShowGLPF(image, 60); // Gaussian Lowpass Filter
    
    AngleImage(image);

    return GaussianImage;
}

Image *NonmaximaSuppression(Image *image) {

    /* Compute by Sobel gradient operator */
    int *Gx = GradientOperator(image, 2, 0);
    int *Gy = GradientOperator(image, 2, 1);
    double *Gxy = GradientMagnitude(image);
    
    char comments[] = "# Nonmaxima Suppression";
    Image *outImage = CreateNewImage(image, comments);
    unsigned char *outData = outImage->data;
    int width = outImage->Width;
    int height = outImage->Height;


    int *Gindexs = (int *)malloc(sizeof(int) * width * height);
    double *Gup = (double *)malloc(sizeof(double) * width * height);
    double *Gdown = (double *)malloc(sizeof(double) * width * height);

    /* Determine the range of gradient direction */
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {

            int ix = Gx[i * width + j];
            int iy = Gy[i * width + j];

            if (((ix <= 0) && (ix > -iy)) || ((iy >= 0) && (ix < -iy))) {
                /* Range 1 */
                Gindexs[i * width + j] = 1;
            } else if (((ix > 0) && (ix <= -iy)) || ((iy < 0) && (ix >= -iy))) {
                /* Range 2 */
                Gindexs[i * width + j] = 2;
            } else if (((ix <= 0) && (ix > iy)) || ((iy >= 0) && (ix < iy))) {
                /* Range 3 */
                Gindexs[i * width + j] = 3;
            } else if (((ix < 0) && (ix <= iy)) || ((iy > 0) && (ix >= iy))) {
                /* Range 4 */
                Gindexs[i * width + j] = 4;
            } else {
                /* No gradient */
                Gindexs[i * width + j] = 5;
            }

        }
    }

    /* Compute Gup and Gdown */
    for (int i = 1; i < height - 1; i++) {
        for (int j = 1; j < width - 1; j++) {

            double ix = Gx[i * width + j] * 1.0;
            double iy = Gy[i * width + j] * 1.0;
            double t = abs(iy / ix);

            if (Gindexs)

        }
    }

    return outImage;
}
double *GradientMagnitude(Image *image) {

    /* Compute by Sobel gradient operator */
    int *Gx = GradientOperator(image, 2, 0);
    int *Gy = GradientOperator(image, 2, 1);
    
    int width = image->Width;
    int height = image->Height;
    double *outData = (double *)malloc(sizeof(double) * width * height);

    for (int k = 0; k < (width * height); k++) {
        outData[k] = sqrt(pow(Gx[k], 2) + pow(Gy[k], 2));
    }

    return outData;
}
double *AngleImage(Image *image) {

    /* Compute by Sobel gradient operator */
    int *Gx = GradientOperator(image, 2, 0);
    int *Gy = GradientOperator(image, 2, 1);

    int width = image->Width;
    int height = image->Height;
    double *outData = (double *)malloc(sizeof(double) * width * height);

    for (int k = 0; k < (width * height); k++) {
        double temp = tan(Gy[k] / Gx[k] * 1.0);
        outData[k] = pow(temp, -1);
        printf("%.2f\n", outData[k]);
    }

    return outData;
}

/* Step 1 of Canny */
Image *ShowGLPF(Image *image, int d) {

    int width = image->Width;
    int height = image->Height;

    double *imageData = Image2Double(image, width, height);
    imageData = CenterTranslation(imageData, width, height);
    Complex *imageArr = DFT(imageData, width, height);
    imageArr = GLPF(imageArr, width, height, d); // Pass Filter
    double *retArr = IDFT(imageArr, width, height);
    retArr = CenterTranslation(retArr, width, height);
    Image *outputImage = GenerateImage(image, retArr, width, height);
    
    return outputImage;
}
Image *GenerateImage(Image *image, double *imageArr, int width, int height) {
    
    // New image
    char comment[] = "# New Image";
    Image *outImage = CreateNewImage(image, comment);
    unsigned char *imageData = outImage->data;

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

    return outImage;
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

/*
 Input: original image
 @type: Roberts(0) Prewitt(1) Sobel(2)
 @direction: x(0) y(1) p_digonal(2) n_digonal(3)
 Output: gradient image
 */
Image *GetGradientImage(Image *image, int type) {

    unsigned char *inData, *outData;
    int width = image->Width;
    int height = image->Height;
    inData = image->data;
    char comments[] = "# Gradient Operator";
    Image *outImage = CreateNewImage(image, comments);
    outData = outImage->data;

    /* Compute Gx and Gy */
    int *Gx = GradientOperator(image, type, 0);
    int *Gy = GradientOperator(image, type, 1);

    /* Create the gradient G = Gx + Gy */
    int *G = (int *)malloc(sizeof(int) * width * height);

    /* Compute |Gx| and |Gy|, then, Normalization */
    for (int k = 0; k < (width * height); k++) {
        if (Gx[k] < 0) {
            Gx[k] = -Gx[k];
        }
        if (Gy[k] < 0) {
            Gy[k] = -Gy[k];
        }
        G[k] = Gx[k] + Gy[k]; // approximate the magnitude of the gradient
    }
    int min = *min_element(G, G + (width * height));
    int max = *max_element(G, G + (width * height));
    int scale = (max - min) / 255;
    for (int k = 0; k < (width * height); k++) {
        G[k] = (G[k] - min) / scale;
        outData[k] = G[k];
    }

    return outImage;
}
int *GradientOperator(Image *image, int type, int direction) {

    unsigned char *inData;
    int width = image->Width;
    int height = image->Height;
    inData = image->data;
    int *tempData = (int *)malloc(sizeof(int) * width * height);

    /* Initialize */
    for (int k = 0; k < (width * height); k++) {
        tempData[k] = 0;
    }

    /* Gradient Operator */
    int *gradientKernel;
    if (type == 0) {
        if (direction == 0) {
            int kernel[3][3] = {{0, 0, 0}, {0, -1, 0},{0, 1, 0}};
            gradientKernel = &kernel[0][0];
        } else if (direction == 1) {
            int kernel[3][3] = {{0, 0, 0}, {0, -1, 1},{0, 0, 0}};
            gradientKernel = &kernel[0][0];
        } else if (direction == 2) {
            int kernel[3][3] = {{0, 0, 0}, {0, 0, 1},{0, -1, 0}};
            gradientKernel = &kernel[0][0];
        } else if (direction == 3) {
            int kernel[3][3] = {{0, 0, 0}, {0, 0, -1},{0, 1, 0}};
            gradientKernel = &kernel[0][0];
        } else {
            printf("Error! Invalid Direction, try x(0) y(1) p_digonal(2) n_digonal(3)\n");
            exit(0);
        }
    } else if (type == 1) {
        if (direction == 0) {
            int kernel[3][3] = {{-1, -1, -1}, {0, 0, 0},{1, 1, 1}};
            gradientKernel = &kernel[0][0];
        } else if (direction == 1) {
            int kernel[3][3] = {{-1, 0, 1}, {-1, 0, 1},{-1, 0, 1}};
            gradientKernel = &kernel[0][0];
        } else if (direction == 2) {
            int kernel[3][3] = {{0, 1, 1}, {-1, 0, 1},{-1, -1, 0}};
            gradientKernel = &kernel[0][0];
        } else if (direction == 3) {
            int kernel[3][3] = {{-1, -1, 0}, {-1, 0, 1},{0, 1, 1}};
            gradientKernel = &kernel[0][0];
        } else {
            printf("Error! Invalid Direction, try x(0) y(1) p_digonal(2) n_digonal(3)\n");
            exit(0);
        }
    } else if (type == 2) {
        if (direction == 0) {
            int kernel[3][3] = {{-1, -2, -1}, {0, 0, 0},{1, 2, 1}};
            gradientKernel = &kernel[0][0];
        } else if (direction == 1) {
            int kernel[3][3] = {{-1, 0, 1}, {-2, 0, 2},{-1, 0, 1}};
            gradientKernel = &kernel[0][0];
        } else if (direction == 2) {
            int kernel[3][3] = {{0, 1, 2}, {-1, 0, 1},{-2, -1, 0}};
            gradientKernel = &kernel[0][0];
        } else if (direction == 3) {
            int kernel[3][3] = {{-2, -1, 0}, {-1, 0, 1},{0, 1, 2}};
            gradientKernel = &kernel[0][0];
        } else {
            printf("Error! Invalid Direction, try x(0) y(1) p_digonal(2) n_digonal(3)\n");
            exit(0);
        }
    } else {
        printf("Error! Invalid Type, try Roberts(0) Prewitt(1) Sobel(2)\n");
        exit(0);
    }

    int filterSize = 1;
    for (int i = filterSize; i < height - filterSize; i++) {
        for (int j = filterSize; j < width - filterSize; j++) {

            /* Compute gradiant */
            int gradient = 0;

            for (int u = 0; u < filterSize * 2 + 1; u++) {
                for (int v = 0; v < filterSize * 2 + 1; v++) {

                    /* Relative location*/
                    int relative = u * (filterSize * 2 + 1) + v;
                    /* Absolute location */
                    int absolute = (i-filterSize + u) * width + (j-filterSize + v);

                    gradient += (inData[absolute] * gradientKernel[relative]);
                }
            }
            
            /* Corresponding to the output image */
            tempData[i * width + j] = gradient;
        }
    }

    return tempData;
}

int Threshold(int x, int threshold) {
    if (x >= threshold) {
        return 255; // White
    } else {
        return 0; // Black
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

    if (ch == '5') {
        image->Type = GRAY; // Gray (pgm)
    } else if (ch == '6') {
        image->Type = COLOR; //Color (ppm)
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
    fp = fopen(filename, "wb");
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