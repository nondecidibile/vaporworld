#include "core_types.h"
#include "coremin.h"
#include "gldrv/gldrv.h"
#include "math/math.h"
#include <SDL2/SDL.h>
#include "world/world.h"

Malloc * gMalloc = nullptr;

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
	Map<uint8, int32> axes;

	initOpenGL();
	SDL_Window *window = SDL_CreateWindow("OpenGL",0,0,640,360,SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(window);

	//glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

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

	World world = World(64,64,0,5,0.5);
	uint32 numVertices = world.getNumVert();
	uint32 numIndices = world.getNumInd();
	Vertex *vertices = world.vertices;
	uint32 *indices = world.indices;	
	
	/* ------------------------------------------------------------- */

	uint32 vao;
	glGenVertexArrays(1,&vao);
	glBindVertexArray(vao);

	uint32 vbo, ebo;
	glGenBuffers(1,&vbo);
	glGenBuffers(1,&ebo);
	glBindBuffer(GL_ARRAY_BUFFER,vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ebo);

	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex,pos));
	glVertexAttribPointer(1,4,GL_UNSIGNED_BYTE,GL_TRUE,sizeof(Vertex),(void*)offsetof(Vertex,color));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	/* ------------------------------------------------------------- */

	cameraLocation = vec3(0,0,0);
	cameraRotation = quat(0,vec3::up);
	cameraVelocity = vec3::zero;
	projectionMatrix = mat4::glProjection(M_PI/2, 0.1f);

	int32 viewMatrixLoc = glGetUniformLocation(program,"viewMatrix");

	struct Model {
		vec3 location;
		quat rotation;
		vec3 scaling;
		FORCE_INLINE mat4 getTransform(){
			return mat4::transform(location,rotation,scaling);
		}
	};

	int32 modelMatrixLoc = glGetUniformLocation(program,"modelMatrix");

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	/* ------------------------------------------------------------- */

	bool bRunning = true;
	while(bRunning){

		// Update delta time
		currTick = SDL_GetPerformanceCounter();
		dt = (currTick - prevTick) / (float64)SDL_GetPerformanceFrequency();
		prevTick = currTick;

		SDL_Event e;
		while(SDL_PollEvent(&e)){
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
				case SDL_MOUSEMOTION:
					axes[0] = e.motion.xrel;
					axes[1] = e.motion.yrel;
					break;
				default:
					break;
			}
		}

		/*
		*	CAMERA
		*/

		//float x_rotation = axes[0]*dt;
		//float y_rotation = axes[1]*dt;
		//axes[0] = 0; axes[1] = 0;
		float x_rotation = (keys[SDLK_RIGHT]-keys[SDLK_LEFT])*2*dt;
		float y_rotation = (keys[SDLK_DOWN]-keys[SDLK_UP])*2*dt;
		cameraRotation = quat(x_rotation,vec3::up)*quat(y_rotation,cameraRotation.right()) * cameraRotation;

		const float32 accelFactor = 12.f;
		const float32 brakeFactor = 2.f;
		vec3 cameraAcceleration = vec3(
			(keys[SDLK_d] - keys[SDLK_a]),
			0.f,
			(keys[SDLK_w] - keys[SDLK_s])
		);
		vec3 direction = (cameraRotation * cameraAcceleration);
		direction.y = 0.0f;
		if(!direction.isNearlyZero()) direction.normalize();
		cameraVelocity += (direction * accelFactor - cameraVelocity * brakeFactor) * dt;
		cameraLocation += cameraVelocity * dt;

		mat4 cameraMatrix = mat4::rotation(!cameraRotation) * mat4::translation(-cameraLocation);
		glUniformMatrix4fv(viewMatrixLoc,1,GL_TRUE,(projectionMatrix * cameraMatrix).array);

		/*
		*	DRAW
		*/

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		world.updateWorld(cameraLocation.x,cameraLocation.z);
		
		glBufferData(GL_ARRAY_BUFFER,numVertices*sizeof(Vertex),vertices,GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,numIndices*sizeof(uint32),indices,GL_STATIC_DRAW);
				
		mat4 transformation = mat4::translation(vec3(0,-5,0))*mat4(1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1);
		glUniformMatrix4fv(modelMatrixLoc,1,GL_TRUE,transformation.array);
		glDrawElements(GL_TRIANGLES,numIndices,GL_UNSIGNED_INT,(void*)0);

		SDL_GL_SwapWindow(window);
	}

	return 0;
}
