#define _CRT_SECURE_NO_WARNINGS

#include <cstdio>

#include "Pch.h"

namespace RT
{

static Color FixForPaintDotNet (const Color & color)
{
    return Color(color.b, color.g, color.r, color.a);
}

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
    FILE * f = fopen(filename, "w");

    fprintf(f, "P3\n%u %u\n255\n", m_width, m_height);


    for (uint y = 0, i = 0; y < m_height; ++y)
    {
        for (uint x = 0; x < m_width; ++x, ++i)
        {
            const Color & c = FixForPaintDotNet(m_pixels[i]);
            const uint r = FloatToUint(c.r * 255);
            const uint g = FloatToUint(c.g * 255);
            const uint b = FloatToUint(c.b * 255);
            fprintf(f, "%u %u %u ", r, g, b);
        }
        fprintf(f, "\n");
    }

    fclose(f);
}

}