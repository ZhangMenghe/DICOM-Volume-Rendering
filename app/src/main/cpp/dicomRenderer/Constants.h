#ifndef DICOMRENDERER_CONSTANTS_H
#define DICOMRENDERER_CONSTANTS_H
namespace dvr{
    enum PARAM_BOOL{
        CHECK_RAYCAST=0,
        CHECK_COLOR_TRANS,
        CHECK_CUTTING,
        CHECK_MASKON,
        CHECK_FREEZE_VOLUME,
        CHECK_FREEZE_CPLANE
    };
    enum PARAM_TEX{
        TUNE_OVERALL=0,
        TUNE_LOWEST,
        TUNE_CUTOFF,
        TUNE_CUTTING_TEX
    };
    enum PARAM_RAYCAST{
        TUNE_DENSITY = 0,
        TUNE_CONTRAST,
        TUNE_BRIGHT,
        TUNE_CUTTING_RAY
    };

    //UIs
    const float MOUSE_ROTATE_SENSITIVITY = 0.005f;
    const float MOUSE_SCALE_SENSITIVITY = 0.8f;
    const float MOUSE_PAN_SENSITIVITY = 1.2f;

    //TRS
    const glm::mat4 DEFAULT_ROTATE = glm::mat4(1.0f);
    const glm::vec3 DEFAULT_SCALE = glm::vec3(1.0f, 1.0f,0.5f);
    const glm::vec3 DEFAULT_POS = glm::vec3(.0f);

}
#endif