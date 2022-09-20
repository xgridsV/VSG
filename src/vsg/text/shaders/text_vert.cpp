#include <vsg/io/VSG.h>
static auto text_vert = []() {std::istringstream str(
R"(#vsga 0.5.4
Root id=1 vsg::ShaderStage
{
  userObjects 0
  stage 1
  entryPointName "main"
  module id=2 vsg::ShaderModule
  {
    userObjects 0
    hints id=0
    source "#version 450
#extension GL_ARB_separate_shader_objects : enable

#pragma import_defines (GPU_LAYOUT, CPU_LAYOUT, BILLBOARD)

#ifdef GPU_LAYOUT

    // GPU layout provides computes vertex values based on layout computed in the vertex shader
    #define GLYPH_DIMENSIONS 0
    #define GLYPH_BEARINGS 1
    #define GLYPH_UVRECT 2

    // specialization constants
    layout(constant_id = 0) const uint numTextIndices = 256;

    layout(set = 0, binding = 1) uniform sampler2D glyphMetricsSampler;

    layout(set = 1, binding = 0) uniform TextLayout {
        vec4 position;
        vec4 horizontal;
        vec4 vertical;
        vec4 color;
        vec4 outlineColor;
        float outlineWidth;
    } textLayout;

    layout(set = 1, binding = 1) uniform TextIndices {
        uvec4 glyph_index[numTextIndices];
    } text;

#else

    // CPU layout provides all vertex data
    layout(location = 1) in vec4 inColor;
    layout(location = 2) in vec4 inOutlineColor;
    layout(location = 3) in float inOutlineWidth;
    layout(location = 4) in vec3 inTexCoord;

    #ifdef BILLBOARD
    layout(location = 5) in vec4 inCenterAndAutoScaleDistance;
    #endif

#endif



layout(push_constant) uniform PushConstants {
    mat4 projection;
    mat4 modelview;
} pc;

layout(location = 0) in vec3 inPosition;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 outlineColor;
layout(location = 2) out float outlineWidth;
layout(location = 3) out vec2 fragTexCoord;

out gl_PerVertex {
    vec4 gl_Position;
};

void main() {
#ifdef GPU_LAYOUT
    // compute the position of the glyph
    float horiAdvance = 0.0;
    float vertAdvance = 0.0;
    for(uint i=0; i<gl_InstanceIndex; ++i)
    {
        uint glyph_index = text.glyph_index[i / 4][i % 4];
        if (glyph_index==0)
        {
            // treat as a newlline
            vertAdvance -= 1.0;
            horiAdvance = 0.0;
        }
        else
        {
            horiAdvance += texture(glyphMetricsSampler, vec2(GLYPH_DIMENSIONS, glyph_index))[2];
        }
    }
    vec3 cursor = textLayout.position.xyz + textLayout.horizontal.xyz * horiAdvance + textLayout.vertical.xyz * vertAdvance;

    // compute the position of vertex
    uint glyph_index = text.glyph_index[gl_InstanceIndex / 4][gl_InstanceIndex % 4];

    vec4 dimensions = texture(glyphMetricsSampler, vec2(GLYPH_DIMENSIONS, glyph_index));
    vec4 bearings = texture(glyphMetricsSampler, vec2(GLYPH_BEARINGS, glyph_index));
    vec4 uv_rec = texture(glyphMetricsSampler, vec2(GLYPH_UVRECT, glyph_index));

    vec3 pos = cursor + textLayout.horizontal.xyz * (bearings.x + inPosition.x * dimensions.x) + textLayout.vertical.xyz * (bearings.y + (inPosition.y-1.0) * dimensions.y);

    gl_Position = (pc.projection * pc.modelview) * vec4(pos, 1.0);
    gl_Position.z -= inPosition.z*0.001;

    fragColor = textLayout.color;
    outlineColor = textLayout.outlineColor;
    outlineWidth = textLayout.outlineWidth;
    fragTexCoord = vec2(mix(uv_rec[0], uv_rec[2], inPosition.x), mix(uv_rec[1], uv_rec[3], inPosition.y));
#else
    // CPU layout provides all vertex data
    #ifdef BILLBOARD
    vec4 center_object = vec4(inCenterAndAutoScaleDistance.xyz, 1.0);
    float autoScaleDistance = inCenterAndAutoScaleDistance.w;

    vec4 center_eye = pc.modelview * center_object;
    float distance = -center_eye.z;

    float scale = (distance < autoScaleDistance) ? distance/autoScaleDistance : 1.0;
    mat4 S = mat4(scale, 0.0, 0.0, 0.0,
                  0.0, scale, 0.0, 0.0,
                  0.0, 0.0, scale, 0.0,
                  0.0, 0.0, 0.0, 1.0);

    mat4 T = mat4(1.0, 0.0, 0.0, 0.0,
                  0.0, 1.0, 0.0, 0.0,
                  0.0, 0.0, 1.0, 0.0,
                  center_eye.x, center_eye.y, center_eye.z, 1.0);

    gl_Position = (pc.projection * (T*S)) * vec4(inPosition, 1.0);
    #else
    gl_Position = (pc.projection * pc.modelview) * vec4(inPosition, 1.0);
    #endif

    gl_Position.z -= inTexCoord.z*0.001;
    fragColor = inColor;
    outlineColor = inOutlineColor;
    outlineWidth = inOutlineWidth;
    fragTexCoord = inTexCoord.xy;
#endif

}
"
    code 490
     119734787 65536 524298 64 0 131089 1 393227 1 1280527431 1685353262 808793134
     0 196622 0 1 983055 0 4 1852399981 0 10 26 36
     49 51 53 54 56 57 61 196611 2 450 589828 1096764487
     1935622738 1918988389 1600484449 1684105331 1868526181 1667590754 29556 262149 4 1852399981 0 393221
     8 1348430951 1700164197 2019914866 0 393222 8 0 1348430951 1953067887 7237481 196613
     10 0 393221 14 1752397136 1936617283 1953390964 115 393222 14 0 1785688688
     1769235301 28271 393222 14 1 1701080941 1701410412 119 196613 16 25456 327685
     26 1867542121 1769236851 28271 327685 36 1700032105 1869562744 25714 327685 49 1734439526
     1869377347 114 262149 51 1866690153 7499628 393221 53 1819571567 1130720873 1919904879 0
     393221 54 1968139881 1852402804 1819231077 29295 393221 56 1819571567 1466265193 1752458345 0
     393221 57 1968139881 1852402804 1684625253 26740 393221 61 1734439526 1131963732 1685221231 0
     327752 8 0 11 0 196679 8 2 262216 14 0 5
     327752 14 0 35 0 327752 14 0 7 16 262216 14
     1 5 327752 14 1 35 64 327752 14 1 7 16
     196679 14 2 262215 26 30 0 262215 36 30 4 262215
     49 30 0 262215 51 30 1 262215 53 30 1 262215
     54 30 2 262215 56 30 2 262215 57 30 3 262215
     61 30 3 131091 2 196641 3 2 196630 6 32 262167
     7 6 4 196638 8 7 262176 9 3 8 262203 9
     10 3 262165 11 32 1 262187 11 12 0 262168 13
     7 4 262174 14 13 13 262176 15 9 14 262203 15
     16 9 262176 17 9 13 262187 11 20 1 262167 24
     6 3 262176 25 1 24 262203 25 26 1 262187 6
     28 1065353216 262176 34 3 7 262203 25 36 1 262165 37
     32 0 262187 37 38 2 262176 39 1 6 262187 6
     42 981668463 262176 44 3 6 262203 34 49 3 262176 50
     1 7 262203 50 51 1 262203 34 53 3 262203 50
     54 1 262203 44 56 3 262203 39 57 1 262167 59
     6 2 262176 60 3 59 262203 60 61 3 327734 2
     4 0 3 131320 5 327745 17 18 16 12 262205 13
     19 18 327745 17 21 16 20 262205 13 22 21 327826
     13 23 19 22 262205 24 27 26 327761 6 29 27
     0 327761 6 30 27 1 327761 6 31 27 2 458832
     7 32 29 30 31 28 327825 7 33 23 32 327745
     34 35 10 12 196670 35 33 327745 39 40 36 38
     262205 6 41 40 327813 6 43 41 42 393281 44 45
     10 12 38 262205 6 46 45 327811 6 47 46 43
     393281 44 48 10 12 38 196670 48 47 262205 7 52
     51 196670 49 52 262205 7 55 54 196670 53 55 262205
     6 58 57 196670 56 58 262205 24 62 36 458831 59
     63 62 62 0 1 196670 61 63 65789 65592
  }
  NumSpecializationConstants 0
}
)");
vsg::VSG io;
return io.read_cast<vsg::ShaderStage>(str);
};
