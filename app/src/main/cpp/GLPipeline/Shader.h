#ifndef GL_SHADER_H
#define GL_SHADER_H

// Include the latest possible header file( GL version header )
#if __ANDROID_API__ >= 24
#include <GLES3/gl32.h>
#elif __ANDROID_API__ >= 21
#include <GLES3/gl31.h>
#else
#include <GLES3/gl3.h>
#endif
#include <glm/glm.hpp>

class Shader {
public:
	~Shader();

	// global utility functions
// utility uniform functions
	// ------------------------------------------------------------------------
	void setBool(const char* name, bool value) const{
		glUniform1i(glGetUniformLocation(mProgram, name), (int)value);
	}
	// ------------------------------------------------------------------------
	void setInt(const char* name, int value) const{
		glUniform1i(glGetUniformLocation(mProgram, name), value);
	}
	// ------------------------------------------------------------------------
	void setFloat(const char*  name, float value) const{
		glUniform1f(glGetUniformLocation(mProgram, name), value);
	}
	// ------------------------------------------------------------------------
	void setVec2(const char*  name, const glm::vec2 &value) const{
		glUniform2fv(glGetUniformLocation(mProgram, name), 1, &value[0]);
	}
	void setVec2(const char*  name, float x, float y) const{
		glUniform2f(glGetUniformLocation(mProgram, name), x, y);
	}
	// ------------------------------------------------------------------------
	void setVec3(const char*  name, const glm::vec3 &value) const{
		glUniform3fv(glGetUniformLocation(mProgram, name), 1, &value[0]);
	}
	void setVec3(const char*  name, float x, float y, float z) const{
		glUniform3f(glGetUniformLocation(mProgram, name), x, y, z);
	}
	// ------------------------------------------------------------------------
	void setVec4(const char*  name, const glm::vec4 &value) const{
		glUniform4fv(glGetUniformLocation(mProgram, name), 1, &value[0]);
	}
	void setVec4(const char*  name, float x, float y, float z, float w){
		glUniform4f(glGetUniformLocation(mProgram, name), x, y, z, w);
	}
	// ------------------------------------------------------------------------
	void setMat2(const char*  name, const glm::mat2 &mat) const{
		glUniformMatrix2fv(glGetUniformLocation(mProgram, name), 1, GL_FALSE, &mat[0][0]);
	}
	// ------------------------------------------------------------------------
	void setMat3(const char*  name, const glm::mat3 &mat) const{
		glUniformMatrix3fv(glGetUniformLocation(mProgram, name), 1, GL_FALSE, &mat[0][0]);
	}
	// ------------------------------------------------------------------------
	void setMat4(const char*  name, const glm::mat4 &mat) const{
		glUniformMatrix4fv(glGetUniformLocation(mProgram, name), 1, GL_FALSE, &mat[0][0]);
	}

	void Use(){glUseProgram(mProgram);}
	void unUse(){glUseProgram(0);}
    bool Create(const char* vert_file, const char *_frag_file);
private:
    GLuint mProgram;
    bool check_gl_error(const char *funcName);
    bool create_program(const char* vtxSrc, const char* fragSrc);
    GLuint create_shader(GLenum shaderType, const char *pSource);
};

#endif