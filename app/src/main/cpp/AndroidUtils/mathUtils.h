#ifndef DICOM_VOLUME_RENDERING_MATHUTILS_H
#define DICOM_VOLUME_RENDERING_MATHUTILS_H
inline void getScreenToClientPos(float &x, float &y, float sw, float sh) {
    x = (2.0 * x - sw * 0.5f) / sw;
    y = 2.0f * (sh * 0.5f - y) / sh;
}
#endif //DICOM_VOLUME_RENDERING_MATHUTILS_H
