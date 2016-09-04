extern "C"
{

//  utility, data

void cudaInit(int argc, char **argv);
void threadSync();

void allocateArray(void **devPtr, int size);
void freeArray(void *devPtr);

void copyFromDevice(void* host, const void* device, uint vbo, int size);
void copyToDevice(void* device, const void* host, int offset, int size);

void registerGLvbo(uint vbo);
void unregGLvbo(uint vbo);


void setParameters(SimParams *hostParams);


//  System

void integrate(uint vboOldPos, uint vboNewPos, float4* oldVel, float4* newVel,
		int numParticles);

void calcHash(uint vboPos, uint2* particleHash, int numParticles);

void reorder(uint vboOldPos, float4* oldVel, float4* sortedPos, float4* sortedVel,
		uint2* particleHash, uint* cellStart,  uint numParticles, uint numCells);

void collide(uint timer, uint vboOldPos, uint vboNewPos, /**/uint vboCLR2,
		float4* sortedPos, float4* sortedVel, float4* oldVel, float4* newVel,
		float* pressure, float* density, float* dyeColor,//
		uint2* particleHash, uint* cellStart,  uint numParticles, uint numCells);


}
