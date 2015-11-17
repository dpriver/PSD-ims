
#ifndef  LEAK_DETECTOR_C_H
#define  LEAK_DETECTOR_C_H

#define  FILE_NAME_LENGTH   	   256
#define  OUTPUT_FILE			   "leak_info.txt"
#define  OUTPUT_LOG_FILE		 "leak_log.txt"
#define  malloc(size) 	    	   	xmalloc (size, __FILE__, __LINE__)
#define  calloc(elements, size)  	xcalloc (elements, size, __FILE__, __LINE__)
#define  realloc(ptr, size)			xrealloc (ptr, size, __FILE__, __LINE__)
#define  free(mem_ref) 		  	 	xfree(mem_ref)

struct _MEM_INFO
{
	void			*address;
	unsigned int	size;
	char			file_name[FILE_NAME_LENGTH];
	unsigned int	line;
};
typedef struct _MEM_INFO MEM_INFO;

struct _MEM_LEAK {
	MEM_INFO mem_info;
	struct _MEM_LEAK * next;
};
typedef struct _MEM_LEAK MEM_LEAK;

void clear(void);

void * xmalloc(unsigned int size, const char * file, unsigned int line);
void * xcalloc(unsigned int elements, unsigned int size, const char * file, unsigned int line);
void * xrealloc(void *ptr, size_t size, const char * file, unsigned int line);
void xfree(void * mem_ref);

void add_mem_info (void * mem_ref, unsigned int size,  const char * file, unsigned int line);
int remove_mem_info (void * mem_ref);
void report_mem_leak(void);
//void log_mem_alloc(const char *operation, void * mem_ref, unsigned int size,  const char * file, unsigned int line) 

#endif
