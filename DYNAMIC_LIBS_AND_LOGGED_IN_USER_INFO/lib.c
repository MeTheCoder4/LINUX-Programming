#include <stddef.h>
#include <stdlib.h>
#include <grp.h>
#include <string.h>
#include <pwd.h>

char* get_groups(uid_t user_id) {			
	struct passwd* pass = NULL;
	gid_t* groups = NULL;
	char* result = NULL;
	int ngroups = 0, string_size = 0;
	char* prefix = "[ ", *sufix = "]";

	if((pass = getpwuid(user_id)) == NULL)
		return NULL;

	if(getgrouplist(pass->pw_name, pass->pw_gid, // This call sets ngroups variable.
		groups, &ngroups)) {					 // It uses the fact, that an user should always belong to at least one group (primary group).										 
													// The value returned, as has been stated in documentation, can be used for resizing
		if((groups = (gid_t*)malloc(sizeof(gid_t) * ngroups)) == NULL) // the size of a buffer passed to a further call of getgrouplist().
			return NULL;
					
		if(getgrouplist(pass->pw_name, pass->pw_gid,
			groups, &ngroups) == -1)
				return NULL;
	}
	
	for(int i = 0; i < ngroups; i++)
		string_size += strlen(getgrgid(groups[i])->gr_name);
	
	string_size += ngroups; // For each space between group names
	string_size += strlen(prefix) + strlen(sufix) + 1; // length of prefix and postfix + one byte for null terminator

	if((result = (char*)calloc(string_size, 1)) == NULL)
		return NULL;
		
	strcat(result, prefix);

	for(int i = 0; i < ngroups; i++) {
		strcat(result, getgrgid(groups[i])->gr_name);
		strcat(result, " ");
	}

	strcat(result, sufix);
	free(groups);
	return result;
}