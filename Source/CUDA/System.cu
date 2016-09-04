#include <cutil.h>
#include <GL/glut.h>
#include <cuda_gl_interop.h>

#include "Kernel.cu"
#include "radixsort.cu"


extern "C"
{

//  Utility, data
#define CU  CUDA_SAFE_CALL

#define cuMapVbo(pv,vbo)		CU(cudaGLMapBufferObject((void**)&pv, vbo));
#define cuUnMapVbo(vbo)			CU(cudaGLUnmapBufferObject(vbo));

#define cuBindTex(tex,pv,size)	CU(cudaBindTexture(0, tex, pv, size));
#define cuUnbindTex(tex)		CU(cudaUnbindTexture(tex));


void cudaInit(int argc, char **argv)	{   CUT_DEVICE_INIT(argc, argv);	}
bool cudaInitB(int argc, char **argv, bool showInfo)	//  no exit + info
{
	int deviceCount;
	CUDA_SAFE_CALL_NO_SYNC(cudaGetDeviceCount(&deviceCount));
	if (deviceCount == 0)  {
		fprintf(stderr, "Error: no devices supporting CUDA.\n");
		return false;  }  //exit(EXIT_FAILURE);

	int dev = 0;
	cutGetCmdLineArgumenti(argc, (const char**)argv, "device", &dev);
	if (dev > deviceCount-1) dev = deviceCount - 1;
	cudaDeviceProp p;

	CUDA_SAFE_CALL_NO_SYNC(cudaGetDeviceProperties(&p, dev));
	if (p.major < 1)  {
		fprintf(stderr, "Error: device does not support CUDA.\n");
		return false;	}
    if (dev == 0)  {
        if (p.major == 9999 && p.minor == 9999)
            printf("There is no device supporting CUDA.\n");
        else  if (deviceCount != 1)
            printf("There are %d devices supporting CUDA\n", deviceCount);  }
	
	if (cutCheckCmdLineFlag(argc, (const char**)argv, "quiet") == CUTFalse)
		fprintf(stderr, "Using device %d: %s\n\n", dev, p.name);
		
	if (showInfo)
	{
        printf("CUDA version:  %d.%d\n", p.major, p.minor);
        printf("Total memory:  %u bytes  (%u MB)\n", p.totalGlobalMem, p.totalGlobalMem/1024/1024);
		#if CUDART_VERSION >= 2000
        printf("Multiprocessors:  %d  Cores:  %d\n", p.multiProcessorCount, 8*p.multiProcessorCount);
		#endif
		printf("Clock rate:  %.4f GHz\n", p.clockRate * 1e-6f);
		printf(" Warp size:  %d\n", p.warpSize);
		printf(" Constant memory:  %u bytes\n", p.totalConstMem); 
		printf("  Shared memory per block:  %u bytes\n", p.sharedMemPerBlock);
		printf("  Registers per block:   %d\n", p.regsPerBlock);
		printf("  Max threads per block: %d\n", p.maxThreadsPerBlock);
		printf("  Max sizes of a block:  %d x %d x %d\n", p.maxThreadsDim[0], p.maxThreadsDim[1], p.maxThreadsDim[2]);
		printf("  Max sizes of a grid:   %d x %d x %d\n", p.maxGridSize[0], p.maxGridSize[1], p.maxGridSize[2]);
		printf("  Max memory pitch:    %u bytes\n", p.memPitch);
		printf("  Texture alignment:   %u bytes\n", p.textureAlignment);
	    #if CUDART_VERSION >= 2000
        printf("  Concurrent copy and execution:  %s\n\n", p.deviceOverlap ? "Yes" : "No");
		#endif
	}
	
	CUDA_SAFE_CALL(cudaSetDevice(dev));
	return true;
}
void threadSync()						{	CU(cudaThreadSynchronize());	}


void allocateArray(void **devPtr, size_t size)	{	CU(cudaMalloc(devPtr, size));	}
void freeArray(void *devPtr)					{	CU(cudaFree(devPtr));	}

void registerGLvbo(uint vbo)		{	CU(cudaGLRegisterBufferObject(vbo));	}
void unregGLvbo(uint vbo)			{	CU(cudaGLUnregisterBufferObject(vbo));	}


void copyFromDevice(void* host, const void* device, unsigned int vbo, int size)
{
	if (vbo)  cuMapVbo(device, vbo);
	CU(cudaMemcpy(host, device, size, cudaMemcpyDeviceToHost));
	if (vbo)  cuUnMapVbo(vbo);
}
void copyToDevice(void* device, const void* host, int offset, int size)
{
	CU(cudaMemcpy((char*) device + offset, host, size, cudaMemcpyHostToDevice));
}

void setParameters(SimParams *hostParams)
{	// copy parameters to constant memory
	CU(cudaMemcpyToSymbol(par, hostParams, sizeof(SimParams)));
}


//  Round a / b to nearest higher integer value
int iDivUp(int a, int b) {	return a%b != 0 ? a/b+1 : a/b;	}

//  compute grid and thread block size for a given number of elements
void computeGridSize(int n, int blockSize, int &numBlocks, int &numThreads)
{
	numThreads = min(blockSize, n);  numBlocks = iDivUp(n, numThreads);
}
//----------------------------------------------------------------------------------



///  integrate
void integrate(uint vboOldPos, uint vboNewPos, float4* oldVel, float4* newVel, int numParticles)
{
	int numThreads, numBlocks;
	computeGridSize(numParticles, 256, numBlocks, numThreads);

	float4 *oldPos, *newPos;
	cuMapVbo(oldPos, vboOldPos);  cuMapVbo(newPos, vboNewPos);

	integrateD<<< numBlocks, numThreads >>>(newPos, newVel, oldPos, oldVel);

	CUT_CHECK_ERROR("Kernel execution failed: Integrate");
	cuUnMapVbo(vboOldPos);  cuUnMapVbo(vboNewPos);

	/***/threadSync();
}


///  calcHash
void calcHash(uint vboPos, uint2* particleHash, int numParticles)
{
	int numThreads, numBlocks;
	computeGridSize(numParticles, 2*256, numBlocks, numThreads);

	float4 *pos;
	cuMapVbo(pos, vboPos);

	calcHashD<<< numBlocks, numThreads >>>(pos, particleHash);

	CUT_CHECK_ERROR("Kernel execution failed: calcHash");
	cuUnMapVbo(vboPos);

	/***/threadSync();
}


///  reorder
void reorder(uint vboOldPos, float4* oldVel, float4* sortedPos, float4* sortedVel,
		uint2* particleHash, uint* cellStart,  uint numParticles, uint numCells)
{
	int numThreads, numBlocks;
	computeGridSize(numParticles, 256, numBlocks, numThreads);
	CU(cudaMemset(cellStart, 0xffffffff, numCells*sizeof(uint)));

	float4 *oldPos;
	cuMapVbo(oldPos, vboOldPos);
	#if USE_TEX
	uint spar4 = numParticles*sizeof(float4);
	cuBindTex(oldPosTex, oldPos, spar4);	cuBindTex(oldVelTex, oldVel, spar4);
	#endif

	reorderD<<< numBlocks, numThreads >>>(particleHash, cellStart,
		oldPos, oldVel, sortedPos, sortedVel);

	CUT_CHECK_ERROR("Kernel execution failed: reorder");
	#if USE_TEX
	cuUnbindTex(oldPosTex);  cuUnbindTex(oldVelTex);
	#endif
	cuUnMapVbo(vboOldPos);

	/***/threadSync();
}


///  collide
void collide(uint timer,  uint vboOldPos, uint vboNewPos, /**/uint vboCLR2,
		float4* sortedPos, float4* sortedVel, float4* oldVel, float4* newVel,
		float* pressure, float* density, float* dyeColor,
		uint2* particleHash, uint* cellStart, uint numParticles, uint numCells)
{
	/**/CU(cutStartTimer(timer));
	float4 *oldPos, *newPos, *clr2;
	cuMapVbo(clr2, vboCLR2);
	cuMapVbo(oldPos, vboOldPos);  cuMapVbo(newPos, vboNewPos);  

	#if USE_TEX
	uint spar4 = numParticles*sizeof(float4), spar = numParticles*sizeof(float);
	cuBindTex(oldPosTex, sortedPos, spar4);  cuBindTex(pressureTex, pressure, spar);
	cuBindTex(oldVelTex, sortedVel, spar4);	 cuBindTex(densityTex, density, spar);  cuBindTex(dyeColorTex, dyeColor, spar);//
	cuBindTex(particleHashTex, particleHash, numParticles*sizeof(uint2));
	cuBindTex(cellStartTex, cellStart, numCells*sizeof(uint));
	#endif


	int numThreads, numBlocks;
	computeGridSize(numParticles, 64, numBlocks, numThreads);
	

	computeDensityD<<< numBlocks, numThreads >>>(clr2, sortedPos,  pressure, density,  particleHash, cellStart);
	/***/threadSync();
	/**/CU(cutStopTimer(timer));

	  computeForceD<<< numBlocks, numThreads >>>(newPos, newVel, sortedPos, sortedVel,  clr2, pressure, density, dyeColor/**/, particleHash, cellStart);
		//collideD<<< numBlocks, numThreads >>>(newPos, newVel, sortedPos, sortedVel, /**/clr2,  particleHash, cellStart);

	CUT_CHECK_ERROR("Kernel execution failed: Collide");

	cuUnMapVbo(vboNewPos);  cuUnMapVbo(vboOldPos);  cuUnMapVbo(vboCLR2);
	#if USE_TEX
	cuUnbindTex(oldPosTex);		cuUnbindTex(oldVelTex);
	cuUnbindTex(pressureTex)	cuUnbindTex(densityTex);	cuUnbindTex(dyeColorTex);//
	cuUnbindTex(particleHashTex);	cuUnbindTex(cellStartTex);
	#endif
	/***/threadSync();
}


}  //extern "C"