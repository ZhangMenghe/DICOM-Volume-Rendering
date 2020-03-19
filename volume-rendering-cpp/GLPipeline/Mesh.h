#ifndef GLPIPELINE_MESH_H
#define GLPIPELINE_MESH_H

#include <platforms/platform.h>
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
	static void InitQuadWithTex(GLuint &vao, GLuint &vbo, int vertex_num,
			const unsigned int* indices, int indice_num);
	static void InitQuadWithTex(GLuint &vao, const float* vertices,
								int vertex_num, const unsigned int* indices, int indice_num);
	static void InitQuad(GLuint &vao, const float* vertices, int vertex_num,
			const unsigned int* indices, int indice_num);
	static void InitQuad(GLuint &vao, GLuint &vbo, const unsigned int* indices, int indice_num);
private:
	unsigned int mElementCount;
	GLuint mVAO;
	GLuint mVBO;
	GLuint mIBO;
};

#endif