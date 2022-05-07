//
//  scaling.cpp
//  DIP_Space
//
//  Created by Wenyuanchun on 2022/2/25.
//

#include <stdio.h>

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
#include "proto.h"

Image *ReadPNMImage(char *filename);
Image *CreateNewImage(int type, int width, int height, char *comment);
void SavePNMImage(Image *, char *);
// new functions
Image *PixelReplication(Image *image, double rate);
Image *NearestNeighborInterpolation(Image *image, double rate);
Image *BilinearInterpolation(Image *image, double rate);
Image *NegativeImageOperation(Image *image);

int main(int argc, const char * argv[]) {
    printf("Hello, DIP!\n");

    Image *imageLena, *imageNoise, *newImage1, *newImage2, *newImage3, *newImage4, *newImage5, *newImage6;
    // Raw image
    char lena[] = ".\\lena.pgm";
    // char noise[] = "/Users/wenyuanchun/Desktop/DIP/DIP_Space/DIP_Space/Resource/images/noise.pgm";
    imageLena = ReadPNMImage(lena);
    // imageNoise = ReadPNMImage(noise);
    
    // char PR1[] = "/Users/wenyuanchun/Desktop/DIP/DIP_Space/Scaling/Scaling/PR1.pgm";
    char PR2[] = ".\\PR2.pgm";
    // char PR3[] = "/Users/wenyuanchun/Desktop/DIP/DIP_Space/Scaling/Scaling/PR3.pgm";
    // char PR4[] = "/Users/wenyuanchun/Desktop/DIP/DIP_Space/Scaling/Scaling/PR4.pgm";
    // char PR5[] = "/Users/wenyuanchun/Desktop/DIP/DIP_Space/Scaling/Scaling/PR5.pgm";
    // char PR6[] = "/Users/wenyuanchun/Desktop/DIP/DIP_Space/Scaling/Scaling/PR6.pgm";
    
    // Pixel Replication (If need other method, please change function name)
    // newImage1 = BilinearInterpolation(imageLena, 1.3); // Input resize rate HERE
    // newImage2 = BilinearInterpolation(imageLena, 0.25);
    // newImage3 = BilinearInterpolation(imageNoise, 1.3); // Input resize rate HERE
    // newImage4 = BilinearInterpolation(imageNoise, 0.7);
    // newImage5 = NegativeImageOperation(imageLena); // Input resize rate HERE
    // newImage6 = NegativeImageOperation(imageNoise);
    
//    SavePNMImage(newImage1, PR1);
    SavePNMImage(imageLena, PR2);
//    SavePNMImage(newImage3, PR3);
//    SavePNMImage(newImage4, PR4);
//    SavePNMImage(newImage5, PR5);
//    SavePNMImage(newImage6, PR6);
    
    return 0;
}

/*
 Negative image operation
 */
Image *NegativeImageOperation(Image *image) {
    // New image
    char comment[] = "#Negative image operation";
    Image *newImage = CreateNewImage(image->Type, image->Width, image->Height, comment);
    
    unsigned char *tempin, *tempout;
    tempin = image->data;
    tempout = newImage->data;
    
    for (int i = 0; i < image->Height; i++) {
        for (int j = 0; j < image->Width; j++) {
            *tempout = 255 - *tempin;
            tempin++;
            tempout++;
        }
    }
    
    return newImage;
}

/*
 Bilinear Interpolation
 rate -> Any double which larger than 0
 */
Image *BilinearInterpolation(Image *image, double rate) {
    
    // Check the validity of rate
    if (rate <= 0) {
        printf("Error: Inlegal rate!\n");
        exit(0);
    }
    
    // New image
    int width = int(ceil(image->Width * rate));
    int height = int(ceil(image->Height * rate));
    char comment[] = "#Bilinear Interpolation";
    Image *newImage = CreateNewImage(image->Type, width, height, comment);
    
    unsigned char *tempin, *tempout;
    tempin = image->data;
    tempout = newImage->data;
    
    // Extract data of old image
    int prev_width = image->Width;
    int prev_height = image->Height;
    int rawMatrix[1][1];
    for (int i = 0; i < prev_height; i++) {
        for (int j = 0; j < prev_width; j++) {
            rawMatrix[i][j] = *tempin;
            tempin++;
        }
    }
    
    // Do resizing
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            double x = i / rate;
            double y = j / rate;
            int A = rawMatrix[int(x)][int(y)];
            int B = rawMatrix[int(floor(x))][int(y)];
            int C = rawMatrix[int(x)][int(floor(y))];
            int D = rawMatrix[int(floor(x))][int(floor(y))];
            double p = x - int(x);
            double q = y - int(y);
            int pixelValue = int(round((1-p)*(1-q)*A + p*(1-q)*B + (1-p)*q*C + p*q*D));
            *tempout = pixelValue;
            tempout++;
        }
    }
    
    return newImage;
}

/*
 Nearest Neighbor Interpolation
 rate -> Any double which larger than 0
 */
Image *NearestNeighborInterpolation(Image *image, double rate) {
    
    // Check the validity of rate
    if (rate <= 0) {
        printf("Error: Inlegal rate!\n");
        exit(0);
    }
    
    // New image
    int width = int(ceil(image->Width * rate));
    int height = int(ceil(image->Height * rate));
    char comment[] = "#Nearest Neighbor Interpolation";
    Image *newImage = CreateNewImage(image->Type, width, height, comment);
    
    unsigned char *tempin, *tempout;
    tempin = image->data;
    tempout = newImage->data;
    
    // Extract data of old image
    int prev_width = image->Width;
    int prev_height = image->Height;
    int rawMatrix[1][1];
    for (int i = 0; i < prev_height; i++) {
        for (int j = 0; j < prev_width; j++) {
            rawMatrix[i][j] = *tempin;
            tempin++;
        }
    }
    
    // Do resizing
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            int u = int(round(i / rate));
            int v = int(round(j / rate));
            *tempout = rawMatrix[u][v]; // Load into new image
            tempout++;
        }
    }
    
    return newImage;
}

/*
 Pixel Replication & Alternative Line Reduction
 rate -> positive integer or shaped like 1/x
 */
Image *PixelReplication(Image *image, double rate) {
    // Previous image's info
    int prev_width = image->Width;
    int prev_height = image->Height;
    
    // Check the validity of rate
    if (rate > 1) { // enlargement
        if (rate - int(rate) != 0) {
            printf("Error: Rixel Relication only supports the integer operation!\n");
            exit(0);
        }
    } else if ((rate > 0) && (rate <= 1)) { // reduction
        if ((1 / rate) - int(1 / rate) != 0) { // Only accept rate which is shaped like 1/x
            printf("Error: Alternative Line Reduction only supports the integer operation!\n");
            exit(0);
        }
    } else { // negative rate
        printf("Error: Inlegal rate!\n");
        exit(0);
    }
    
    // Calculate new width and height
    int width = prev_width * rate;
    int height = prev_height * rate;
    
    // New image
    char comment[] = "#Pixel Replication";
    Image *newImage = CreateNewImage(image->Type, width, height, comment);
    
    // Extract data of old image
    unsigned char *tempin, *tempout;
    tempin = image->data;
    tempout = newImage->data;
    
    // Use matrix to store
    int rawMatrix[1][1];
    for (int i = 0; i < prev_height; i++) {
        for (int j = 0; j < prev_width; j++) {
            rawMatrix[i][j] = *tempin;
            tempin++;
        }
    }
    
    // Do resizing
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            int u = int(i / rate);
            int v = int(j / rate);
            *tempout = rawMatrix[u][v]; // Load into new image
            tempout++;
        }
    }
    
    return newImage;
}


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

    outimage->data = (unsigned char *)malloc(size);
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
