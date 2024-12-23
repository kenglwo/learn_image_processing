#include "CImg.h"
using namespace cimg_library;

int main() {
	CImg<unsigned char> img("./images/night_garden.bmp");
	img.display("Hello World!");

	return 0;
}