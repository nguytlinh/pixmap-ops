#include "ppm_image.h"
#include <string>
#include <fstream>

using namespace agl;
using namespace std;

ppm_image::ppm_image() 
{
    set_width(0);
    set_height(0);
    version = "P3";

    create_data();
}

ppm_image::ppm_image(int weight, int height) 
{
    set_width(weight);
    set_height(height);
    version = "P3";

    create_data();
}

ppm_image::ppm_image(const ppm_image& orig)
{
    this->set_width(orig.width());
    this->set_height(orig.height());
    version = "P3";

    data = new ppm_pixel * [orig.height()];

    for (int i = 0; i < orig.height(); i++)
    {
        data[i] = new ppm_pixel[orig.width()];
        for (int j = 0; j < orig.width(); j++)
        {
            data[i][j].r = orig.data[i][j].r;
            data[i][j].g = orig.data[i][j].g;
            data[i][j].b = orig.data[i][j].b;
        }
    }
}


ppm_image::~ppm_image()
{
    for (int i = 0; i < height(); i++)
    {
        delete data[i];
    }
    delete data;
}

// load the given filename
// returns true if the load is successful; false otherwise
bool ppm_image::load(const std::string& filename)
{
    ifstream input(filename, ios::binary);
    if (input.is_open())
    {
        char ver[3];

        input.read(ver, 2);
        version = ver;
        input >> wid;
        input >> hei;
        input.read(ver, 1);

        create_data();

        int value = 0;
        for (int i = 0; i < hei; i++)
        {
            for (int j = 0; j < wid; j++)
            {
                input >> value;
                data[i][j].r = value;
                input >> value;
                data[i][j].g = value;
                input >> value;
                data[i][j].b = value;

            }
        }
        input.close();
        return true;
    }
    input.close();
    return false;
}

ppm_image ppm_image::read(const std::string& filename)
{
    ppm_image result;
    if (load) 
    {
        ifstream input(filename, ios::binary);
        char ver[3];

        input.read(ver, 2);
        result.version = ver;
        input >> result.wid;
        input >> result.hei;
        input.read(ver, 1);

        result.create_data();

        int value = 0;
        for (int i = 0; i < hei; i++)
        {
            for (int j = 0; j < wid; j++)
            {
                input >> value;
                result.data[i][j].r = value;
                input >> value;
                result.data[i][j].g = value;
                input >> value;
                result.data[i][j].b = value;
               
            }
        }
        input.close();
        return result;
    }
}

// save the given filename
// returns true if the save is successful; false otherwise
bool ppm_image::save(const std::string& filename) const
{
    ofstream output(filename, ios::binary);

    if (output.is_open())
    {
        output << version << endl;
        output << wid << endl;
        output << hei << endl;
        output << 255 << endl;

        for (int i = 0; i < hei; i++)
        {
            for (int j = 0; j < wid; j++)
            {
                output << (int)data[i][j].r << " ";
                output << (int)data[i][j].g << " ";
                output << (int)data[i][j].b << " ";
            }
        }
        output.close();
        return true;
    }
    output.close();
    return false;
}

// Returns a copy of this image resized to the given width and height
 ppm_image ppm_image::resize(int w, int h) const
{   
     ppm_image result = ppm_image(*this);

     result.data = new ppm_pixel * [h];
     
     for (int i = 0; i < h; i++) 
     {
         result.data[i] = new ppm_pixel[w];
         for (int j = 0; j < w; j++) 
         {
             result.data[i][j].r = 255;
             result.data[i][j].g = 255;
             result.data[i][j].b = 255;
         }
     }

     for (int i = 0; i < h; i++) 
     {
         for (int j = 0; j < w; j++)
         {
             result.data[i][j] = data[i * this->height() / h][j * this->width() / w];
         }
     }
     return result;
}

 // Return a copy of this image flipped around the horizontal midline
ppm_image ppm_image::flip_horizontal() const
{
    ppm_image result = ppm_image(*this);

    for (int i = 0; i < result.height()/2; i++)
    {
        for (int j = 0; j < result.width() ; j++)
        {
            swap(result.data[i][j], result.data[result.height() - 1 - i][j]);
        }
    }
    return result;
}

// Return a sub-image having the given top,left coordinate and (width, height)
ppm_image ppm_image::subimage(int startx, int starty, int w, int h) const
{
    ppm_image result;

    int countInner=0, countOuter=0;
    for (int i = 0; i < h; i++)
    {   
        result.data = new ppm_pixel * [h];
        if (i == countOuter + 1) starty++;
        for (int j = 0; j < w; j++)
        {
            result.data[i] = new ppm_pixel[w];
            if (j == countInner + 1) startx++;
            result.data[i][j].r = this->data[startx][starty].r;
            result.data[i][j].g = this->data[startx][starty].g;
            result.data[i][j].b = this->data[startx][starty].b;
            countInner = j;
        }
        countOuter = i;
    }
    return result;
}

// Replace the portion starting at (row, col) with the given image
// Clamps the image if it doesn't fit on this image
void ppm_image::replace(const ppm_image& image, int startx, int starty)
{

    int countInner = 0, countOuter = 0;
    for (int i = 0; i < image.height(); i++)
    {
        if (countOuter == i - 1) starty++;
        for (int j = 0; j < image.width(); j++)
        {
            if (countInner == j - 1) startx++;
            this->data[startx][starty].r = image.data[i][j].r;
            this->data[startx][starty].g = image.data[i][j].g;
            this->data[startx][starty].b = image.data[i][j].b;
            countInner = j;
        }
        countOuter = i;
    }

}

// Apply the following calculation to the pixels in 
// our image and the given image:
//    this.pixels = this.pixels * (1-alpha) + other.pixel * alpha
// Can assume that the two images are the same size
ppm_image ppm_image::alpha_blend(const ppm_image& other, float alpha) const
{
    ppm_image result = ppm_image(*this);

    for (int i = 0; i < result.height(); i++)
    {
        for (int j = 0; j < result.width(); j++)
        {
            result.data[i][j].r = result.data[i][j].r * (1 - alpha) + other.data[i][j].r * alpha;
            result.data[i][j].g = result.data[i][j].g * (1 - alpha) + other.data[i][j].g * alpha;
            result.data[i][j].b = result.data[i][j].b * (1 - alpha) + other.data[i][j].b * alpha;
        }
    }
    return result;
}

// Return a copy of this image with the given gamma correction applied to it
ppm_image ppm_image::gammaCorrect(float gamma) const
{
    ppm_image result = ppm_image(*this);

    for (int i = 0; i < result.height(); i++)
    {
        for (int j = 0; j < result.width(); j++)
        {
            result.data[i][j].r = pow((result.data[i][j].r / 255.0),(1.0 / gamma)) * 255.0f;
            result.data[i][j].g = pow((result.data[i][j].g / 255.0), (1.0 / gamma)) * 255.0f;
            result.data[i][j].b = pow((result.data[i][j].b / 255.0), (1.0 / gamma)) * 255.0f;
        }
    }
    return result;
}

// Return a copy of this image converted to grayscale
ppm_image ppm_image::grayscale() const
{
    ppm_image result = ppm_image(*this);
    const float red = 0.3f;
    const float green = 0.6f;
    const float blue = 0.1f;
    float gsVal;

    for (int i = 0; i < result.height() / 2; i++)
    {
        for (int j = 0; j < result.width(); j++)
        {
            gsVal = result.data[i][j].r * red + result.data[i][j].g * green + result.data[i][j].b * blue;
            result.data[i][j].r = gsVal;
            result.data[i][j].g = gsVal;
            result.data[i][j].b = gsVal;
        }
    }
    return result;
}

// Get the pixel at index (row, col)
ppm_pixel ppm_image::get(int row, int col) const
{
   int r = 0, g = 0, b = 0;
   r = data[row][col].r;
   g = data[row][col].g;
   b = data[row][col].b;
   return ppm_pixel{ r,g,b };
}

// Set the pixel at index (row, col)
void ppm_image::set(int row, int col, const ppm_pixel& c)
{
    this->data[row][col].r = c.r;
    this->data[row][col].g = c.g;
    this->data[row][col].b = c.b;
}

// return the height of this image
int ppm_image::height() const
{
   return hei;
}

// return the width of the image
int ppm_image::width() const
{
   return wid;
}

void ppm_image::set_width(int width)
{
    this->wid = width;
}

void ppm_image::set_height(int height)
{
    this->hei = height;
}


void ppm_image::create_data()
{
    this->data = new ppm_pixel * [this->height()];

    for (int i = 0; i < this->height(); i++)
    {
        this->data[i] = new ppm_pixel[this->width()];
        for (int j = 0; j < this->width(); j++)
        {
            this->data[i][j].r = 255;
            this->data[i][j].g = 255;
            this->data[i][j].b = 255;
        }
    }
}

// Return a copy of the color inverted image
ppm_image ppm_image::invert() const 
{
    ppm_image result = ppm_image(*this);

    for (int i = 0; i < result.height(); i++)
    {
        for (int j = 0; j < result.width(); j++)
        {
            result.data[i][j].r = 255 - result.data[i][j].r;
            result.data[i][j].g = 255 - result.data[i][j].g;
            result.data[i][j].b = 255 - result.data[i][j].b;
        }
    }
    return result;
}
