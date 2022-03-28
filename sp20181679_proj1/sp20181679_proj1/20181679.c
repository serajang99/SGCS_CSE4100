#include "20181679.h"
#include <ctype.h>

//for h[help]
//print the kinds of the commands exist.
void print_all_commands(){
	
	printf("h[elp]\nd[ir]\nq[uit]\nhi[story]\ndu[mp] [start, end]\ne[dit] address, value\nf[ill] start, end, value\nreset\nopcode mnemonic\nopcodelist\n");

}

//for d[ir]
//print the files in present directory
//use dirent.h, sys/stat.h
int print_file(){
	
	DIR *dp = NULL;//현재 디렉토리 가리킴
	struct dirent *d = NULL;
	struct stat buf;

	if((dp = opendir(".")) == NULL){
		printf("현재 디렉토리를 열 수 없습니다...\n");
		return -1;
	}
	//디렉토리 읽음
	while((d = readdir(dp)) != NULL){
		lstat(d->d_name,&buf);//dirent 구조체는 d_name에 filename 저장되어있음. 
		if(S_ISDIR(buf.st_mode))//directory인지 확인
			printf("%s/\t",d->d_name);
		else if((buf.st_mode&S_IXUSR)||(buf.st_mode&S_IXGRP)||(buf.st_mode&S_IXOTH))//실행 권한 확인(user,group,etc)
			printf("%s*\t",d->d_name);
		else if(S_ISREG(buf.st_mode))//일반 파일 확인
			printf("%s\t",d->d_name);
		
	}
	printf("\n");
	closedir(dp);//디렉토리 스트림 닫음

	return 0;
}

//store the command in NODE tmp
void for_history(char* command, NODE* tmp){

	if(!strcmp(command,"help") || !strcmp(command,"h") || !strcmp(command,"dir") || !strcmp(command,"d") || !strcmp(command,"history") || !strcmp(command,"hi") || !strncmp(command,"dump",4) || !strncmp(command,"du",2) || !strncmp(command,"edit",4) || !strncmp(command,"e",1) || !strncmp(command,"fill",4) || !strncmp(command,"f",1) || !strncmp(command,"opcode ",7) || !strcmp(command, "opcodelist") || !strcmp(command,"reset")){
		strcpy(tmp->cmd_name,command);
	}	
	else
		strcpy(tmp->cmd_name,"");
}

//change value to ASCII if value is in range(0x20,0x7e)
void printchar(int val){
	if(0x20 <= val && val <= 0x7E)
		printf("%c",(char)val);
	else
		printf(".");
}

//print dump memory
void dump_print(int start, int end,int* last_addr,unsigned char** dump){

	int i,j;

	//du[mp]
	if(start == -1 && end == -1){
		if(*last_addr >= 0xfffff)
			start = 0;
		else
			start = *last_addr+1;
		end = start+160-1;
		if(end>0xfffff)
			end = 0xfffff;
	}
	//du[mp] start
	else if(start != -1 && end == -1){
		end = start+160-1;
		if(end>0xfffff)
			end = 0xfffff;
	}
	//du[mp] start, end
	if(start != -1 && end != -1){
		//start can't bigger than end
		if(start>end){
			printf("errormsg");
		}

		if(start%16!=0){
			printf("%05X ",start-start%16);
			for(i=0;i<start%16*3;i++)
				printf(" ");
		}
		
		for (i=start;i<=end;i++){
			// 16바이트 단위로 시작 주소 출력
			if(i%16==0)
				printf("%05X ",i);
			// hex 값으로 메모리의 내용 출력
			printf("%02X ",(*dump)[i]);
			// 해당 16진수들을 각각 아스키 문자로 출력
			if(i%16-15==0) {
				printf(";");
				if(i==start+(16-start%16)-1){
					for(j=0;j<(start%16);j++)
						printf(".");
					for(j=start;j<=i;j++)
						printchar((*dump)[j]);
				}
				else{
					for(j=i-15;j<=i;j++)
						printchar((*dump)[j]);
				}
				printf("\n");
			}
		}
		// 마지막 라인이 16바이트 이하일 경우 정렬 유지
		if(end%16!=0xf){
			for(i=0;i<(16-(end%16+1))*3;i++)
				printf(" "); // 부족한 공간만큼 space로 이동한 후,
			printf(";");
			for(i=end-(end%16);i<=end;i++)
				printchar((*dump)[i]); // 남은 아스키 문자 값들 출력
			for(i=0;i<16-(end%16+1);i++)
				printf("."); 
			printf("\n");
		}
	}
	*last_addr = end;

} 

//classify the commands by space
void cmd_classifier(char* command,char** cmd_arr,int* idx){

	int i=0;
	char *ptr = strtok(command," ");
	
	while(ptr!=NULL){
		cmd_arr[i] = ptr;
		i++;
		ptr = strtok(NULL," ");
	}
	for(i=0;i<4;i++)
		if(cmd_arr[i]!=NULL){
			(*idx)++;
		}

}

//for fill and reset
void fill_dump(int start, int end, int value, unsigned char** dump){
	for(int i=start;i<=end;i++)
		(*dump)[i] = value;
}

//hash_func
//make hash key
int hash_func(int opcode,char* opcode_name){
	int re = (strlen(opcode_name)-opcode)%20;
	return re;
}

//make hash table with opcode
//opnode has information of opcode, opcode name and code format
void make_hash_table(int opcode, char *opcode_name, char *code_format){

	OPNODE* op = malloc(sizeof(OPNODE));
	op->opcode = opcode;
	strcpy(op->opcode_name,opcode_name);
	strcpy(op->code_format,code_format);
	int key = hash_func(opcode,opcode_name);
	if(ophash[key] == NULL)
		ophash[key] = op;
	else{
		op->next = ophash[key];
		ophash[key] = op;
	}
}

//print the hash table
void print_hash_table(){
	
	OPNODE* tmp = malloc(sizeof(OPNODE));
	for(int i=0;i<20;i++){
		printf("%d : ",i);
		tmp = ophash[i];
		while(tmp!=NULL){
			if(tmp->next==NULL)
				printf("[%s,%X]",tmp->opcode_name,tmp->opcode);
			else
				printf("[%s,%X]->",tmp->opcode_name,tmp->opcode);
			tmp = tmp->next;
		}
		printf("\n");
	}

}

//for opcode mnemonic
//read hash table to find opcode
int find_opcode(char* opcode_name){
	
	OPNODE* tmp = malloc(sizeof(OPNODE));
	for(int i=0;i<20;i++){
		tmp = ophash[i];
		while(tmp!=NULL){
			if(!strcmp(tmp->opcode_name,opcode_name)){
				printf("opcode is %X\n",tmp->opcode);
				return 1;
			}
			tmp = tmp->next;
		}
	}
	return 0;
}

int main (){
	
	char command[100];
	unsigned char* dump = (unsigned char*)calloc(sizeof(unsigned char),0xfffff);
	int last_addr = -1;
	NODE *head = malloc(sizeof(NODE));
	NODE *tmp = malloc(sizeof(NODE));
	head->next = tmp;
	strcpy(tmp->cmd_name,"");
	
	//read opcode.txt and make hash table
	int opcode = 0;
	char opcode_name[10] = {0,};
	char code_format[5] = {0,};
	FILE *fp = fopen("opcode.txt","r");
	while(fscanf(fp,"%x %s %s",&opcode,opcode_name,code_format)==3)
		make_hash_table(opcode,opcode_name,code_format);

	//반복 명령 시작
	while(1){
		printf("sicsim> ");
		fgets(command,sizeof(command),stdin);
		command[strlen(command)-1] = '\0';
		//잘못된 명령어를 입력하는 경우, 수행하지 않고 history에 안남음.

		//h[elp]
		if(!strcmp(command,"help") || !strcmp(command,"h")){
			print_all_commands();
			for_history(command,tmp);
		}
		//d[ir]
		else if(!strcmp(command,"dir") || !strcmp(command,"d")){
			print_file();	
			for_history(command,tmp);
		}
		//hi[story]
		else if(!strcmp(command,"history") || !strcmp(command,"hi")){
			for_history(command,tmp);
			NODE* cur = head->next;
			int nu = 1;
			while(cur != NULL){
				if(strcmp(cur->cmd_name,"")!=0)
					printf("%d\t%s\n",nu++,cur->cmd_name);
				cur = cur->next;
			}
		}
		//du[mp] [start, end]
		else if(!strncmp(command,"dump",4) || !strncmp(command,"du",2)){
			
			int idx=0;
			int dec_start=-1,dec_end=-1;
			char *cmd_arr[3] = {NULL,};
			char *ptr1, *ptr2;
			char *cmd = (char*)malloc(sizeof(char)*strlen(command));
			strcpy(cmd,command);
			cmd_classifier(command,cmd_arr,&idx);
			
			if(!strcmp(cmd_arr[0],"du") || !strcmp(cmd_arr[0],"dump")){
				//du[mp]
				if(idx == 1){
					dec_start = -1; dec_end = -1;
					dump_print(dec_start,dec_end,&last_addr,&dump);
				}
				//du[mp] start
				else if(idx == 2){
					dec_start = strtol(cmd_arr[1],&ptr1,16);
					dec_end = -1;
					//주소 범위 체크
					if(dec_start>=0x00000 && dec_start<=0xfffff){
						dump_print(dec_start,dec_end,&last_addr,&dump);
					}
					else{
						printf("ERROR:wrong value!!\n");
						strcpy(cmd,"");
					}
				}
				//du[mp] start, end
				else if(idx == 3){
					if(!strcmp(&cmd_arr[1][strlen(cmd_arr[1])-1],",")){
						cmd_arr[1][strlen(cmd_arr[1])-1] = '\0';
						dec_start = strtol(cmd_arr[1],&ptr1,16);
						dec_end = strtol(cmd_arr[2],&ptr2,16);
						//주소 범위 체크
						if(dec_start>=0x00000 && dec_start<=0xfffff && dec_end>=0x00000 && dec_end<=0xfffff && dec_start<=dec_end){
							dump_print(dec_start,dec_end,&last_addr,&dump);
						}
						else{
							printf("ERROR:wrong value!!\n");
							strcpy(cmd,"");
						}
					}
					else{
						printf("ERROR:wrong value or wrong command!!\n");
						strcpy(cmd,"");
					}
				}
				else{
					printf("ERROR:wrong command!! | du[mp] [start, end]\n");
					strcpy(cmd,"");
				}
			}
			else{
				printf("ERROR:wrong command!! | du[mp] [start, end]\n");
				strcpy(cmd,"");
			}

			for_history(cmd,tmp);

			for(int i=0;i<idx;i++)	
				cmd_arr[i] = NULL;
		}
		//e[dit] address, value
		else if(!strncmp(command,"edit",4) || !strncmp(command,"e",1)){
			int idx=0;
			int dec_addr=0;
			int value=0;
			char *cmd_arr[3] = {NULL,};
			char *ptr1, *ptr2;
			char *cmd = (char*)malloc(sizeof(char)*strlen(command));
			strcpy(cmd,command);

			cmd_classifier(command,cmd_arr,&idx);
			
			if(idx == 3){
				if(!strcmp(&cmd_arr[1][strlen(cmd_arr[1])-1],",")){
					//erase comma
					cmd_arr[1][strlen(cmd_arr[1])-1] = '\0';
					//get value
					dec_addr = strtol(cmd_arr[1],&ptr1,16);
					value = strtol(cmd_arr[2],&ptr2,16);
					//주소 범위, value 범위 체크
					if(value>=0x00 && value<=0xff && dec_addr>=0x00000 && dec_addr<=0xfffff){
						dump[dec_addr] = value;
					}
					else{
						printf("ERROR:wrong value or wrong address!!\n");
						strcpy(cmd,"");
					}
				}
				else{
					printf("ERROR:wrong command!! | e[dit] address, value\n");
					strcpy(cmd,"");
				}
			}
			else{
				printf("ERROR:wrong command!! | e[dit] address, value\n");
				strcpy(cmd,"");
			}
			for_history(cmd,tmp);

			for(int i=0;i<idx;i++)	
				cmd_arr[i] = NULL;
		}
		//f[ill] start, end, value
		else if(!strncmp(command,"fill",4) || !strncmp(command,"f",1)){
			int idx=0;
			char *cmd_arr[4] = {NULL,};
			int start_addr=0,end_addr=0,value=0;
			char *ptr1, *ptr2;
			char *cmd = (char*)malloc(sizeof(char)*strlen(command));
			strcpy(cmd,command);

			cmd_classifier(command,cmd_arr,&idx);

			if(idx == 4){
				if(!strcmp(&cmd_arr[1][strlen(cmd_arr[1])-1],",") && !strcmp(&cmd_arr[2][strlen(cmd_arr[2])-1],",")){
					//erase comma
					cmd_arr[1][strlen(cmd_arr[1])-1] = '\0';
					cmd_arr[2][strlen(cmd_arr[2])-1] = '\0';
					//get value
					start_addr = strtol(cmd_arr[1],&ptr1,16);
					end_addr = strtol(cmd_arr[2],&ptr1,16);
					value = strtol(cmd_arr[3],&ptr2,16);
					//주소 범위, value 범위 체크
					if(value>=0x00 && value<=0xff && start_addr>=0x00000 && start_addr<=0xfffff && end_addr>=0x00000 && end_addr<=0xfffff){
						fill_dump(start_addr, end_addr, value,&dump);
					}
					else{
						printf("ERROR:wrong value or wrong address!!\n");
						strcpy(cmd,"");
					}
				}
				else{
					printf("ERROR:wrong command!! | f[ill] start, end, value\n");
					strcpy(cmd,"");
				}


			}
			else{
				printf("ERROR:wrong command!! | f[ill] start, end, value\n");
				strcpy(cmd,"");
			}

			for_history(cmd,tmp);
			
			for(int i=0;i<idx;i++)	
				cmd_arr[i] = NULL;
		}
		//reset
		else if(!strcmp(command,"reset")){
			fill_dump(0,0xfffff,0,&dump);
			for_history(command,tmp);
		}
		//opcode mnemonic
		else if(!strncmp(command,"opcode ",7)){
			
			int idx=0;
			char *cmd = (char*)malloc(sizeof(char)*strlen(command));
			strcpy(cmd,command);
			char *cmd_arr[2] = {NULL,};
			cmd_classifier(command,cmd_arr,&idx);

			if(idx == 2){
				//if opcode exists, find_opcode() return is 1
				//else 0
				if(find_opcode(cmd_arr[1])==0){
					printf("ERROR:there is no such mnemonic\n");
					strcpy(cmd,"");
				}
			}
			else{
				printf("ERROR:wrong command!! | opcode mnemonic \n");
				strcpy(cmd,"");
			}

			for_history(cmd,tmp);
			
			for(int i=0;i<idx;i++)	
				cmd_arr[i] = NULL;
		}
		//opcodelist
		else if(!strcmp(command,"opcodelist")){
			print_hash_table();
			for_history(command,tmp);
		}
		//q[uit]
		else if(!strcmp(command,"quit") || !strcmp(command,"q")){
			break;
		}
		//else is wrong command
		else
			printf("ERROR:wrong command!! check again!!\n");

		//for_history_linkedList
		NODE *new = malloc(sizeof(NODE));
		tmp->next = new;
		strcmp(new->cmd_name,"");
		new->next = NULL;
		tmp = new;
	}

	return 0;
}
