#include <AndroidUtils/AndroidHelper.h>
#include "Texture.h"

Texture::Texture(GLenum internalFormat, GLenum format, GLenum type, unsigned int width, unsigned int height)
	: mTexture(0), mInternalFormat(internalFormat), mFormat(format), mType(type), mWidth(width), mHeight(height), mDepth(0) {
	glGenTextures(1, &mTexture);
	glBindTexture(GL_TEXTURE_2D, mTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, mInternalFormat, mWidth, mHeight, 0, mFormat, mType, nullptr);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);
}
Texture::Texture(GLenum internalFormat, GLenum format, GLenum type, unsigned int width, unsigned int height, unsigned int depth)
	: mTexture(0), mInternalFormat(internalFormat), mFormat(format), mType(type), mWidth(width), mHeight(height), mDepth(depth) {
	glGenTextures(1, &mTexture);
	glBindTexture(GL_TEXTURE_3D, mTexture);
	glTexImage3D(GL_TEXTURE_3D, 0, mInternalFormat, mWidth, mHeight, mDepth, 0, mFormat, mType, nullptr);

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_3D, 0);
}

Texture::Texture(GLenum internalFormat, GLenum format, GLenum type, unsigned int width, unsigned int height, void* data)
	: mTexture(0), mInternalFormat(internalFormat), mFormat(format), mType(type), mWidth(width), mHeight(height), mDepth(0) {
	glGenTextures(1, &mTexture);
	glBindTexture(GL_TEXTURE_2D, mTexture);
//	glTexImage2D(GL_TEXTURE_2D, 0, mInternalFormat, mWidth, mHeight, 0, mFormat, mType, data);
	//	glTexImage3D(GL_TEXTURE_3D, 0, mInternalFormat, mWidth, mHeight, mDepth, 0, mFormat, mType, data);
	glTexStorage2D(GL_TEXTURE_2D, 1, mInternalFormat, mWidth, mHeight);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, mWidth, mHeight, mFormat, mType, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);

}
Texture::Texture(GLenum internalFormat, GLenum format, GLenum type, unsigned int width, unsigned int height, unsigned int depth, void* data)
	: mTexture(0), mInternalFormat(internalFormat), mFormat(format), mType(type), mWidth(width), mHeight(height), mDepth(depth) {
	glGenTextures(1, &mTexture);
	glBindTexture(GL_TEXTURE_3D, mTexture);

//	glTexImage3D(GL_TEXTURE_3D, 0, mInternalFormat, mWidth, mHeight, mDepth, 0, mFormat, mType, data);
    glTexStorage3D(GL_TEXTURE_3D, 1, mInternalFormat, mWidth, mHeight, mDepth);
    glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, mWidth, mHeight, mDepth, mFormat, mType, data);

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_3D, 0);
}

Texture::~Texture() {
	glDeleteTextures(1, &mTexture);
}
void Texture::initFBO(GLuint& fbo, Texture* colorTex, Texture* depthTex){
    GLenum err;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	//color attachment
	if(colorTex){
		glBindTexture(GL_TEXTURE_2D, colorTex->GLTexture());
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTex->GLTexture(), 0);
	}


	// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
	if(depthTex){
		GLuint rbo;
		glGenRenderbuffers(1, &rbo);
        if((err = glGetError()) != GL_NO_ERROR){
            LOGE("=====err0 %d, ",err);
        }
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        if((err = glGetError()) != GL_NO_ERROR){
            LOGE("=====err0.5 %d, ",err);
        }
		// use a single renderbuffer object for both a depth AND stencil buffer.
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, depthTex->Width(), depthTex->Height());
        if((err = glGetError()) != GL_NO_ERROR){
            LOGE("=====err1 %d, ",err);
        }
		// now actually attach it
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);
	}

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		LOGE("===ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}