#ifndef COLORBAR_RENDERER_H
#define COLORBAR_RENDERER_H

#include <dicomRenderer/basequadRenderer.h>

class ColorbarRenderer:public baseQuad{
public:
    ColorbarRenderer(std::string vertex_shader, std::string frag_shader);
    void Draw();
};
#endif