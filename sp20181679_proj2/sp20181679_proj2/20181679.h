#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>// DIR entry
#include <sys/stat.h>// stat>

#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_RESET "\x1b[0m"

typedef struct _node{
	char cmd_name[100];	
	struct _node *next;
}NODE;
NODE *head = NULL;

typedef struct _opnode{
	unsigned int opcode;
	char opcode_name[10];
	char code_format[5];
	struct _opnode *next;
}OPNODE;
OPNODE *ophash[20];

typedef struct _symtab{
	char symbol[20];
	unsigned int locctr;
	int flag;
	struct _symtab *next;
}SYM;
SYM *shead[20];

//dump, edit, fill, opcode mnemonic commands use this function.
//classify words from command.
void cmd_classifier(char* command, char** cmd_arr, int* idx);


/*--------------shell-------------------*/

//h[elp]
void print_all_commands();

//d[ir]
int print_file();

//hi[story]
//find right command
void for_history(char* command, NODE* tmp);


/*--------------memory------------------*/

//change to ASCII
void printchar(int val);

//du[mp] [start, end]
//print the dump memory with start address and end address
//last address is used for dump again
void dump_print(int start, int end, int *last_addr, unsigned char **dump);

//f[ill] [start, end, value]
//from start to end, fill dump with the value
void fill_dump(int start, int end, int value, unsigned char **dump);


/*--------------opcode------------------*/

//make random hash key for opcodes
int hash_func(int opcode,char* opcode_name);

//make hash table with opcodes
void make_hash_table(int opcode, char *opcode_name, char *code_format);

//opcodelist
void print_hash_table();

//opcode mnemonic
//find and print mnemonic's opcode
int find_opcode(char *opcode_name);

/*--------------assemble----------------*/

int find_opcode_2(char *opcode_name);

int find_sym(char *op);

void PASS2(int prog_len, FILE *fp, char *f_name);

int assemble_file(FILE *fp, char *filename);

/*----------------type------------------*/

int find_file(char *filename);

void read_file(char *filename);

/*---------------symbol-----------------*/

//for align
void SWAP(int j);

//print the symbol table
void print_symbol();


