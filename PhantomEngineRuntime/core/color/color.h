#pragma once

class Color4
{
public:
    Color4()
        : r(0), g(0), b(0), a(0)
    {}
    Color4(float _r, float _g, float _b, float _a = 1.0f)
        : r(_r), g(_g), b(_b), a(_a)
    {}
    float r, g, b, a;
};