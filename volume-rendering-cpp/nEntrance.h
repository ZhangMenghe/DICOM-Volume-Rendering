#ifndef NATIVE_ENTRANCE_H
#define NATIVE_ENTRANCE_H

class nEntrance{
public:
    virtual void onViewCreated() = 0;
    virtual void onViewChange(int width, int height) = 0;
    virtual void onViewChange(int rot, int width, int height) = 0;
    virtual void onDraw()=0;
    virtual void onReset()=0;
    virtual void onPause()=0;
    virtual void onDestroy()=0;
    virtual void onResume(void* env, void* context, void* activity)=0;

    virtual void onSingleTouchDown(float x, float y)=0;
    virtual void onTouchMove(float x, float y) = 0;
    virtual void onScale(float sx, float sy) = 0;
    virtual void onPan(float x, float y) = 0;
};
#endif