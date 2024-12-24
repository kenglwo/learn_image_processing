#include "CImg.h"
using namespace cimg_library;

int main() {
	CImg<unsigned char> img("./images/night_garden.bmp");
	//img.display("Hello World!");

	CImg<> lum = img.get_norm().blur(1).normalize(0, 255);
	//lum.display("Hello World!");

	CImgList<> grad = lum.get_gradient("xy");
	//grad.display("Hello World!");

	CImg<> normGrad = (grad[0].get_sqr() + grad[1].get_sqr()).sqrt();
	//normGrad.display("Hello World!");
	//normGrad.get_histogram(256).display_graph("Gradient Histogram");

	
	// compute the block decomposition
	CImgList<int> blocks;
	//CImg<int>::vector(0, 0, img.width() - 1, img.height() - 1).move_to(blocks);
	blocks.insert(CImg<int>::vector(0, 0, img.width() - 1, img.height() - 1));
	
	for (unsigned int l = 0; l < blocks.size();) {
		CImg<int>& block = blocks[l];
		int
			x0 = block[0], y0 = block[1],
			x1 = block[2], y1 = block[3];
		if (std::min(x1 - x0, y1 - y0) > 8 &&
			normGrad.get_crop(x0, y0, x1, y1).max() > 30) {
			int
				xc = (x0 + x1) / 2,
				yc = (y0 + y1) / 2;
			CImg<int>::vector(x0, y0, xc - 1, yc - 1).move_to(blocks);
			CImg<int>::vector(xc, y0, x1, yc - 1).move_to(blocks);
			CImg<int>::vector(x0, yc, xc - 1, y1).move_to(blocks);
			CImg<int>::vector(xc, yc, x1, y1).move_to(blocks);
			blocks.remove(l);
		} else ++l;
	}

	CImg<unsigned char> res(img.width(), img.height(), 1, 3, 0);

	// Render the blocks
	cimglist_for(blocks, l)
	{
		CImg<int>& block = blocks[l];
		int
			x0 = block[0], y0 = block[1],
			x1 = block[2], y1 = block[3];
		CImg<unsigned char> color = img.get_crop(x0, y0, x1, y1).resize(1, 1, 1, 3, 2);
		res.draw_rectangle(x0, y0, x1, y1, color.data(), 1);


	}

	CImg<unsigned char> mask = (res.get_shift(-1, -1, 0, 0, 0) - res).norm().cut(0, 1);
	res.mul(1 - mask);
	res.display("AAAA");


	return 0;
}