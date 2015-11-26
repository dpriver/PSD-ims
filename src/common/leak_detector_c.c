#include	<stdio.h>
#include	<malloc.h>
#include	<string.h>
#include	"leak_detector_c.h"

#undef		malloc
#undef		calloc
#undef		realloc
#undef 		free


static MEM_LEAK * ptr_start = NULL;
static MEM_LEAK * ptr_last =  NULL;

static int n_elems_alloc = 0;
static int n_elems_list = 0;

static int n_alloc_errors = 0;
static int n_free_errors = 0;

/*
 * logs mem_alloc information
 */
/*
void log_mem_alloc(const char *operation, void * mem_ref, unsigned int size,  const char * file, unsigned int line) 
{
	char info[1024];
	FILE * fp_write = fopen (OUTPUT_LOG_FILE, "a+");
	fseek(fp_write, 0, SEEK_END);
	sprintf(info, "%s: ADDR=%p SIZE=%d FILE=%s [%d]\n", operation, mem_ref, size, file, line);
	fwrite(info, (strlen(info) + 1) , 1, fp_write);
	fclose(fp_write);
}
*/

/*
 * writes all info of the unallocated memory into a file
 */
void report_mem_leak(void) 
{
	unsigned short index;
	MEM_LEAK * leak_info;

	FILE * fp_write = fopen (OUTPUT_FILE, "wt");
	char info[1024];

	if(fp_write != NULL)
	{
		sprintf(info, "%s\n", "Memory Leak Summary");
		fwrite(info, (strlen(info) + 1) , 1, fp_write);
		sprintf(info, "%s\n", "-----------------------------------");	
		fwrite(info, (strlen(info) + 1) , 1, fp_write);
		
		for(leak_info = ptr_start; leak_info != NULL; leak_info = leak_info->next)
		{
			sprintf(info, "address : %p\n", leak_info->mem_info.address);
			fwrite(info, (strlen(info) + 1) , 1, fp_write);
			sprintf(info, "size    : %d bytes\n", leak_info->mem_info.size);			
			fwrite(info, (strlen(info) + 1) , 1, fp_write);
			sprintf(info, "file    : %s\n", leak_info->mem_info.file_name);
			fwrite(info, (strlen(info) + 1) , 1, fp_write);
			sprintf(info, "line    : %d\n", leak_info->mem_info.line);
			fwrite(info, (strlen(info) + 1) , 1, fp_write);
			sprintf(info, "%s\n", "-----------------------------------");	
			fwrite(info, (strlen(info) + 1) , 1, fp_write);
		}
	}
	sprintf(info, "LIST=%d, ALLOC=%d\n", n_elems_list, n_elems_alloc);
	fwrite(info, (strlen(info) + 1) , 1, fp_write);
	sprintf(info, "ALLOC_ERR=%d, FREE_ERR=%d\n\n", n_alloc_errors, n_free_errors);
	fwrite(info, (strlen(info) + 1) , 1, fp_write);
	
	fclose(fp_write);
	clear();
}



/*
 * deletes all the elements from the list
 */
void clear()
{
	MEM_LEAK * temp = NULL;
	MEM_LEAK * alloc_info = ptr_start;

	while(alloc_info != NULL) 
	{
		temp = alloc_info;
		alloc_info = alloc_info->next;
		free(temp);
	}
}


/*
 * replacement of malloc
 */
void * xmalloc (unsigned int size, const char * file, unsigned int line)
{
	void * ptr = malloc (size);
	if (ptr == NULL) {
		n_alloc_errors++;
	}
	else {
		n_elems_alloc++;
		add_mem_info(ptr, size, file, line);
	}
	return ptr;
}


void *xrealloc(void *ptr, size_t size, const char * file, unsigned int line)
{
	void *ptr_new = realloc(ptr, size);
	if(ptr_new != NULL)
	{
		remove_mem_info(ptr);
		add_mem_info(ptr_new, size, file, line);
	}
	return ptr_new;
}

/*
 * replacement of calloc
 */
void * xcalloc (unsigned int elements, unsigned int size, const char * file, unsigned int line)
{
	unsigned total_size;
	void * ptr = calloc(elements , size);
	if(ptr != NULL)
	{
		n_elems_alloc++;
		total_size = elements * size;
		add_mem_info (ptr, total_size, file, line);
	}
	return ptr;
}


/*
 * replacement of free
 */
void xfree(void * mem_ref)
{
	if( remove_mem_info(mem_ref) == 0 ) {
		n_elems_alloc--;
		free(mem_ref);
	}
	else {
		n_free_errors++;
	}
}


/*
 * gets the allocated memory info and adds it to a list
 *
 */
void add_mem_info (void * mem_ref, unsigned int size,  const char * file, unsigned int line)
{
	/* fill up the structure with all info */
	MEM_LEAK * mem_leak = NULL;

	mem_leak = (MEM_LEAK *) malloc (sizeof(MEM_LEAK));
	mem_leak->mem_info.address = mem_ref;
	mem_leak->mem_info.size = size;
	strcpy(mem_leak->mem_info.file_name, file);
	mem_leak->mem_info.line = line;
	mem_leak->next = NULL;

	if (ptr_start == NULL)	
	{
		ptr_start = mem_leak;
		ptr_last = mem_leak;
	}
	else {
		ptr_last->next = mem_leak;
		ptr_last = mem_leak;				
	}
	n_elems_list++;
}


/*
 * if the allocated memory info is part of the list, removes it
 *
 */
int remove_mem_info (void * mem_ref)
{
	MEM_LEAK  *leak_info, *leak_info_prev = NULL;

	/* check if allocate memory is in our list */
	leak_info = ptr_start;

	while( leak_info != NULL ) {

		if( leak_info->mem_info.address == mem_ref ) {

			// Deletes the mem registry
			if( leak_info_prev == NULL ) { // is the first element
				ptr_start = leak_info->next;
				if( ptr_last == leak_info ) { // is also the last
					ptr_last = NULL;
				}	
			}
			else { // is not the first element
				leak_info_prev->next = leak_info->next;
				if( ptr_last == leak_info ) { // but is the last one
					ptr_last = leak_info_prev;
				}
			}
			n_elems_list--;

			free(leak_info);
			return 0;
		}
		leak_info_prev = leak_info;
		leak_info = leak_info->next;
	}
	return -1;
}
