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
Complex *ModifiedHPF(Complex *imageArr, int width, int height, int d, double c, double H, double L);
Complex *IBRF(Complex *imageArr, int width, int height, int d, int w); // Ideal Bandreject Selective Filter
Complex *RectangleFilter(Complex *imageArr, int width, int height, int direction, int w, int interval); // Custom Rectangle Filter
double *MedianFilter(double *imageArr, int width, int height, int size); // Median Filter
double *ArithmeticMeanFilter(double *imageArr, int width, int height, int size); // Arithmetic Mean Filter
double *GeometricMeanFilter(double *imageArr, int width, int height, int size); // Geometric Mean Filter
double *AlphaTrimmedMeanFilter(double *imageArr, int width, int height, int size, int d); // Alpha Trimmed Mean Filter
double *AdaptiveMedianFilter(double *imageArr, int width, int height, int max_size); // Adaptive Median Filter
/*
 Support Method
 */
double *AddSinusoidalNoise(double *imageArr, int width, int height, double range, double strength);
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

Image *ShowHomomorphic(Image *image, int d, double c, double H, double L) {

    double e = 2.718281828;

    int width = image->Width;
    int height = image->Height;
    
    double *imageData = Image2Double(image, width, height);

    // Step 1 --- ln()
    for (int i = 0; i < (width * height); i++) {
        imageData[i] = log(imageData[i] + 1) / log(e);
    }
    // Step 2 --- Centralization & DFT
    imageData = CenterTranslation(imageData, width, height);
    Complex *complexArr = DFT(imageData, width, height);
    // Step 3 --- Filtering
    complexArr = ModifiedHPF(complexArr, width, height, d, c, H, L);
    // Step 4 --- IDFT & Uncentralization
    imageData = IDFT(complexArr, width, height);
    imageData = CenterTranslation(imageData, width, height);
    // Step 5 --- exp()
    for (int i = 0; i < (width * height); i++) {
        imageData[i] = exp(imageData[i]) - 1;
    }

    Image *outputImage = GenerateImage(imageData, width, height);
    return outputImage;
}

Image *ShowSinNoise(Image *image, double range, double strength) {

    int width = image->Width;
    int height = image->Height;
    
    double *imageData = Image2Double(image, width, height);

    imageData = AddSinusoidalNoise(imageData, width, height, range, strength);

    Image *outputImage = GenerateImage(imageData, width, height);

    return outputImage;
}

Image *ShowSpectrum(Image *image) {

    int width = image->Width;
    int height = image->Height;

    double *imageData = Image2Double(image, width, height);

    imageData = CenterTranslation(imageData, width, height);

    Complex *complexArr = DFT(imageData, width, height);

    imageData = FourierSpectrum(complexArr, width, height);
    
    Image *outputImage = GenerateImage(imageData, width, height);

    return outputImage;
}

Image *ShowBandreject(Image *image, int d, int w) {

    int width = image->Width;
    int height = image->Height;

    double *imageData = Image2Double(image, width, height);

    imageData = CenterTranslation(imageData, width, height);

    Complex *complexArr = DFT(imageData, width, height);

    complexArr = IBRF(complexArr, width, height, d, w);

    imageData = IDFT(complexArr, width, height);

    imageData = CenterTranslation(imageData, width, height);
    
    Image *outputImage = GenerateImage(imageData, width, height);

    return outputImage;
}

Image *ShowRectangleFilter(Image *image, int direction, int w, int interval) {

    int width = image->Width;
    int height = image->Height;

    double *imageData = Image2Double(image, width, height);

    imageData = CenterTranslation(imageData, width, height);

    Complex *complexArr = DFT(imageData, width, height);

    complexArr = RectangleFilter(complexArr, width, height, direction, w, interval);

    imageData = IDFT(complexArr, width, height);

    imageData = CenterTranslation(imageData, width, height);
    
    Image *outputImage = GenerateImage(imageData, width, height);

    return outputImage;
}

Image *ShowMedianFilter(Image *image, int size) {

    int width = image->Width;
    int height = image->Height;

    double *imageData = Image2Double(image, width, height);

    imageData = MedianFilter(imageData, width, height, size);
    
    Image *outputImage = GenerateImage(imageData, width, height);

    return outputImage;
}

Image *ShowArithmeticMeanFilter(Image *image, int size) {

    int width = image->Width;
    int height = image->Height;

    double *imageData = Image2Double(image, width, height);

    imageData = ArithmeticMeanFilter(imageData, width, height, size);
    
    Image *outputImage = GenerateImage(imageData, width, height);

    return outputImage;
}

Image *ShowGeometricMeanFilter(Image *image, int size) {

    int width = image->Width;
    int height = image->Height;

    double *imageData = Image2Double(image, width, height);

    imageData = GeometricMeanFilter(imageData, width, height, size);
    
    Image *outputImage = GenerateImage(imageData, width, height);

    return outputImage;
}

Image *ShowAlphaTrimmedMeanFilter(Image *image, int size, int d) {

    int width = image->Width;
    int height = image->Height;

    double *imageData = Image2Double(image, width, height);

    imageData = AlphaTrimmedMeanFilter(imageData, width, height, size, d);
    
    Image *outputImage = GenerateImage(imageData, width, height);

    return outputImage;
}

Image *ShowAdaptiveMedianFilter(Image *image, int max_size) {

    int width = image->Width;
    int height = image->Height;

    double *imageData = Image2Double(image, width, height);

    imageData = AdaptiveMedianFilter(imageData, width, height, max_size);
    
    Image *outputImage = GenerateImage(imageData, width, height);

    return outputImage;
}

int main() {

    printf("Hello Lab7\n");

    Image *image, *imageResult, *lena_noise, *lena_result, *lena_rectangle, *lena_rectangle_specturm;
    // char bridge[] = "/Users/wenyuanchun/Desktop/DIP/Digital-Image-Processing/images/bridge.pgm";
    // char goldhill[] = "/Users/wenyuanchun/Desktop/DIP/Digital-Image-Processing/images/goldhill.pgm";
    // char noise[] = "/Users/wenyuanchun/Desktop/DIP/Digital-Image-Processing/Lab_7/SinNoise.pgm";
    // char lena_noise_path[] = "/Users/wenyuanchun/Desktop/DIP/Digital-Image-Processing/images/LenaWithNoise.pgm";
    // char camera_noise_path[] = "/Users/wenyuanchun/Desktop/DIP/Digital-Image-Processing/images/cameraWithNoise.pgm";
    // char SinNoise[] = "/Users/wenyuanchun/Desktop/DIP/Digital-Image-Processing/Lab_7/Q2/SinNoise.pgm";
    // char SinNoise_path[] = "/Users/wenyuanchun/Desktop/DIP/Digital-Image-Processing/Lab_7/Q2/Spectrum.pgm";
    char lena[] = ".\\lena.pgm";

    image = ReadPNMImage(lena);

    imageResult = ShowSpectrum(image);

    char imagePath[] = ".\\spectrum.pgm";
    SavePNMImage(imageResult, imagePath);

    /*
     Question 1 ->
     */
    // char homomorphic_path[] = "/Users/wenyuanchun/Desktop/DIP/Digital-Image-Processing/Lab_7/homomorphic.pgm";
    // imageResult = ShowHomomorphic(image, 10, 1, 1.2, 0.8);
    // SavePNMImage(imageResult, homomorphic_path);

    /*
     Question 2 ->
     */
    // ShowSinNoise(image, 5, 10);
    // imageResult = ShowBandreject(image, 50, 5);
    // imageResult = ShowSpectrum(image);
    // SavePNMImage(imageResult, SinNoise_path);

    /* 
     Question 3 ->
     */
    // lena_noise = ReadPNMImage(lena_noise_path);

    // char lena_spectrum_path[] = "/Users/wenyuanchun/Desktop/DIP/Digital-Image-Processing/Lab_7/Lena_Noise_Spectrum.pgm";
    // lena_result = ShowSpectrum(lena_noise);
    // SavePNMImage(lena_result, lena_spectrum_path);

    // char lena_rec_path[] = "/Users/wenyuanchun/Desktop/DIP/Digital-Image-Processing/Lab_7/Lena_Rec.pgm";
    // lena_rectangle = ShowRectangleFilter(lena_noise, 1, 5, 20);
    // SavePNMImage(lena_rectangle, lena_rec_path);

    // char lena_rec_result_path[] = "/Users/wenyuanchun/Desktop/DIP/Digital-Image-Processing/Lab_7/Lena_Rec_Spectrum.pgm";
    // lena_rectangle_specturm = ShowSpectrum(lena_rectangle);
    // SavePNMImage(lena_rectangle_specturm, lena_rec_result_path);

    // Image *camera_noise, *camera_result;

    // camera_noise = ReadPNMImage(camera_noise_path);

    // char camera_result_path[] = "/Users/wenyuanchun/Desktop/DIP/Digital-Image-Processing/Lab_7/Camera_result.pgm";
    // camera_result = ShowMedianFilter(camera_noise, 1);
    // SavePNMImage(camera_result, camera_result_path);

    /*
     Question 4 ->
     */
    // Image *arithmeticImage, *geometricImage, *medianImage, *AlphaTrimmedImage, *adaptiveMedianImage;
    // char D1[] = "/Users/wenyuanchun/Desktop/DIP/Digital-Image-Processing/images/lenaD1.pgm";
    // char D2[] = "/Users/wenyuanchun/Desktop/DIP/Digital-Image-Processing/images/lenaD2.pgm";
    // char D3[] = "/Users/wenyuanchun/Desktop/DIP/Digital-Image-Processing/images/lenaD3.pgm";
    
    // image = ReadPNMImage(D2);

    // arithmeticImage = ShowArithmeticMeanFilter(image, 1);
    // geometricImage = ShowGeometricMeanFilter(image, 1);
    // medianImage = ShowMedianFilter(image, 1);
    // AlphaTrimmedImage = ShowAlphaTrimmedMeanFilter(image, 1, 2);
    // adaptiveMedianImage = ShowAdaptiveMedianFilter(image, 5);

    // char arithmetic_path[] = "/Users/wenyuanchun/Desktop/DIP/Digital-Image-Processing/Lab_7/arithmetic_mean.pgm";
    // char geometric_path[] = "/Users/wenyuanchun/Desktop/DIP/Digital-Image-Processing/Lab_7/geometric_mean.pgm";
    // char median_path[] = "/Users/wenyuanchun/Desktop/DIP/Digital-Image-Processing/Lab_7/median.pgm";
    // char Alpha_trimmed_path[] = "/Users/wenyuanchun/Desktop/DIP/Digital-Image-Processing/Lab_7/Alpha-trimmed_mean.pgm";
    // char adaptive_median_path[] = "/Users/wenyuanchun/Desktop/DIP/Digital-Image-Processing/Lab_7/adaptive_median.pgm";

    // SavePNMImage(arithmeticImage, arithmetic_path);
    // SavePNMImage(geometricImage, geometric_path);
    // SavePNMImage(medianImage, median_path);
    // SavePNMImage(AlphaTrimmedImage, Alpha_trimmed_path);
    // SavePNMImage(adaptiveMedianImage, adaptive_median_path);

    return 0;
}

double AdaptiveMedianOperator(double *imageArr, int width, int height, int x, int y, int size, int max_size) {

    int asize = size * 2 + 1;

    double *medArr = (double *)malloc(sizeof(double) * asize * asize);
    for (int u = 0; u < asize; u++) {
        for (int v = 0; v < asize; v++) {
            medArr[u * asize + v] = imageArr[(x-size+u) * width + (y-size+v)];
        }
    }
    sort(medArr, medArr + (asize * asize));

    double z_med = medArr[asize * asize / 2];
    double z_min = medArr[0];
    double z_max = medArr[asize * asize - 1];

    // Stage A
    double A1 = z_med - z_min;
    double A2 = z_med - z_max;

    if ((A1 > 0) && (A2 < 0)) {
        // Stage B
        double B1 = imageArr[x * width + y] - z_med;
        double B2 = imageArr[x * width + y] - z_max;
        if ((B1 > 0) && (B2 < 0)) {
            return imageArr[x * width + y];
        } else {
            return z_med;
        }
    } else {
        int new_size = size + 1;
        if (new_size <= max_size) {
            // Repeat Stage A
            return AdaptiveMedianOperator(imageArr, width, height, x, y, new_size, max_size);
        } else {
            return z_med;
        }
    }
}
double *AdaptiveMedianFilter(double *imageArr, int width, int height, int max_size) {

    int amax = max_size * 2 + 1;

    // Temporary Arr
    double *tempArr = (double *)malloc(sizeof(double) * width * height);

    for (int i = amax; i < (width - amax); i++) {
        for (int j = amax; j < (height - amax); j++) {
            tempArr[i * width + j] = AdaptiveMedianOperator(imageArr, width, height, i, j, 1, max_size);
        }
    }

    return tempArr;
}

double *AlphaTrimmedMeanFilter(double *imageArr, int width, int height, int size, int d) {

    int asize = size * 2 + 1;

    if (d >= (asize * asize)) {
        printf("AlphaTrimmedMeanFilter ~ d ERROR!\n");
        exit(0);
    }

    // Temporary Arr
    double *tempArr = (double *)malloc(sizeof(double) * width * height);

    for (int i = size; i < (width - size); i++) {
        for (int j = size; j < (height - size); j++) {
            int *med = (int *)malloc(sizeof(int) * asize * asize);
            for (int u = 0; u < asize; u++) {
                for (int v = 0; v < asize; v++) {
                    med[u * asize + v] = int(round(imageArr[(i-size+u) * width + (j-size+v)]));
                }
            }
            sort(med, med + (asize * asize));
            int start = int(ceil(double(d/2)));
            int end = asize * asize - int(floor(double(d/2)));
            int sum = 0;
            for (int k = start; k < end; k++) {
                sum += med[k];
            }
            tempArr[i * width + j] = sum / (asize * asize - d);
        }
    }

    return tempArr;
}

double *GeometricMeanFilter(double *imageArr, int width, int height, int size) {

    int asize = size * 2 + 1;

    // Temporary Arr
    double *tempArr = (double *)malloc(sizeof(double) * width * height);

    for (int i = size; i < (width - size); i++) {
        for (int j = size; j < (height - size); j++) {
            double product = 1;
            for (int u = 0; u < asize; u++) {
                for (int v = 0; v < asize; v++) {
                    product = product * imageArr[(i-size+u) * width + (j-size+v)];
                }
            }
            tempArr[i * width + j] = pow(product, 1.0 / (asize * asize));
        }
    }

    return tempArr;
}

double *ArithmeticMeanFilter(double *imageArr, int width, int height, int size) {

    int asize = size * 2 + 1;

    // Temporary Arr
    double *tempArr = (double *)malloc(sizeof(double) * width * height);

    for (int i = size; i < (width - size); i++) {
        for (int j = size; j < (height - size); j++) {
            int mean = 0;
            for (int u = 0; u < asize; u++) {
                for (int v = 0; v < asize; v++) {
                    mean += int(round(imageArr[(i-size+u) * width + (j-size+v)]));
                }
            }
            tempArr[i * width + j] = mean / (asize * asize);
        }
    }

    return tempArr;
}

double *MedianFilter(double *imageArr, int width, int height, int size) {

    int asize = size * 2 + 1;

    // Temporary Arr
    double *tempArr = (double *)malloc(sizeof(double) * width * height);

    for (int i = size; i < (width - size); i++) {
        for (int j = size; j < (height - size); j++) {
            int *med = (int *)malloc(sizeof(int) * asize * asize);
            for (int u = 0; u < asize; u++) {
                for (int v = 0; v < asize; v++) {
                    med[u * asize + v] = int(round(imageArr[(i-size+u) * width + (j-size+v)]));
                }
            }
            sort(med, med + asize * asize);
            tempArr[i * width + j] = med[asize * asize / 2];
        }
    }

    return tempArr;
}

Complex *RectangleFilter(Complex *imageArr, int width, int height, int direction, int w, int interval) {

    double center_x = width / double(2);
    double center_y = height / double(2);

    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            double D = sqrt(pow(center_x-i, 2) + pow(center_y-j, 2));
            int H = 1;
            if (direction == 0) { // Horizontal
                if (((center_x - w / 2) <= i) && (i <= (center_x + w / 2)) && ( ((center_y - interval / 2) > j) \
                 || ((center_y + interval / 2) < j) )) {
                    H = 0;
                }
            } else { // Vertical
                if (((center_x - w / 2) <= j) && (j <= (center_x + w / 2)) && ( ((center_y - interval / 2) > i) \
                 || ((center_y + interval / 2) < i) )) {
                    H = 0;
                }
            }
            
            imageArr[i * width + j].real *= H;
            imageArr[i * width + j].imag *= H;
        }
    }

    return imageArr;
}

Complex *IBRF(Complex *imageArr, int width, int height, int d, int w) {

    double center_x = width / double(2);
    double center_y = height / double(2);

    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            double D = sqrt(pow(center_x-i, 2) + pow(center_y-j, 2));
            int H = 1;
            if (((d - w / 2) <= D) && ((d + w / 2) >= D)) {
                H = 0;
            }
            imageArr[i * width + j].real *= H;
            imageArr[i * width + j].imag *= H;
        }
    }

    return imageArr;
}

double *AddSinusoidalNoise(double *imageArr, int width, int height, double range, double strength) {

    double normalization = 2 * PI / range;

    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            double sinNoise_x = sin(i * normalization) * strength;
            double sinNoise_y = sin(j * normalization) * strength;
            imageArr[i * width + j] += sinNoise_x + sinNoise_y;
        }
    }

    return imageArr;
}



Complex *ModifiedHPF(Complex *imageArr, int width, int height, int d, double c, double H, double L) {

    double e = 2.718281828;

    double center_x = width / double(2);
    double center_y = height / double(2);

    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            double D = sqrt(pow(center_x-i, 2) + pow(center_y-j, 2));
            double K = (H - L) * (1 - (pow(e, (-c * pow(D, 2) / pow(d, 2))))) + L;
            imageArr[i * width + j].real *= K;
            imageArr[i * width + j].imag *= K;
        }
    }

    return imageArr;
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