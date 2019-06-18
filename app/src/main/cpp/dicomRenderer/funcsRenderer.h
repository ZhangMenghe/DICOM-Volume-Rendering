#ifndef FUNCTION_RENDERER_H
#define FUNCTION_RENDERER_H
// Include the latest possible header file( GL version header )
#if __ANDROID_API__ >= 24
#include <GLES3/gl32.h>
#include <glm/vec2.hpp>
#include <GLPipeline/Shader.h>

#elif __ANDROID_API__ >= 21
#include <GLES3/gl31.h>
#else
#include <GLES3/gl3.h>
#endif

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
    const unsigned int indices_func_[6]= {0,1,2,
                                    0,2,3};
    const float quad_vertices[12]={
            0.5f, 1.0f, .0f,//top-right
            -0.5f,1.0f, .0f,//top-left
            -0.5f, .0f, .0f,//bottom-left
            0.5f, .0f,  .0f//bottom-right
    };
    float quad_vertices_tex[24]={
            0.5f, 1.0, .0f, 1.0, .0,.0f,//top-right
            -0.5f,1.0, .0f,.0, .0, .0f,//top-left
            -0.5f, .0f,.0f, .0 , .0,.0f,//bottom-left
            0.5f, .0f, .0f,1.0, .0, .0f,//bottom-right
    };
    GLuint VAO_FUNC, VBO_FUNC,EBO_Func;
    GLuint VAO_QUAD;
    GLuint VAO_COLOR_BAR;

    Shader shader_func, shader_quad, shader_colorbar;
    GLfloat *vertices_func_;

    void draw_color_bar();
    void draw_opacity_func();
};
#endif