#include <stdio.h>
#include <stddef.h>
#include <unistd.h>
#include <utmp.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <grp.h>
#include <pwd.h>
#include <dlfcn.h>

#define USER_FILE_DIR "/var/run/utmp"
#define LIB_DIR "./lib.so.0.1"
#define OPTS "12"
#define USAGE_ERR "Usage: %s [-1] [-2]\n"

char* (*get_groups_ptr)(uid_t);

int set_flags(char* flag, int argc, char** argv) {
	int opt;

	while((opt = getopt(argc, argv, OPTS)) != -1) {
		switch(opt) {
			case '1':
				*flag = *flag | 0x01;
				break;
			case '2':
				*flag = *flag | 0x02;
				break;
			default:
				return 1;
		}
	}
	
	return 0;
}

int show_users(const char flag) {
	if(utmpname(USER_FILE_DIR) == -1)
		return 1;
	
	setutent(); // Rewind the file pointer
	struct utmp* user = NULL;
	struct passwd* pass = NULL;
	
	while((user = getutent()) != NULL) { 
		if(user->ut_type == USER_PROCESS) {
			if((pass = getpwnam(user->ut_user)) == NULL)
				return 1;

			printf("%s ", user->ut_user); // Print user's name
			
			if(!(flag & 0x04) && get_groups_ptr != NULL) {
				if(flag & 0x01)
					printf("%s ", user->ut_host); // Print user's host
			
				if(flag & 0x02) {
					char* groups = NULL;
					if((groups = get_groups_ptr(pass->pw_uid)) == NULL) // Get all the groups
						return 1;
					
					printf("%s", groups);
					free(groups);
				}
			}
			
			puts("");
		}	
	}

	endutent(); 
	return 0;
}

int main(int argc, char** argv) {
	char flag = 0x0;
	void* handle = dlopen(LIB_DIR, RTLD_LAZY);

	if(!handle) {
		puts(dlerror());	
		flag |= 0x04;
	}
	
	get_groups_ptr = dlsym(handle, "get_groups");

	if(set_flags(&flag, argc, argv)) {
		fprintf(stderr, USAGE_ERR,
			argv[0]);
		return 1;
	}
	
	if(show_users(flag)) {
		fprintf(stderr, "%s", strerror(errno));
		return 1;
	}

	if(handle)
		dlclose(handle);

	return 0;
}