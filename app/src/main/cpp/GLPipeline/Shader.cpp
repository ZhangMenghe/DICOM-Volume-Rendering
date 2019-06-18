#include "Shader.h"
#include <string>
#include "AndroidUtils/AndroidHelper.h"
using namespace glm;
Shader::~Shader() {
    glDeleteProgram(mProgram);
}

bool Shader::Create(const char* vert_file, const char *_frag_file){
    std::string VertexShaderContent;
    auto loader = assetLoader::instance();
    if (!loader->LoadTextFileFromAssetManager(vert_file, &VertexShaderContent)) {
        LOGE("Failed to load file: %s", vert_file);
        return false;
    }

    std::string FragmentShaderContent;
    if (!loader->LoadTextFileFromAssetManager(_frag_file, &FragmentShaderContent)) {
        LOGE("Failed to load file: %s", _frag_file);
        return false;
    }

    return create_program(VertexShaderContent.c_str(), FragmentShaderContent.c_str());
}
bool Shader::check_gl_error(const char *funcName){
    GLint err = glGetError();
    if (err != GL_NO_ERROR) {
        LOGE("GL error after %s(): 0x%08x\n", funcName, err);
        return true;
    }
    return false;
}
GLuint Shader::create_shader(GLenum shaderType, const char *pSource){
    GLuint shader = glCreateShader(shaderType);
    if (!shader) {
        check_gl_error("glCreateShader");
        return 0;
    }
    glShaderSource(shader, 1, &pSource, NULL);

    GLint compiled = GL_FALSE;
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        GLint infoLogLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen);
        if (infoLogLen > 0) {
            GLchar* infoLog = (GLchar*)malloc(infoLogLen);
            if (infoLog) {
                glGetShaderInfoLog(shader, infoLogLen, NULL, infoLog);
                LOGE("Could not compile %s shader:\n%s\n",
                     shaderType == GL_VERTEX_SHADER ? "vertex" : "fragment",
                     infoLog);
                free(infoLog);
            }
        }
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}
bool Shader::create_program(const char* vtxSrc, const char* fragSrc){
    GLuint vtxShader = 0;
    GLuint fragShader = 0;
    GLint linked = GL_FALSE;

    vtxShader = create_shader(GL_VERTEX_SHADER, vtxSrc);
    if (!vtxShader){glDeleteShader(vtxShader);glDeleteShader(fragShader); return false;}


    fragShader = create_shader(GL_FRAGMENT_SHADER, fragSrc);
    if (!fragShader){glDeleteShader(vtxShader); glDeleteShader(fragShader); return false;}

    mProgram = glCreateProgram();
    if (!mProgram) {
        check_gl_error("glCreateProgram");
        glDeleteShader(vtxShader); glDeleteShader(fragShader); return false;
    }
    glAttachShader(mProgram, vtxShader);
    glAttachShader(mProgram, fragShader);

    glLinkProgram(mProgram);
    glGetProgramiv(mProgram, GL_LINK_STATUS, &linked);
    if (!linked) {
        LOGE("Could not link program");
        GLint infoLogLen = 0;
        glGetProgramiv(mProgram, GL_INFO_LOG_LENGTH, &infoLogLen);
        if (infoLogLen) {
            GLchar* infoLog = (GLchar*)malloc(infoLogLen);
            if (infoLog) {
                glGetProgramInfoLog(mProgram, infoLogLen, NULL, infoLog);
                LOGE("Could not link program:\n%s\n", infoLog);
                free(infoLog);
            }
        }
        glDeleteProgram(mProgram);
        mProgram = 0;
        glDeleteShader(vtxShader);
        glDeleteShader(fragShader);
        return false;
    }
    glDeleteShader(vtxShader);
    glDeleteShader(fragShader);
    return true;
}