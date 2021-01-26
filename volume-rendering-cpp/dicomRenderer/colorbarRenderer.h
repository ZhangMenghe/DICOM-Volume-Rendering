#ifndef COLORBAR_RENDERER_H
#define COLORBAR_RENDERER_H

#include <dicomRenderer/basequadRenderer.h>

class ColorbarRenderer:public baseQuad{
public:
    ColorbarRenderer(std::string vertex_shader, std::string frag_shader);
    void Draw();

private:
    GLuint vao_;
    std::vector<float*> rgb_data;

};
#endif