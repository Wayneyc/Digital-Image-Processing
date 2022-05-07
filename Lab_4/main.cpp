//
//  main.cpp
//  Lab_4
//
//  Created by Wenyuanchun on 2022/3/16.
//

// for mac
#include <sys/malloc.h>
// for windows
//#include <malloc.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <algorithm>
#include <cmath>
#include <iostream>
#include "proto.h"


/*
 Basic Operation
 */
Image *ReadPNMImage(char *filename);
Image *CreateNewImage(int type, int width, int height, char *comment);
void SavePNMImage(Image *, char *);
/*
 Q1: Laplacian and Sobel operators
 */
Image *LaplacianOperator(Image *image);
Image *SobelOperator(Image *image, int type);
/*
 Q2: Gamma correction
 */
Image *GammaCorrection(Image *image, double gamma);
/*
 Q3: Histogram enhancement
 */
Image *GlobalHistogramEnhancement(Image *image);
Image *LocalHistogramEnhancement(Image *image, int maskSize);


/*
 Main function
 */
int main(int argc, const char * argv[]) {
    
    Image *image, *imageLena, *imageBridge, *image1, *image2, *image3, *image4, *image5, *image6, *image7, *image8, *image9;
    
    char lena[] = "/Users/wenyuanchun/Desktop/DIP/DIP_Space/DIP_Space/Resource/images/lena.pgm";
    char bridge[] = "/Users/wenyuanchun/Desktop/DIP/DIP_Space/DIP_Space/Resource/images/bridge.pgm";
    imageLena = ReadPNMImage(lena);
    imageBridge = ReadPNMImage(bridge);
    
    // Switch here
    image = imageBridge;
    
    // Q1
    image1 = LaplacianOperator(image);
    image2 = SobelOperator(image, 0); // horizontal
    image3 = SobelOperator(image, 1); // vertical
    // Q2
    image4 = GammaCorrection(image, 0.1);
    image5 = GammaCorrection(image, 0.4);
    image6 = GammaCorrection(image, 0.7);
    image7 = GammaCorrection(image, 1);
    // Q3
    image8 = GlobalHistogramEnhancement(image);
    image9 = LocalHistogramEnhancement(image, 1);
    
    char saveImage1[] = "/Users/wenyuanchun/Desktop/DIP/DIP_Space/Lab_4/Lab_4/lena1.pgm";
    char saveImage2[] = "/Users/wenyuanchun/Desktop/DIP/DIP_Space/Lab_4/Lab_4/lena2.pgm";
    char saveImage3[] = "/Users/wenyuanchun/Desktop/DIP/DIP_Space/Lab_4/Lab_4/lena3.pgm";
    char saveImage4[] = "/Users/wenyuanchun/Desktop/DIP/DIP_Space/Lab_4/Lab_4/lena4.pgm";
    char saveImage5[] = "/Users/wenyuanchun/Desktop/DIP/DIP_Space/Lab_4/Lab_4/lena5.pgm";
    char saveImage6[] = "/Users/wenyuanchun/Desktop/DIP/DIP_Space/Lab_4/Lab_4/lena6.pgm";
    char saveImage7[] = "/Users/wenyuanchun/Desktop/DIP/DIP_Space/Lab_4/Lab_4/lena7.pgm";
    char saveImage8[] = "/Users/wenyuanchun/Desktop/DIP/DIP_Space/Lab_4/Lab_4/lena8.pgm";
    char saveImage9[] = "/Users/wenyuanchun/Desktop/DIP/DIP_Space/Lab_4/Lab_4/lena9.pgm";
    
    SavePNMImage(image1, saveImage1);
    SavePNMImage(image2, saveImage2);
    SavePNMImage(image3, saveImage3);
    SavePNMImage(image4, saveImage4);
    SavePNMImage(image5, saveImage5);
    SavePNMImage(image6, saveImage6);
    SavePNMImage(image7, saveImage7);
    SavePNMImage(image8, saveImage8);
    SavePNMImage(image9, saveImage9);
     
    
    
    return 0;
}

/*
 maskSize:
 1 -> 3 x 3
 2 -> 5 x 5
 ...
 k -> 2k+1 x 2k+1
 */
Image *LocalHistogramEnhancement(Image *image, int maskSize) {
    
    // Store image data
    unsigned char *imageData, *newImageData;
    imageData = image->data;
    int width = image->Width;
    int height = image->Height;
    
    // New image
    char comment[] = "# Local Histogram Enhancement";
    Image *newImage = CreateNewImage(image->Type, width, height, comment);
    newImageData = newImage->data;
    
    // Global Loop
    for (int i = maskSize; i < height-maskSize; i++) {
        for (int j = maskSize; j < width-maskSize; j++) {
            
            // Histogram in the masked area
            int maskWidth = 2 * maskSize + 1;
            int maskArea = maskWidth * maskWidth;
            int intensityList[255]; // Generate intensity list
            for (int k = 0; k < 255; k++) {
                intensityList[k] = 0; // initialization
            }
            
            // Count the number of each intensity in the masked area
            for (int p = i-1; p <= i+1; p++) {
                for (int q = j-1; q <= j+1; q++) {
                    int k = p * width + q;
                    intensityList[imageData[k]]++; // count + 1
                }
            }
            
            // Histogram equalization
            int n = i * width + j;
            int k = imageData[n];
            double sum = 0;
            for (int q = 0; q <= k; q++) {
                sum += ((intensityList[q]*1.0) / maskArea);
            }
            newImageData[n] = int(sum * 255);
        }
    }
    
    return newImage;
}

Image *GlobalHistogramEnhancement(Image *image) {
    
    // Store image data
    unsigned char *imageData, *newImageData;
    imageData = image->data;
    int width = image->Width;
    int height = image->Height;
    
    // New image
    char comment[] = "# Global Histogram Enhancement";
    Image *newImage = CreateNewImage(image->Type, width, height, comment);
    newImageData = newImage->data;
    
    // Store the number of pixel in each intensity
    int intensityList[256];
    for (int i = 0; i < 256; i++) {
        intensityList[i] = 0; // initialization
    }
    for (int i = 0; i < newImage->Width; i++) {
        for (int j = 0; j < newImage->Height; j++) {
            int k = i * width + j;
            intensityList[imageData[k]]++; // count + 1
        }
    }
    
    // Histogram equalization
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            int n = i * width + j;
            int k = imageData[n];
            double sum = 0;
            for (int q = 0; q <= k; q++) {
                sum += ((intensityList[q]*1.0) / (width*height));
            }
            newImageData[n] = int(sum * 255);
        }
    }
    
    return newImage;
}

Image *GammaCorrection(Image *image, double gamma) {
    
    // Store image data
    unsigned char *imageData, *newImageData;
    imageData = image->data;
    int width = image->Width;
    int height = image->Height;
    
    // New image
    char comment[] = "# Gamma correction";
    Image *newImage = CreateNewImage(image->Type, width, height, comment);
    newImageData = newImage->data;
    
    // Gamma correction
    int sum = 0;
    for (int i = 0; i < newImage->Width; i++) {
        for (int j = 0; j < newImage->Height; j++) {
            int r = imageData[i * width + j];
            int s = int(round(pow(r*1.0/255, gamma)*255)); // (3.2-3)
            // Don't out of bounds
            if (s > 255) {
                newImageData[i * width + j] = 255;
            } else if (s < 0) {
                newImageData[i * width + j] = 0;
            } else {
                newImageData[i * width + j] = s;
            }
            sum += newImageData[i * width + j];
        }
    }
    
    // Compute variance
    double average = sum*1.0 / (width*height);
    double variance = 0;
    for (int i = 0; i < newImage->Width; i++) {
        for (int j = 0; j < newImage->Height; j++) {
            variance += pow((newImageData[i * width + j] - average), 2);
        }
    }
    printf("Gamma: %f, variance: %f\n", gamma, variance);
    
    return newImage;
}

/*
 mask type:
 0 -> horizontal
 1 -> vertical
 */
Image *SobelOperator(Image *image, int type) {
    
    // Store image data
    unsigned char *imageData, *newImageData;
    imageData = image->data;
    
    // New image
    char comment[] = "# Sobel operation";
    Image *newImage = CreateNewImage(image->Type, image->Width, image->Height, comment);
    newImageData = newImage->data;
    for (int i = 0; i < newImage->Width; i++) {
        for (int j = 0; j < newImage->Height; j++) {
            newImageData[i * (newImage->Width) + j] = imageData[i * (image->Width) + j];
        }
    }
    int size = 1;
    for (int i = size; i < (newImage->Width)-size; i++) {
        for (int j = size; j < (newImage->Height)-size; j++) {
            int width = newImage->Width;
            
            // Sobel horizontal and vertical operators
            int sobel;
            if (type == 0) {
                // horizontal
                sobel = imageData[(i+1) * width + j - 1] + imageData[(i+1) * width + j + 1] + 2 * imageData[(i+1) * width + j] - imageData[(i-1) * width + j - 1] - imageData[(i-1) * width + j + 1] - 2 * imageData[(i-1) * width + j];
            } else {
                // vertical
                sobel = imageData[(i+1) * width + j + 1] + imageData[(i-1) * width + j + 1] + 2 * imageData[i * width + j - 1] - imageData[(i-1) * width + j - 1] - imageData[(i+1) * width + j - 1] - 2 * imageData[i * width + j];
            }
            
            // Don't out of bounds
            if (imageData[i*(newImage->Width) + j] + sobel > 255) {
                newImageData[i*(newImage->Width) + j] = 255; // no more than 255
            } else if (imageData[i*(newImage->Width) + j] + sobel < 0) {
                newImageData[i*(newImage->Width) + j] = 0; // no less than 0
            } else {
                newImageData[i*(newImage->Width) + j] = imageData[i*(newImage->Width) + j] + sobel;
            }
        }
    }
    
    return newImage;
}

Image *LaplacianOperator(Image *image) {
    
    // Store image data
    unsigned char *imageData, *newImageData;
    imageData = image->data;
    
    // New image
    char comment[] = "# Laplacian operation";
    Image *newImage = CreateNewImage(image->Type, image->Width, image->Height, comment);
    newImageData = newImage->data;
    for (int i = 0; i < newImage->Width; i++) {
        for (int j = 0; j < newImage->Height; j++) {
            newImageData[i * (newImage->Width) + j] = imageData[i * (image->Width) + j];
        }
    }
    int size = 1;
    for (int i = size; i < (newImage->Width)-size; i++) {
        for (int j = size; j < (newImage->Height)-size; j++) {
            int width = newImage->Width;
            
            int laplacian = imageData[i * width + j + 1] + imageData[i * width + j - 1] + imageData[(i + 1) * width + j] + imageData[(i - 1) * width + j] - (4 * imageData[i * width + j]); // (3.6-6)
            
            // Don't out of bounds
            if (imageData[i*(newImage->Width) + j] + laplacian > 255) {
                newImageData[i*(newImage->Width) + j] = 255; // no more than 255
            } else if (imageData[i*(newImage->Width) + j] + laplacian < 0) {
                newImageData[i*(newImage->Width) + j] = 0; // no less than 0
            } else {
                newImageData[i*(newImage->Width) + j] = imageData[i*(newImage->Width) + j] + laplacian; // (3.6-7)
            }
        }
    }
    
    return newImage;
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
