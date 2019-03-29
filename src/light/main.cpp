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

void catchError()
{
	uint32 err = glGetError();
	if (err != GL_NO_ERROR)
		printf("gl error #%x\n", err);
};

class GBuffer
{
public:
	/// GBuffer names
	enum TextureName
	{
		GBUFFER_POSITION	= 0,
		GBUFFER_NORMAL		= 1,
		GBUFFER_COLOR		= 2,
		GBUFFER_DEPTH		= 3,
		GBUFFER_BACKBUFFER	= 4,

		GBUFFER_NUM_BUFFERS
	};

	/// Framebuffer in use
	uint32 framebuffer;

	/// Texture buffers
	uint32 buffers[GBUFFER_NUM_BUFFERS];

public:
	/// Initialize GBuffer
	void init()
	{
		// Create framebuffer
		glGenFramebuffers(1, &framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

		// Create the textures
		glGenTextures(GBUFFER_NUM_BUFFERS, buffers);

		// Position
		glBindTexture(GL_TEXTURE_2D, buffers[GBUFFER_POSITION]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 1280, 720, 0, GL_RGB, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + GBUFFER_POSITION, GL_TEXTURE_2D, buffers[GBUFFER_POSITION], 0);

		// Normal
		glBindTexture(GL_TEXTURE_2D, buffers[GBUFFER_NORMAL]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 1280, 720, 0, GL_RGB, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + GBUFFER_NORMAL, GL_TEXTURE_2D, buffers[GBUFFER_NORMAL], 0);

		// Color
		glBindTexture(GL_TEXTURE_2D, buffers[GBUFFER_COLOR]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1280, 720, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + GBUFFER_COLOR, GL_TEXTURE_2D, buffers[GBUFFER_COLOR], 0);

		// Depth
		glBindTexture(GL_TEXTURE_2D, buffers[GBUFFER_DEPTH]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, 1280, 720, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, buffers[GBUFFER_DEPTH], 0);

		// Backbuffer
		glBindTexture(GL_TEXTURE_2D, buffers[GBUFFER_BACKBUFFER]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 1280, 720, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + GBUFFER_BACKBUFFER, GL_TEXTURE_2D, buffers[GBUFFER_BACKBUFFER], 0);


		uint32 drawBuffers[] = {
			GL_COLOR_ATTACHMENT0 + GBUFFER_POSITION,
			GL_COLOR_ATTACHMENT0 + GBUFFER_NORMAL,
			GL_COLOR_ATTACHMENT0 + GBUFFER_COLOR
		};
		glDrawBuffers(3, drawBuffers);

		uint32 status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE)
			printf("Framebuffer error %x:%u\n", status, status);

		// Reset default framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	/// Bind for writing
	void bind(GLenum target = GL_FRAMEBUFFER)
	{
		glBindFramebuffer(target, framebuffer);
	}
};

GBuffer gBuffer;

int32 main()
{
	Memory::createGMalloc();

	Map<uint32, float32> keys;

	initOpenGL();

	SDL_Window * window = SDL_CreateWindow("light", 0, 0, 1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS);
	SDL_GLContext context = SDL_GL_CreateContext(window);
	SDL_GL_SetSwapInterval(0);

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

	uint32 lightProg = glCreateProgram();
	uint32 cShader = glCreateShader(GL_COMPUTE_SHADER);
	{
		FileReader source("src/light/shaders/default/.comp");
		const char * buffer = source.get<char>();
		glShaderSource(cShader, 1, &buffer, nullptr);
		glCompileShader(cShader);
		glAttachShader(lightProg, cShader);
	}

	status = 1;
	glGetShaderiv(cShader, GL_COMPILE_STATUS, &status);
	if (!status) printf("compute shader not compiled correctly\n");

	glLinkProgram(lightProg);
	glUseProgram(lightProg);

	status = 1;
	glGetProgramiv(lightProg, GL_LINK_STATUS, &status);
	if (!status) printf("light program not linked correctly\n");

	gBuffer.init();

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
			vec3(0.f, -1.f, 0.f),
			quat(M_PI, vec3::right),
			vec3(50.f)
		),
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
	
	const int32 numLights = 4;
	Vec3<float32, true> lights[numLights];

	for (uint32 i = 0; i < numLights; ++i)
		lights[i] = vec3(Math::cos(i / (float32)numLights * M_PI), 0.f, Math::sin(i / (float32)numLights * M_PI)) * 16.f + vec3::up * 2.f;

	glUseProgram(lightProg);

	uint32 numLightsLoc = glGetUniformLocation(lightProg, "numLights");
	uint32 lightPointsLoc = glGetUniformLocation(lightProg, "lightPoints");
	uint32 cameraPosLoc = glGetUniformLocation(lightProg, "cameraPos");

	glUniform1iv(numLightsLoc, 1, &numLights);
	glUniform3fv(lightPointsLoc, numLights, (float32*)lights);

	bool bRunning = true;
	while (bRunning)
	{
		// Update time variables
		dt = ((currTick = SDL_GetPerformanceCounter()) - prevTick) / (float32)SDL_GetPerformanceFrequency();
		prevTick = currTick;

		//printf("%f s -> %f fps\n", dt, 1.f / dt);

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

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, gBuffer.framebuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Update camera matrix
		glUseProgram(prog);

		glUniformMatrix4fv(viewMatrixLoc, 1, GL_TRUE, viewMatrix.array);

		// Draw pyramids
		glBindBuffer(GL_ARRAY_BUFFER, buffers.vbo);

		for (uint32 i = 0; i < sizeof(actors) / sizeof(actors[0]); ++i)
		{
			glUniformMatrix4fv(modelMatrixLoc, 1, GL_TRUE, actors[i].getTransform().array);
			glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(uint32), GL_UNSIGNED_INT, nullptr);
		}

		// Light pass
		glUseProgram(lightProg);
		glUniform3fv(cameraPosLoc, 1, cameraLocation.buffer);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gBuffer.buffers[GBuffer::GBUFFER_POSITION]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gBuffer.buffers[GBuffer::GBUFFER_NORMAL]);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, gBuffer.buffers[GBuffer::GBUFFER_COLOR]);

		glBindImageTexture(0, gBuffer.buffers[GBuffer::GBUFFER_BACKBUFFER], 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
		
		glDispatchCompute(80, 80, 1);
		glMemoryBarrier(GL_ALL_BARRIER_BITS);

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer.framebuffer);

		glReadBuffer(GL_COLOR_ATTACHMENT0 + GBuffer::GBUFFER_POSITION);
		glBlitFramebuffer(0, 0, 1280, 720, 0, 0, 1280 / 2, 720 / 2, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		glReadBuffer(GL_COLOR_ATTACHMENT0 + GBuffer::GBUFFER_NORMAL);
		glBlitFramebuffer(0, 0, 1280, 720, 1280 / 2, 0, 1280, 720 / 2, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		glReadBuffer(GL_COLOR_ATTACHMENT0 + GBuffer::GBUFFER_COLOR);
		glBlitFramebuffer(0, 0, 1280, 720, 0, 720 / 2, 1280 / 2, 720, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		glReadBuffer(GL_COLOR_ATTACHMENT0 + GBuffer::GBUFFER_BACKBUFFER);
		glBlitFramebuffer(0, 0, 1280, 720, 1280 / 2, 720 / 2, 1280, 720, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		SDL_GL_SwapWindow(window);
	}

	return 0;
}