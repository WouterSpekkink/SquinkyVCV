
#pragma once
#include "nanovg.h"
class NoiseDrawer
{
public:
    NoiseDrawer(NVGcontext *vg, float x, float y, float width, float height)
        : _x(x), _y(y), _width(width), _height(height)
    {
        const char * filename = "D:\\VCVRack\\6rack\\plugins\\SquinkyVCV\\res\\test2.png";

        _image =  nvgCreateImage(vg, filename, 0);
        assert(_image != 0);
        _vg = vg;
    }

    ~NoiseDrawer() 
    {
        nvgDeleteImage(_vg, _image);
        _image=0;
    }

    void draw(NVGcontext *vg);

private:
    int _image=0;
    NVGcontext* _vg = nullptr;
    const float _x, _y, _width, _height;



};


inline void NoiseDrawer::draw(NVGcontext *vg)
{
    assert(_image);
    
    // changing ox, oy moves the image around in the rect
    // (ex,ey) the size of one image. if you use the 
    NVGpaint paint = nvgImagePattern(vg,
        0, 0,
        //_width, _height,
        90, 380,
         0, _image, 0.99f);

/*
// Creates and returns an image patter. Parameters (ox,oy) specify the left-top location of the image pattern,
// (ex,ey) the size of one image, angle rotation around the top-left corner, image is handle to the image to render.
// The gradient is transformed by the current transform when it is passed to nvgFillPaint() or nvgStrokePaint().
NVGpaint nvgImagePattern(NVGcontext* ctx, float ox, float oy, float ex, float ey,
						 float angle, int image, float alpha);
                         */

    nvgBeginPath( vg );
    nvgRect( vg, 0, 0, _width, _height);
   // nvgRoundedRect( vg, 0, 0, width, height, 5);
 
    nvgFillPaint( vg, paint );
    nvgFill( vg );
}