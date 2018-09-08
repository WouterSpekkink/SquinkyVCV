#pragma once


static unsigned char gtable[256] =
{
0, 1, 3, 2, 6, 7, 5, 4, 12, 13, 15, 14, 10, 11, 9, 8,
24, 25, 27, 26, 30, 31, 29, 28, 20, 21, 23, 22, 18, 19, 17, 16,
48, 49, 51, 50, 54, 55, 53, 52, 60, 61, 63, 62, 58, 59, 57, 56,
40, 41, 43, 42, 46, 47, 45, 44, 36, 37, 39, 38, 34, 35, 33, 32,
96, 97, 99, 98, 102, 103, 101, 100, 108, 109, 111, 110, 106, 107, 105, 104,
120, 121, 123, 122, 126, 127, 125, 124, 116, 117, 119, 118, 114, 115, 113, 112,
80, 81, 83, 82, 86, 87, 85, 84, 92, 93, 95, 94, 90, 91, 89, 88,
72, 73, 75, 74, 78, 79, 77, 76, 68, 69, 71, 70, 66, 67, 65, 64,
192, 193, 195, 194, 198, 199, 197, 196, 204, 205, 207, 206, 202, 203, 201, 200,
216, 217, 219, 218, 222, 223, 221, 220, 212, 213, 215, 214, 210, 211, 209, 208,
240, 241, 243, 242, 246, 247, 245, 244, 252, 253, 255, 254, 250, 251, 249, 248,
232, 233, 235, 234, 238, 239, 237, 236, 228, 229, 231, 230, 226, 227, 225, 224,
160, 161, 163, 162, 166, 167, 165, 164, 172, 173, 175, 174, 170, 171, 169, 168,
184, 185, 187, 186, 190, 191, 189, 188, 180, 181, 183, 182, 178, 179, 177, 176,
144, 145, 147, 146, 150, 151, 149, 148, 156, 157, 159, 158, 154, 155, 153, 152,
136, 137, 139, 138, 142, 143, 141, 140, 132, 133, 135, 134, 130, 131, 129, 128
};

static unsigned char bgtable[256] = 
{
00, 01, 03, 02, 06, 0E, 0A, 0B, 09, 0D, 0F, 07, 05, 04, 0C, 08,
18, 1C, 14, 15, 17 1F 3F 37 35 34 3C 38 28 2C 24 25
27, 2F, 2D, 29, 39 3D 1D 19 1B 3B 2B 2A 3A 1A 1E 16
36, 3E, 2E, 26, 22 32 12 13 33 23 21 31 11 10 30 20
60, 70, 50, 51, 71 61 63 73 53 52 72 62 66 6E 7E 76
56, 5E, 5A, 7A, 6A 6B EB EA FA DA DE D6 F6 FE EE E6
E2, F2, D2, D3, F3 E3 E1 F1 D1 D0 F0 E0 A0 B0 90 91
B1, A1, A3, B3, 93 92 B2 A2 A6 AE BE B6 96 9E 9A BA
AA, AB, BB, 9B, 99 9D DD D9 DB FB 7B 5B 59 5D 7D 79
F9, FD, BD, B9, A9 E9 69 6D 6F 67 65 64 E4 E5 E7 EF
ED, AD, AF, A7, A5 A4 AC EC 6C 68 E8 A8 B8 F8 78 7C
FC, BC, B4, B5, B7 F7 F5 F4 74 75 77 7F FF BF 9F DF
5F, 57, 55, 54, D4 D5 D7 97 95 94 9C DC 5C 58 D8 98
88, C8, 48, 4C, CC 8C 84 C4 44 45 C5 85 87 C7 47 4F
CF, 8F, 8D, CD, 4D 49 C9 89 8B CB 4B 4A CA 8A 8E CE
4E, 46, C6, 86, 82 C2 42 43 C3 83 81 C1 41 40 C0 80
};




template <class TBase>
class Gray : public TBase
{
public:
    Gray(struct Module * module) : TBase(module)
    {
       // init();
    }
    Gray() : TBase()
    {
       // init();
    }

    enum ParamIds
    {
        PARAM_CLOCK_MUL,
        NUM_PARAMS
    };
  
    enum InputIds
    {
        INPUT_CLOCK,
        NUM_INPUTS
    };

    enum OutputIds
    {
        OUTPUT_0,
        OUTPUT_1,
        OUTPUT_2,
        OUTPUT_3,
        OUTPUT_4,
        OUTPUT_5,
        OUTPUT_6,
        OUTPUT_7,

        NUM_OUTPUTS
    };

    enum LightIds
    {
        LIGHT_0,
        LIGHT_1,
        LIGHT_2,
        LIGHT_3,
        LIGHT_4,
        LIGHT_5,
        LIGHT_6,
        LIGHT_7,
        NUM_LIGHTS
    };

    /**
     * Main processing entry point. Called every sample
     */
    void step() override;

private:
    int counterValue = 0;
  
};


template <class TBase>
void  Gray<TBase>::step()
{

} 
