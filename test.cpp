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
	CImg<int> coords(img.width(), img.height(), 1, 4, 0);

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

	//CImg<unsigned char> mask = (res.get_shift(-1, -1, 0, 0, 0) - res).norm().cut(0, 1);
	//res.mul(1 - mask);
	CImg<unsigned char>
		mask(res.width(), res.height(), 1, 1, 1),
		V(3, 3);

	cimg_forC(res, c)
		cimg_for3x3(res, x, y, 0, c, V, unsigned char)
		if (V[4] != V[5] || V[4] != V[7])
			mask(x, y) = 0;
	res.mul(mask);

	// display
	CImgDisplay disp(res, "CImg display", 0);
	unsigned char white[] = { 255, 255, 255 }, black[] = { 0, 0, 0 };

	while (!disp.is_closed() && !disp.is_keyESC()) {
		int
			x = disp.mouse_x(),
			y = disp.mouse_y();
		if (x >= 0 && y >= 0) {
			int
				x0 = coords(x, y, 0), y0 = coords(x, y, 1),
				x1 = coords(x, y, 2), y1 = coords(x, y, 3),
				xc = (x0 + x1) / 2, yc = (y0 + y1) / 2;

			CImg<unsigned char>
				pImg = img.get_crop(x0, y0, x1, y1).resize(128, 128, 1, 3, 1),
				pGrad = normGrad.get_crop(x0, y0, x1, y1).resize(128, 128, 1, 3, 1).
				normalize(0, 255).
				map(CImg<unsigned char>::hot_LUT256());

			(+res).
				draw_text(10, 3, "X, Y + %d, %d", white, 0, 1, 24, x, y).
				draw_rectangle(x0, y0, x1, y1, black, 0.25f).
				draw_line(74, 109, xc, yc, white, 0.75, 0xCCCCCCCC).
				draw_line(74, 264, xc, yc, white, 0.75, 0xCCCCCCCC).
				draw_rectangle(7, 32, 140, 165, white).
				draw_rectangle(7, 197, 140, 330, white).
				draw_image(10, 35, pImg).
				draw_image(10, 200, pGrad).
				display(disp);

		}
		
		disp.wait();
		if (disp.is_resized()) disp.resize(disp);
	}


	return 0;
}