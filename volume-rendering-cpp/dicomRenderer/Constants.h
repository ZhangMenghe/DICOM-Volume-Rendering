#ifndef DICOMRENDERER_CONSTANTS_H
#define DICOMRENDERER_CONSTANTS_H
#include <string>
namespace dvr{
    enum PARAM_BOOL{
        CHECK_RAYCAST=0,
        CHECK_CUTTING,
        CHECK_FREEZE_VOLUME,
        CHECK_FREEZE_CPLANE,
        CHECK_MASKON,
    };
    enum PARAM_TEX{
        TT_OVERALL=0,
        TT_LOWEST,
        TT_CUTOFF,
        TT_CUTTING_TEX
    };
    enum PARAM_RAYCAST{
        TR_DENSITY = 0,
        TR_OVERALL,
        TR_LOWEST,
        TR_CUTOFF,
        TR_CUTTING_RAY
    };
    enum SHADER_FILES{
        SHADER_TEXTUREVOLUME_VERT = 0,
        SHADER_TEXTUREVOLUME_FRAG,
        SHADER_RAYCASTVOLUME_VERT,
        SHADER_RAYCASTVOLUME_FRAG,
        SHADER_RAYCASTCOMPUTE_GLSL,
        SHADER_RAYCASTVOLUME_GLSL,
        SHADER_QUAD_VERT,
        SHADER_QUAD_FRAG,
        SHADER_CPLANE_VERT,
        SHADER_CPLANE_FRAG,
        SHADER_COLOR_VIZ_VERT,
        SHADER_COLOR_VIZ_FRAG,
        SHADER_OPA_VIZ_VERT,
        SHADER_OPA_VIZ_FRAG,
        SHADER_END
    };
    enum TEX_IDS{
        BAKED_TEX_ID=0,
        SCREEN_QUAD_TEX_ID,
    };

    //UIs
    const float MOUSE_ROTATE_SENSITIVITY = 0.005f;
    const float MOUSE_SCALE_SENSITIVITY = 0.8f;
    const float MOUSE_PAN_SENSITIVITY = 1.2f;

    //TRS
    const glm::mat4 DEFAULT_ROTATE = glm::mat4(1.0f);
    const glm::vec3 DEFAULT_SCALE = glm::vec3(1.0f, 1.0f,0.5f);
    const glm::vec3 DEFAULT_POS = glm::vec3(.0f);

    //color scheme
    constexpr char* COLOR_SCHEMES[3]={"COLOR_GRAYSCALE", "COLOR_HSV", "COLOR_BRIGHT"};
}
#endif