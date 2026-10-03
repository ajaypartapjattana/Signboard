#version 450

struct Camera {
    mat4 view;
    mat4 proj;
};

struct Object {
    mat4 model;
};

layout(set = 0, binding = 0) readonly buffer CameraBuffer {
    Camera cameras[];
};

layout(set = 1, binding = 0) readonly buffer ObjectBuffer {
    Object objects[];
};

layout(push_constant) uniform PushConstants {
    uint cameraIndex;
};

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inUV;

layout(location = 0) out vec2 fragUV;

void main() {
    Camera camera = cameras[cameraIndex];
    Object object = objects[gl_InstanceIndex];

    gl_Position = camera.proj * camera.view * object.model * vec4(inPosition, 1.0);
    fragUV = inUV;
}