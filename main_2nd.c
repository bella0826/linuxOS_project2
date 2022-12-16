#include <syscall.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <stdlib.h>
#include <dlfcn.h>

int init_data = 30;
int non_init_data;
long ret = 0;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

void* our_func()
{
	printf("Hello World!\n");	
}

int main(){

	int local_data = 30;
	int* heapAddress = malloc(sizeof(int));
	char test = '\n';
	printf("start! %c\n", test);
	void* fHandle;
	void* func;
	fHandle = dlopen("/lib/x86_64-linux-gnu/libc.so.6",RTLD_LAZY);
	printf("libc base: %lx\n\n",*(size_t*)fHandle);
	if (!fHandle) {
		printf ("%s\n", dlerror());
		exit(1);
	}
	func = dlsym(fHandle, "printf");
	static __thread int tls = 0;
	if(dlerror() != NULL){
		printf("%s\n", dlerror());
		exit(1);
	}
	
	pid_t PID = fork();
	
	if(PID == -1)
	{
		printf("error\n");
	}
	else if(PID == 0)
	{
		printf("It's child process, PID is %d.", getpid());

		size_t* va[7] = {
			(size_t*)&init_data,
			(size_t*)&non_init_data,
			(size_t*)&our_func,
			(size_t*)&local_data,
			(size_t*)func,
			(size_t*)heapAddress,
			(size_t*)&tls
		};
		size_t* pa[7];
		ret = syscall(333, va, pa, 7);
		char printArray [7][20] = {"Data", "BSS", "Code", "Stack", 
				   "Library", "heap", "thread"};
		for(int i=0;i<7;i++){
		printf("\n %d) %8s (va/pa) = %16p / %20p", i+1, printArray[i], va[i], pa[i]);
		}
		tls++;
		printf("\n\n");
		sleep(10);
	}
	else if(PID > 0)
	{
		printf("It's parent process!, PID is %d.", getpid());

		size_t* va[7] = {
			(size_t*)&init_data,
			(size_t*)&non_init_data,
			(size_t*)&our_func,
			(size_t*)&local_data,
			(size_t*)func,
			(size_t*)heapAddress,
			(size_t*)&tls
		};

		size_t* pa[7];
		ret = syscall(333, va, pa, 7);
		char printArray [7][20] = {"Data", "BSS", "Code", "Stack", 
				   "Library", "heap", "thread"};
		for(int i=0;i<7;i++){
		printf("\n %d) %8s (va/pa) = %16p / %20p", i+1, printArray[i], va[i], pa[i]);
		}
		tls++;
		printf("\n\n");
		
		pid_t rpid = wait(NULL);
		printf("Catch the child process pid : %d\n",rpid);
	}
	
	if(ret > 0) printf("error!!!QQQQQQQ");
	return 0;
}             
