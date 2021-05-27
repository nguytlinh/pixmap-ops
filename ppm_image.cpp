#include "ppm_image.h"
#include <string>
#include <fstream>
#include <sstream>
#include <math.h>
#include <iostream>

#define VERSION "P3";
#define MAX_VALUE 255;

using namespace agl;
using namespace std;

ppm_image::ppm_image() 
{
    wid = 0; hei = 0;
}

ppm_image::ppm_image(int width, int height) 
{
    wid = width;
    hei = height;
    data = new ppm_pixel * [hei];

    for (int i = 0; i < hei; i++) {
        data[i] = new ppm_pixel[wid];
    }
}

ppm_image::ppm_image(const ppm_image& orig)
{
    wid = orig.wid;
    hei = orig.hei;
    data = new ppm_pixel * [hei];
    for (int i = 0; i < hei; i++) {
        data[i] = new ppm_pixel[wid];
        for (int j = 0; j < wid; j++) {
            data[i][j] = orig.data[i][j];
        }
    }
}

void ppm_image::clear() {
    for (int i = 0; i < hei; i++) {
        delete[] data[i];
    }
    delete[] data;
}

ppm_image::~ppm_image()
{
    clear();
}

// load the given filename
// returns true if the load is successful; false otherwise
bool ppm_image::load(const std::string& filename)
{
    clear();
    ifstream input(filename);
    if (input)
    {
        string version, max;
        input >> version >> wid >> hei >> max;

        data = new ppm_pixel * [hei];
        for (int i = 0; i < hei; i++) {
            data[i] = new ppm_pixel[wid];
        }

        int red, green, blue;
        for (int i = 0; i < hei; i++)
        {
            for (int j = 0; j < wid; j++)
            {
                input >> red >> green >> blue;
                data[i][j].r = (unsigned char) red;
                data[i][j].g = (unsigned char) green;
                data[i][j].b = (unsigned char) blue;
            }
        }
        input.close();
        return true;
    }
    input.close();
    return false;
}


// save the given filename
// returns true if the save is successful; false otherwise
bool ppm_image::save(const std::string& filename) const
{
    ofstream output(filename);

    if (output)
    {
        string version;
        int max=255; 

        output << version << endl;
        output << wid << " ";
        output << hei << endl;
        output << max << endl;

        for (int i = 0; i < hei; i++)
        {
            for (int j = 0; j < wid; j++)
            {
                output << (int)data[i][j].r << " ";
                output << (int)data[i][j].g << " ";
                output << (int)data[i][j].b << " ";
                output << std::endl;
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
     ppm_image result = ppm_image(w, h);
     int oldHei, oldWid;
     
     for (int i = 0; i < h; i++) 
     {
         for (int j = 0; j < w; j++) 
         {
             oldWid = floor((((double)j)/(w - 1)) * (wid - 1));
             oldHei = floor((((double)i) /(h - 1)) * (hei - 1));
             result.data[i][j] = data[oldHei][oldWid];
         }
     }
     return result;
}

 // Return a copy of this image flipped around the horizontal midline
ppm_image ppm_image::flip_horizontal() const
{
    ppm_image result = ppm_image(wid, hei);

    for (int i = 0; i < hei; i++)
    {
        for (int j = 0; j < wid ; j++)
        {
            result.data[i][j] = data[hei - 1 - i][j];
            result.data[i][j] = data[i][j];
        }
    }
    return result;
}

// Return a sub-image having the given top,left coordinate and (width, height)
ppm_image ppm_image::subimage(int startx, int starty, int w, int h) const
{
    int x = 0, y = 0;
    ppm_image result = ppm_image(wid, hei);
    for (int i = startx; i < (startx+h); i++)
    {   
        for (int j = starty; j < (starty+w); j++)
        {
            result.data[x][y] = data[i][j];
            y++;
        }
        y = 0; 
        x++;
    }
    return result;
}

// Replace the portion starting at (row, col) with the given image
// Clamps the image if it doesn't fit on this image
void ppm_image::replace(const ppm_image& image, int startx, int starty)
{
    for (int i = startx; i < (startx +image.hei); i++)
    {
        for (int j = starty; j < (starty + image.wid); j++)
        {
            data[i][j] = image.data[i - startx][j - starty];
        }
    }

}

// Apply the following calculation to the pixels in 
// our image and the given image:
//    this.pixels = this.pixels * (1-alpha) + other.pixel * alpha
// Can assume that the two images are the same size
ppm_image ppm_image::alpha_blend(const ppm_image& other, float alpha) const
{
    unsigned char red, green, blue;
    ppm_image result(wid, hei);

    for (int i = 0; i < hei; i++)
    {
        for (int j = 0; j < wid; j++)
        {
            red = (float) data[i][j].r * (1 - alpha) + (float) other.data[i][j].r * alpha;
            green = (float) data[i][j].g * (1 - alpha) + (float) other.data[i][j].g * alpha;
            blue = (float) data[i][j].b * (1 - alpha) + (float) other.data[i][j].b * alpha;
            result.data[i][j] = ppm_pixel{ red,green,blue };
        }
    }
    return result;
}

// Return a copy of this image with the given gamma correction applied to it
ppm_image ppm_image::gammaCorrect(float gamma) const
{
    ppm_image result = ppm_image(wid, hei);

    for (int i = 0; i < hei; i++)
    {
        for (int j = 0; j < wid; j++)
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
    ppm_image result = ppm_image(wid, hei);
    const float red = 0.3f;
    const float green = 0.6f;
    const float blue = 0.1f;
    float gsVal;

    for (int i = 0; i < hei; i++)
    {
        for (int j = 0; j < wid; j++)
        {
            gsVal = data[i][j].r * red + data[i][j].g * green + data[i][j].b * blue;
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
   unsigned char r, g, b;
   r = data[row][col].r;
   g = data[row][col].g;
   b = data[row][col].b;

   return ppm_pixel{ r,g,b };
}

// Set the pixel at index (row, col)
void ppm_image::set(int row, int col, const ppm_pixel& c)
{
    data[row][col].r = c.r;
    data[row][col].g = c.g;
    data[row][col].b = c.b;
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

// Return a copy of the color inverted image
ppm_image ppm_image::invert() const 
{
    unsigned char red, green, blue;
    ppm_image result = ppm_image(wid, hei);

    for (int i = 0; i < hei; i++)
    {
        for (int j = 0; j < wid; j++)
        {
            result.data[i][j].r = 255 - data[i][j].r;
            result.data[i][j].g = 255 - data[i][j].g;
            result.data[i][j].b = 255 - data[i][j].b;
        }
    }
    return result;
}

// Unique
ppm_image& ppm_image::operator=(const ppm_image& orig) {
    clear();
    wid = orig.wid;
    hei = orig.hei;
    data = new ppm_pixel * [hei];
    for (int i = 0; i < hei; i++) {
        data[i] = new ppm_pixel[wid];
        for (int j = 0; j < wid; j++) {
            data[i][j] = orig.data[i][j];
        }
    }
    return *this;
}









