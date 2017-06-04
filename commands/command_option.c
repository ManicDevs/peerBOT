#include <stdio.h>
#include <stdlib.h>

#include "ipfs/commands/command_option.h"

int commands_command_option_init(struct CommandOption* option, char* description) {
	option->description = description;
	// allocate memory for names
	option->names = malloc(option->name_count * sizeof(char*));
	if (option->names == NULL)
		return 0;
	return 1;
}

int commands_command_option_free(struct CommandOption* option) {
	free(option->names);
	free(option);
	return 0;
}
