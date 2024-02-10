#include <iostream>

int main() {

    int image_width = 256;
    int image_height = 256;

    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
    for(int j = 0; j < image_height; ++j) {
	std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
	for(int i = 0; i < image_width; ++i) {
	    auto r = double(i) / (image_width-1);
	    auto g = double(j) / (image_height-1);
	    auto b = 0;

	    int ir = static_cast<int>(255.999 * r);
	    int ig = static_cast<int>(255.999 * g);
	    int ib = static_cast<int>(255.999 * b);

	    std::cout << ir << ' ' << ig << ' ' << ib << '\n';
	}
    }

    std::clog << "\rDone.                 \n" << std::flush;

    return 0;
}
