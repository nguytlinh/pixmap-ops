#include <iostream>
#include "ppm_image.h"

using namespace std;
using namespace agl;

int main(int argc, char** argv)
{
	ppm_image image;
	image.load("../images/earth-ascii.ppm");
	image.save("earth-test-save.ppm"); // should match original
	cout << "loaded earth: " << image.width() << " " << image.height() << endl;

	// invert
	ppm_image invert = image.invert();
	invert.save("earth-invert.ppm");

	// lightest
	ppm_image lightest = image.lightest(invert);
	lightest.save("earth-lightest.ppm");

	// darkest
	ppm_image darkest = image.darkest(invert);
	darkest.save("earth-darkest.ppm");

	// difference
	ppm_image diff = image.difference(invert);
	diff.save("earth-diff.ppm");

	// multiply
	ppm_image multi = image.multiply(invert);
	multi.save("earth-multiply.ppm");

	// swirl
	ppm_image swirl = image.swirl();
	invert.save("earth-swirl.ppm");

}

