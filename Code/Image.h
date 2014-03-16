
namespace RT
{


class CImage : public Lockable
{
public:
    CImage () :
        m_pixels(),
        m_width(0),
        m_height(0)
    {}
    CImage (uint width, uint height);
    CImage (const CImage & rhs) :
        m_pixels(rhs.m_pixels),
        m_width(rhs.m_width), 
        m_height(rhs.m_height)
    {}

    CImage (CImage && rhs) :
        m_pixels(std::forward<TArray<Color>>(rhs.m_pixels)),
        m_width(rhs.m_width), 
        m_height(rhs.m_height)
    {}

    void Resize (uint width, uint height);

    uint GetWidth () const { return m_width; }
    uint GetHeight () const { return m_height; }

    void SetPixel (uint x, uint y, const Color & color);
    Color GetPixel (uint x, uint y) const;

    void Save (const char filename[]) const;
private:
    uint m_width;
    uint m_height;
    TArray<Color> m_pixels;
};


} // namespace RT