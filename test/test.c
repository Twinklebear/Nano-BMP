#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "nano_bmp.h"

int main(int argc, char **argv){
	//Test creation 32bit bmp
	bmp_t *bmp = create_bmp(4, 4, 24);
	for (int i = 0; i < 4; ++i){
		set_pixel(bmp, 0, i, 255, 0, 0);
		set_pixel(bmp, 1, i, 0, 255, 0);
		set_pixel(bmp, 2, i, 0, 0, 255);
		set_pixel(bmp, 3, i, 255, 255, 255);
	}

	//Now make a bilinear filtered copy of the gradient
	bmp_t *filtered = create_bmp(1, 1, 24);
	uint8_t col[3] = { 0 };
	bilinear_filter(bmp, 0, 1, &col[0], &col[1], &col[2]);
	set_pixel(filtered, 0, 0, col[0], col[1], col[2]);

	write_bmp("filtered.bmp", filtered);
	write_bmp("rgb.bmp", bmp);

	destroy_bmp(filtered);
	destroy_bmp(bmp);

	if (argc == 3){
		printf("Copying bmp %s to %s as a different bpp\n", argv[1], argv[2]);
		bmp = load_bmp(argv[1]);
		bmp_t *out = NULL;
		if (bmp->info.bpp == 24){
			printf("Copying to a 32bpp format\n");
			out = convert_32bpp(bmp);
		}
		else {
			printf("Copying to a 24bpp format\n");
			out = convert_24bpp(bmp);
		}
		write_bmp(argv[2], out);
		destroy_bmp(out);
		destroy_bmp(bmp);
	}
	return 0;
}

