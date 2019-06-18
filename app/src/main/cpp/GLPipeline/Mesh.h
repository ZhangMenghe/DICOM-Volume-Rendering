#ifndef GL_MESH_H
#define GL_MESH_H
// Include the latest possible header file( GL version header )
#if __ANDROID_API__ >= 24
#include <GLES3/gl32.h>
#elif __ANDROID_API__ >= 21
#include <GLES3/gl31.h>
#else
#include <GLES3/gl3.h>
#endif

class Mesh {
public:
	Mesh();
	~Mesh();

	void BindVAO() const;
	void BindVBO() const;
	void BindIBO() const;
	void unBind() const;

	inline void ElementCount(unsigned int x) {
		mElementCount = x;
	}
	inline unsigned int ElementCount() const {
		return mElementCount;
	}
	static void InitQuadWithTex(GLuint &vao, float* vertices, int vertex_num, unsigned int* indices, int indice_num);
private:
	unsigned int mElementCount;
	GLuint mVAO;
	GLuint mVBO;
	GLuint mIBO;
};

#endif