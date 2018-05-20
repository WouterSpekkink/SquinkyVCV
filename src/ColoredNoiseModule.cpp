
#include "Squinky.hpp"

#include "WidgetComposite.h"
#include "ColoredNoise.h"
#include "NoiseDrawer.h"

/**
 * Implementation class for VocalWidget
 */
struct ColoredNoiseModule : Module
{
    ColoredNoiseModule();

    /**
     * Overrides of Module functions
     */
    void step() override;
    void onSampleRateChange() override;

    ColoredNoise<WidgetComposite> noiseSource;
private:
    typedef float T;
};

ColoredNoiseModule::ColoredNoiseModule() 
    : Module(noiseSource.NUM_PARAMS,
        noiseSource.NUM_INPUTS, 
        noiseSource.NUM_OUTPUTS, 
        noiseSource.NUM_LIGHTS),
        noiseSource(this)
{
    onSampleRateChange();
    noiseSource.init();
}

void ColoredNoiseModule::onSampleRateChange()
{
    T rate = engineGetSampleRate();
    noiseSource.setSampleRate(rate);
}

void ColoredNoiseModule::step()
{
    noiseSource.step();
}

////////////////////
// module widget
////////////////////

struct ColoredNoiseWidget : ModuleWidget
{
    ColoredNoiseWidget(ColoredNoiseModule *);
    Label * slopeLabel;
    Label * signLabel;
};

static const unsigned char red[3] = {0xff, 0x04, 0x14 };
static const unsigned char pink[3] = {0xff, 0x3a, 0x6d };
static const unsigned char white[3] = {0xff, 0xff, 0xff };
static const unsigned char blue[3] = {0x54, 0x43, 0xc1 };
static const unsigned char violet[3] = {0x9d, 0x3c, 0xe6 };

// 0 <= x <= 1
static float interp(float x, int x0, int x1) 
{
    return x1 * x + x0 * (1 - x);
}

// 0 <= x <= 3
static void interp(unsigned char * out, float x, const unsigned char* y0, const unsigned char* y1)
{
    x = x * 1.0/3.0;    // 0..1
    out[0] = interp (x, y0[0], y1[0]);
    out[1] = interp (x, y0[1], y1[1]);
    out[2] = interp (x, y0[2], y1[2]);
}

static void copyColor(unsigned char * out, const unsigned char* in)
{
    out[0] = in[0];
    out[1] = in[1];
    out[2] = in[2];
}

static void getColor(unsigned char * out,  float x)
{
    if (x < -6) {
       copyColor(out, red);
    } else if (x >= 6) {
        copyColor(out, violet);
    } else {
        if (x < -3) {
            interp(out, x + 6, red, pink);
        } else if ( x < 0) {
            interp(out, x + 3, pink, white);
        } else if ( x < 3) {
            interp(out, x + 0, white, blue);
        } else if ( x < 6) {
            interp(out, x - 3, blue, violet);
        } else {
            copyColor(out, white);
        }
    }
}


void doDraw(NVGcontext *vg) 
{
  

    //const int width = 100;
    //const int height = 100;
    const int width = 6 * RACK_GRID_WIDTH;
    const int height = RACK_GRID_HEIGHT;
    const char * filename = "D:\\VCVRack\\6rack\\plugins\\SquinkyVCV\\res\\test2.png";
   // printf("try load %s\n", filename);

   // context, file, imageFlags
   static int handle = 0;
   if (!handle) {
        handle =  nvgCreateImage(vg, filename,
      //NVG_IMAGE_REPEATX | NVG_IMAGE_REPEATY);
        0);
   }
    if (!handle) printf("image load fail\n");


    {
        int x, y;
        nvgImageSize(vg, handle, &x, &y);
       // printf("loaded image size = %d, %d\n", x, y);
    }

    NVGpaint paint;
    paint.image = 567;


// changing ox, oy moves the image around in the rect
   paint = nvgImagePattern(vg,
      //  0, 0,
        -30, -30,
        width, height, 0, handle, 0.99f);
  // printf("paint image = %d\n", paint.image);

/*
// Creates and returns an image patter. Parameters (ox,oy) specify the left-top location of the image pattern,
// (ex,ey) the size of one image, angle rotation around the top-left corner, image is handle to the image to render.
// The gradient is transformed by the current transform when it is passed to nvgFillPaint() or nvgStrokePaint().
NVGpaint nvgImagePattern(NVGcontext* ctx, float ox, float oy, float ex, float ey,
						 float angle, int image, float alpha);
                         */

    nvgBeginPath( vg );
    nvgRect( vg, 0, 0, width, height);
   // nvgRoundedRect( vg, 0, 0, width, height, 5);

 


        
    nvgFillPaint( vg, paint );
    nvgFill( vg );

  //  nvgDeleteImage(vg, handle);
 //   printf("leaving draw\n");
}

struct ColorDisplay : OpaqueWidget {
    ColoredNoiseModule *module;
    ColorDisplay(Label *slopeLabel, Label *signLabel) 
        : _slopeLabel(slopeLabel),
         _signLabel(signLabel)
     //    _noiseDrawer(0,0, 100, 100)
         {}

    Label* _slopeLabel;
    Label* _signLabel;
    std::unique_ptr<NoiseDrawer> _noiseDrawer;

    void draw(NVGcontext *vg) override 
    {
        if (!_noiseDrawer) {
            _noiseDrawer.reset( new NoiseDrawer(vg, 0, 0, 100, 100));
        }
        _noiseDrawer->draw(vg);
       // doDraw(vg);
    }

    #if 0
    void draw(NVGcontext *vg) override 
    {
        const float slope = module->noiseSource.getSlope();
        unsigned char color[3];
        getColor(color, slope);
        nvgFillColor(vg, nvgRGBA(color[0], color[1], color[2], 0xff));

        nvgBeginPath(vg);
        // todo: pass in ctor
        nvgRect(vg, 0, 0, 6 * RACK_GRID_WIDTH,RACK_GRID_HEIGHT);
		nvgFill(vg);

        const bool slopeSign = slope >= 0;
        const float slopeAbs = std::abs(slope);
        std::stringstream s;
    
        s.precision(1);
        s.setf( std::ios::fixed, std::ios::floatfield );
        s << slopeAbs << " db/oct";
        _slopeLabel->text = s.str();

        const char * mini = "\u2005-";
        _signLabel->text = slopeSign ? "+" : mini;

    // let's overlay some noise
    #if 1
        {
            const int width = 6 * RACK_GRID_WIDTH;
            const int height = RACK_GRID_HEIGHT;
           // const int size = 100;
            const int memSize = width * height * 4;
            //make the noise data
            unsigned char * memImage = new unsigned char[memSize];
       // printf("mem size = %d\n", memSize);

#if 1
int vmin=100;
int vmax= -1000;
            for (int row=0; row<height; ++row) {
                for (int col=0; col<width; ++col) {
                    int value = int((255.f * rand()) / float(RAND_MAX));
                    vmin = std::min(value, vmin);
                    vmax = std::max(value, vmax);
                    unsigned char * pix = memImage + (4 * (row*width + col));
                    pix[0] = value;
                    pix[1] = value;
                    pix[2] = value; 
                    pix[3] = 255;
                }
            } 

            printf("val range %d, %d\n", vmin, vmax);
            #endif


//int nvgCreateImageRGBA(NVGcontext* ctx, int w, int h, int imageFlags, const unsigned char* data);

            const int handle = nvgCreateImageRGBA(vg,
             width, height, NVG_IMAGE_REPEATX | NVG_IMAGE_REPEATY,
              memImage );

            NVGpaint paint = nvgImagePattern(vg, 0, 0, width, height, 0, handle, 1.f);


            nvgBeginPath( vg );
            nvgRect( vg, 0, 0, width, height);

        
            nvgFillPaint( vg, paint );
            nvgFill( vg );

    
            printf("image handle = %d\n", handle);
        
            nvgDeleteImage(vg, handle);
            delete[] memImage;
        
        }
        #endif
    }
    #endif
};

/**
 * Widget constructor will describe my implementation structure and
 * provide meta-data.
 * This is not shared by all modules in the DLL, just one
 */
ColoredNoiseWidget::ColoredNoiseWidget(ColoredNoiseModule *module) : ModuleWidget(module)
{
 
    box.size = Vec(6 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);

    // save so we can update later.
    slopeLabel = new Label();
    signLabel = new Label();

    // add the color display
    #if 1
	{
		ColorDisplay *display = new ColorDisplay(slopeLabel, signLabel);
		display->module = module;
		display->box.pos = Vec( 0, 0);
		display->box.size = Vec(6 * RACK_GRID_WIDTH,RACK_GRID_HEIGHT);
		addChild(display);
        display->module = module;
	}
    #endif
    #if 0
    {
        SVGPanel *panel = new SVGPanel();
        panel->box.size = box.size;
        panel->setBackground(SVG::load(assetPlugin(plugin, "res/colors_panel.svg")));
        addChild(panel);
    }
    #endif

#if 0
    Label * label = new Label();
    label->box.pos = Vec(23, 24);
    label->text = "Noise";
    label->color = COLOR_BLACK;
    addChild(label);
 #endif

    addOutput(Port::create<PJ301MPort>(
        Vec(30, 310),
        Port::OUTPUT,
        module,
        module->noiseSource.AUDIO_OUTPUT));

//Davies1900hBlackKnob
//Rogan1PSWhite

    addParam(ParamWidget::create<Rogan2PSWhite>(
        Vec(22, 80), module, module->noiseSource.SLOPE_PARAM, -5.0, 5.0, 0.0));

    addParam(ParamWidget::create<Trimpot>(
        Vec(58, 46),
        module, module->noiseSource.SLOPE_TRIM, -1.0, 1.0, 1.0));

    addInput(Port::create<PJ301MPort>(
        Vec(14, 42),
        Port::INPUT,
        module,
        module->noiseSource.SLOPE_CV));


    const float labelY = 146;    
    slopeLabel->box.pos = Vec(12, labelY);
    slopeLabel->text = "foo";
    slopeLabel->color = COLOR_BLACK;
    addChild(slopeLabel);
    signLabel->box.pos = Vec(2, labelY);
    signLabel->text = "x";
    signLabel->color = COLOR_BLACK;
    addChild(signLabel);
}

Model *modelColoredNoiseModule = Model::create<ColoredNoiseModule, ColoredNoiseWidget>(
    "Squinky Labs",
    "squinkylabs-coloredNoise",
    "Colored Noise", EFFECT_TAG, FILTER_TAG);

