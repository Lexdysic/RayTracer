#define _CRT_SECURE_NO_WARNINGS

#include <cstdio>

#include "Pch.h"

namespace RT
{

CImage::CImage (uint width, uint height) :
    m_width(width),
    m_height(height)
{
    m_pixels.Resize(width * height);
}

void CImage::Resize (uint width, uint height)
{
    m_width = width;
    m_height = height;
    m_pixels.Resize(m_width * m_height);
}

void CImage::SetPixel (uint x, uint y, const Color & color)
{
    ASSERT(x < m_width);
    ASSERT(y < m_height);

    const uint index = y * m_width + x;
    m_pixels[index] = color;
}

Color CImage::GetPixel (uint x, uint y) const
{
    ASSERT(x < m_width);
    ASSERT(y < m_height);

    const uint index = y * m_width + x;
    return m_pixels[index];
}

void CImage::Save (const char filename[]) const
{
    FILE * f = fopen(filename, "wb");

#include <pshpack1.h>
    struct TgaHeader {
        char      id_length         = 0;
        char      color_map_type    = 0;
        char      data_type_code    = 2;    // uncompress RGB
        short int color_map_origin  = 0;
        short int color_map_length  = 0;
        char      color_map_depth   = 0;
        short int x_origin          = 0;
        short int y_origin          = 0;
        short     width             = 0;
        short     height            = 0;
        char      bits_per_pixel    = 24;   // bgr888
        char      image_descriptor  = 0;
    };
#include <poppack.h>

    TgaHeader header;
    header.width  = m_width;
    header.height = m_height;

    fwrite(&header, sizeof(TgaHeader), 1, f);

    for (const Color & c : m_pixels) {
        const uint r = Min(FloatToUint(c.r * 255), uint(255));
        const uint g = Min(FloatToUint(c.g * 255), uint(255));
        const uint b = Min(FloatToUint(c.b * 255), uint(255));
        fputc(b, f);
        fputc(g, f);
        fputc(r, f);
    }

    fclose(f);
}

}