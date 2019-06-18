#ifndef RAYCAST_RENDERER_H
#define RAYCAST_RENDERER_H

#include <cstring>
#include <GLES3/gl32.h>
#include <vector>
#include <GLPipeline/Mesh.h>
#include <GLPipeline/Shader.h>

class raycastRenderer{
private:
    const GLfloat sVertex[48] = {//World
            -0.5f,-0.5f,0.5f,   0.0f,0.0f,1.0f,
            0.5f,-0.5f,0.5f,    1.0f,0.0f,1.0f,
            0.5f,0.5f,0.5f,	    1.0f,1.0f,1.0f,
            -0.5f,0.5f,0.5f,    0.0f,1.0f,1.0f,
            -0.5f,-0.5f,-0.5f,  0.0f,0.0f,0.0f,
            0.5f,-0.5f,-0.5f,   1.0f,0.0f,0.0f,
            0.5f,0.5f,-0.5f,    1.0f,1.0f,0.0f,
            -0.5f,0.5f,-0.5f,   0.0f,1.0f,0.0f,
    };
    const GLuint sIndices[36] = { 0,1,2,0,2,3,	//front
                                  4,6,7,4,5,6,	//back
                                  4,0,3,4,3,7,	//left
                                  1,5,6,1,6,2,	//right
                                  3,2,6,3,6,7,	//top
                                  4,5,1,4,1,0,	//bottom
    };

    Shader* shader_;
    glm::mat4 modelMat_;
    GLuint VAO_;
public:
    raycastRenderer();
    glm::mat4 ModelMat(){return modelMat_;}
    void setModelMat(glm::mat4 mat){modelMat_ = mat;}
    void Draw();
};
#endif