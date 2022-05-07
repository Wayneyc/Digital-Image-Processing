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
double *FourierSpectrum(Complex *imageArr, int width, int height);
double *GradientMagnitude(Image *image);
double *AngleImage(Image *image);
Image *NonmaximaSuppression(Image *image);
int *ZeroCrossing(int *inData, int width, int height);

/*
 Main Method
 */
Image *GetGradientImage(Image *image, int type);
Image *CannyAlgorithm(Image *image);
Image *LoGAlgorithm(Image *image);
Image *GlobalThresholding(Image *image, double T0);


int main() {

    printf("Hello Lab 9\n");

    Image *inImage, *outImage;
    
    char inPath_1[] = ".\\headCT-Vandy.pgm";
    char inPath_2[] = ".\\building_original.pgm";
    char inPath_3[] = ".\\noisy_fingerprint.pgm";
    char inPath_4[] = ".\\polymersomes.pgm";

    /* Question 1 - Roberts(0) Prewitt(1) Sobel(2) */
    
    // headCT-Vandy.pgm
    inImage = ReadPNMImage(inPath_1);
    outImage = GetGradientImage(inImage, 0);
    char outPath10[] = ".\\q1\\headCT-Roberts.pgm";
    SavePNMImage(outImage, outPath10);
    inImage = ReadPNMImage(inPath_1);
    outImage = GetGradientImage(inImage, 1);
    char outPath11[] = ".\\q1\\headCT-Prewitt.pgm";
    SavePNMImage(outImage, outPath11);
    inImage = ReadPNMImage(inPath_1);
    outImage = GetGradientImage(inImage, 2);
    char outPath12[] = ".\\q1\\headCT-Sobel.pgm";
    SavePNMImage(outImage, outPath12);

    // building_original
    inImage = ReadPNMImage(inPath_2);
    outImage = GetGradientImage(inImage, 0);
    char outPath13[] = ".\\q1\\building-Roberts.pgm";
    SavePNMImage(outImage, outPath13);
    inImage = ReadPNMImage(inPath_2);
    outImage = GetGradientImage(inImage, 1);
    char outPath14[] = ".\\q1\\building-Prewitt.pgm";
    SavePNMImage(outImage, outPath14);
    inImage = ReadPNMImage(inPath_2);
    outImage = GetGradientImage(inImage, 2);
    char outPath15[] = ".\\q1\\building-Sobel.pgm";
    SavePNMImage(outImage, outPath15);

    // noisy_fingerprint.pgm
    inImage = ReadPNMImage(inPath_3);
    outImage = GetGradientImage(inImage, 0);
    char outPath16[] = ".\\q1\\fingerprint-Roberts.pgm";
    SavePNMImage(outImage, outPath16);
    inImage = ReadPNMImage(inPath_3);
    outImage = GetGradientImage(inImage, 1);
    char outPath17[] = ".\\q1\\fingerprint-Prewitt.pgm";
    SavePNMImage(outImage, outPath17);
    inImage = ReadPNMImage(inPath_3);
    outImage = GetGradientImage(inImage, 2);
    char outPath18[] = ".\\q1\\fingerprint-Sobel.pgm";
    SavePNMImage(outImage, outPath18);
    

    /* Question 2 */

    // headCT-Vandy.pgm
    inImage = ReadPNMImage(inPath_1);
    outImage = CannyAlgorithm(inImage);
    char outPath20[] = ".\\q2\\headCT-Canny.pgm";
    SavePNMImage(outImage, outPath20);
    inImage = ReadPNMImage(inPath_1);
    outImage = LoGAlgorithm(inImage);
    char outPath31[] = ".\\q2\\headCT-LoG.pgm";
    SavePNMImage(outImage, outPath31);

    // noisy_fingerprint.pgm
    inImage = ReadPNMImage(inPath_3);
    outImage = CannyAlgorithm(inImage);
    char outPath32[] = ".\\q2\\fingerprint-Canny.pgm";
    SavePNMImage(outImage, outPath32);
    inImage = ReadPNMImage(inPath_3);
    outImage = LoGAlgorithm(inImage);
    char outPath33[] = ".\\q2\\fingerprint-LoG.pgm";
    SavePNMImage(outImage, outPath33);


    /* Question 3 */
    
    // polymersomes.pgm
    inImage = ReadPNMImage(inPath_4);
    outImage = GlobalThresholding(inImage, 0.1);
    char outPath40[] = ".\\q3\\polymersomes-threshold.pgm";
    SavePNMImage(outImage, outPath40);
    // noisy_fingerprint.pgm
    inImage = ReadPNMImage(inPath_3);
    outImage = GlobalThresholding(inImage, 0.1);
    char outPath41[] = ".\\q3\\fingerprint-threshold.pgm";
    SavePNMImage(outImage, outPath41);
    

    return 0;
}

double FindThreshold(int *data, int width, int height, double threshold, double T0) {

    int group_1 = 0; // sum
    int group_2 = 0;
    int count_1 = 0; // number
    int count_2 = 0;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (data[i * width + j] < threshold) {
                group_1 += data[i * width + j];
                count_1++;
            } else {
                group_2 += data[i * width + j];
                count_2++;
            }
        }
    }

    double avg_1 = group_1 / count_1 * 1.0;
    double avg_2 = group_2 / count_2 * 1.0;
    
    double new_threshold = (avg_1 + avg_2) / 2;

    if (abs(new_threshold - threshold) < T0) {
        // Find it
        return threshold;
    } else {
        // Recursive continuely
        return FindThreshold(data, width, height, new_threshold, T0);
    }
}
Image *GlobalThresholding(Image *image, double T0) {

    /* Prepare data */
    int width = image->Width;
    int height = image->Height;
    char comments[] = "# Global Thresholding";
    Image *outImage = CreateNewImage(image, comments);
    unsigned char *inData = image->data;
    unsigned char *outData = outImage->data;

    /* Initialize data */
    int *initData = (int *)malloc(sizeof(int) * width * height);
    for (int k = 0; k < (width * height); k++) {
        initData[k] = inData[k];
    }

    /* Initialize threshold */
    int initMin = *min_element(initData, initData + (width * height));
    int initMax = *max_element(initData, initData + (width * height));
    double initThreshold = (initMin + initMax) / 2 * 1.0;

    /* Subfuntions */
    double target_threshold = FindThreshold(initData, width, height, initThreshold, T0);

    /* Apply to the image */
    for (int k = 0; k < (width * height); k++) {
        if (inData[k] > target_threshold) {
            outData[k] = 255;
        } else {
            outData[k] = 0;
        }
    }

    return outImage;
}

Image *LoGAlgorithm(Image *image) {

    int LoGFilter[] = {
        0, 0, -1, 0, 0,
        0, -1, -2, -1, 0,
        -1, -2, 16, -2, -1,
        0, -1, -2, -1, 0,
        0, 0, -1, 0, 0
    };

    int width = image->Width;
    int height = image->Height;
    char comments[] = "# LoG Algorithm";
    Image *outImage = CreateNewImage(image, comments);
    unsigned char *inData = image->data;
    unsigned char *outData = outImage->data;

    int *LoG_Gradients = (int *)malloc(sizeof(int) * width * height);
    for (int k = 0; k < (width * height); k++) {
        LoG_Gradients[k] = 0;
    }

    int filterSize = 2;
    for (int i = filterSize; i < (height - filterSize); i++) {
        for (int j = filterSize; j < (width -filterSize); j++) {

            int LoG_gradient = 0;

            for (int u = 0; u < (filterSize * 2 + 1); u++) {
                for (int v = 0; v < (filterSize * 2 + 1); v++) {

                    /* Relative location*/
                    int relative = u * (filterSize * 2 + 1) + v;
                    /* Absolute location */
                    int absolute = (i-filterSize + u) * width + (j-filterSize + v);

                    LoG_gradient += inData[absolute] * LoGFilter[relative];

                }
            }

            LoG_Gradients[i * width + j] = LoG_gradient;
            // printf("%d ", LoG_gradient);

        }
    }

    /* Find zero crossing and apply */
    int *zeroCrossing = ZeroCrossing(LoG_Gradients, width, height);
    for (int k = 0; k < (width * height); k++) {
        outData[k] = zeroCrossing[k];
    }

    return outImage;
}

int IsDifferent(int x, int y) {
    int ret;
    if ( ((x > 0) && (y < 0)) || ((x < 0) && (y > 0)) ) {
        ret = 1;
    } else {
        ret = 0;
    }
    return ret;
}
int *ZeroCrossing(int *inData, int width, int height) {

    int *outData = (int *)malloc(sizeof(int) * width * height);

    /* Initialize */
    for (int k = 0; k < (width * height); k++) {
        outData[k] = 0;
    }

    /* Find zero crossing and apply */
    int filterSize = 1;
    for (int i = filterSize; i < (height - filterSize); i++) {
        for (int j = filterSize; j < (width - filterSize); j++) {

            int center = inData[i * width + j];
            int up = inData[(i-1) * width + j];
            int down = inData[(i+1) * width + j];
            int left = inData[i * width + j-1];
            int right = inData[i * width + j+1];
            
            if (IsDifferent(center, up)) {
                outData[i * width + j] = 255;
            } else if (IsDifferent(center, down)) {
                outData[i * width + j] = 255;
            } else if (IsDifferent(center, left)) {
                outData[i * width + j] = 255;
            } else if (IsDifferent(center, right)) {
                outData[i * width + j] = 255;
            }

        }
    }

    return outData;
}

Image *CannyAlgorithm(Image *image) {

    Image *GaussianImage = ShowGLPF(image, 60); // Gaussian Lowpass Filter
    
    Image *CannyImage = NonmaximaSuppression(GaussianImage); // Nonmaxima Suppression

    return CannyImage;
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
    double *Gxy_NMX = (double *)malloc(sizeof(double) * width * height);
    int *f_final = (int *)malloc(sizeof(int) * width * height);

    /* Initialize*/
    for (int k = 0; k < (width * height); k++) {
        Gindexs[k] = 0;
        Gup[k] = 0;
        Gdown[k] = 0;
        Gxy_NMX[k] = 0;
        f_final[k] = 0;
    }

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

            if (Gindexs[i * width + j] == 1) {
                Gup[i * width + j] = Gxy[i * width + j+1] * (1-t) + Gxy[(i-1) * width + j+1] * t;
                Gdown[i * width + j] = Gxy[i * width + j-1] * (1-t) + Gxy[(i+1) * width + j-1] * t;
            } else if (Gindexs[i * width + j] == 2) {
                Gup[i * width + j] = Gxy[(i-1) * width + j] * (1-t) + Gxy[(i-1) * width + j+1] * t;
                Gdown[i * width + j] = Gxy[(i+1) * width + j] * (1-t) + Gxy[(i+1) * width + j-1] * t;
            } else if (Gindexs[i * width + j] == 3) {
                Gup[i * width + j] = Gxy[(i-1) * width + j] * (1-t) + Gxy[(i-1) * width + j-1] * t;
                Gdown[i * width + j] = Gxy[(i+1) * width + j] * (1-t) + Gxy[(i+1) * width + j+1] * t;
            } else if (Gindexs[i * width + j] == 4) {
                Gup[i * width + j] = Gxy[i * width + j-1] * (1-t) + Gxy[(i-1) * width + j-1] * t;
                Gdown[i * width + j] = Gxy[i * width + j+1] * (1-t) + Gxy[(i+1) * width + j+1] * t;
            }

        }
    }

    /* Determine whether it is the maximum value of gradient direction */
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if ((Gxy[i * width + j] >= Gup[i * width + j]) && (Gxy[i * width + j] >= Gdown[i * width + j])) {
                Gxy_NMX[i * width + j] = Gxy[i * width + j];
            }
        }
    }

    int Tl = 15;
    int Th = 35;
    int connectNum = 1;

    for (int i = 1; i < height - 1; i++) {
        for (int j = 1; j < width - 1; j++) {
            if (Gxy_NMX[i * width + j] >= Th) {
                /* Threshold High */
                f_final[i * width + j] = 1;
            } else if (Gxy_NMX[i * width + j] <= Tl) {
                /* Threshold Low */
                f_final[i * width + j] = 0;
            } else {
                /* Weak edges */
                int count = 0;
                if (Gxy_NMX[(i-1) * width + j-1] != 0) {
                    /* lu */
                    count++;
                }
                if (Gxy_NMX[(i-1) * width + j] != 0) {
                    /* u */
                    count++;
                }
                if (Gxy_NMX[(i-1) * width + j+1] != 0) {
                    /* ru */
                    count++;
                }
                if (Gxy_NMX[i * width + j-1] != 0) {
                    /* l */
                    count++;
                }
                if (Gxy_NMX[i * width + j] != 0) {
                    /* c */
                    count++;
                }
                if (Gxy_NMX[i * width + j+1] != 0) {
                    /* r */
                    count++;
                }
                if (Gxy_NMX[(i+1) * width + j-1] != 0) {
                    /* ld */
                    count++;
                }
                if (Gxy_NMX[(i+1) * width + j] != 0) {
                    /* d */
                    count++;
                }
                if (Gxy_NMX[(i+1) * width + j+1] != 0) {
                    /* rd */
                    count++;
                }
                /* If a weak edge is not isolated, it is an edge */
                if (count >= connectNum) {
                    f_final[i * width + j] = 1;
                }
            }
        }
    }

    /* Store as an image */
    for (int k = 0; k < (width * height); k++) {
        if (f_final[k] == 1) {
            outData[k] = 255;
        } else {
            outData[k] = 0;
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
    // double *retArr = FourierSpectrum(imageArr, width, height);
    imageArr = GLPF(imageArr, width, height, d); // Pass Filter
    double *retArr = IDFT(imageArr, width, height);
    retArr = CenterTranslation(retArr, width, height);
    Image *outputImage = GenerateImage(image, retArr, width, height);
    
    return outputImage;
}
double *FourierSpectrum(Complex *imageArr, int width, int height) {
    
    double *outputArr = (double *)malloc(sizeof(double) * width * height);

    int enhance = 100;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            double real = imageArr[i * width + j].real;
            double imag = imageArr[i * width + j].imag;
            outputArr[i * width + j] = sqrt(real * real + imag * imag) * enhance;
        }
    }

    return outputArr;   
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
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
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
Complex *GLPF(Complex *imageArr, int width, int height, int d) {

    double e = 2.718281828;

    double center_x = height / double(2);
    double center_y = width / double(2);

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
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

    printf("Starting DFT ...\n");

    // DFT for cols
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            for (int m = 0; m < height; m++) {
                double theta = double(-2) * PI * (i * m) / height;
                t[i * width + j].real += imageArr[m * width + j] * cos(theta);
                t[i * width + j].imag += imageArr[m * width + j] * sin(theta);
            }
        }
    }

    // DFT for rows
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            for (int n = 0; n < width; n++) {
                double theta = double(-2) * PI * (j * n) / width;
                g[i * width + j].real += t[i * width + n].real * cos(theta) - t[i * width + n].imag * sin(theta);
                g[i * width + j].imag += t[i * width + n].real * sin(theta) + t[i * width + n].imag * cos(theta);
            }
        }
    }

    // Divide by image size
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            g[i * width + j].real /= height * width;
            g[i * width + j].imag /= height * width;
        }
    }

    printf("DFT finished\n");

    return g;
}
double *IDFT(Complex *imageArr, int width, int height) {

    Complex *t = (Complex *)malloc(sizeof(Complex) * width * height);
    double *g = (double *)malloc(sizeof(double) * width * height);

    printf("Starting IDFT ...\n");

    // IDFT for cols
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            for (int m = 0; m < height; m++) {
                double theta = double(2) * PI * (i * m) / height;
                t[i * width + j].real += imageArr[m * width + j].real * cos(theta) - imageArr[m * width + j].imag * sin(theta);
                t[i * width + j].imag += imageArr[m * width + j].real * sin(theta) + imageArr[m * width + j].imag * cos(theta);
            }
        }
    }

    // IDFT for rows
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            for (int n = 0; n < width; n++) {
                double theta = double(2) * PI * (j * n) / width;
                g[i * width + j] += t[i * width + n].real * cos(theta) - t[i * width + n].imag * sin(theta);
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

    /* Compute |Gx| and |Gy| */
    for (int k = 0; k < (width * height); k++) {
        if (Gx[k] < 0) {
            Gx[k] = -Gx[k];
        }
        if (Gy[k] < 0) {
            Gy[k] = -Gy[k];
        }
        G[k] = Gx[k] + Gy[k]; // approximate the magnitude of the gradient
    }

    /* Threshold */
    int *zeroCrossing = ZeroCrossing(G, width, height);
    for (int k = 0; k < (width * height); k++) {
        // outData[k] = zeroCrossing[k];
        outData[k] = Threshold(G[k], 50);
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