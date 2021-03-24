/*
	Copyright 2011-2020 Daniel S. Buckstein

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

		http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/

/*
	animal3D SDK: Minimal 3D Animation Framework
	By Daniel S. Buckstein
	
	passTexcoord_transform_instanced_vs4x.glsl
	Calculate final position and pass atlas texture coordinate for instance.
*/

#version 450

#define MAX_INSTANCES 256

layout (location = 0)	in vec4 aPosition;
layout (location = 8)	in vec4 aTexcoord;

uniform ubTransformMVP {
	mat4 uMVP[MAX_INSTANCES];
	mat4 uAtlas[MAX_INSTANCES];
};

out vec4 vTexcoord_atlas;

flat out int vVertexID;
flat out int vInstanceID;

void main()
{
	vTexcoord_atlas = uAtlas[gl_InstanceID] * aTexcoord;
	gl_Position = uMVP[gl_InstanceID] * aPosition;

	vVertexID = gl_VertexID;
	vInstanceID = gl_InstanceID;
}
