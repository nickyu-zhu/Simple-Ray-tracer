#include <FreeImage.h>
#include "film.h"

void Film::draw()
{
	for (int i = 0; i < 3 * w * h; i ++)
	{
		pixels[i] = 0;
	}

	for (int i = 1; i < 3 * w * h; i += 3)
		pixels[i] = 255;
	for (int i = 2; i < 3 * w * h; i += 3)
		pixels[i] = 255;
	FreeImage_Initialise();
	FIBITMAP* img = FreeImage_ConvertFromRawBits(pixels, w, h, w * 3, 24, 0xFF0000, 0x00FF00, 0x0000FF, true);

	FreeImage_Save(FIF_PNG, img, "generated_image.png", 0);
	FreeImage_DeInitialise();
}
