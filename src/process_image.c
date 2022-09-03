#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "image.h"

float get_pixel(image im, int x, int y, int c)
{
    //column x, row y
    if (c >= im.c) {
        return 0;
    }
    if (x >= im.w) {
        x = im.w - 1;
    }
    if (y >= im.h) {
        y = im.h - 1;
    }
    return *(im.data + c * im.h * im.w + y * im.w + x);
}

void set_pixel(image im, int x, int y, int c, float v)
{
    if (c >= im.c || x >= im.w || y >= im.h) {
        return;
    }
    *(im.data + c * im.h * im.w + y * im.w + x) = v;
    return;
}

image copy_image(image im)
{
    image copy = make_image(im.w, im.h, im.c);
    // TODO Fill this in
    memcpy(copy.data, im.data, im.w * im.h * im.c * sizeof(float));
    return copy;
}

image rgb_to_grayscale(image im)
{
    assert(im.c == 3);
    image gray = make_image(im.w, im.h, 1);
    //Y' = 0.299 R' + 0.587 G' + .114 B'
    int area = im.w * im.h;
    for (int i = 0; i < im.h * im.w; i++) {
            gray.data[i] = 0.299 * im.data[i] +
                              0.587 * im.data[area + i] +
                              0.114 * im.data[2*area + i];
    }
    return gray;
}

void shift_image(image im, int c, float v)
{
    int area = im.h * im.w;
    for (int i =0; i < area; i++) {
        im.data[c * area + i] += v;
    }
}

void clamp_image(image im)
{
    for (int i=0; i < im.c * im.h * im.w; i++) {
        if (im.data[i] < 0) {
            im.data[i] = 0;
        }
        if (im.data[i] > 1) {
            im.data[i] = 1;
        }
    }
}

void scale_image(image im, int c, float v)
{
    int area = im.h * im.w;
    for (int i =0; i < area; i++) {
        im.data[c * area + i] *= v;
    }
}


// These might be handy
float three_way_max(float a, float b, float c)
{
    return (a > b) ? ( (a > c) ? a : c) : ( (b > c) ? b : c) ;
}

float three_way_min(float a, float b, float c)
{
    return (a < b) ? ( (a < c) ? a : c) : ( (b < c) ? b : c) ;
}

void rgb_to_hsv(image im)
{
    int area = im.w * im.h;
    for (int i = 0; i < area; i++) {
       float r = im.data[i];
       float g = im.data[area + i];
       float b = im.data[2*area + i];
       float V = three_way_max(r,g,b);
       float m = three_way_min(r,g,b);
       float C = V - m;
       float S = 0;
       if (r != 0 || g != 0 || b != 0) {
        S = C / V;
       }
       float Hdash = 0;
       if (C != 0) {
        if (V == r) {
            Hdash = (g-b)/C;
        }
        if (V == g) {
            Hdash = (b-r)/C + 2;
        }
        if (V == b) {
            Hdash = (r-g)/C + 4;
        }
       }
       float H = 0;
       if (Hdash < 0) {
        H = Hdash/6 + 1;
       } else {
        H = Hdash/6;
       }
       im.data[i] = H;
       im.data[area + i] = S;
       im.data[2*area + i] = V;
    }
}

//From here: https://cs.stackexchange.com/questions/64549/convert-hsv-to-rgb-colors
void hsv_to_rgb(image im)
{
   int area = im.w * im.h;
   for (int i = 0; i < area; i++) {
    float H = im.data[i];
    float S = im.data[area + i];
    float V = im.data[2*area + i];

    float max = V;
    float C  = S * V;
    float min = max - C;
    float Hdash = 0;
    if (H >= (5./6)) {
        Hdash = 6 * (H - 1);
    }
    else {
        Hdash = 6 * H;
    }
    float r = 0;
    float g = 0;
    float b = 0;
    if (Hdash >= -1 && Hdash < 1) {
        r = max;
        if (Hdash < 0) {
            g = min;
            b = g - Hdash * C;
        } else {
            b = min;
            g = b + Hdash * C;
        }
    } else if (Hdash >=1 && Hdash < 3) {
        if ((Hdash - 2) < 0) {
            g = max;
            b = min;
            r = b - (Hdash - 2) * C;
        } else {
            r = min;
            g = max;
            b = r + (Hdash - 2) * C;
        }
    } else if (Hdash >=3 && Hdash < 5) {
        if ((Hdash - 4) < 0) {
            r = min;
            g = r - (Hdash - 4) * C;
            b = max;
        } else {
            g = min;
            r = g + (Hdash - 4) * C;
            b = max;
        }
    }
    im.data[i] = r;
    im.data[area + i] = g;
    im.data[2*area + i] = b;
   }
}
