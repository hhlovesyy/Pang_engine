#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 projection;
uniform mat4 view;

out vec3 WorldPos;

void main()
{
    WorldPos = aPos;

	mat4 rotView = mat4(mat3(view));  //去掉平移带来的影响，相机的平移分量被忽略掉了，在立方体上渲染此着色器会得到非移动状态下的环境贴图。
	vec4 clipPos = projection * rotView * vec4(WorldPos, 1.0);

	gl_Position = clipPos.xyww; //注意这里的小技巧 xyww 可以确保渲染的立方体片段的深度值总是 1.0，即最大深度，这样天空盒就是最靠后的
}