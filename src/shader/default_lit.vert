#version 330 core
out vec3 pos;
out vec3 normal;
layout(location = 0) in vec3 posIn;
layout(location = 1) in vec3 normalIn;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main() {
    pos = vec3(model * vec4(posIn, 1.0));
    normal = mat3(transpose(inverse(model))) * normalIn;
    gl_Position = projection * view * model * vec4(posIn, 1.0);
}
