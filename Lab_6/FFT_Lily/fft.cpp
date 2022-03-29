#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <float.h>

#define M_PI 3.14159

// ���ٸ���Ҷ�任�����ݱ����� 2^n  
int NUM_x = 256, NUM_y = 256;

// ������Ҷ�任����Ľ��Ƶ����� 
void fft_shift(struct _complex *src)
{
	int x, y, a, b;
	struct _complex tmp;

	for(y = 0; y<NUM_y/2; y++){
		for(x = 0; x<NUM_x; x++){	
			a = y * NUM_x + x;
			b = ( (y + NUM_y/2)%NUM_y )*NUM_x + (NUM_x/2 + x)%NUM_x;
			
			tmp 	= src[a];
			src[a]  = src[b];
			src[b]  = tmp;
		}
	}
}

// �� {0��2��4��6�� 1��3��5��7} ����
// ���鴫��ʹ�� &f[y][x] ����ʽ���룬ȡ����ʱʹ�� f[y*NUM_x + x].x ��ʽ��ȡ
// split_array(&f[u][0], &F[u][0], NUM_x, 0, 0);	// �� x���� 
// split_array(&f[u][0], &F[u][0], 0, NUM_y, 1);	// �� y���� 
// flag: 0 ʱ����x ���� 
// flag: 1 ʱ����y ���� 
void split_array(struct _complex *src, struct _complex *dst , int x_n, int y_n, int flag)
{	
	int i;

//	struct _complex t[flag == 0 ? x_n/2 : y_n/2]; 
	struct _complex *s = src, *d = dst;
	struct _complex *t = (struct _complex*)malloc(sizeof(struct _complex)*(flag == 0 ? x_n/2 : y_n/2));
	
	if(flag == 0){ 
		if(x_n <= 1) return;
		
		for(i = 0; i<x_n/2 ; i++){
			t[i].x = s[i*2 + 1].x;		// �ݴ������� 
			t[i].y = s[i*2 + 1].y;
			
			d[i].x = s[i*2].x;			// ����ż�����λ 
			d[i].y = s[i*2].y;
		}
		for(i = 0; i<x_n/2 ; i++){
			d[i + x_n/2].x = t[i].x;	// �����������λ 
			d[i + x_n/2].y = t[i].y;
		}
		split_array(dst, 		dst, 		x_n/2, y_n, flag);
		split_array(dst+x_n/2, 	dst+x_n/2, 	x_n/2, y_n, flag);	
	} 
	else
	{
		if(y_n <= 1) return;
			
		for(i = 0; i<y_n/2 ; i++){
			t[i].x = s[(i*2 + 1)*NUM_x ].x;		// �ݴ������� 
			t[i].y = s[(i*2 + 1)*NUM_x ].y;
			
			d[i*NUM_x ].x = s[(i*2)*NUM_x ].x;			// ����ż�����λ 
			d[i*NUM_x ].y = s[(i*2)*NUM_x ].y;
		}
		for(i = 0; i<y_n/2 ; i++){
			d[(i+y_n/2)*NUM_x ].x = t[i].x;	// �����������λ 
			d[(i+y_n/2)*NUM_x ].y = t[i].y;
		}
		split_array(dst, 				dst, 				x_n, y_n/2, flag);
		split_array(dst+NUM_x*y_n/2, 	dst+NUM_x*y_n/2, 	x_n, y_n/2, flag);	
	} 
}

void printf_array(struct _complex *dst)
{
	int x,y;
	for(y = 0; y<NUM_y; y++)
	{
		for(x = 0; x<NUM_x; x++){
			printf("[%7.2lf+%7.2lfj] ", dst[y*NUM_x +x].x, dst[y*NUM_x +x].y);
		}
		printf("\n");
	}	
}
// src:Դ���� &f[y][x]  dst:�������&F[y][x]   flag: 1:���任   -1 ��任
void fft(struct _complex *src, struct _complex *dst, int flag)
{
	int y, x, i, u, k , n;
	double wu;
	struct _complex w, a0, a1, t; 
	clock_t start, end;
	start = clock();
	
	// ����Ҷ���任ʱ��������ƽ�Ƶ��ı� 
	if(flag == -1)
		fft_shift(src);
	
	/// 
	// ��xÿһ��������Ҷ�任 
	for(y = 0; y<NUM_y; y++){ 	
		// �ȷָ����� 
		split_array(&src[y*NUM_x+0], &dst[y*NUM_x+0], NUM_x, 0, 0);	// �� x���� 	
		
		// �� f[y][] ��һ�������и���Ҷ�任 
		for(i = 0; i<log(1.0*NUM_x)/log(2.0); i++){	//�������Ϊ 2^n = num����n = log2^num 
			// ÿ�μ���ļ���� 2^n,�ֱ�Ϊ 1��2��4��8 
			n = 2 * pow(2.0, i);						// ����һ�����Ϊ 2 * 2^n���ֱ�Ϊ 2,4,8,���3�� 
			for(k = 0; k<NUM_x/n; k++){				// num/n Ϊ��ǰ���������ֱ�Ϊ 4��2��1 
				for(u=0; u<n/2; u++){ 				// ��ÿ����м���, a0 �� b0 �ĸ����ֱ�Ϊ n/2 
					wu = -1 * 2 * M_PI * u / n;		// ������ת����
					w.x = cos(wu);
					w.y = flag * sin(wu);			//  ����Ǹ���Ҷ��任���˴� flag = -1 
					
					a0 = dst[y*NUM_x + k*n + u];			// ������ 	[y][k*n+u]
					a1 = dst[y*NUM_x + k*n + u + n/2];		// ż���� 	[y][k*n+u+n/2]
					
					t.x =  w.x*a1.x - w.y*a1.y;
					t.y =  w.x*a1.y + w.y*a1.x;
					
					dst[y*NUM_x + k*n + u].x =  a0.x + t.x;				// F[u] = A0 + wA1
					dst[y*NUM_x + k*n + u].y =  a0.y + t.y;
					dst[y*NUM_x + k*n + u + n/2].x =  a0.x - t.x;		// F[u+n/2] = A0 - wA1
					dst[y*NUM_x + k*n + u + n/2].y =  a0.y - t.y;
				}
			}
		}
		if(flag == 1)			// ����ʱ����һ��X���ȣ��������ݹ��󣬱�����Ӧ�ó��� 
			for(u = 0; u<NUM_x; u++){
				dst[y*NUM_x + u].x /= NUM_x;
				dst[y*NUM_x + u].y /= NUM_x;
		} 
	}
	/*
	// ��ӡ���任��� 
	end = clock();
	printf("\n\nÿ�и���Ҷ%s�任���Ϊ�� ��ʱ%fs, NUM=%d x %d\n",flag==1?"":"��", (double)(end-start)/CLOCKS_PER_SEC, NUM_x, NUM_y);
	
	printf_array(dst);
	*/
	
	
	/// 
	// ��yÿһ��������Ҷ�任 
	for(x = 0; x<NUM_x; x++){
		// �ȷָ����� 
		split_array(&dst[0*NUM_x+x], &dst[0*NUM_x+x], 0, NUM_y, 1);	// �� y���� 	
		
		// �� f[][x] ��һ�������и���Ҷ�任 
		for(i = 0; i<log(1.0*NUM_y)/log(2.0); i++){	//�������Ϊ 2^n = num����n = log2^num 
			// ÿ�μ���ļ���� 2^n,�ֱ�Ϊ 1��2��4��8 
			n = 2 * pow(2.0, i);				// ����һ�����Ϊ 2 * 2^n���ֱ�Ϊ 2,4,8,���3�� 
			for(k = 0; k<NUM_y/n; k++){		// num/n Ϊ��ǰ���������ֱ�Ϊ 4��2��1 
				for(u=0; u<n/2; u++){		// ��ÿ����м���, a0 �� b0 �ĸ����ֱ�Ϊ n/2 
					wu = -1 * 2 * M_PI * u / n;	// ������ת����
					w.x = cos(wu);
					w.y = flag * sin(wu);		//  ����Ǹ���Ҷ��任���˴� flag = -1 
					
					a0 = dst[(k*n + u)*NUM_x 		+ x ];			// ������ 	[k*n+u][x]
					a1 = dst[(k*n + u + n/2)*NUM_x 	+ x ];		// ż���� 		[(k*n + u + n/2)*NUM_y 	+ x ][x]
					
					t.x =  w.x*a1.x - w.y*a1.y;
					t.y =  w.x*a1.y + w.y*a1.x;
					
					dst[(k*n + u)*NUM_x 		+ x ].x =  a0.x + t.x;				// F[u] = A0 + wA1
					dst[(k*n + u)*NUM_x 		+ x ].y =  a0.y + t.y;
					dst[(k*n + u + n/2)*NUM_x 	+ x ].x =  a0.x - t.x;				// F[u+n/2] = A0 - wA1
					dst[(k*n + u + n/2)*NUM_x 	+ x ].y =  a0.y - t.y;
					
				}
			}
		}
		if(flag == -1)
			for(u = 0; u<NUM_y; u++){
				dst[u*NUM_x + x].x /= NUM_y;
				dst[u*NUM_x + x].y /= NUM_y;
			}
	}

	// ����Ҷ���任ʱ�����ı�ƽ�Ƶ����� 
	if(flag == 1)
		fft_shift(dst);
	
	// ��ӡ���任��� 
	end = clock(); 
	printf("\n����Ҷ%s�任��������ʱ%fs, NUM=%d x %d\n",flag==1?"":"��", (double)(end-start)/CLOCKS_PER_SEC, NUM_x, NUM_y);
 
//	printf_array(dst);
}

double* getResult(struct _complex *src, int size){
	int i;
	double* res = (double*)malloc(sizeof(double)*size);

	for(i = 0; i < size; ++i)
		res[i] = sqrt(src[i].x * src[i].x + src[i].y * src[i].y);

	return res;
}


/*
��������������������������������
��Ȩ����������ΪCSDN������CielleeX����ԭ�����£���ѭCC 4.0 BY-SA��ȨЭ�飬ת���븽��ԭ�ĳ������Ӽ���������
ԭ�����ӣ�https://blog.csdn.net/Ciellee/article/details/108520298
*/