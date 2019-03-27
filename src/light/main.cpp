#include "coremin.h"
#include "math/math.h"
#include "gldrv/gldrv.h"

#include <SDL.h>

Malloc * gMalloc = nullptr;

/// Time variables
float32 dt;
uint64 currTick;
uint64 prevTick;

const float vertices[] = {
	-0.5f,0.f,-0.5f,
	0.5f,0.f,-0.5f,
	0.5f,0.f,0.5f,
	-0.5f,0.f,0.5f,
	0.f, 1.f, 0.f
};

const uint32 indices[] = {
	0,2,1,
	0,3,2,
	0,1,4,
	1,2,4,
	3,4,2,
	0,4,3
};

class FileReader
{
protected:
	FILE * fp;
	void * buffer;
	uint64 len;

public:
	FileReader(const char * filename) :
		fp(fopen(filename, "r"))
	{
		if (fp)
		{
			// Get file length
			fseek(fp, 0, SEEK_END);
			len = ftell(fp);
			fseek(fp, 0, SEEK_SET);

			// Allocate buffer
			if (posix_memalign((void**)&buffer, sizeof(void*), len + 1) == 0)
			{
				get<char>()[len] = 0;
				fread(buffer, 1, len, fp);
			}
		}
	}

	~FileReader()
	{
		if (fp) fclose(fp);
		if (buffer) free(buffer);
	}

	FORCE_INLINE bool isValid() const { return fp && buffer && len > 0; }

	template<typename T>
	FORCE_INLINE T * get() { return reinterpret_cast<T*>(buffer); }
};



int32 main()
{
	Memory::createGMalloc();

	Map<uint32, float32> keys;

	initOpenGL();

	SDL_Window * window = SDL_CreateWindow("light", 0, 0, 1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS);
	SDL_GLContext context = SDL_GL_CreateContext(window);

	glEnable(GL_DEPTH_TEST);

	//////////////////////////////////////////////////
	// Program setup
	//////////////////////////////////////////////////
	
	uint32 prog = glCreateProgram();
	
	uint32 vShader = glCreateShader(GL_VERTEX_SHADER);
	uint32 gShader = glCreateShader(GL_GEOMETRY_SHADER);
	uint32 fShader = glCreateShader(GL_FRAGMENT_SHADER);

	{
		FileReader source = "src/light/shaders/default/.vert";
		const char * buffer = source.get<char>();
		glShaderSource(vShader, 1, &buffer, nullptr);
		glCompileShader(vShader);
		glAttachShader(prog, vShader);
	}
	{
		FileReader source = "src/light/shaders/default/.geom";
		const char * buffer = source.get<char>();
		glShaderSource(gShader, 1, &buffer, nullptr);
		glCompileShader(gShader);
		glAttachShader(prog, gShader);
	}
	{
		FileReader source = "src/light/shaders/default/.frag";
		const char * buffer = source.get<char>();
		glShaderSource(fShader, 1, &buffer, nullptr);
		glCompileShader(fShader);
		glAttachShader(prog, fShader);
	}

	glLinkProgram(prog);
	glUseProgram(prog);

	int32 status = 1;
	glGetProgramiv(prog, GL_LINK_STATUS, &status);
	if (!status) printf("program not linked correctly\n");

	//////////////////////////////////////////////////
	// Primitive setup
	//////////////////////////////////////////////////
	
	uint32 vao;
	struct
	{
		uint32 vbo;
		uint32 ebo;
	} buffers;
	glGenVertexArrays(1, &vao);
	glGenBuffers(2, &buffers.vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, buffers.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers.ebo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(0);

	//////////////////////////////////////////////////
	// Camera setup
	//////////////////////////////////////////////////
	
	vec3 cameraLocation(0.f, 1.f, -5.f);
	vec3 cameraVelocity(vec3::zero);
	quat cameraRotation(0.f, vec3::up);
	mat4 projectionMatrix = mat4::glProjection(M_PI_2, 0.5f);

	uint32 modelMatrixLoc = glGetUniformLocation(prog, "modelMatrix");
	uint32 viewMatrixLoc = glGetUniformLocation(prog, "viewMatrix");
	uint32 cameraLocationLoc = glGetUniformLocation(prog, "cameraLocation");

	//////////////////////////////////////////////////
	// Ground setup
	//////////////////////////////////////////////////
	
	const float32 groundVertices[] = {
		-1.f, -1.f, 0.f,
		1.f, 1.f, 0.f,
		1.f, -1.f, 0.f,

		-1.f, -1.f, 0.f,
		-1.f, 1.f, 0.f,
		1.f, 1.f, 0.f
	};

	uint32 groundVbo;
	glGenBuffers(1, &groundVbo);
	glBindBuffer(GL_ARRAY_BUFFER, groundVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(groundVertices), groundVertices, GL_STATIC_DRAW);

	//////////////////////////////////////////////////
	// Actors setup
	//////////////////////////////////////////////////

	struct Actor
	{
	public:
		vec3 location;
		vec3 velocity;
		quat rotation;
		vec3 scale;

	public:
		FORCE_INLINE Actor(const vec3 & _location, const quat & _rotation, const vec3 & _scale = vec3::unit) :
			location(_location),
			velocity(0.f),
			rotation(_rotation),
			scale(_scale) {}

		FORCE_INLINE mat4 getTransform() const
		{
			return mat4::transform(location, rotation, scale);
		}
	};

	Actor actors[] = {
		Actor(
			vec3(1.f, 0.f, 5.f),
			quat(6.4f, vec3::up),
			vec3(1.5f)
		),
		Actor(
			vec3(-1.f, 0.f, 0.f),
			quat(1.2f, vec3::unit),
			vec3(1.2f)
		),
		Actor(
			vec3(1.f, 0.f, -5.f),
			quat(2.9f, vec3::up),
			vec3(0.8f)
		),
		Actor(
			vec3(-5.f, 0.f, -4.5f),
			quat(2.7f, vec3::forward),
			vec3(2.1f)
		),
		Actor(
			vec3(-5.f, 0.f, 6.f),
			quat(1.f, vec3::up),
			vec3(1.f)
		),
		Actor(
			vec3(4.f, 0.f, 0.f),
			quat(0.6f, vec3(1.f, 2.f, 0.f)),
			vec3(1.f)
		)
	};

	//////////////////////////////////////////////////
	// Environment setup
	//////////////////////////////////////////////////
	
	const int32 numLights = 8;
	Vec3<float32, true> lights[numLights];

	for (uint32 i = 0; i < numLights; ++i)
		lights[i] = vec3(Math::cos(i / (float32)numLights * M_PI), 1.f + rand() / (float32)RAND_MAX, Math::sin(i / (float32)numLights * M_PI)) * 16.f;

	uint32 numLightsLoc = glGetUniformLocation(prog, "numLights");
	uint32 lightPointsLoc = glGetUniformLocation(prog, "lightPoints");

	glUniform1iv(numLightsLoc, 1, &numLights);
	glUniform3fv(lightPointsLoc, numLights, (float32*)lights);

	bool bRunning = true;
	while (bRunning)
	{
		// Update time variables
		dt = ((currTick = SDL_GetPerformanceCounter()) - prevTick) / (float32)SDL_GetPerformanceFrequency();
		prevTick = currTick;

		printf("%f s -> %f fps\n", dt, 1.f / dt);

		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
				case SDL_QUIT:
					bRunning = false;
					break;
				
				case SDL_KEYDOWN:
					keys[e.key.keysym.sym] = 1.f;
					bRunning &= e.key.keysym.sym != SDLK_ESCAPE;
					break;
				
				case SDL_KEYUP:
					keys[e.key.keysym.sym] = 0.f;
					break;
			}
		}

		const float32 cameraSpeed = 8.f;
		const float32 cameraBrake = 3.f;
		vec3 cameraAcceleration = cameraRotation * vec3(
			keys[SDLK_d] - keys[SDLK_a],
			keys[SDLK_SPACE] - keys[SDLK_LCTRL],
			keys[SDLK_w] - keys[SDLK_s]
		) * cameraSpeed - (cameraVelocity) * cameraBrake;
		cameraVelocity += cameraAcceleration * dt;
		cameraLocation += cameraVelocity * dt;

		cameraRotation = quat((keys[SDLK_RIGHT] - keys[SDLK_LEFT]) * dt, vec3::up) * quat((keys[SDLK_DOWN] - keys[SDLK_UP]) * dt, cameraRotation.right()) * cameraRotation;
		const mat4 viewMatrix = projectionMatrix * (mat4::rotation(!cameraRotation) * mat4::translation(-cameraLocation));

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Update camera matrix
		glUseProgram(prog);

		glUniformMatrix4fv(viewMatrixLoc, 1, GL_TRUE, viewMatrix.array);
		glUniform3fv(cameraLocationLoc, 1, cameraLocation.buffer);

		// Draw pyramids
		glBindBuffer(GL_ARRAY_BUFFER, buffers.vbo);

		for (uint32 i = 0; i < sizeof(actors) / sizeof(actors[0]); ++i)
		{
			glUniformMatrix4fv(modelMatrixLoc, 1, GL_TRUE, actors[i].getTransform().array);
			glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(uint32), GL_UNSIGNED_INT, nullptr);
		}

		SDL_GL_SwapWindow(window);
	}

	return 0;
}