#ifndef DICOM_VOLUME_RENDERING_MATHUTILS_H
#define DICOM_VOLUME_RENDERING_MATHUTILS_H
inline void getScreenToClientPos(float &x, float &y, float sw, float sh) {
    x = (2.0 * x - sw * 0.5f) / sw;
    y = 2.0f * (sh * 0.5f - y) / sh;
}
inline float shortest_distance(float x1, float y1,
                               float z1, float a,
                               float b, float c,
                               float d){
    d = fabs((a * x1 + b * y1 +
              c * z1 + d));
    float e = sqrt(a * a + b *
                           b + c * c);
    return d/e;
}
#endif //DICOM_VOLUME_RENDERING_MATHUTILS_H
