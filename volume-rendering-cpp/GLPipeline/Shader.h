#ifndef GLPIPELINE_SHADER_H
#define GLPIPELINE_SHADER_H

#include <platforms/platform.h>
#include <glm/glm.hpp>

#include <unordered_set>
#include <unordered_map>
#include <string>
#include <vector>
typedef struct  {
	GLuint mProgram;
	std::vector<GLuint> mShaders;
}ShaderProgram;

class Shader {
public:
	Shader(){}
	~Shader();
//	bool AddShaderFile(GLenum type, const char* filename);
	bool AddShader(GLenum type, std::string content){
		if(content.empty()) return false;
		mShadersToLink.emplace(type, content);
		return true;
	}
	bool CompileAndLink();

	GLuint Use();
	void UnUse(){glUseProgram(0);}

	void EnableKeyword(std::string keyword);
	void DisableKeyword(std::string keyword);

	// set int
	static void Uniform(GLuint program, const GLchar* name, int x){glUniform1i(glGetUniformLocation(program, name), x);}
	//set uint
	static void Uniform(GLuint program, const GLchar* name, unsigned int x){glUniform1ui(glGetUniformLocation(program, name), x);}
	//set float
	static void Uniform(GLuint program, const GLchar* name, float x){glUniform1f(glGetUniformLocation(program, name), x);}
	//set bool
    static void Uniform(GLuint program, const GLchar* name, bool x){glUniform1i(glGetUniformLocation(program, name), (int)x);}
	//set vec2
    static void Uniform(GLuint program, const GLchar* name, float x, float y){glUniform2f(glGetUniformLocation(program, name), x, y);}
    static void Uniform(GLuint program, const GLchar* name, const glm::vec2& v){glUniform2f(glGetUniformLocation(program, name), v.x, v.y);}
	//set vec3
	static void Uniform(GLuint program, const GLchar* name, const glm::vec3& v){glUniform3f(glGetUniformLocation(program, name), v.x, v.y, v.z);}
	//set vec4
	static void Uniform(GLuint program, const GLchar* name, const glm::vec4& v){glUniform4f(glGetUniformLocation(program, name), v.x, v.y, v.z, v.w);}
	//set mat4
	static void Uniform(GLuint program, const GLchar* name, const glm::mat4& m){glUniformMatrix4fv(glGetUniformLocation(program, name), 1, GL_FALSE, &m[0][0]);}

private:
	std::vector<std::vector<std::string>> available_keywords_;
	std::vector<std::unordered_set<std::string>> active_keywords_;

//	std::unordered_set<std::string> mAvailableKeywords;
//	std::unordered_set<std::string> mActiveKeywords;

	// indexed by keyword combo
	std::unordered_map<std::string, ShaderProgram> mPrograms;

	std::unordered_map<GLenum, std::string> mShadersToLink;

	bool LinkShader(std::vector<std::string> keywords, ShaderProgram& pgm);
};

#endif