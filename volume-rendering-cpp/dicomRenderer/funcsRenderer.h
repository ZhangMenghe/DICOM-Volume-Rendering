#ifndef FUNCTION_RENDERER_H
#define FUNCTION_RENDERER_H

#include <platforms/platform.h>
enum FUNC_TYPE{
    OPACITY_FUN = 0,
    COLOR_BAR
};
class FuncRenderer{
public:
    FuncRenderer();
    void CreateFunction(FUNC_TYPE type);

    void UpdateFuncPoints(FUNC_TYPE type);
    void UpdateFuncPoints(FUNC_TYPE type, float* points, bool is_quad = true);
    void UpdateFuncPoints(FUNC_TYPE type, glm::vec2 p1, glm::vec2 p2, bool is_quad = true);
    void Draw();
protected:
    const size_t MAX_VERTICS=10;

    GLuint VAO_FUNC, VBO_FUNC, EBO_Func;
    GLuint VAO_QUAD;
    GLuint VAO_COLOR_BAR;

    Shader shader_func, shader_quad, shader_colorbar;
    GLfloat *vertices_func_;

    void draw_color_bar();
    void draw_opacity_func();
};
#endif