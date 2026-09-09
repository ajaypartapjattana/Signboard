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

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

void main() {
    Camera camera = cameras[cameraIndex];
    Object object = objects[gl_InstanceIndex];

    gl_Position = camera.proj * camera.view * object.model * vec4(inPosition, 0.0, 1.0);
    fragColor = inColor;
}