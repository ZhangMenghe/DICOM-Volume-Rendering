#include "Shader.h"
#include <string>
#include "AndroidUtils/AndroidHelper.h"
using namespace glm;
Shader::~Shader() {
    glDeleteProgram(mProgram);
}

bool Shader::Create(const char* vert_file, const char *_frag_file, const char* _geo_file){
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
    const char* vcontent = VertexShaderContent.c_str();
    const char* fcontent = FragmentShaderContent.c_str();
    const char* gcontent = nullptr;

    if(_geo_file != nullptr) {
        std::string GeometryShaderContent;
        if (!loader->LoadTextFileFromAssetManager(_geo_file, &GeometryShaderContent)) {
            LOGE("Failed to load file: %s", _geo_file);
            return false;
        }
        gcontent = GeometryShaderContent.c_str();
        return create_program(&vcontent, &fcontent, &gcontent);
    }
    return create_program(&vcontent, &fcontent);
}
//for geometry shader
bool Shader::Create(const char* filename){
    auto loader = assetLoader::instance();
    std::string GeometryShaderContent;
    if (!loader->LoadTextFileFromAssetManager(filename, &GeometryShaderContent)) {
        LOGE("Failed to load file: %s", filename);
        return false;
    }
    const char* content = GeometryShaderContent.c_str();
    GLuint shader = glCreateShader(GL_COMPUTE_SHADER);//create_shader(GL_GEOMETRY_SHADER, &content, 1, nullptr);
    if (!shader) {
        check_gl_error("glCreateShader");
        return false;
    }
    glShaderSource(shader, 1, &content, nullptr);
    glCompileShader(shader);

    int rvalue;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &rvalue);
    if (!rvalue) {
        LOGE("====Error in compiling the compute shader\n");
        GLchar log[10240];
        GLsizei length;
        glGetShaderInfoLog(shader, 10239, &length, log);
        LOGE("=====Compiler log:\n%s\n", log);
    }



    mProgram = glCreateProgram();
    if(!shader || !mProgram){glDeleteShader(shader); return false;}
    glAttachShader(mProgram, shader);
    glLinkProgram(mProgram);
    GLint linked = GL_FALSE;
    glGetProgramiv(mProgram, GL_LINK_STATUS, &linked);
    if(!linked){
        GLint infoLogLen = 0;
        glGetProgramiv(mProgram, GL_INFO_LOG_LENGTH, &infoLogLen);
        if (infoLogLen) {
            GLchar* infoLog = (GLchar*)malloc(infoLogLen);
            if (infoLog) {
                glGetProgramInfoLog(mProgram, infoLogLen, NULL, infoLog);
                LOGE("===Could not link program:\n%s\n", infoLog);
                free(infoLog);
            }
        }
        glDeleteShader(shader); glDeleteProgram(mProgram); mProgram = 0; return false;
    }
    glDeleteShader(shader);
    return true;
}

//bool Shader::Create(const char* vert_files[], const char* frag_files[], int vert_size, int frag_size){
//    std::string content;
//    auto loader = assetLoader::instance();
//    char* vcontent[vert_size]; char* fcontent[frag_size];
//    int*vcon_len=new int[vert_size], *fcon_len= new int [frag_size];
//
//    for(size_t i=0; i<vert_size; i++){
//        if (!loader->LoadTextFileFromAssetManager(vert_files[i], &content)) {
//            LOGE("Failed to load file: %s", vert_files[i]);
//            return false;
//        }
//        vcon_len[i] = content.size();
//        vcontent[i] = new char[vcon_len[i]];
//        strcpy(vcontent[i], content.c_str());
//    }
//    for(int i=0; i<frag_size; i++){
//        if (!loader->LoadTextFileFromAssetManager(frag_files[i], &content)) {
//            LOGE("Failed to load file: %s", frag_files[i]);
//            return false;
//        }
//        fcon_len[i] = content.size();
//        fcontent[i] = new char[fcon_len[i]];
//        strcpy(fcontent[i], content.c_str());
//    }
//
//    if(vert_size == 1) vcon_len = nullptr;
//    if(frag_size == 1) fcon_len = nullptr;
//
//    return create_program((const char **)&vcontent[0], (const char **)&fcontent[0], vert_size, frag_size, vcon_len, fcon_len);
//}

bool Shader::check_gl_error(const char *funcName){
    GLint err = glGetError();
    if (err != GL_NO_ERROR) {
        LOGE("GL error after %s(): 0x%08x\n", funcName, err);
        return true;
    }
    return false;
}
GLuint Shader::create_shader(GLenum shaderType, const char **pSource, int src_size, int* arr_length){
    GLuint shader = glCreateShader(shaderType);
    if (!shader) {
        check_gl_error("glCreateShader");
        return 0;
    }
    glShaderSource(shader, src_size, pSource, arr_length);

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
                     shaderType == GL_VERTEX_SHADER ? "vertex" : (shaderType == GL_FRAGMENT_SHADER ?"fragment":"geometry"),
                     infoLog);
                free(infoLog);
            }
        }
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}
bool Shader::create_program(const char** vtxSrc, const char** fragSrc, const char** geoSrc){
    GLuint vtxShader = 0;
    GLuint fragShader = 0;
    GLuint geoShader = 0;
    GLint linked = GL_FALSE;

    vtxShader = create_shader(GL_VERTEX_SHADER, vtxSrc, 1, nullptr);
    if (!vtxShader){glDeleteShader(vtxShader);return false;}

    fragShader = create_shader(GL_FRAGMENT_SHADER, fragSrc, 1, nullptr);
    if (!fragShader){glDeleteShader(vtxShader); glDeleteShader(fragShader); return false;}

    if(geoSrc != nullptr){
        geoShader = create_shader(GL_GEOMETRY_SHADER, geoSrc, 1, nullptr);
        if(!geoShader){glDeleteShader(vtxShader); glDeleteShader(fragShader);glDeleteShader(geoShader); return false;}
    }
    mProgram = glCreateProgram();
    if (!mProgram) {
        check_gl_error("glCreateProgram");
        glDeleteShader(vtxShader); glDeleteShader(fragShader); return false;
    }
    glAttachShader(mProgram, vtxShader);
    glAttachShader(mProgram, fragShader);
    if(geoSrc != nullptr)
        glAttachShader(mProgram, geoShader);
    glLinkProgram(mProgram);
    glGetProgramiv(mProgram, GL_LINK_STATUS, &linked);
    if (!linked) {
        LOGE("===Could not link program");
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

