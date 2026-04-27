#pragma once

#ifndef HELPERS_H
#define HELPERS_H

#include "libs.h"


inline uint32_t rgb_to_int32(int r, int g, int b)
{
    return (r << 16) | (g << 8) | (b);
}

// small font.
// taken from https://github.com/Michaelangel007/nanofont3x4?tab=readme-ov-file
// thanks to Michaelangel007 :D
const uint16_t aGlyphs3x4[] =
{           // Hex    [1] [2] [3] [4] Scanlines
    0x0000, // 0x20
    0x4400, // 0x21 ! 010 010 000
    0xAA00, // 0x22 " 101 101 000
    0xAEE0, // 0x23 # 101 111 111
    0x64C4, // 0x24 $ 011 010 110 010
    0xCE60, // 0x25 % 110 111 011
    0x4C60, // 0x26 & 010 110 011
    0x4000, // 0x27 ' 010 000 000
    0x4840, // 0x28 ( 010 100 010
    0x4240, // 0x29 ) 010 001 010
    0x6600, // 0x2A * 011 011 000
    0x4E40, // 0x2B + 010 111 010
    0x0088, // 0x2C , 000 000 100 100
    0x0E00, // 0x2D - 000 111 000
    0x0080, // 0x2E . 000 000 100
    0x2480, // 0x2F / 001 010 100

    0x4A40, // 0x30 0 010 101 010
    0x8440, // 0x31 1 100 010 010
    0xC460, // 0x32 2 110 010 011
    0xE6E0, // 0x33 3 111 011 111
    0xAE20, // 0x34 4 101 111 001
    0x64C0, // 0x35 5 011 010 110
    0x8EE0, // 0x36 6 100 111 111
    0xE220, // 0x37 7 111 001 001
    0x6EC0, // 0x38 8 011 111 110
    0xEE20, // 0x39 9 111 111 001
    0x4040, // 0x3A : 010 000 010
    0x0448, // 0x3B ; 000 010 010 100 
    0x4840, // 0x3C < 010 100 010
    0xE0E0, // 0x3D = 111 000 111
    0x4240, // 0x3E > 010 001 010
    0x6240, // 0x3F ? 011 001 010

    0xCC20, // 0x40 @ 110 110 001 
    0x4EA0, // 0x41 A 010 111 101 
    0xCEE0, // 0x42 B 110 111 111
    0x6860, // 0x43 C 011 100 011
    0xCAC0, // 0x44 D 110 101 110
    0xECE0, // 0x45 E 111 110 111
    0xEC80, // 0x46 F 111 110 100
    0xCAE0, // 0x47 G 110 101 111
    0xAEA0, // 0x48 H 101 111 101
    0x4440, // 0x49 I 010 010 010
    0x22C0, // 0x4A J 001 001 110
    0xACA0, // 0x4B K 101 110 101
    0x88E0, // 0x4C L 100 100 111
    0xEEA0, // 0x4D M 111 111 101
    0xEAA0, // 0x4E N 111 101 101
    0xEAE0, // 0x4F O 111 101 111

    0xEE80, // 0x50 P 111 111 100
    0xEAC0, // 0x51 Q 111 101 110
    0xCEA0, // 0x52 R 110 111 101
    0x64C0, // 0x53 S 011 010 110
    0xE440, // 0x54 T 111 010 010
    0xAAE0, // 0x55 U 101 101 111
    0xAA40, // 0x56 V 101 101 010
    0xAEE0, // 0x57 W 101 111 111
    0xA4A0, // 0x58 X 101 010 101
    0xA440, // 0x59 Y 101 010 010
    0xE4E0, // 0x5A Z 111 010 111
    0xC8C0, // 0x5B [ 110 100 110
    0x8420, // 0x5C \ 100 010 001
    0x6260, // 0x5D ] 011 001 011
    0x4A00, // 0x5E ^ 010 101 000
    0x00E0, // 0x5F _ 000 000 111

    0x8400, // 0x60 ` 100 010 000
    0x04C0, // 0x61 a 000 010 110
    0x8CC0, // 0x62 b 100 110 110
    0x0CC0, // 0x63 c 000 110 110
    0x4CC0, // 0x64 d 010 110 110
    0x08C0, // 0x65 e 000 100 110
    0x2440, // 0x66 f 001 010 010
    0x0CCC, // 0x67 g 000 110 110 110
    0x8CC0, // 0x68 h 100 110 110
    0x0440, // 0x69 i 000 010 010
    0x0448, // 0x6A j 000 010 010 100
    0x8CA0, // 0x6B k 100 110 101
    0x4420, // 0x6C l 010 010 001
    0x0CE0, // 0x6D m 000 110 111
    0x0CA0, // 0x6E n 000 110 101
    0x0CC0, // 0x6F o 000 110 110

    0x0CC8, // 0x70 p 000 110 110 100
    0x0CC4, // 0x71 q 000 110 110 010
    0x0C80, // 0x72 r 000 110 100
    0x0480, // 0x73 s 000 010 100
    0x4C60, // 0x74 t 010 110 011
    0x0AE0, // 0x75 u 000 101 111
    0x0A40, // 0x76 v 000 101 010
    0x0E60, // 0x77 w 000 111 011
    0x0CC0, // 0x78 x 000 110 110
    0x0AE2, // 0x79 y 000 101 111 001
    0x0840, // 0x7A z 000 100 010
    0x6C60, // 0x7B { 011 110 011
    0x4444, // 0x7C | 010 010 010 010
    0xC6C0, // 0x7D } 110 011 110
    0x6C00, // 0x7E ~ 011 110 000
    0xA4A4  // 0x7F   101 010 101 010 // Alternative: Could even have a "full" 4x4 checkerboard
};

inline void setPixel(Hub75& hub75, int x, int y, int r, int g, int b, bool vertical)
{
    if(vertical)
    {
        x = SCREEN_HEIGHT - x;
        std::swap(x, y);
    }
    if(x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT)
        hub75.set_pixel(x, y, r, g, b);
}
inline void draw4x3Char(Hub75& hub75, uint16_t hex, int x, int y, int r, int g, int b, bool vertical)
{
    for(int i = 0; i < 4*3; i++)
    {
        if((hex >> (15 - i)) & 1)
        {
            int point_x = x + (i % 4);
            int point_y = y + (i / 4);

            setPixel(hub75, point_x, point_y, r, g, b, vertical);
            //printf("setting pixel: %i %i", point_x, point_y);
        }
    }
}

inline void cropTextToFit(std::string& text, int font_width, int pixels_limit)
{
    if(text.size() * font_width >= pixels_limit)
        text.replace((pixels_limit / font_width) - 2, text.size() - (pixels_limit / font_width - 2), ".");
}

// the second parameter is not used, its just there for compatibility with the scrolling function.
inline void draw4x3Text(Hub75& hub75, PicoGraphics_PenRGB888& graphics, const std::string& text, int x, int y, int r, int g, int b, bool vertical)
{
    
    if(text.size() != 0)
    {
        for(int i = 0; i < (int)text.size(); i++)
        {
            int index = (int)text[i] - 32;
            if(index > 0 && index < 95)
            {
                //printf("printing char!\n\n");
                draw4x3Char(hub75, aGlyphs3x4[index], x + (i * 4), y, r, g, b, vertical);
            }
            else
            {
                printf("index out of array bounds!!\n\n");
            }
        }
    }
}

inline void drawScrollingText(Hub75& hub75, PicoGraphics_PenRGB888& graphics, const std::string& text, int x, int y, int r, int g, int b, bool vertical, int frames_to_scroll, int& new_pos_x, int frames, int font_width,
    void(*drawing_function)(Hub75& hub75, PicoGraphics_PenRGB888& graphics, const std::string& text, int x, int y, int r, int g, int b, bool vertical))
{
    const int initial_offset = 5;

    if(new_pos_x < (x - ((int)text.size() * font_width)))
    {
        new_pos_x += (vertical) ? (SCREEN_HEIGHT) : (SCREEN_WIDTH);
        new_pos_x += + ((int)text.size() * font_width);
    }
        

    if(frames % frames_to_scroll == 0)
        new_pos_x--;

    drawing_function(hub75, graphics, text, new_pos_x, y, r, g, b, vertical);


}

// since this uses the pico api, it needs a graphics object.
inline void draw5x5Text(Hub75& hub75, PicoGraphics_PenRGB888& graphics, const std::string& text, int x, int y, int r, int g, int b, bool vertical)
{

    float angle = vertical ? 270.0f : 0.0f;

    int pos_x = x - (vertical ? (6 / 2) * 6 : (12 / 2) * 6);
    // i hate these hardcoded values but they must stick for now so that the title is actually centered :P
    pos_x += vertical ? 3 : 6;
    int count = std::ranges::count(text, ' ') + 1;
    int pos_y = y - (count * 7);

    if (vertical)
    {
        pos_x = SCREEN_HEIGHT - pos_x;
        std::swap(pos_x, pos_y);
    }

    graphics.set_pen(0, 0, 0);
    graphics.clear();
    graphics.set_pen(r, g, b);
    graphics.text(text, Point(pos_x, pos_y), 1, 1.0f, angle);
}
inline void drawBitmap(Hub75& hub75, std::vector<std::vector<int>> bitmap, int width, int height, int x, int y, bool vertical)
{
    for(int i = 0; i < height; i++)
    {
        for(int j = 0; j < width; j++)
        {
            int point_x = x + i;
            int point_y = y + j;
            
            int color = bitmap[j][i];
            uint8_t r = (color >> 16) & 0xFF;
            uint8_t g = (color >> 8) & 0xFF;
            uint8_t b = color & 0xFF;
            setPixel(hub75, point_x, point_y, r, g, b, vertical);
        }
    }
}


// accepts an array of size 3 that represents a color, and will change it once per call to this function.
// so that several continuos calls make a rainbow effect:P
// you must provide the array containing the increments that each of the rgb values will be incremented with.
inline void rainbowEffect(std::array<uint8_t, 3>& rgb, std::array<int, 3>& increments)
{
    int new_rgb[3] = {rgb[0], rgb[1], rgb[2]};
    new_rgb[0] += increments[0];
    new_rgb[1] += increments[1];
    new_rgb[2] += increments[2];

    for(int i = 0; i < 3; i++)
    {
        if(new_rgb[i] >= 255)
        {
            new_rgb[i] = 255;
            increments[i] *= -1;
        }   
        else if(new_rgb[i] <= 0)
        {
            new_rgb[i] = 0;
            increments[i] *= -1;
        }
        rgb[i] = new_rgb[i];
    }
}


// had to do this since for some reason making this an inline function causes UB for some reason :DDDDDDD
#define drawSquare(hub75, size, x, y, r, g, b)\
    for(int i = 0; i < size; i++)\
    {\
        for(int j = 0; j < size; j++)\
        {\
            hub75.set_pixel(x + j, y + i, r, g, b);\
        }\
    }
    
#endif