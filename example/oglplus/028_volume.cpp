/**
 *  @example oglplus/028_volume.cpp
 *  @brief Shows volumetric data polygonization
 *
 *  @image html 028_volume.png
 *
 *  Copyright 2008-2012 Matus Chochlik. Distributed under the Boost
 *  Software License, Version 1.0. (See accompanying file
 *  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#include <oglplus/gl.hpp>
#include <oglplus/all.hpp>

#include <oglplus/bound/texture.hpp>

#include <oglplus/images/cloud.hpp>

#include <oglplus/shapes/tetrahedrons.hpp>
#include <oglplus/shapes/plane.hpp>
#include <oglplus/shapes/wrapper.hpp>

#include <cmath>

#include "example.hpp"

namespace oglplus {

class VolumeVertShader
 : public VertexShader
{
public:
	VolumeVertShader(void)
	 : VertexShader(
		"Volume vertex shader",
		"#version 330\n"
		"uniform sampler3D VolumeTex;"
		"uniform float Threshold;"
		"uniform float GridStep;"

		"in vec4 Position;"

		"out vec3 vertGradient;"
		"out float vertValue;"

		"void main(void)"
		"{"
		"	gl_Position = Position;"
		"	float Density = texture(VolumeTex, Position.xyz).r;"
		"	vertValue = Density - Threshold;"
		"	vertGradient = vec3(0.0, 0.0, 0.0);"
		"	for(int z=-1; z!=2; ++z)"
		"	for(int y=-1; y!=2; ++y)"
		"	for(int x=-1; x!=2; ++x)"
		"	{"
		"		vec3 Offs = vec3(GridStep*x, GridStep*y, GridStep*z);"
		"		vec3 Coord = Position.xyz + Offs;"
		"		float Diff = Density - texture(VolumeTex, Coord).r;"
		"		vertGradient += Diff * Offs;"
		"	}"
		"}"
	)
	{ }
};

class VolumeGeomShader
 : public GeometryShader
{
public:
	VolumeGeomShader(void)
	 : GeometryShader(
		"Volume geometry shader",
		"#version 330\n"
		"layout(triangles_adjacency) in;"
		"layout(triangle_strip, max_vertices = 4) out;"

		"uniform mat4 TransformMatrix;"
		"uniform vec3 CameraPosition, LightPosition;"

		"in vec3 vertGradient[];"
		"in float vertValue[];"

		"out vec3 geomNormal, geomLightDir, geomViewDir;"

		"void do_nothing(void){ };"

		"float find_t(int i1, int i2)"
		"{"
		"	float d = vertValue[i2] - vertValue[i1];"
		"	if(d <= 0.0) return 0.0;"
		"	else return -vertValue[i1]/d;"
		"}"

		"void make_vertex(int i1, int i2)"
		"{"
		"	float t = find_t(i1, i2);"
		"	gl_Position = mix("
		"		gl_in[i1].gl_Position,"
		"		gl_in[i2].gl_Position,"
		"		t"
		"	);"
		"	geomNormal = mix("
		"		vertGradient[i1], "
		"		vertGradient[i2], "
		"		t"
		"	);"
		"	geomLightDir = LightPosition - gl_Position.xyz;"
		"	geomViewDir = CameraPosition - gl_Position.xyz;"
		"	gl_Position = TransformMatrix * gl_Position;"
		"	EmitVertex();"
		"}"

		"void make_triangle(int a1, int a2, int b1, int b2, int c1, int c2)"
		"{"
		"	make_vertex(a1, a2);"
		"	make_vertex(b1, b2);"
		"	make_vertex(c1, c2);"
		"	EndPrimitive();"
		"}"

		"void make_quad(int a1,int a2,int b1,int b2,int c1,int c2,int d1,int d2)"
		"{"
		"	make_vertex(a1, a2);"
		"	make_vertex(b1, b2);"
		"	make_vertex(c1, c2);"
		"	make_vertex(d1, d2);"
		"	EndPrimitive();"
		"}"

		"void process_tetrahedron(int a, int b, int c, int d)"
		"{"
		"	if(vertValue[a] >= 0.0)"
		"	{"
		"		if(vertValue[b] >= 0.0)"
		"		{"
		"			if(vertValue[c] >= 0.0)"
		"			{"
		"				if(vertValue[d] >= 0.0)"
		"					do_nothing();"
		"				else make_triangle(d,a, d,b, d,c);"
		"			}"
		"			else"
		"			{"
		"				if(vertValue[d] >= 0.0)"
		"					make_triangle(c,a, c,d, c,b);"
		"				else make_quad(c,a, d,a, c,b, d,b);"
		"			}"
		"		}"
		"		else"
		"		{"
		"			if(vertValue[c] >= 0.0)"
		"			{"
		"				if(vertValue[d] >= 0.0)"
		"					make_triangle(b,c, b,d, b,a);"
		"				else make_quad(b,c, d,c, b,a, d,a);"
		"			}"
		"			else"
		"			{"
		"				if(vertValue[d] >= 0.0)"
		"					make_quad(c,a, c,d, b,a, b,d);"
		"				else make_triangle(c,a, d,a, b,a);"
		"			}"
		"		}"
		"	}"
		"	else"
		"	{"
		"		if(vertValue[b] >= 0.0)"
		"		{"
		"			if(vertValue[c] >= 0.0)"
		"			{"
		"				if(vertValue[d] >= 0.0)"
		"					make_triangle(a,b, a,d, a,c);"
		"				else make_quad(a,c, a,b, d,c, d,b);"
		"			}"
		"			else"
		"			{"
		"				if(vertValue[d] >= 0.0)"
		"					make_quad(a,b, a,d, c,b, c,d);"
		"				else make_triangle(a,b, d,b, c,b);"
		"			}"
		"		}"
		"		else"
		"		{"
		"			if(vertValue[c] >= 0.0)"
		"			{"
		"				if(vertValue[d] >= 0.0)"
		"					make_quad(b,c, b,d, a,c, a,d);"
		"				else make_triangle(b,c, d,c, a,c);"
		"			}"
		"			else"
		"			{"
		"				if(vertValue[d] >= 0.0)"
		"					make_triangle(a,d, c,d, b,d);"
		"				else do_nothing();"
		"			}"
		"		}"
		"	}"
		"}"

		"void main(void)"
		"{"
		"	process_tetrahedron(0, 2, 4, 1);"
		"}"
	)
	{ }
};

class VolumeFragShader
 : public FragmentShader
{
public:
	VolumeFragShader(void)
	 : FragmentShader(
		"Volume fragment shader",
		"#version 330\n"

		"in vec3 geomNormal, geomLightDir, geomViewDir;"

		"out vec4 fragColor;"

		"void main(void)"
		"{"
		"	vec3 Normal = normalize(geomNormal);"
		"	vec3 LightDir = normalize(geomLightDir);"
		"	vec3 ViewDir = normalize(geomViewDir);"
		"	vec3 ViewRefl = reflect(-ViewDir, Normal);"
		"	vec3 Color = abs(vec3(1.0, 1.0, 1.0) - Normal);"

		"	float Ambient = 0.3;"
		"	float Diffuse = max(dot(Normal, LightDir), 0.0);"
		"	float Specular = pow(max(dot(ViewRefl, LightDir), 0.0), 16.0);"
		"	fragColor = vec4("
		"		Ambient * vec3(0.2, 0.1, 0.1)+"
		"		Diffuse * Color +"
		"		Specular* vec3(1.0, 1.0, 1.0),"
		"		1.0"
		"	);"
		"}"
	)
	{ }
};

class VolumeProgram
 : public HardwiredProgram<VolumeVertShader, VolumeGeomShader, VolumeFragShader>
{
private:
	const Program& prog(void) const { return *this; }
public:
	ProgramUniform<GLfloat> threshold, grid_step;
	ProgramUniform<Mat4f> transform_matrix;
	ProgramUniform<Vec3f> camera_position, light_position;
	ProgramUniformSampler volume_tex;

	VolumeProgram(void)
	 : HardwiredProgram<VolumeVertShader, VolumeGeomShader, VolumeFragShader>()
	 , threshold(prog(), "Threshold")
	 , grid_step(prog(), "GridStep")
	 , transform_matrix(prog(), "TransformMatrix")
	 , camera_position(prog(), "CameraPosition")
	 , light_position(prog(), "LightPosition")
	 , volume_tex(prog(), "VolumeTex")
	{ }
};

class Grid
{
protected:
	const size_t grid_div;
	shapes::Tetrahedrons make_grid;
	shapes::DrawingInstructions grid_instr;
	typename shapes::Tetrahedrons::IndexArray grid_indices;

	Context gl;

	// A vertex array object for the rendered shape
	VertexArray vao;

	// VBO for the grids's vertex positions
	Buffer vbo;

public:
	Grid(const Program& prog)
	 : grid_div(64)
	 , make_grid(1.0, grid_div)
	 , grid_instr(make_grid.InstructionsWithAdjacency())
	 , grid_indices(make_grid.IndicesWithAdjacency())
	{
		// bind the VAO for the shape
		vao.Bind();

		std::vector<GLfloat> data;
		// bind the VBO for the vertex attribute
		vbo.Bind(Buffer::Target::Array);
		GLuint n_per_vertex = make_grid.Positions(data);
		// upload the data
		Buffer::Data(Buffer::Target::Array, data);
		// setup the vertex attribs array
		VertexAttribArray attr(prog, "Position");
		attr.Setup(n_per_vertex, DataType::Float);
		attr.Enable();
	}

	double Step(void) const
	{
		return 1.0 / grid_div;
	}

	void Use(void)
	{
		vao.Bind();
	}

	void Draw(void)
	{
		grid_instr.Draw(grid_indices);
	}
};

class VolumeExample : public Example
{
private:

	// wrapper around the current OpenGL context
	Context gl;

	VolumeProgram volume_prog;

	Grid grid;

	size_t width, height;

	// A 3D texture containing density data
	Texture volume_tex;
public:
	VolumeExample(void)
	 : volume_prog()
	 , grid(volume_prog)
	{
		volume_prog.grid_step = grid.Step();

		std::srand(3456);
		Texture::Active(0);
		volume_prog.volume_tex = 0;
		{
			auto bound_tex = Bind(volume_tex, Texture::Target::_3D);
			bound_tex.Image3D(
				images::Cloud(
					128, 128, 128,
					Vec3f(0.0f, 0.0f, 0.0f),
					0.5f, 0.5f, 0.5f, 0.1f
				)
			);
			bound_tex.MinFilter(TextureMinFilter::Linear);
			bound_tex.MagFilter(TextureMagFilter::Linear);
			bound_tex.BorderColor(Vec4f(0.0f, 0.0f, 0.0f, 0.0f));
			bound_tex.WrapS(TextureWrap::ClampToBorder);
			bound_tex.WrapT(TextureWrap::ClampToBorder);
			bound_tex.WrapR(TextureWrap::ClampToBorder);
		}

		const Vec3f light_position(12.0, 1.0, 8.0);
		volume_prog.light_position = light_position;

		grid.Use();
		volume_prog.Use();

		gl.ClearColor(0.8f, 0.7f, 0.6f, 0.0f);
		gl.ClearDepth(1.0f);
		gl.Enable(Capability::DepthTest);

		gl.Enable(Capability::CullFace);
		gl.FrontFace(FaceOrientation::CW);
		gl.CullFace(Face::Back);
	}


	void Render(double time)
	{
		gl.Viewport(width, height);
		gl.Clear().ColorBuffer().DepthBuffer();
		//

		Mat4f perspective = CamMatrixf::Perspective(
			Degrees(48),
			double(width)/height,
			1, 100
		);

		auto camera = CamMatrixf::Orbiting(
			Vec3f(0, 0, 0),
			1.52 - SineWave(time / 14.0) * 0.1,
			FullCircles(time / 19.0),
			Degrees(45 + SineWave(time / 17.0) * 40)
		);
		Vec3f camera_position = camera.Position();

		auto model = ModelMatrixf::Translation(-0.5, -0.5, -0.5);

		volume_prog.camera_position = camera_position;
		volume_prog.transform_matrix = perspective*camera*model;
		volume_prog.threshold = 0.5 + SineWave(time / 7.0) * 0.4;

		grid.Draw();
	}

	void Reshape(size_t vp_width, size_t vp_height)
	{
		width = vp_width;
		height = vp_height;
	}

	bool Continue(double time)
	{
		return time < 30.0;
	}
};

std::unique_ptr<Example> makeExample(void)
{
	return std::unique_ptr<Example>(new VolumeExample);
}

} // namespace oglplus
