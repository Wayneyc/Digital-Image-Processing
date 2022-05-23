

#define GRAY    1
#define COLOR   2

#define PI 3.141592653589793

/*
 Define the structure "Image"
 */
typedef struct Image
{
  int Width;
  int Height;
  int Type;  // Gray=1, Color=2;
  unsigned char *data;
  char comments[15][100];
  int num_comment_lines;
} Image;

/*
 Define the structure "Complex"
 */
typedef struct {
    double real;
    double imag;
} Complex;
