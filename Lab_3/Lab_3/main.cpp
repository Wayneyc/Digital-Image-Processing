//
//  main.cpp
//  Lab_3
//
//  Created by Wenyuanchun on 2022/3/8.
//

#define SIZE 3
#define PI 3.1415926
#define ANGLE 0.017453292 // PI / 180

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
 Matrix Operation
 */
double** MatrixMul(double **x, double **y);
double* AffineMulCoordinate(double **T, int x, int y);

/*
 Image Operation
 */
Image *Translation(Image *image, int x, int y);
Image *Rotation(Image *image, int degree);
Image *ShearVertical(Image *image, double vertical);
Image *ShearHorizontal(Image *image, double horizontal);

bool CheckMargin(int index, int width, int height, int margin);
Image *AverageOperator(Image *image, int windowSize);
Image *MedianOperator(Image *image, int windowSize);


double **ArrayToPointer(double arr[SIZE][SIZE]) {
    double **ret;
    ret = (double **)malloc(sizeof(double *) * SIZE);
    for (int i = 0; i < SIZE; i++) {
        ret[i] = (double *)malloc(sizeof(double) * SIZE);
        for (int j = 0; j < SIZE; j++) {
            ret[i][j] = arr[i][j];
        }
    }
    return ret;
}

int main(int argc, const char * argv[]) {
    
    Image *imageLena, *image1, *image2, *image3, *image4, *image5, *image6;
    
    char lena[] = "/Users/wenyuanchun/Desktop/DIP/DIP_Space/DIP_Space/Resource/images/lena.pgm";
    
    imageLena = ReadPNMImage(lena);
    
    char outfile1[] = "/Users/wenyuanchun/Desktop/DIP/DIP_Space/Lab_3/Lab_3/lena1.pgm";
    char outfile2[] = "/Users/wenyuanchun/Desktop/DIP/DIP_Space/Lab_3/Lab_3/lena7.pgm";
    char outfile3[] = "/Users/wenyuanchun/Desktop/DIP/DIP_Space/Lab_3/Lab_3/lena3.pgm";
    char outfile4[] = "/Users/wenyuanchun/Desktop/DIP/DIP_Space/Lab_3/Lab_3/lena4.pgm";
    char outfile5[] = "/Users/wenyuanchun/Desktop/DIP/DIP_Space/Lab_3/Lab_3/lena5.pgm";
    char outfile6[] = "/Users/wenyuanchun/Desktop/DIP/DIP_Space/Lab_3/Lab_3/lena6.pgm";

    image1 = Translation(imageLena, -40, -20); // x, y
    image2 = Rotation(imageLena, -30); // degree - clockwise
    image3 = ShearVertical(imageLena, 0.7); // vertical
    image4 = ShearHorizontal(imageLena, 0.7); // horizontal
    image5 = AverageOperator(imageLena, 3); // window size 3x3
    image6 = MedianOperator(imageLena, 5); // window size 5x5
    
    SavePNMImage(image1, outfile1);
    SavePNMImage(image2, outfile2);
    SavePNMImage(image3, outfile3);
    SavePNMImage(image4, outfile4);
    SavePNMImage(image5, outfile5);
    SavePNMImage(image6, outfile6);
    
    return 0;
}

/*
 Shear Operation
 */
Image *ShearHorizontal(Image *image, double horizontal) {
    
    double center_x = image->Width*1.0 / 2;
    double center_y = image->Height*1.0 / 2;
    
    // Shear Matrix
    double shear[SIZE][SIZE] = {
        {1, 0, 0},
        {-horizontal, 1, 0},
        {0, 0, 1}
    };
    // Translation Matrix - to center and back
    double translation_1[SIZE][SIZE] = {
        {1, 0, -center_x},
        {0, 1, -center_y},
        {0, 0, 1}
    };
    double translation_2[SIZE][SIZE] = {
        {1, 0, center_x},
        {0, 1, center_y},
        {0, 0, 1}
    };
    
    // transMat_2 X shearMat X transMat_1
    double **combinationMatrix;
    combinationMatrix = MatrixMul(ArrayToPointer(translation_2), MatrixMul(ArrayToPointer(shear), ArrayToPointer(translation_1)));
    
    // New image
    char comment[] = "#Shear operation";
    int newWidth = image->Width + int(abs(ceil(image->Height)*horizontal));
    int newHeight = image->Height;
    Image *newImage = CreateNewImage(image->Type, newWidth, newHeight, comment);
    
    int offset_y = int((newWidth - image->Width)*1.0 / 2);
    int offset_x = int((newHeight - image->Height)*1.0 / 2);
    
    unsigned char *tempin, *tempout;
    tempin = image->data;
    tempout = newImage->data;
    
    // Initialize the background
    for (int k = 0; k < newImage->Width * newImage->Height; k++) {
        tempout[k] = 0;
    }
    
    for (int i = 0; i < image->Height; i++) {
        for (int j = 0; j < image->Width; j++) {
            // new position of each pixel
            double *newPosition = AffineMulCoordinate(combinationMatrix, i, j);
            int nx = int(round(newPosition[0])) + offset_x;
            int ny = int(round(newPosition[1])) + offset_y;
            // mapping
            if ((nx >= 0) && (nx < newImage->Height) && (ny >= 0) && (ny < newImage->Width)) {
                tempout[nx * newImage->Width + ny] = tempin[i * image->Width + j];
            }
        }
    }
    
    return newImage;
}

Image *ShearVertical(Image *image, double vertical) {
    
    double center_x = image->Width*1.0 / 2;
    double center_y = image->Height*1.0 / 2;
    
    // Shear Matrix
    double shear[SIZE][SIZE] = {
        {1, vertical, 0},
        {0, 1, 0},
        {0, 0, 1}
    };
    // Translation Matrix - to center and back
    double translation_1[SIZE][SIZE] = {
        {1, 0, -center_x},
        {0, 1, -center_y},
        {0, 0, 1}
    };
    double translation_2[SIZE][SIZE] = {
        {1, 0, center_x},
        {0, 1, center_y},
        {0, 0, 1}
    };
    
    // transMat_2 X shearMat X transMat_1
    double **combinationMatrix;
    combinationMatrix = MatrixMul(ArrayToPointer(translation_2), MatrixMul(ArrayToPointer(shear), ArrayToPointer(translation_1)));
    
    // New image
    char comment[] = "#Shear operation";
    int newWidth = image->Width;
    int newHeight = image->Height + int(abs(ceil(image->Width)*vertical));
    Image *newImage = CreateNewImage(image->Type, newWidth, newHeight, comment);
    
    int offset_y = int((newWidth - image->Width)*1.0 / 2);
    int offset_x = int((newHeight - image->Height)*1.0 / 2);
    
    unsigned char *tempin, *tempout;
    tempin = image->data;
    tempout = newImage->data;
    
    // Initialize the background
    for (int k = 0; k < newImage->Width * newImage->Height; k++) {
        tempout[k] = 0;
    }
    
    for (int i = 0; i < image->Height; i++) {
        for (int j = 0; j < image->Width; j++) {
            // new position of each pixel
            double *newPosition = AffineMulCoordinate(combinationMatrix, i, j);
            int nx = int(round(newPosition[0])) + offset_x;
            int ny = int(round(newPosition[1])) + offset_y;
            // mapping
            if ((nx >= 0) && (nx < newImage->Height) && (ny >= 0) && (ny < newImage->Width)) {
                tempout[nx * newImage->Width + ny] = tempin[i * image->Width + j];
            }
        }
    }
    
    return newImage;
}

/*
 Rotate Operation
 */
Image *Rotation(Image *image, int degree) {
    
    double center_x = image->Width*1.0 / 2;
    double center_y = image->Height*1.0 / 2;
    
    // Rotation Matrix - clockwise
    double rotation[SIZE][SIZE] = {
        {cos(degree * ANGLE), sin(degree * ANGLE), 0},
        {-sin(degree * ANGLE), cos(degree * ANGLE), 0},
        {0, 0, 1}
    };
    // Translation Matrix - to center and back
    double translation_1[SIZE][SIZE] = {
        {1, 0, -center_x},
        {0, 1, -center_y},
        {0, 0, 1}
    };
    double translation_2[SIZE][SIZE] = {
        {1, 0, center_x},
        {0, 1, center_y},
        {0, 0, 1}
    };
    
    // transMat_2 X rotateMat X transMat_1
    double **combinationMatrix;
    combinationMatrix = MatrixMul(ArrayToPointer(translation_2), MatrixMul(ArrayToPointer(rotation), ArrayToPointer(translation_1)));
    
    // A larger canvas
    int diagonal = int(ceil(sqrt(pow(image->Width, 2) + pow(image->Height, 2))));
    int newWidth = diagonal;
    int newHeight = diagonal;
//    int newWidth = abs(int(floor(diagonal * cos((45 - degree) * ANGLE))));
//    int newHeight = abs(int(floor(diagonal * sin((45 + degree) * ANGLE))));
    int offset_x = int((newWidth - image->Width)*1.0 / 2);
    int offset_y = int((newHeight - image->Height)*1.0 / 2);
    
    // New image
    char comment[] = "#Rotate operation";
    Image *newImage = CreateNewImage(image->Type, diagonal, diagonal, comment);
    
    unsigned char *tempin, *tempout;
    tempin = image->data;
    tempout = newImage->data;
    
    // Initialize the background
    for (int k = 0; k < newImage->Width * newImage->Height; k++) {
        tempout[k] = 0;
    }
    
    // Move the raw image to new space
    for (int i = 0; i < image->Height; i++) {
        for (int j = 0; j < image->Width; j++) {
            // new position of each pixel
            double *newPosition = AffineMulCoordinate(combinationMatrix, i, j);
            int nx = int(round(newPosition[0]));
            int ny = int(round(newPosition[1]));
            nx += offset_x;
            ny += offset_y;
            // mapping
            if ((nx >= 0) && (nx < newImage->Width) && (ny >= 0) && (ny < newImage->Height)) {
                tempout[nx * newImage->Width + ny] = tempin[i * image->Width + j];
            }
        }
    }
    
    return newImage;
}

/*
 Translate Operation
 */
Image *Translation(Image *image, int x, int y) {
    
    // Translation Matrix
    double translation[SIZE][SIZE] = {
        {1, 0, x*1.0},
        {0, 1, y*1.0},
        {0, 0, 1}
    };
    
    // New image
    char comment[] = "#Translation operation";
    Image *newImage = CreateNewImage(image->Type, image->Width, image->Height, comment);
    
    unsigned char *tempin, *tempout;
    tempin = image->data;
    tempout = newImage->data;
    
    // Initialize the background
    for (int k = 0; k < newImage->Width * newImage->Height; k++) {
        tempout[k] = 0;
    }
    
    for (int i = 0; i < image->Height; i++) {
        for (int j = 0; j < image->Width; j++) {
            // new position of each pixel
            double *newPosition = AffineMulCoordinate(ArrayToPointer(translation), i, j);
            int nx = int(round(newPosition[0]));
            int ny = int(round(newPosition[1]));
            // mapping
            if ((nx >= 0) && (nx < image->Width) && (ny >= 0) && (ny < image->Height)) {
                tempout[nx * image->Width + ny] = tempin[i * image->Width + j];
            }
        }
    }
    
    return newImage;
}

/*
 Matrix X Vector (xy coordinate)
 */
double* AffineMulCoordinate(double **T, int x, int y) {
    double* XY = NULL;
    XY = (double *)malloc(sizeof(double) * SIZE);
    double vector[] = {x*1.0, y*1.0, 1};
    for (int i = 0; i < SIZE; i++) {
        double sum = 0;
        for (int k = 0; k < SIZE; k++) {
            sum += T[i][k] * vector[k];
        }
        XY[i] = sum;
    }
    return XY;
}

/*
 Matrix X Matrix
 */
double** MatrixMul(double **x, double **y) {
    // Create space for return matrix
    double ** ret = NULL;
    ret = (double **)malloc(sizeof(double *) * SIZE);
    for (int i = 0; i < SIZE; i++) {
        ret[i] = (double *)malloc(sizeof(double) * SIZE);
    }
    // Matrix multiplication
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            double sum = 0.0;
            for (int k = 0; k < SIZE; k++) {
                sum += x[i][k] * y[k][j];
            }
            ret[i][j] = sum;
        }
    }
    return ret;
}

/* --------------- Smooth --------------- */

/*
 To ignorn all around the margins.
 index: the index of each pixel
 width: width of image
 height: height of image
 margin: size of filter (eg. 3x3 -> margin = 1; 5x5 -> margin = 2)
 */
bool CheckMargin(int index, int width, int height, int margin) {
    
    /* filter area is too large */
    if (((margin*2 + 1) > width) || ((margin*2 + 1) > height)) {
        printf("Error: check image or filter again!\n");
        exit(0);
    }
    
    int size = width * height; // total size
    margin = margin - 1; // for computing
    bool ret = true; // Is the margin or not
    
    /* Check top & bottom margin */
    if ((((index % width) >= 0) && ((index % width) <= margin)) || (((index % width) >= (width - 1 - margin)) && ((index % width) <= (width - 1)))) {
        ret = false;
    }
    /* Check left & right margin */
    if (((index >= 0) && (index <= (width * (margin + 1) - 1))) || ((index <= size - 1) && (index >= (size - (width * (margin + 1)))))) {
        ret = false;
    }
    /*
     true: not the margin
     false: is the margin
     */
    return ret;
}

/*
 For average filter
 */
Image *AverageOperator(Image *image, int windowSize) {
    /* Grab image info */
    int width = image->Width;
    int height = image->Height;
    
    unsigned char *tempin, *tempout;
    int size;
    Image *outimage;
    
    /* Create a host image */
    char comment[] = "#Average-Filter"; // '#' is necessary!
    outimage = CreateNewImage(image->Type, image->Width, image->Height, comment);
    tempin = image->data;
    tempout = outimage->data;
    /*
     Ignore margin (1 pixel)
     */
    int margin = (windowSize-1) / 2;
    size = width * height;
    for (int i = 0; i < size; i++) {
        if (CheckMargin(i, width, height, margin)) {
            int sum = 0; // sum of intensity of the pixel in filter area
            for (int j = -margin; j <= margin; j++) { // each line in filter area
                for (int k = -margin; k <= margin; k++) { // each item in the line
                    sum = sum + *(tempin + (j * width) + k);
                }
            }
            sum /= (margin * 2 + 1) * (margin * 2 + 1); // as the average
            *tempout = sum; // Update
        } else {
            *tempout = *tempin;
        }
        tempin++;
        tempout++;
    }
    return outimage;
}

/*
 For median filter
 */
Image *MedianOperator(Image *image, int windowSize) {
    
    /* Grab image info */
    int width = image->Width;
    int height = image->Height;
    
    unsigned char *tempin, *tempout;
    int size;
    Image *outimage;
    
    /* Create a host image */
    char comment[] = "#Median-Filter"; // '#' is necessary!
    outimage = CreateNewImage(image->Type, image->Width, image->Height, comment);
    tempin = image->data;
    tempout = outimage->data;
    
    /*
     Ignore margin (1 pixel)
     */
    int margin = (windowSize-1) / 2;
    size = width * height;
    for (int i = 0; i < size; i++) {
        if (CheckMargin(i, width, height, margin)) {
            int arr[(margin * 2 + 1) * (margin * 2 + 1)]; // to store the pixel in filter area
            int count = 0; // as the index in arr
            for (int j = -margin; j <= margin; j++) { // each line
                for (int k = -margin; k <= margin; k++) { // each item in the line
                    arr[count] = *(tempin + (j * width) + k); // add to array
                    count++;
                }
            }
            std::sort(arr, arr + (margin * 2 + 1) * (margin * 2 + 1)); // sort the pixel
            *tempout = arr[((margin * 2 + 1) * (margin * 2 + 1) / 2)]; // Update
        } else {
            *tempout = *tempin;
        }
        tempin++;
        tempout++;
    }
    
    return outimage;
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
