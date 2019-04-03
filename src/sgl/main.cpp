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
	SDL_Window *window = SDL_CreateWindow("OpenGL",0,0,1280,720,SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(window);

	//glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	/* ------------------------------------------------------------- */

	uint32 programT = glCreateProgram();
	uint32 programL = glCreateProgram();
	uint32 vertexShaderT = glCreateShader(GL_VERTEX_SHADER);
	uint32 geometryShaderT = glCreateShader(GL_GEOMETRY_SHADER);
	uint32 fragmentShaderT = glCreateShader(GL_FRAGMENT_SHADER);
	uint32 vertexShaderL = glCreateShader(GL_VERTEX_SHADER);
	uint32 fragmentShaderL = glCreateShader(GL_FRAGMENT_SHADER);

	char* vertexShaderTSource = readFile("shaders/shaderT.vert");
	glShaderSource(vertexShaderT,1,&vertexShaderTSource,nullptr);
	glCompileShader(vertexShaderT);
	char* geometryShaderTSource = readFile("shaders/shaderT.geom");
	glShaderSource(geometryShaderT,1,&geometryShaderTSource,nullptr);
	glCompileShader(geometryShaderT);
	char* fragmentShaderTSource = readFile("shaders/shaderT.frag");
	glShaderSource(fragmentShaderT,1,&fragmentShaderTSource,nullptr);
	glCompileShader(fragmentShaderT);
	char* vertexShaderLSource = readFile("shaders/shaderL.vert");
	glShaderSource(vertexShaderL,1,&vertexShaderLSource,nullptr);
	glCompileShader(vertexShaderL);
	char* fragmentShaderLSource = readFile("shaders/shaderL.frag");
	glShaderSource(fragmentShaderL,1,&fragmentShaderLSource,nullptr);
	glCompileShader(fragmentShaderL);

	glAttachShader(programT,vertexShaderT);
	glAttachShader(programT,geometryShaderT);
	glAttachShader(programT,fragmentShaderT);
	glLinkProgram(programT);

	glAttachShader(programL,vertexShaderL);
	glAttachShader(programL,fragmentShaderL);
	glLinkProgram(programL);

	/* ------------------------------------------------------------- */

	World world = World(128,128, 0.5, -2,2,0, 1);
	uint32 numVertices = world.N;
	uint32 numTrianglesIndices = world.numTrianglesIndices;
	uint32 numLinesIndices = world.numLinesIndices;
	
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
	glEnableVertexAttribArray(0);

	/* ------------------------------------------------------------- */

	cameraLocation = vec3(0,0,0);
	cameraRotation = quat(0,vec3::up);
	cameraVelocity = vec3::zero;
	projectionMatrix = mat4::glProjection(M_PI/2, 0.1f);

	int32 viewMatrixLocT = glGetUniformLocation(programT,"viewMatrix");
	int32 viewMatrixLocL = glGetUniformLocation(programL,"viewMatrix");

	uint32 camLocUniformT = glGetUniformLocation(programT, "cameraLocation");
	uint32 camLocUniformL = glGetUniformLocation(programL, "cameraLocation");

	struct Model {
		vec3 location;
		quat rotation;
		vec3 scaling;
		FORCE_INLINE mat4 getTransform(){
			return mat4::transform(location,rotation,scaling);
		}
	};

	int32 modelMatrixLocT = glGetUniformLocation(programT,"modelMatrix");
	int32 modelMatrixLocL = glGetUniformLocation(programL,"modelMatrix");
	
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
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

		/*
		*	DRAW
		*/

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * */
		world.updateWorld(dt);

		glUseProgram(programT);
		glUniform3fv(camLocUniformT, 1, cameraLocation.buffer);
		glUniformMatrix4fv(viewMatrixLocT,1,GL_TRUE,(projectionMatrix * cameraMatrix).array);

		glBufferData(GL_ARRAY_BUFFER,numVertices*sizeof(Vertex),world.vertices,GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,numTrianglesIndices*sizeof(uint32),world.trianglesIndices,GL_STATIC_DRAW);
		
		float height = 1;

		mat4 transformation = mat4::translation(vec3(-(float)world.X*world.stepSize/2,-height-0.001,0));
		glUniformMatrix4fv(modelMatrixLocT,1,GL_TRUE,transformation.array);
		glDrawElements(GL_TRIANGLES,numTrianglesIndices,GL_UNSIGNED_INT,(void*)0);

		
		glUseProgram(programL);
		glUniform3fv(camLocUniformL, 1, cameraLocation.buffer);
		glUniformMatrix4fv(viewMatrixLocL,1,GL_TRUE,(projectionMatrix * cameraMatrix).array);

		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);

		glLineWidth(3);
		//transformation = mat4::translation(vec3(-0.25,-height,pos))*mat4(1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1);
		glUniformMatrix4fv(modelMatrixLocL,1,GL_TRUE,transformation.array);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,numLinesIndices*sizeof(uint32),world.linesIndices,GL_STATIC_DRAW);
		glDrawElements(GL_LINES,numLinesIndices,GL_UNSIGNED_INT,(void*)0);
		

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * */

		SDL_GL_SwapWindow(window);
	}

	return 0;
}
