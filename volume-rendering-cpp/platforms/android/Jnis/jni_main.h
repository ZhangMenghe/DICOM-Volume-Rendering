#ifndef JNI_MAIN_H
#define JNI_MAIN_H

#include <jni.h>
#include <string>
#include "nEntrance.h"
#include <GLPipeline/Camera.h>

#define JNI_METHOD(returnType, funcName)\
    JNIEXPORT returnType JNICALL        \
        Java_helmsley_vr_JNIInterface_##funcName

#define JUI_METHOD(returnType, funcName)\
    JNIEXPORT returnType JNICALL        \
        Java_helmsley_vr_DUIs_JUIInterface_##funcName

namespace dvr{
    // maintain a reference to the JVM so we can use it later.
    inline static JavaVM *g_vm = nullptr;

    //native address to vr application
    inline jlong nativeAddr;

    //Camera instances
    inline Camera virtualCam("virtual"), arCam("ar");
    //model
    struct ModelMatStatus{
        glm::mat4 modelMat, rotMat;
        glm::vec3 scaleVec, posVec;
        ModelMatStatus(glm::mat4 rot_mat, glm::vec3 s, glm::vec3 p){
            ModelMatStatus(glm::translate(glm::mat4(1.0), p)
                         * rot_mat
                         * glm::scale(glm::mat4(1.0), s),
                         rot_mat, s, p);
        }
        ModelMatStatus(glm::mat4 model_mat, glm::mat4 rot_mat, glm::vec3 s, glm::vec3 p){
            modelMat = model_mat;
            rotMat = rot_mat;
            scaleVec = s;
            posVec = p;
        }
    };

    inline ModelMatStatus virtualMMS(glm::mat4(1.0), glm::vec3(1.0f, 1.0f,0.5f), glm::vec3(.0f)),
    arMMS(glm::mat4(1.0), glm::vec3(0.2f), glm::vec3(.0f));

    inline jlong getNativeClassAddr(nEntrance * native_controller){
        return reinterpret_cast<intptr_t>(native_controller);
    }
    inline nEntrance * nativeApp(jlong ptr){
        return reinterpret_cast<nEntrance *>(ptr);
    }

    inline std::string jstring2string(JNIEnv *env, jstring jStr){
        const char *cstr = env->GetStringUTFChars(jStr, NULL);
        std::string str = std::string(cstr);
        env->ReleaseStringUTFChars(jStr, cstr);
        return str;
    }
};
#endif