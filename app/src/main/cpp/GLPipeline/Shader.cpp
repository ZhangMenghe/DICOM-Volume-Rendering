#include "Shader.h"
#include <string>
#include "AndroidUtils/AndroidHelper.h"
#include <sstream>
using namespace glm;
using namespace std;

Shader::~Shader() {
    for (const auto& p : mPrograms) {
        glDeleteProgram(p.second.mProgram);
        for (const auto& s : p.second.mShaders)
            glDeleteShader(s);
    }
}
bool Shader::AddShaderFile(GLenum type, const char* filename){
    string content;
    if (!assetLoader::instance()->LoadTextFileFromAssetManager(filename, &content)) {
        LOGE("====Failed to load file: %s", filename);
        return false;
    }
    mShadersToLink.emplace(type, content);
    return true;
}
bool Shader::CompileAndLink(){
    vector<vector<string>> keywords;
    keywords.push_back(vector<string>());
    keywords[0].push_back("");
    // create keyword variants
    for (const auto it : mShadersToLink) {
        istringstream iss(it.second);
        string line;
        while(getline(iss, line)){
            unsigned int kwc = keywords.size();
            stringstream ss(line);
            string token;
            int mode = 0;
            while (getline(ss, token, ' ')) {
                if (mode == 2) {
                    // create variants
                    for (unsigned int i = 0; i < kwc; i++) {
                        vector<string> k(keywords[i]);
                        k.push_back(token);
                        keywords.push_back(k);
                    }
                    mAvailableKeywords.insert(token);
                } else {
                    if (token == "#pragma")
                        mode = 1;
                    else if (mode == 1 && token == "multi_compile")
                        mode = 2;
                }
            }
        }
    }

    LOGI("===Compiling %d shader variants\n", (int)keywords.size());

    for (auto& it : keywords) {
        it.erase(it.begin());
        string kw = "";
        for (const auto& k : it)
            kw += k + " ";
        ShaderProgram cpr;
        if(!LinkShader(it, cpr))
            return false;
        mPrograms.emplace(kw, cpr);
    }
    return true;
}

GLuint Shader::Use(){
    string kw = "";
    for (const auto& k : mActiveKeywords)
        kw += k + " ";
    assert(mPrograms.count(kw));

    GLuint p = mPrograms.at(kw).mProgram;
    glUseProgram(p);
    return p;
}

GLuint CompileShader(GLenum type, string content, const vector<string>& keywords) {
    //construct key_word line
    string key_word_str = "";
    if(keywords[0].size()){
        key_word_str = "#define";
        for (const auto& kw : keywords){ key_word_str+=" " + kw;LOGI("====KEY WORDS: %s", kw.c_str());}
        key_word_str += "\r\n";

    }

    bool insertLine = false;
    string firstLine;
    istringstream iss(content);
    while(getline(iss, firstLine)){
        if (firstLine.substr(0, 8) == "#version"){
            content.insert(iss.tellg(), key_word_str);
            insertLine = true;
            break;
        }
    }
    if(!insertLine)
        content.insert(0, key_word_str);

    const char* src = content.c_str();

    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, 0);
    glCompileShader(shader);

    GLint isCompiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE) {
        GLint maxLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

        vector<GLchar> info(maxLength);
        glGetShaderInfoLog(shader, maxLength, &maxLength, info.data());

        string kw = "";
        for (const auto& it : keywords) kw += it + " ";
        printf("===Error compiling shader with keywords %s: ", kw.c_str());

        glDeleteShader(shader);
        return 0;
    } else {
        return shader;
    }
}

bool Shader::LinkShader(const std::vector<std::string>& keywords, ShaderProgram& pgm){
    // compile shaders with keywords
    for (const auto& it : mShadersToLink){
        GLuint shader_id = CompileShader(it.first, it.second, keywords);
        if(!shader_id)
            return false;
        pgm.mShaders.push_back(shader_id);
    }

    pgm.mProgram = glCreateProgram();
    for (const auto& s : pgm.mShaders)
        glAttachShader(pgm.mProgram, s);
    glLinkProgram(pgm.mProgram);

    GLint isLinked = 0;
    glGetProgramiv(pgm.mProgram, GL_LINK_STATUS, (int*)&isLinked);
    if (isLinked == GL_FALSE) {
        GLint maxLength = 0;
        glGetProgramiv(pgm.mProgram, GL_INFO_LOG_LENGTH, &maxLength);
        GLchar* info = (GLchar*)malloc(maxLength);
        glGetProgramInfoLog(pgm.mProgram, maxLength, &maxLength, info);
        LOGE("===Could not link program:\n%s\n", info);
        free(info);
        glDeleteProgram(pgm.mProgram);
        for (const auto& s : pgm.mShaders)
            glDeleteShader(s);
    } else {
        for (const auto& s : pgm.mShaders)
            glDetachShader(pgm.mProgram, s);
    }
    return true;
}