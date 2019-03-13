#include "core_types.h"
#include "coremin.h"
#include "gldrv/gldrv.h"
#include "math/math.h"
#include <SDL2/SDL.h>

Malloc * gMalloc = nullptr;

float vertices[] = {
	-0.5f,-0.5f,0.f,
	0.5f,-0.5f,0.f,
	0.5f,0.5f,0.f,
	-0.5f,0.5f,0.f,
	0.f, 0.f, 1.f
};

uint32 indices[] = {
	0,2,1,
	0,3,2,
	0,1,4,
	1,2,4,
	3,4,2,
	0,4,3
};

vec3 cameraLocation;
vec3 cameraVelocity;
quat cameraRotation;
mat4 projectionMatrix;

uint64 prevTick;
uint64 currTick;
float32 dt;

char* readFile(const char *filename){

	FILE *f = fopen(filename,"r");
	if(!f){
		fprintf(stderr,"Error in readFile: file %s not found.\n",filename);
		exit(0);
	}
	fseek(f,0,SEEK_END);
	uint64 n = ftell(f);
	fseek(f,0,SEEK_SET);

	char *data = (char*) malloc(n+1);
	fread(data,1,n,f);
	data[n] = 0;
	return data;
}

int main(){

	Memory::createGMalloc();

	Map<uint32, float32> keys;

	initOpenGL();
	SDL_Window *window = SDL_CreateWindow("OpenGL",0,0,640,360,SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(window);

	/* ------------------------------------------------------------- */

	uint32 program = glCreateProgram();
	uint32 vertexShader = glCreateShader(GL_VERTEX_SHADER);
	uint32 fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	char* vertexShaderSource = readFile("shaders/shader.vert");
	glShaderSource(vertexShader,1,&vertexShaderSource,nullptr);
	glCompileShader(vertexShader);
	char* fragmentShaderSource = readFile("shaders/shader.frag");
	glShaderSource(fragmentShader,1,&fragmentShaderSource,nullptr);
	glCompileShader(fragmentShader);

	glAttachShader(program,vertexShader);
	glAttachShader(program,fragmentShader);

	glLinkProgram(program);
	glUseProgram(program);
	
	/* ------------------------------------------------------------- */

	uint32 vao;
	glGenVertexArrays(1,&vao);
	glBindVertexArray(vao);

	uint32 vbo, ebo;
	glGenBuffers(1,&vbo);
	glGenBuffers(1,&ebo);
	glBindBuffer(GL_ARRAY_BUFFER,vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ebo);

	glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indices),indices,GL_STATIC_DRAW);

	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void*)0);
	glEnableVertexAttribArray(0);

	/* ------------------------------------------------------------- */

	cameraLocation = vec3(0,0.5,-2);
	cameraRotation = quat(0,vec3::up);
	projectionMatrix = mat4::glProjection(M_PI/2, 0.25f);

	int32 viewMatrixLoc = glGetUniformLocation(program,"viewMatrix");

	struct Model {
		vec3 location;
		quat rotation;
		vec3 scaling;
		FORCE_INLINE mat4 getTransform(){
			return mat4::transform(location,rotation,scaling);
		}
	};

	Model model;
	model.location = vec3::zero;
	model.rotation = quat(M_PI/2, vec3::left);
	model.scaling = vec3::unit;

	int32 modelMatrixLoc = glGetUniformLocation(program,"modelMatrix");

	int32 modelColorLoc = glGetUniformLocation(program,"modelColor");
	vec4 color = vec4(1,0,0,1);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	/* ------------------------------------------------------------- */

	bool bRunning = true;
	while(bRunning){

		// Update delta time
		currTick = SDL_GetPerformanceCounter();
		dt = (currTick - prevTick) / (float64)SDL_GetPerformanceFrequency();
		prevTick = currTick;

		SDL_Event e;
		SDL_PollEvent(&e);
		switch(e.type){
			case SDL_QUIT:
				bRunning = false;
				break;
			case SDL_KEYDOWN:
				keys[e.key.keysym.sym] = 1.f;
				if (e.key.keysym.sym == SDLK_ESCAPE) bRunning = false;
				break;
			case SDL_KEYUP:
				keys[e.key.keysym.sym] = 0.f;
				break;
			default:
				break;
		}

		const float32 accelFactor = 20.f;
		const float32 brakeFactor = 10.f;
		vec3 cameraAcceleration = vec3(
			(keys[SDLK_d] - keys[SDLK_a]) * accelFactor,
			0.f,
			(keys[SDLK_w] - keys[SDLK_s]) * accelFactor
		);
		cameraVelocity += ((cameraRotation * cameraAcceleration) - cameraVelocity * brakeFactor) * dt;
		cameraLocation += cameraVelocity * dt;

		glUniformMatrix4fv(modelMatrixLoc,1,GL_TRUE,model.getTransform().array);

		mat4 cameraMatrix = mat4::rotation(!cameraRotation) * mat4::translation(-cameraLocation);
		glUniformMatrix4fv(viewMatrixLoc,1,GL_TRUE,(projectionMatrix * cameraMatrix).array);

		glUniform4fv(modelColorLoc,1,color.buffer);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(uint32), GL_UNSIGNED_INT, (void*)0);

		SDL_GL_SwapWindow(window);
	}

	return 0;
}