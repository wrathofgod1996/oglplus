/**
 *  @example oglplus/027_smoke_trails.cpp
 *  @brief Shows how to draw multpile lighted smoke trails
 *
 *  @image html 027_smoke_trails.png
 *
 *  Copyright 2008-2011 Matus Chochlik. Distributed under the Boost
 *  Software License, Version 1.0. (See accompanying file
 *  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#include <oglplus/gl.hpp>
#include <oglplus/all.hpp>
#include <oglplus/curve.hpp>
#include <oglplus/bound/texture.hpp>
#include <oglplus/images/cloud.hpp>

#include <vector>
#include <algorithm>
#include <cmath>
#include <cassert>

#include "example.hpp"

namespace oglplus {

// Encapsulates a particle emitter
class ParticleSystem
{
private:
	const CubicBezierLoop<Vec3f, double> path;
	const double cycle_time;
	const double lifetime;
	const double spawn_interval;
	double spawn_time;

	std::vector<Vec3f> positions;
	std::vector<Vec3f> directions;
	std::vector<float> ages;
	std::vector<int> ids;

	// creates a directional vector for a new particle
	static Vec3f NewDirection(void)
	{
		float disp = 0.6f;
		float dx = (0.5f - (float(std::rand())/RAND_MAX))*disp;
		float dy = (0.5f - (float(std::rand())/RAND_MAX))*disp;
		float dz = (0.5f - (float(std::rand())/RAND_MAX))*disp;
		return Vec3f(dx, dy, dz);
	}

	// return a new particle identifier
	static int NewID(void)
	{
		return rand();
	}

	// Spawn a new particle if the time is right
	bool SpawnParticle(
		double time,
		Vec3f& position,
		Vec3f& direction,
		float& age,
		int& id
	)
	{
		float new_age = time - spawn_time - spawn_interval;
		if(new_age >= 0.0f)
		{
			spawn_time += spawn_interval;
			direction = NewDirection();
			Vec3f emitter_pos = path.Position(spawn_time/cycle_time);
			position = emitter_pos + direction;
			age = new_age;
			id = NewID();
			return true;
		}
		return false;
	}
public:
	ParticleSystem(
		const std::initializer_list<Vec3f>& path_points,
		double path_time,
		double part_per_sec
	): path(path_points)
	 , cycle_time(path_time)
	 , lifetime(10.0)
	 , spawn_interval(1.0 / part_per_sec)
	 , spawn_time(0.0)
	{
		assert(cycle_time > 0.0);
	}

	// Updates the emitter
	// Changes its position, emits new particles
	// return the number of particles that are currently "alive"
	void Update(double time, double prev_time)
	{
		assert(positions.size() == directions.size());
		assert(positions.size() == ages.size());
		assert(positions.size() == ids.size());

		double time_diff = time - prev_time;
		double drag = 0.1 * time_diff;
		if(drag > 1.0) drag = 1.0;

		// go through the existing particles
		for(size_t i=0, n=positions.size(); i!=n; ++i)
		{
			// update the age
			ages[i] += time_diff / lifetime;
			// if the particle is "too old"
			if(ages[i] > 1.0)
			{
				// try to spawn a new one in its place
				SpawnParticle(
					time,
					positions[i],
					directions[i],
					ages[i],
					ids[i]
				);
			}
			else
			{
				// otherwise just update its motion
				directions[i] *= (1.0 - drag);
				positions[i] += directions[i] * time_diff;
			}
		}
		Vec3f position;
		Vec3f direction;
		float age;
		int id;
		// spawn new particles if necessary
		while(SpawnParticle(time, position, direction, age, id))
		{
			positions.push_back(position);
			directions.push_back(direction);
			ages.push_back(age);
			ids.push_back(id);
		}
	}

	void Upload(
		std::vector<Vec3f>& pos,
		std::vector<float>& age,
		std::vector<int>& id
	)
	{
		pos.insert(pos.end(), positions.begin(), positions.end());
		age.insert(age.end(), ages.begin(), ages.end());
		id.insert(id.end(), ids.begin(), ids.end());
	}
};

class SmokeExample : public Example
{
private:
	// A list of particle emitters
	std::vector<ParticleSystem> emitters;

	// wrapper around the current OpenGL context
	Context gl;

	// Vertex shader
	VertexShader vs;

	// Geometry shader
	GeometryShader gs;

	// Fragment shader
	FragmentShader fs;

	// Program
	Program prog;

	// A vertex array object for the particles
	VertexArray particles;

	// VBOs for the particle positions, ages and identifiers
	Buffer pos_buf, age_buf, id_buf;

	// The smoke texture for the particles
	Texture smoke_tex;

	std::vector<Vec3f> positions;
	std::vector<float> ages;
	std::vector<int> ids;
	double prev_time;
public:
	SmokeExample(void)
	 : emitters(
		{
			{
				{
					{-20.0f, -10.0f,  10.0f},
					{ 20.0f,   0.0f, -20.0f},
					{ 20.0f,  10.0f,  20.0f},
					{-20.0f,   0.0f, -10.0f}
				}, 15.0, 200.0
			},
			{
				{
					{ 30.0f,   0.0f,   5.0f},
					{-30.0f,   0.0f,  -5.0f},
					{-20.0f,  20.0f,   5.0f},
					{ 20.0f, -10.0f,  -5.0f}
				}, 17.0, 200.0
			},
		}
	), vs("Vertex")
	 , gs("Geometry")
	 , fs("Fragment")
	 , prev_time(0.0)
	{
		// Set the vertex shader source
		vs.Source(
			"#version 330\n"
			"uniform mat4 CameraMatrix;"
			"in vec4 Position;"
			"in float Age;"
			"in int Id;"
			"out float vertAge;"
			"out int vertId;"
			"void main(void)"
			"{"
			"	gl_Position = CameraMatrix * Position;"
			"	vertAge = Age;"
			"	vertId = Id;"
			"}"
		);
		// compile it
		vs.Compile();

		// Set the geometry shader source
		gs.Source(
			"#version 330\n"
			"layout(points) in;"
			"layout(triangle_strip, max_vertices = 4) out;"
			"uniform vec3 LightCamPos;"
			"uniform mat4 ProjectionMatrix;"
			"in float vertAge[];"
			"in int vertId[];"
			"out vec2 geomTexCoord;"
			"out float geomAge;"
			"out float geomLightVal;"
			"out float geomLightBias;"
			"void main(void)"
			"{"
			"	if(vertAge[0] > 1.0) return;"
			"	vec3 pos = gl_in[0].gl_Position.xyz;"
			"	vec3 lightDir = normalize(LightCamPos - pos);"
			"	float s = 0.8, g = 3.0;"
			"	float yo[2] = float[2](-1.0, 1.0);"
			"	float xo[2] = float[2](-1.0, 1.0);"
			"	float angle = vertId[0];"
			"	float cx = cos(angle), sx = sin(angle);"
			"	mat2 rot = mat2(cx, sx, -sx, cx);"
			"	for(int j=0;j!=2;++j)"
			"	for(int i=0;i!=2;++i)"
			"	{"
			"		float xoffs = xo[i]*(1.0+vertAge[0]*g)*s;"
			"		float yoffs = yo[j]*(1.0+vertAge[0]*g)*s;"
			"		vec2 offs = rot*vec2(xoffs, yoffs);"
			"		gl_Position = ProjectionMatrix * vec4("
			"			pos.x-offs.x,"
			"			pos.y-offs.y,"
			"			pos.z,"
			"			1.0"
			"		);"
			"		geomTexCoord = vec2(float(i), float(j));"
			"		geomAge = vertAge[0];"
			"		geomLightVal = lightDir.z;"
			"		geomLightBias = -dot("
			"			normalize(vec3(offs, 0.0)),"
			"			lightDir"
			"		);"
			"		EmitVertex();"
			"	}"
			"	EndPrimitive();"
			"}"
		);
		// compile it
		gs.Compile();

		// set the fragment shader source
		fs.Source(
			"#version 330\n"
			"uniform sampler2D SmokeTex;"
			"in vec2 geomTexCoord;"
			"in float geomAge;"
			"in float geomLightVal;"
			"in float geomLightBias;"
			"out vec4 fragColor;"
			"void main(void)"
			"{"
			"	vec3 c = texture(SmokeTex, geomTexCoord).rgb;"
			"	float depth = c.g - c.r;"
			"	if(depth == 0.0) discard;"
			"	float density = min(depth * c.b * 2.0, 1.0);"
			"	float intensity = min("
			"		max("
			"			geomLightVal+"
			"			geomLightBias,"
			"			0.0"
			"		)+max("
			"			-geomLightVal*"
			"			(1.0 - density)*"
			"			geomLightBias * 5.0,"
			"			0.0"
			"		),"
			"		1.0"
			"	) + 0.1;"
			"	fragColor = vec4("
			"		intensity,"
			"		intensity,"
			"		intensity,"
			"		(1.0 - geomAge)*density"
			"	);"
			"}"
		);
		// compile it
		fs.Compile();

		// attach the shaders to the program
		prog.AttachShader(vs);
		prog.AttachShader(gs);
		prog.AttachShader(fs);
		// link and use it
		prog.Link();
		prog.Use();

		// bind the VAO for the particles
		particles.Bind();

		// bind the VBO for the particle positions
		pos_buf.Bind(Buffer::Target::Array);
		{
			Buffer::Data(Buffer::Target::Array, positions);
			VertexAttribArray attr(prog, "Position");
			attr.Setup(3, DataType::Float);
			attr.Enable();
		}

		// bind the VBO for the particle ages
		age_buf.Bind(Buffer::Target::Array);
		{
			Buffer::Data(Buffer::Target::Array, ages);
			VertexAttribArray attr(prog, "Age");
			attr.Setup(1, DataType::Float);
			attr.Enable();
		}

		// bind the VBO for the particle identifiers
		id_buf.Bind(Buffer::Target::Array);
		{
			Buffer::Data(Buffer::Target::Array, ids);
			VertexAttribArray attr(prog, "Id");
			attr.Setup(1, DataType::Int);
			attr.Enable();
		}

		Texture::Active(0);
		Uniform(prog, "SmokeTex").Set(0);
		{
			auto bound_tex = Bind(smoke_tex, Texture::Target::_2D);
			bound_tex.Image2D(
				images::Cloud2D(
					images::Cloud(
						128, 128, 128,
						Vec3f(0.1f, -0.5f, 0.3f),
						0.5f
					)
				)
			);
			bound_tex.GenerateMipmap();
			bound_tex.MinFilter(TextureMinFilter::LinearMipmapLinear);
			bound_tex.MagFilter(TextureMagFilter::Linear);
			bound_tex.BorderColor(Vec4f(0.0f, 0.0f, 0.0f, 0.0f));
			bound_tex.WrapS(TextureWrap::ClampToBorder);
			bound_tex.WrapT(TextureWrap::ClampToBorder);
		}
		//
		gl.ClearColor(0.0f, 0.1f, 0.2f, 0.0f);
		gl.ClearDepth(1.0f);
		gl.Enable(Capability::DepthTest);
		gl.Enable(Capability::Blend);
		gl.BlendFunc(BlendFn::SrcAlpha, BlendFn::OneMinusSrcAlpha);
	}

	void Reshape(size_t width, size_t height)
	{
		gl.Viewport(width, height);
		prog.Use();
		Uniform(prog, "ProjectionMatrix").SetMatrix(
			CamMatrixf::Perspective(
				Degrees(24),
				double(width)/height,
				1, 100
			)
		);
	}

	void Render(double time)
	{
		positions.clear();
		ages.clear();
		ids.clear();

		// update the emitters and get the particle data
		for(auto i=emitters.begin(), e=emitters.end(); i!=e; ++i)
		{
			i->Update(time, prev_time);
			i->Upload(positions, ages, ids);
		}
		assert(positions.size() == ages.size());
		assert(positions.size() == ids.size());

		// make a camera matrix
		auto cameraMatrix = CamMatrixf::Orbiting(
			Vec3f(),
			35.0 - std::sin(time) * 10.0,
			FullCircles(time * 0.1),
			Degrees(std::sin(time * 0.3) * 60)
		);

		std::vector<float> depths(positions.size());
		std::vector<GLuint> indices(positions.size());
		// calculate the depths of the particles
		for(GLuint i=0, n=positions.size(); i!=n; ++i)
		{
			depths[i] = (cameraMatrix * Vec4f(positions[i], 1.0)).z();
			indices[i] = i;
		}

		// sort the indices by the depths
		std::sort(
			indices.begin(),
			indices.end(),
			[&depths](GLuint i, GLuint j)
			{
				return depths[i] < depths[j];
			}
		);

		// upload the particle positions
		pos_buf.Bind(Buffer::Target::Array);
		Buffer::Data(Buffer::Target::Array, positions);
		// upload the particle ages
		age_buf.Bind(Buffer::Target::Array);
		Buffer::Data(Buffer::Target::Array, ages);
		// upload the particle identifiers
		id_buf.Bind(Buffer::Target::Array);
		Buffer::Data(Buffer::Target::Array, ids);

		gl.Clear().ColorBuffer().DepthBuffer();
		Uniform(prog, "LightCamPos").Set(
			(cameraMatrix*Vec4f(30.0f, 30.0f, 30.0f, 1.0f)).xyz()
		);
		Uniform(prog, "CameraMatrix").SetMatrix(cameraMatrix);
		// use the indices to draw the particles
		gl.DrawElements(
			PrimitiveType::Points,
			indices.size(),
			DataType::UnsignedInt,
			indices.data()
		);

		prev_time = time;
	}

	bool Continue(double time)
	{
		return time < 60.0;
	}
};

std::unique_ptr<Example> makeExample(void)
{
	return std::unique_ptr<Example>(new SmokeExample);
}

} // namespace oglplus
