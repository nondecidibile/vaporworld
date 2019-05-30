#include "core_types.h"
#include "coremin.h"
#include "gldrv/gldrv.h"
#include "math/math.h"
#include <SDL2/SDL.h>
#include "world/world.h"
#include "game/importer.h"

#define STB_IMAGE_IMPLEMENTATION
#include "game/stb_image.h"

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

template<typename T>
T lerp(T a, T b, float32 alpha)
{
	return a * (1.f - alpha) + b * alpha;
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

	uint32 programC = glCreateProgram();
	uint32 vertexShaderC = glCreateShader(GL_VERTEX_SHADER);
	uint32 fragmentShaderC = glCreateShader(GL_FRAGMENT_SHADER);

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

	char* vertexShaderCSource = readFile("shaders/shaderC.vert");
	glShaderSource(vertexShaderC,1,&vertexShaderCSource,nullptr);
	glCompileShader(vertexShaderC);
	char* fragmentShaderCSource = readFile("shaders/shaderC.frag");
	glShaderSource(fragmentShaderC,1,&fragmentShaderCSource,nullptr);
	glCompileShader(fragmentShaderC);

	glAttachShader(programT,vertexShaderT);
	glAttachShader(programT,geometryShaderT);
	glAttachShader(programT,fragmentShaderT);
	glLinkProgram(programT);

	glAttachShader(programL,vertexShaderL);
	glAttachShader(programL,fragmentShaderL);
	glLinkProgram(programL);

	glAttachShader(programC,vertexShaderC);
	glAttachShader(programC,fragmentShaderC);
	glLinkProgram(programC);

	/* ------------------------------------------------------------- */

	World world = World(32,2, 0.5, -5,5, 0,5, 0,5);
	uint32 blockVertices = world.worldBlocks[0].N;
	uint32 blockTrianglesIndices = world.worldBlocks[0].numTrianglesIndices;
	uint32 blockLinesIndices = world.worldBlocks[0].numLinesIndices;
	
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
	glVertexAttribPointer(1,3,GL_UNSIGNED_BYTE,GL_TRUE,sizeof(Vertex),(void*)offsetof(Vertex,normal));
	glVertexAttribPointer(2,4,GL_UNSIGNED_BYTE,GL_TRUE,sizeof(Vertex),(void*)offsetof(Vertex,color));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	/* ------------------------------------------------------------- */

	MeshData carMesh;
	Importer importer;

	importer.loadScene("assets/lambo.fbx");
	importer.importStaticMesh(carMesh);

	uint32 car_vao;
	glGenVertexArrays(1,&car_vao);
	glBindVertexArray(car_vao);

	uint32 car_vbo, car_ebo;
	glGenBuffers(1,&car_vbo);
	glGenBuffers(1,&car_ebo);
	glBindBuffer(GL_ARRAY_BUFFER,car_vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,car_ebo);

	glBufferData(GL_ARRAY_BUFFER,carMesh.vertexBuffer.getBytes(),*carMesh.vertexBuffer,GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,carMesh.indexBuffer.getBytes(),*carMesh.indexBuffer,GL_STATIC_DRAW);

	glVertexAttribFormat(0,3,GL_FLOAT,GL_FALSE,0);
	glVertexAttribFormat(1,3,GL_FLOAT,GL_FALSE,12);
	glVertexAttribFormat(3,2,GL_FLOAT,GL_FALSE,36);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(3);

	/* ------------------------------------------------------------- */

	enum TextureType{
		DIFFUSE = 0,
		GLOSS,
		SPECULAR,

		NUM_TEXTURES
	};

	const char *textureFiles[] = {
		"assets/lambo_diffuse.jpeg",
		"assets/lambo_gloss.jpeg",
		"assets/lambo_spec.jpeg"
	};

	uint32 textures[NUM_TEXTURES];
	glGenTextures(NUM_TEXTURES, textures);
	for(int i=0; i<NUM_TEXTURES; i++){
		int32 width, height, channels;
		ubyte *data = stbi_load(textureFiles[i],&width,&height,&channels,0);
		glBindTexture(GL_TEXTURE_2D,textures[i]);
		glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,0,GL_RGB,GL_UNSIGNED_BYTE,data);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glGenerateMipMap(GL_TEXTURE_2D);
		stbi_image_free(data);
	}

	/* ------------------------------------------------------------- */

	cameraLocation = vec3(0,0,0);
	cameraRotation = quat(0,vec3::up);
	cameraVelocity = vec3::zero;
	projectionMatrix = mat4::glProjection(M_PI/2, 0.1f);

	int32 viewMatrixLocT = glGetUniformLocation(programT,"viewMatrix");
	int32 viewMatrixLocL = glGetUniformLocation(programL,"viewMatrix");
	int32 viewMatrixLocC = glGetUniformLocation(programC,"viewMatrix");

	uint32 camLocUniformT = glGetUniformLocation(programT, "cameraLocation");
	uint32 camLocUniformL = glGetUniformLocation(programL, "cameraLocation");
	uint32 camLocUniformC = glGetUniformLocation(programC, "cameraLocation");

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
	int32 modelMatrixLocC = glGetUniformLocation(programC,"modelMatrix");
	
	int32 timeColorLocL = glGetUniformLocation(programL,"timeColor");
	int32 timeColorLocT = glGetUniformLocation(programT,"timeColor");
	vec4 timeColorVec = vec4(0,0,0,0);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	Model carModel{
		vec3(0,-0.5,0),
		quat(M_PI_2,vec3::right),
		vec3(0.001)
	};

	vec3 carSpeed = vec3(0.f);
	
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

		// Move car
		vec3 carAcceleration = vec3::forward * (keys[SDLK_w] - keys[SDLK_s]) * 12.f - carSpeed;
		carSpeed += carAcceleration * dt;
		carModel.location += carSpeed * dt;

		/*
		*	CAMERA
		*/

		//float x_rotation = axes[0]*dt;
		//float y_rotation = axes[1]*dt;
		//axes[0] = 0; axes[1] = 0;
		float x_rotation = (keys[SDLK_RIGHT]-keys[SDLK_LEFT])*2*dt;
		float y_rotation = (keys[SDLK_DOWN]-keys[SDLK_UP])*2*dt;
		cameraRotation = quat(x_rotation,vec3::up)*quat(y_rotation,cameraRotation.right()) * cameraRotation;

		/* const float32 accelFactor = 12.f;
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
		cameraLocation += cameraVelocity * dt; */
		vec3 targetCameraLocation = carModel.location + cameraRotation.backward() * 1.5f;
		cameraLocation = lerp(cameraLocation, targetCameraLocation, 0.5f);

		mat4 cameraMatrix = mat4::rotation(!cameraRotation) * mat4::translation(-cameraLocation);

		/* carModel.location.x = cameraLocation.x;
		carModel.location.z = cameraLocation.z + 1; */


		/*
		*	DRAW
		*/

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER,vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ebo);

		Vertex *vertices;
		uint32 *trianglesIndices, *linesIndices;
		world.updateWorld(cameraLocation.z+256);
		timeColorVec.x += 0.01;
		for(int i=0; i<world.worldBlocks.getCount(); i++){
			WorldBlock *b = &(world.worldBlocks[i]);
			vertices = b->vertices;
			trianglesIndices = b->trianglesIndices;
			linesIndices = b->linesIndices;
			float pos = b->startY;

			if(cameraLocation.z > b->startY-16 && cameraLocation.z < b->endY+16){

				glUseProgram(programT);
				glUniform4fv(timeColorLocT,1,timeColorVec.buffer);
				glUniform3fv(camLocUniformT, 1, cameraLocation.buffer);
				glUniformMatrix4fv(viewMatrixLocT,1,GL_TRUE,(projectionMatrix * cameraMatrix).array);

				glBufferData(GL_ARRAY_BUFFER,blockVertices*sizeof(Vertex),vertices,GL_STATIC_DRAW);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER,blockTrianglesIndices*sizeof(uint32),trianglesIndices,GL_STATIC_DRAW);
				
				float height = 0.5;

				/*mat4 transformation = mat4::translation(vec3(-0.25,-height-0.001,pos))*mat4(1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1);
				glUniformMatrix4fv(modelMatrixLocT,1,GL_TRUE,transformation.array);
				glDrawElements(GL_TRIANGLES,blockTrianglesIndices,GL_UNSIGNED_INT,(void*)0);*/

				mat4 transformation = mat4::translation(vec3(0.25,-height-0.001,pos))*mat4(-1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1);
				glUniformMatrix4fv(modelMatrixLocT,1,GL_TRUE,transformation.array);
				glDrawElements(GL_TRIANGLES,blockTrianglesIndices,GL_UNSIGNED_INT,(void*)0);

				/*glUseProgram(programL);
				glUniform4fv(timeColorLocL,1,timeColorVec.buffer);
				glUniform3fv(camLocUniformL, 1, cameraLocation.buffer);
				glUniformMatrix4fv(viewMatrixLocL,1,GL_TRUE,(projectionMatrix * cameraMatrix).array);
	
				glEnable(GL_LINE_SMOOTH);
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);

				glLineWidth(3);
				transformation = mat4::translation(vec3(-0.25,-height,pos))*mat4(1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1);
				glUniformMatrix4fv(modelMatrixLocL,1,GL_TRUE,transformation.array);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER,blockLinesIndices*sizeof(uint32),linesIndices,GL_STATIC_DRAW);
				glDrawElements(GL_LINES,blockLinesIndices,GL_UNSIGNED_INT,(void*)0);
				
				transformation = mat4::translation(vec3(0.25,-height,pos))*mat4(-1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1);
				glUniformMatrix4fv(modelMatrixLocL,1,GL_TRUE,transformation.array);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER,blockLinesIndices*sizeof(uint32),linesIndices,GL_STATIC_DRAW);
				glDrawElements(GL_LINES,blockLinesIndices,GL_UNSIGNED_INT,(void*)0);
				*/
			}
		}

		//carModel.rotation = quat(dt * 1.f, vec3::up) * carModel.rotation;

		glUseProgram(programC);

		glBindVertexArray(car_vao);
		glBindVertexBuffer(0,car_vbo,0,sizeof(VertexData));
		glVertexAttribBinding(0,0);
		glVertexAttribBinding(1,0);
		glVertexAttribBinding(3,0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, car_ebo);

		glUniform3fv(camLocUniformC, 1, cameraLocation.buffer);
		glUniformMatrix4fv(viewMatrixLocC,1,GL_TRUE,(projectionMatrix * cameraMatrix).array);
		glUniformMatrix4fv(modelMatrixLocC,1,GL_TRUE,carModel.getTransform().array);

		for(int i=0; i<NUM_TEXTURES; i++){
			glActiveTexture(GL_TEXTURE0+i);
			glBindTexture(GL_TEXTURE_2D,textures[i]);
		}

		glDrawElements(GL_TRIANGLES,carMesh.indexBuffer.getCount(),GL_UNSIGNED_INT,0);

		SDL_GL_SwapWindow(window);
	}

	return 0;
}
