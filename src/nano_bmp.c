#include <stdlib.h>
#include <stdio.h>
#include "nano_bmp.h"

bmp_t* create_bmp(unsigned w, unsigned h, unsigned bpp){
	if (bpp != 24 && bpp != 32){
		fprintf(stderr, "create_bmp error: Unsupported bits-per-pixel\n");
		return NULL;
	}
	bmp_t *bmp = malloc(sizeof(bmp_t));
	if (!bmp){
		fprintf(stderr, "create_bmp error: BMP allocation failed\n");
		return NULL;
	}
	//Compute the bytes we'll need to store the image, accounting for padding
	bmp->info.bpp = bpp;
	bmp->padding = (w * bmp->info.bpp / 8) % 4;
	bmp->info.img_size = h * (bmp->info.bpp / 8 * w + bmp->padding);
	bmp->pixels = malloc(bmp->info.img_size);
	if (!bmp->pixels){
		fprintf(stderr, "create_bmp error: Failed to allocate pixel array\n");
		destroy_bmp(bmp);
		return NULL;
	}

	//Setup the BMP File Header
	bmp->file.header[0] = 'B';
	bmp->file.header[1] = 'M';
	bmp->file.file_size = sizeof(bmp_file_header_t) + sizeof(bmp_info_header_t)
		+ bmp->info.img_size;
	bmp->file.px_array = sizeof(bmp_file_header_t) + sizeof(bmp_info_header_t);

	//Setup the BMP info header
	bmp->info.header_size = sizeof(bmp_info_header_t);
	bmp->info.w = w;
	bmp->info.h = h;
	bmp->info.color_planes = 1;
	bmp->info.compression = 0;
	bmp->info.h_res = 2835;
	bmp->info.v_res = 2835;
	bmp->info.color_palette = 0;
	bmp->info.important_colors = 0;
	return bmp;
}
void destroy_bmp(bmp_t *bmp){
	if (bmp->pixels){
		free(bmp->pixels);
	}
	free(bmp);
}
bmp_t *load_bmp(const char *f_name){
	FILE *f = fopen(f_name, "rb");
	if (!f){
		fprintf(stderr, "load_bmp error: Failed to open file %s\n", f_name);
		return NULL;
	}
	bmp_t *bmp = malloc(sizeof(bmp_t));
	if (!bmp){
		fprintf(stderr, "load_bmp error: Failed to allocate a bmp_t\n");
		fclose(f);
		return NULL;
	}
	bmp->pixels = NULL;

	if (fread(&bmp->file, sizeof(bmp_file_header_t), 1, f) != 1){
		fprintf(stderr, "load_bmp error: Failed to read BMP File Header\n");
		fclose(f);
		destroy_bmp(bmp);
		return NULL;
	}
	if (fread(&bmp->info, sizeof(bmp_info_header_t), 1, f) != 1){
		fprintf(stderr, "load_bmp error: Failed to read BMP Info Header\n");
		fclose(f);
		destroy_bmp(bmp);
		return NULL;
	}
	if (bmp->info.header_size != sizeof(bmp_info_header_t)){
		fprintf(stderr, "load_bmp error: Unsupported BMP Info Header, ");
	   	fprintf(stderr, "only the BITMAPINFOHEADER is supported\n");
		fclose(f);
		destroy_bmp(bmp);
		return NULL;
	}
	//w/h can be negative so make sure they aren't
	bmp->info.w = abs(bmp->info.w);
	bmp->info.h = abs(bmp->info.h);
	if (bmp->info.bpp != 24 && bmp->info.bpp != 32){
		fprintf(stderr, "load_bmp error: Only 24 or 32bit BMPs are supported\n");
		fclose(f);
		destroy_bmp(bmp);
		return NULL;
	}
	if (bmp->info.compression != 0){
		fprintf(stderr, "load_bmp error: Compressed BMPs are not supported\n");
		fclose(f);
		destroy_bmp(bmp);
		return NULL;
	}

	//It's possible for img_size to be 0 so if it is determine the image size
	//by examining the file
	if (bmp->info.img_size == 0){
		fseek(f, 0, SEEK_END);
		long end = ftell(f);
		bmp->info.img_size = end - bmp->file.px_array;
		fseek(f, bmp->file.px_array, SEEK_SET);
	}

	bmp->pixels = malloc(bmp->info.img_size);
	if (fread(bmp->pixels, 1, bmp->info.img_size, f) != bmp->info.img_size){
		fprintf(stderr, "load_bmp error: Failed to read pixels\n");
		fclose(f);
		destroy_bmp(bmp);
		return NULL;
	}
	bmp->padding = (bmp->info.w * bmp->info.bpp / 8) % 4;
	return bmp;
}
void write_bmp(const char *f_name, const bmp_t *bmp){
	FILE *f = fopen(f_name, "wb");
	if (!f){
		fprintf(stderr, "write_bmp error: Failed to open file %s\n", f_name);
		return;
	}
	if (fwrite(&bmp->file, sizeof(bmp_file_header_t), 1, f) != 1){
		fprintf(stderr, "write_bmp error: Failed to write BMP File Header\n");
		fclose(f);
		return;
	}
	if (fwrite(&bmp->info, sizeof(bmp_info_header_t), 1, f) != 1){
		fprintf(stderr, "write_bmp error: Failed to write BMP Info Header\n");
		fclose(f);
		return;
	}
	if (fwrite(bmp->pixels, 1, bmp->info.img_size, f) != bmp->info.img_size){
		fprintf(stderr, "write_bmp error: Failed to write pixels\n");
	}
	fclose(f);
}
int pixel_idx(const bmp_t *bmp, int x, int y){
	//Determine # of bytes per row
	size_t bpr = bmp->info.bpp / 8 * bmp->info.w + bmp->padding;
	//Invert y when returning index b/c bmp stored "upside-down"
	return x * bmp->info.bpp / 8 + (bmp->info.h - y - 1) * bpr;
}
void get_pixel(const bmp_t *bmp, int x, int y, uint8_t *r, uint8_t *g, uint8_t *b){
	int i = pixel_idx(bmp, x, y);
	*r = bmp->pixels[i + 2];
	*g = bmp->pixels[i + 1];
	*b = bmp->pixels[i];
}
void set_pixel(bmp_t *bmp, int x, int y, uint8_t r, uint8_t g, uint8_t b){
	int i = pixel_idx(bmp, x, y);
	bmp->pixels[i + 2] = r;
	bmp->pixels[i + 1] = g;
	bmp->pixels[i] = b;
}
bmp_t* convert_32bpp(bmp_t *bmp){
	bmp_t *converted = create_bmp(bmp->info.w, bmp->info.h, 32);
	if (!converted){
		fprintf(stderr, "convert_32bpp error: Failed to allocated room for conversion\n");
		return NULL;
	}
	int w = bmp->info.w, h = bmp->info.h;
	for (int i = 0; i < h; ++i){
		for (int j = 0; j < w; ++j){
			uint8_t color[3];
			get_pixel(bmp, j, i, &color[0], &color[1], &color[2]);
			set_pixel(converted, j, i, color[0], color[1], color[2]);
		}
	}
	return converted;
}
bmp_t* convert_24bpp(bmp_t *bmp){
	bmp_t *converted = create_bmp(bmp->info.w, bmp->info.h, 24);
	if (!converted){
		fprintf(stderr, "convert_24bpp error: Failed to allocate room for conversion\n");
		return NULL;
	}
	int w = bmp->info.w, h = bmp->info.h;
	for (int i = 0; i < h; ++i){
		for (int j = 0; j < w; ++j){
			uint8_t color[3];
			get_pixel(bmp, j, i, &color[0], &color[1], &color[2]);
			set_pixel(converted, j, i, color[0], color[1], color[2]);
		}
	}
	return converted;
}
/*
 * Struct to store some information about a value being blended
 */
typedef struct blend_val_t {
	int idx, x, y;
} blend_val_t;
/*
 * Wrapping function used by bilinear_interpolate when wrapping is desired
 * wraps some float around to keep it in range [0, n]
 */
float wrapf(float x, int n){
	if (x < 0){
		int offset = n * abs((int)x / n + 1);
		return x + offset;
	}
	else if (x >= n){
		int offset = n * ((int)x / n);
		return x - offset;
	}
	return x;
}
int wrapi(int x, int n){
	if (x < 0){
		int offset = n * abs(x / n + 1);
		return x + offset;
	}
	else if (x >= n){
		int offset = n * (x / n);
		return x - offset;
	}
	return x;
}

void bilinear_interpolate(const bmp_t *bmp, float x, float y,
	uint8_t *r, uint8_t *g, uint8_t *b)
{
	int w = bmp->info.w, h = bmp->info.h;
	x = x * w - 0.5f;
	y = y * h - 0.5f;
	if (x < -1 || x > w){
		x = wrapf(x, w);
	}
	if (y < -1 || y > h){
		y = wrapf(y, h);
	}
	//Apply offset to treat pixels as being centered in their location

	printf("pixel loc: (%f, %f)\n", x, y);

	//Need to pick the nearest 4 pixels not just the positive dir 2x2 block
	blend_val_t vals[4];
	for (int i = 0; i < 4; ++i){
		vals[i].x = wrapi(x + i % 2, w);
		vals[i].y = wrapi(y + i / 2, h);
		vals[i].idx = pixel_idx(bmp, vals[i].x, vals[i].y);
		printf("bval %d, pos: (%d, %d), idx: %d\n", i, vals[i].x, vals[i].y, vals[i].idx);
	}
	//Translate x,y pos into the unit square we're going to blend in
	float x_range[2], y_range[2];
	if (x < 0){
		x_range[0] = -1.f;
		x_range[1] = 0.f;
	}
	else if (x >= w){
		x_range[0] = w - 1;
		x_range[1] = w;
	}
	else {
		x_range[0] = vals[0].x;
		x_range[1] = vals[1].x;
	}
	if (y < 0){
		y_range[0] = -1.f;
		y_range[1] = 0.f;
	}
	else if (y >= h){
		y_range[0] = h - 1;
		y_range[1] = h;
	}
	else {
		y_range[0] = vals[0].y;
		y_range[1] = vals[2].y;
	}
	//Scale x,y into the unit square
	x = (x - x_range[0]) / (x_range[1] - x_range[0]);
	y = (y - y_range[0]) / (y_range[1] - y_range[0]);
	printf("Blend pos: (%f, %f)\n", x, y);

	//Blend the RGB values
	*r = bmp->pixels[vals[0].idx + 2] * (1 - x) * (1 - y) + bmp->pixels[vals[1].idx + 2] * x * (1 - y)
		+ bmp->pixels[vals[2].idx + 2] * (1 - x) * y + bmp->pixels[vals[3].idx + 2] * x * y;

	*g = bmp->pixels[vals[0].idx + 1] * (1 - x) * (1 - y) + bmp->pixels[vals[1].idx + 1] * x * (1 - y)
		+ bmp->pixels[vals[2].idx + 1] * (1 - x) * y + bmp->pixels[vals[3].idx + 1] * x * y;
	
	*b = bmp->pixels[vals[0].idx] * (1 - x) * (1 - y) + bmp->pixels[vals[1].idx] * x * (1 - y)
		+ bmp->pixels[vals[2].idx] * (1 - x) * y + bmp->pixels[vals[3].idx] * x * y;
}

