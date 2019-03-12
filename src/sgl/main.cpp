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
quat cameraRotation;
mat4 projectionMatrix;

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
	projectionMatrix = mat4::glProjection(M_PI/2);

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

	bool key_w=false, key_s=false, key_a=false, key_d=false;
	bool key_left=false, key_right=false;
	
	/* ------------------------------------------------------------- */

	bool bRunning = true;
	while(bRunning){

		SDL_Event e;
		SDL_PollEvent(&e);
		switch(e.type){
			case SDL_QUIT:
				bRunning = false;
				break;
			case SDL_KEYDOWN:
				switch(e.key.keysym.sym){
					case SDLK_w:
						key_w = true;
						break;
					case SDLK_s:
						key_s = true;
						break;
					case SDLK_a:
						key_a = true;
						break;
					case SDLK_d:
						key_d = true;
						break;
					case SDLK_LEFT:
						key_left = true;
						break;
					case SDLK_RIGHT:
						key_right = true;
						break;
					case SDLK_ESCAPE:
						bRunning = false;
						break;
					default:
						break;
				}
				break;
			case SDL_KEYUP:
				switch(e.key.keysym.sym){
					case SDLK_w:
						key_w = false;
						break;
					case SDLK_s:
						key_s = false;
						break;
					case SDLK_a:
						key_a = false;
						break;
					case SDLK_d:
						key_d = false;
						break;
					case SDLK_LEFT:
						key_left = false;
						break;
					case SDLK_RIGHT:
						key_right = false;
						break;
					default:
						break;
				}
				break;
			default:
				break;
		}

		if(key_w && !key_s){
			cameraLocation = cameraLocation + mat4::eye(0.05)*cameraRotation.forward();
		}
		else if(key_s && !key_w){
			cameraLocation = cameraLocation + mat4::eye(0.05)*cameraRotation.backward();
		}
		if(key_a && !key_d){
			cameraLocation = cameraLocation + mat4::eye(0.05)*cameraRotation.left();
		}
		else if(key_d && !key_a){
			cameraLocation = cameraLocation + mat4::eye(0.05)*cameraRotation.right();
		}

		/*if(key_left && !key_right){
			cameraRotation = quat(-0.1,vec3::up)*cameraRotation;
		}
		else if(key_right && !key_left){
			cameraRotation = quat(0.1,vec3::up)*cameraRotation;
		}*/

		glUniformMatrix4fv(modelMatrixLoc,1,GL_TRUE,model.getTransform().array);
		mat4 cameraMatrix = mat4::rotation(!cameraRotation)*mat4::translation(-cameraLocation);
		glUniformMatrix4fv(viewMatrixLoc,1,GL_TRUE,(projectionMatrix*cameraMatrix).array);

		glUniform4fv(modelColorLoc,1,color.buffer);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDrawElements(GL_TRIANGLES,sizeof(indices)/sizeof(uint32),GL_UNSIGNED_INT,(void*)0);

		SDL_GL_SwapWindow(window);
	}

	return 0;
}