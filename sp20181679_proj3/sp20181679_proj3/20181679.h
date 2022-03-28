#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>// DIR entry
#include <sys/stat.h>// stat>

#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_RESET "\x1b[0m"

unsigned int PROG_ADDR = 0x00;
unsigned int bp[0x1077];
int bp_idx = 0;
unsigned int RETADR;
unsigned int BUFFER;
unsigned int LENGTH=0x3;

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

typedef struct _estab{
	char cs[20];
	char symbol[20];
	unsigned int addr;
	int cslth;
	int flag;
	struct _estab *next;
}ESTAB;
ESTAB *eshead[20];

char **ref;

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


/*----------------loader----------------*/

//find symbol or cs in estab
//return 1 or 0
int find_sym_estab(char *sym);

//return index
int find_sym_estab2(char *sym);

//linking loader PASS1
int ll_pass1(char *file1, char *file2, char *file3);

//linking loader PASS2
int ll_pass2(char *file1, char *file2, char *file3, unsigned char **dump);

/*------------------run-----------------*/

//do value exists in bp?
int is_bp(unsigned int value);

//erase used bp
int is_bp2(unsigned int value);

//return mnemonic
char* find_opcode_3(unsigned int opcode);

//run
int run(unsigned char **dump, unsigned int *PC, unsigned int *L, unsigned int *A, unsigned int *X, unsigned int *B, unsigned int *S, unsigned int *T);






