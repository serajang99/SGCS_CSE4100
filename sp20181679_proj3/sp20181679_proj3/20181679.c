#include "20181679.h"

//for h[help]
//print the kinds of the commands exist.
void print_all_commands(){
	
	printf("h[elp]\nd[ir]\nq[uit]\nhi[story]\ndu[mp] [start, end]\ne[dit] address, value\nf[ill] start, end, value\nreset\nopcode mnemonic\nopcodelist\nassemble filename\ntype filename\nsymbol\nprogaddr [address]\nloader [object filename1] [object filename2] [...]\nbp [address]\nrun\n");

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

	if(!strcmp(command,"help") || !strcmp(command,"h") || !strcmp(command,"dir") || !strcmp(command,"d") || !strcmp(command,"history") || !strcmp(command,"hi") || !strncmp(command,"dump",4) || !strncmp(command,"du",2) || !strncmp(command,"edit",4) || !strncmp(command,"e",1) || !strncmp(command,"fill",4) || !strncmp(command,"f",1) || !strncmp(command,"opcode ",7) || !strcmp(command, "opcodelist") || !strcmp(command,"reset") || !strncmp(command,"type ",5) || !strncmp(command,"type ",5) || !strncmp(command,"assemble ",9) || !strcmp(command,"symbol") || !strncmp(command,"progaddr ",9) || !strncmp(command,"loader ",7) || !strncmp(command,"bp ",3) || !strcmp(command,"run")) {
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
	for(i=0;i<4;i++){
		if(cmd_arr[i]!=NULL){
			(*idx)++;
		}
		else
			continue;
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

//for assemble pass1 algorithm
//read hash table to find opcode
//해당 mnemonic을 optab에서 찾는다.
//opcode를 반환, 없을 경우 -1 반환
int find_opcode_2(char* opcode_name){
	
	OPNODE* tmp = malloc(sizeof(OPNODE));
	for(int i=0;i<20;i++){
		tmp = ophash[i];
		while(tmp!=NULL){
			if(!strcmp(tmp->opcode_name,opcode_name)){
				return tmp->opcode;
			}
			tmp = tmp->next;
		}
	}
	return -1;
}

//use in run()
//return mnemonic
char* find_opcode_3(unsigned int opcode){
	
	OPNODE* tmp = malloc(sizeof(OPNODE));
	for(int i=0;i<20;i++){
		tmp = ophash[i];
		while(tmp!=NULL){
			if(tmp->opcode==opcode){
				return tmp->opcode_name;
			}
			tmp = tmp->next;
		}
	}
	return " ";
}

int find_file(char *filename){
	
	DIR *dp = NULL;//현재 디렉토리 가리킴
	struct dirent *d = NULL;
	struct stat buf;

	if((dp = opendir(".")) == NULL){
		printf("현재 디렉토리를 열 수 없습니다...\n");
		return -1;
	}
	//디렉토리 읽음
	while((d = readdir(dp)) != NULL){
		lstat(d->d_name,&buf);
		if(!strcmp(filename,d->d_name) && !S_ISDIR(buf.st_mode)){
			return 1;
		}
	}
	closedir(dp);//디렉토리 스트림 닫음
	return 0;
}

//read and print file
void read_file(char *filename){
	
	FILE *fp = fopen(filename,"r");
	char ch;
	while((ch = fgetc(fp))!=EOF){
		putchar(ch);
	}
	fclose(fp);
}

int assemble_file(FILE* fp,char *filename){
	
	char *f_name = (char*)malloc(sizeof(char)*strlen(filename));
	strcpy(f_name,filename);
	FILE *inter = fopen("inter.txt","w");
	int line_num = 5, locctr=0, start_addr=0, prog_len=0;
	int idx=0, format=3;
	char str[100]={0,};
	char label[20]={0,}, mnemonic[100]={0,};
	char op[100]={0,}, op2[100]={0,}, tmp[100]={0,};

	/*make SYMTAB*/
	for(int i=0;i<20;i++){
		SYM *tmp;
		SYM *cur = (SYM*)malloc(sizeof(SYM));
		cur = shead[i];
		while(cur!=NULL){
			tmp = cur->next;
			free(cur);
			cur = tmp;
		}
		shead[i] = NULL;
	}

	/*PASS1*/
	do{
		/*read line by line*/
		/*get label, mnemonic, op(, op2)*/
		fgets(str,80,fp);
		if(sscanf(str,"%[^,],%[^\n]",tmp,op2)==1)
			strcpy(op2," ");
		idx = sscanf(str,"%s %s %s", label, mnemonic, op);
		if(idx==2){
			strcpy(op,mnemonic);
			strcpy(mnemonic,label);
			strcpy(label," ");
		}
		else if(idx==1){
			if(!strcmp(label,"RSUB")){
				strcpy(mnemonic,label);
				strcpy(label," ");
				strcpy(op," ");
			}
			else{
				strcpy(mnemonic," ");
				strcpy(op," ");
			}
		}
		else if(idx==3){
			if(!strcmp(label,".")){
				strcpy(mnemonic," ");
				strcpy(op," ");
			}
			if(strcmp(op2," ")){
				strcpy(op,mnemonic);
				sscanf(op,"%[^,], %s",op,op2);
				strcpy(mnemonic,label);
				strcpy(label," ");
			}
		}
		/*make symbol table*/
		//if there is symbol in label field then
		if(strcmp(label," ") && strcmp(label,".") && strcmp(mnemonic,"START")){
			//search symtab for label
			for(int i=0;i<20;i++){
				if(shead[i]==NULL){
					SYM *temp = (SYM*)malloc(sizeof(SYM));
					strcpy(temp->symbol,label);
					temp->locctr = locctr;
					temp->flag = 0;
					temp->next = NULL;
					shead[i] = temp;
					break;
				}
				//error flag(duplicate symbol)
				else{
					if(!strcmp(shead[i]->symbol,label)){
						printf("ERROR: symbol already exists. | %d\n",line_num);
						break;
					}
					else 
						continue;
				}
			}//for
		}//symtab

		fprintf(inter,"%04x|",locctr);
		/*if opcode == start, set start addr(locctr)*/
		char *ptr; format = 3;
		if(!strcmp(mnemonic,"START")){
			start_addr = strtol(op,&ptr,16);
			locctr = start_addr;
		}
		else{
			if(find_opcode_2(mnemonic)!=-1){
				if(!strcmp(mnemonic,"CLEAR") || !strcmp(mnemonic,"COMPR") || !strcmp(mnemonic,"TIXR")){
					locctr += 0x0002;
					format = 2;
				}
				else
					locctr += 0x0003;
			}
			else{
				if(!strcmp(mnemonic,"WORD")){
					locctr += 0x0003;
					format = 5;
				}
				else if(!strcmp(mnemonic,"RESW")){
					locctr += 0x0003*atoi(op);
					format = 5;
				}
				else if(!strcmp(mnemonic,"RESB")){
					locctr += 0x0001*atoi(op);
					format = 5;
				}
				else if(!strcmp(mnemonic,"BYTE")){
					format = 5;
					if(!strncmp(op,"C",1))
						locctr += 0x0001*(strlen(op)-3);
					else if(!strncmp(op,"X",1))
						locctr += 0x0001*((strlen(op)-3)/2);
				}
				else if(!strncmp(mnemonic,"+",1)){
					locctr += 0x0004;
					format = 4;
				}
				else if(!strcmp(label,".") || !strcmp(mnemonic,"BASE") || !strcmp(mnemonic,"END")){
				}
				else
					printf("ERROR: invalid operation code | %d\n",line_num);
			}
		}//else_full
		fprintf(inter,"%d|%d|%04X|%s|%s|%s|%s\n",line_num,format,locctr,label,mnemonic,op,op2);
		line_num += 5;
	}while(strcmp(mnemonic,"END"));

	prog_len = locctr - start_addr;
	fclose(inter);	
	PASS2(prog_len, fp, f_name);
	return 0;
}

//for PASS2
//find label in symtab
int find_sym(char *op){
	for(int i=0;i<20;i++){
		if(shead[i]!=NULL){
			if(!strcmp(shead[i]->symbol,op)){
				return shead[i]->locctr;
			}
			else
				continue;
		}
		else
			break;
	}
	return -1;
}

//PASS2 algorithm
void PASS2(int prog_len,FILE* fp,char *f_name){
	
	char *f_name2 = (char*)malloc(sizeof(char)*strlen(f_name));
	strcpy(f_name2,f_name);
	FILE *lst = fopen(strcat(f_name,".lst"),"w");
	FILE *obj = fopen(strcat(f_name2,".obj"),"w");
	FILE *temp = fopen("temp.txt","w");
	FILE *temp2 = fopen("temp_modi.txt","w");
	free(f_name2);
	FILE *inter = fopen("inter.txt","r");
	int line_num=0, locctr=0, format=3, locctr_real=0;
	int opcode=0, xbpe=2;
	int disp=0;
	char label[20]={0,}, mnemonic[100]={0,};
	char op[100]={0,}, op2[100]={0,}, str[100]={0,};
	char base_op[100]; int base_loc=0;
	char buf[30], obj_code[256]={0,};
	int r1=0, r2=0, start_addr=0;

	fgets(str,100,inter);
	sscanf(str,"%04X|%d|%d|%04X|%[^|]|%[^|]|%[^|]|%[^\n]\n",&locctr_real,&line_num,&format,&locctr,label,mnemonic,op,op2);

	if(!strcmp(mnemonic,"START")){
		start_addr = locctr_real;
		fprintf(temp,"%d",start_addr);
		fprintf(obj,"H%-6s%06X%06X\n",label,locctr_real,prog_len);
		fprintf(lst,"%-6d %04X\t%-6s %-6s %-8s %-2s\n",line_num,locctr_real,label,mnemonic,op,op2);
	}

	do{
		fgets(str,100,inter);
		sscanf(str,"%04X|%d|%d|%04X|%[^|]|%[^|]|%[^|]|%[^\n]\n",&locctr_real,&line_num,&format,&locctr,label,mnemonic,op,op2);

		//initialize xbpe
		xbpe = 2;
		if(format == 4){
			strcpy(mnemonic,strtok(mnemonic,"+"));
			xbpe |= 1;
		}
		//base_location counter
		if(!strcmp(label,base_op)){
			base_loc = locctr-0x0003*atoi(op);
		}
		//optab에 존재
		if((opcode = find_opcode_2(mnemonic))!=-1){
			/*ni==11
			 simple addressing*/
			opcode |= 2;
			opcode |= 1;
			//operand가 존재
			if(strcmp(op," ")){
				//symtab에 없을 경우
				if((disp = find_sym(op)) == -1){
					if(!strncmp(op,"#",1)){
						/*ni==01
						  immediate addressing*/
						opcode ^= 2;
						char* ptr;
						char tmp[100];
						strcpy(tmp,op);
						strcpy(op,strtok(op,"#"));
						if(op[0]>='0' && op[0]<='9'){
							disp = strtol(op,&ptr,10);
							xbpe ^= 2;
						}
						else{
							disp = find_sym(op) - locctr;
							if(disp<0){
								disp &= 0x00000FFF;
							}
						}
						strcpy(op,tmp);
					}
					else if(!strncmp(op,"@",1)){
						/*ni==10
						 indirect addressing*/
						char tmp[100];
						strcpy(tmp,op);
						opcode ^= 1;
						strcpy(op,strtok(op,"@"));
						disp = find_sym(op) - locctr;
						strcpy(op,tmp);
					}
					else if(format == 2){
						/*ni==00*/
						opcode ^= 1;
						opcode ^= 2;
						if(!strcmp(op,"X"))
							r1 = 1;
						else if(!strcmp(op,"A"))
							r1 = 0;
						else if(!strcmp(op,"S"))
							r1 = 4;
						else if(!strcmp(op,"T"))
							r1 = 5;
						
						if(!strcmp(op2," X"))
							r2 = 1;
						else if(!strcmp(op2," A"))
							r2 = 0;
						else if(!strcmp(op2," S"))
							r2 = 4;
					}
					else{
						printf("ERROR: there is no such variable | %d\n",line_num);
						disp = 0x0000;
					}
				}
				//symtab에 존재
				else{
					if(format == 4){
						xbpe ^= 2;
					}
					else{
						if(locctr - disp >= 0x1000){
							xbpe |= 4;
							xbpe ^= 2;
							if(!strcmp(op,"X") || !strcmp(op2," X")){
								xbpe |= 8;
							}
							disp -= base_loc;
						}
						else{
							disp -= locctr;
							if(disp<0){
								disp &= 0x00000FFF;
							}
						}
					}
				}	
			}
			//RSUB " "
			else{
				disp = 0x0000;
				xbpe = 0x0000;
			}

			/*출력 관련*/
			if(format == 4){
				char tmp[100] = "+";
				strcat(tmp,mnemonic);
				strcpy(mnemonic,tmp);
			}
			if(strcmp(op2," ")){
				strcat(op,",");
			}
			
			fprintf(lst,"%-6d %04X\t%-6s %-6s %-8s %-2s ",line_num,locctr_real,label,mnemonic,op,op2);
			if(format == 2){
				fprintf(lst,"  %02X%01X%01X\n",opcode,r1,r2);
				sprintf(obj_code+strlen(obj_code),"%02X%01X%01X",opcode,r1,r2);
				r1=0; r2=0;
			}
			else if(format == 3){
				fprintf(lst,"  %02X%01X%03X\n",opcode,xbpe,disp);
				sprintf(obj_code+strlen(obj_code),"%02X%01X%03X",opcode,xbpe,disp);
			}
			else if(format == 4){
				fprintf(lst,"  %02X%01X%05X\n",opcode,xbpe,disp);
				sprintf(obj_code+strlen(obj_code),"%02X%01X%05X",opcode,xbpe,disp);
				if(strncmp(op,"#",1) && strncmp(op,"@",1))
					fprintf(temp2,"%04X\n",locctr_real);
			}
		}
		//optab에 없을 경우
		else{
			//그 값을 목적코드로 변환
			if(!strcmp(mnemonic,"BYTE")){
				if(!strncmp(op,"C",1)){
					char* str;
					char tmp[100];
					strcpy(tmp,op);
					str = strtok(op,"'");
					for(int i=0;i<1;i++)
						str = strtok(NULL,"'");
					sprintf(buf,"%02X%02X%02X",(int)str[0],(int)str[1],(int)str[2]);
					strcpy(op,tmp);
				}
				else if(!strncmp(op,"X",1)){
					char* str;
					char tmp[100];
					strcpy(tmp,op);
					str = strtok(op,"'");
					for(int i=0;i<1;i++)
						str = strtok(NULL,"'");
					sprintf(buf,"%s",str);
					strcpy(op,tmp);
				}
				sprintf(obj_code+strlen(obj_code),"%-s",buf);
			}
			//BASE
			else if(!strcmp(mnemonic,"BASE")){
				strcpy(base_op,op);
				sprintf(buf," ");
			}

			/*출력관련*/
			if(!strcmp(mnemonic,"BASE") || !strcmp(label,".") || !strcmp(mnemonic,"END"))
				fprintf(lst,"%-6d %4s\t%-6s %-6s %-8s %-2s ",line_num," ",label,mnemonic,op,op2);
			else
				fprintf(lst,"%-6d %04X\t%-6s %-6s %-8s %-2s ",line_num,locctr_real,label,mnemonic,op,op2);
			fprintf(lst,"  %-6s\n",buf);
			strcpy(buf," ");
		}
		
		
		if(((int)strlen(obj_code)/2)>28){
			if(strlen(obj_code)!=0){
				fprintf(temp," %s\n%d",obj_code,locctr);
			}
			memset(obj_code,'\0',sizeof(obj_code));
		}
		else{
		 	if(!strcmp(mnemonic,"RESW") || !strcmp(mnemonic,"END")){
				fprintf(temp," %s\n%d",obj_code,locctr);
				memset(obj_code,'\0',sizeof(obj_code));
			}	
		}	
	}while(strcmp(mnemonic,"END"));	

	fclose(temp);
	fclose(temp2);
	fclose(inter);
	fclose(lst);
	
	temp = fopen("temp.txt","r");
	while(fgets(str,100,temp)!=NULL){
		sscanf(str,"%d %s\n",&locctr,obj_code);
		if(strcmp(obj_code,""))
			fprintf(obj,"T%06X%02X%s\n",locctr,(int)strlen(obj_code)/2,obj_code);
		memset(obj_code,'\0',sizeof(obj_code));
	}
	fclose(temp);

	temp2 = fopen("temp_modi.txt","r");
	while(fscanf(temp2,"%x\n",&locctr)!=EOF){
		fprintf(obj,"M%06X%02X\n",locctr+1,0x05);
	}
	fclose(temp2);
	fprintf(obj,"E%06X\n",start_addr);
	fclose(obj);

	remove("temp.txt");
	remove("temp_modi.txt");
	remove("inter.txt");

	fclose(fp);
}

//for print symbol
//alphabet align
void SWAP(int j){
	char temp[20]={0,}; int tmp;
	strcpy(temp,shead[j+1]->symbol);
	strcpy(shead[j+1]->symbol,shead[j]->symbol);
	strcpy(shead[j]->symbol,temp);
	tmp = shead[j+1]->locctr;
	shead[j+1]->locctr = shead[j]->locctr;
	shead[j]->locctr = tmp;
}

//print symbol table
void print_symbol(){
	for (int i=0;i<20;i++){
		for(int j=0;j<19;j++){
			if(shead[j]!=NULL && shead[j+1]!=NULL){
				if((shead[j]->symbol)[0] > (shead[j+1]->symbol)[0])
					SWAP(j);
				else if((shead[j]->symbol)[0] == (shead[j+1]->symbol)[0])
					if((shead[j]->symbol)[1] > (shead[j+1]->symbol)[1])
						SWAP(j);
			}
			else 
				continue;
		}
	}
	for(int i=0;i<20;i++){
		if(shead[i]!=NULL){
			printf("\t%-8s%04X\n",shead[i]->symbol,shead[i]->locctr);
		}
		else
			continue;
	}
}

//find symbol or cs in estab
int find_sym_estab(char *sym){
	for(int i=0;i<20;i++){
		if(eshead[i]!=NULL){
			if(!strcmp(eshead[i]->symbol,sym)||!strcmp(eshead[i]->cs,sym)){
				return 1;
			}
			else
				continue;
		}
		else
			break;
	}
	return 0;
}

//find symbol or cs in estab for pass2
int find_sym_estab2(char *sym){
	for(int i=0;i<20;i++){
		if(eshead[i]!=NULL){
			if(!strcmp(eshead[i]->symbol,sym)||!strcmp(eshead[i]->cs,sym)){
				return i;
			}
			else
				continue;
		}
		else
			break;
	}
	return -1;
}


//linking loader PASS1
int ll_pass1(char *file1, char *file2, char *file3){
	int j=0, cslth=0;
	unsigned int CSADDR = PROG_ADDR;
	char *filename[3] = {file1,file2,file3};
	/*make ESTAB*/
	for(int k=0;k<20;k++){
		eshead[k] = (ESTAB*)malloc(sizeof(ESTAB));
		eshead[k]->next = NULL;
		eshead[k]->flag = 0;
		strcpy(eshead[k]->cs,"");
		strcpy(eshead[k]->symbol,"");
	}

	for(int i=0;(i<3 && strcmp(filename[i],""));i++){
		FILE *fp = fopen(filename[i],"r");
		char str[100];
		//PASS1
		do{
			fgets(str,sizeof(str),fp);
			if(!strncmp(str,"H",1)){
				char cs[20];
				strcpy(str,strtok(str,"H"));
				strcpy(cs,strtok(str," "));
				strcpy(str,strtok(NULL,"\n"));
				if(find_sym_estab(cs)==1)
					printf("ERROR::duplicate external symbol\n");
				else{
					while(eshead[j]->flag!=0)
						j++;
					strcpy(eshead[j]->cs,cs);
					eshead[j]->addr = CSADDR;
					cslth = strtol(str,NULL,16);
					eshead[j]->cslth = cslth;
					eshead[j]->flag = 1;
				}
			}
			do{
				fgets(str,sizeof(str),fp);
				if(!strncmp(str,"D",1)){
					char symbol[20];
					unsigned int addr=0x00;
					int start=1;
					while(sscanf(str+start,"%6s%6X",symbol,&addr) != EOF) {
						start += 12;
						if(find_sym_estab(symbol)==1)
							printf("ERROR::duplicate external symbol\n");
						else{
							while(eshead[j]->flag!=0)
								j++;
							strcpy(eshead[j]->symbol,symbol);
							eshead[j]->addr = CSADDR+addr;
							eshead[j]->flag = 1;
						}
					}//end {while}
				}//end {if}
			}while(strncmp(str,"E",1));// end {while{} 'E'}
		}while(!feof(fp));
		CSADDR += cslth;
		fclose(fp);
	}
	printf("control symbol address length\n");
	printf("section name\n");
	printf("--------------------------------\n");
	for(int i=0;i<20;i++){
		if(eshead[i]->flag!=0){
			if(strcmp(eshead[i]->cs,""))
				printf("%-6s  %6s   %04X   %04X \n",eshead[i]->cs,eshead[i]->symbol,eshead[i]->addr,eshead[i]->cslth);
			else
				printf("%-6s  %6s   %04X        \n",eshead[i]->cs,eshead[i]->symbol,eshead[i]->addr);
		}
		else
			continue;
	}
	printf("--------------------------------\n");
	printf("           total length %04X \n",CSADDR-PROG_ADDR);

	return 0;
}

//linking loader PASS2
int ll_pass2(char *file1, char *file2, char *file3, unsigned char **dump){

	unsigned int CSADDR = PROG_ADDR;
	int cslth=0;
	char *filename[3] = {file1,file2,file3};

	for(int i=0;(i<3 && strcmp(filename[i],""));i++){
		FILE *fp = fopen(filename[i],"r");
		char str[100];
		ref = (char**)malloc(sizeof(char*)*10);
		for(int j=0;j<10;j++)
			ref[j] = (char*)calloc(sizeof(char),5);
		
		//PASS2
		do{
			fgets(str,sizeof(str),fp);
			if(!strncmp(str,"H",1)){
				char cs[20];
				strcpy(str,strtok(str,"H"));
				strcpy(cs,strtok(str," "));
				strcpy(str,strtok(NULL,"\n"));
				cslth = strtol(str,NULL,16);
				strcpy(ref[1],cs);
			}
			do{
				fgets(str,sizeof(str),fp);
				if(!strncmp(str,"R",1)){
					unsigned int num=0x00;
					int start=1;
					char symbol[20];
					strcpy(symbol,"");
					while(sscanf(str+start,"%2X%6s",&num,symbol) != EOF) {
						start += 8;
						if(1<num && num<10)
							strcpy(ref[num],symbol);
					}//end {while}
				}//end {R}
				else if(!strncmp(str,"T",1)){
					unsigned int addr=0x00;
					int tlth=0,k=0;
					char obj[100];
					sscanf(str+1,"%6X%2X%[^\n]",&addr,&tlth,obj);
					for(int j=0;j<tlth*2;j+=2){
						char tmp[2];
						sprintf(tmp,"%c%c",obj[j],obj[j+1]);
						(*dump)[CSADDR+addr+k] = strtol(tmp,NULL,16);
						k++;
					}
				}//end {T}	
				else if(!strncmp(str,"M",1)){
					unsigned int addr = 0x00;
					int mlth,num;
					char op;
					sscanf(str+1,"%6X%2X%c%02X",&addr,&mlth,&op,&num);

					if(find_sym_estab(ref[num])==1){
						int idx = find_sym_estab2(ref[num]);
						int tmp = eshead[idx]->addr;
						char temp[6]; int value;
						if(mlth%2==1)
							mlth++;
						
						sprintf(temp,"%02X%02X%02X",(*dump)[CSADDR+addr+(mlth/2)-3],(*dump)[CSADDR+addr+(mlth/2)-2],(*dump)[CSADDR+addr+(mlth/2)-1]);
						value = strtol(temp,NULL,16);
						
						if(op=='+')
							value += tmp;
						else
							value -= tmp;

						(*dump)[CSADDR+addr+(mlth/2)-3] = (value&0xFF0000)>>16;
						(*dump)[CSADDR+addr+(mlth/2)-2] = (value&0xFF00)>>8;
						(*dump)[CSADDR+addr+(mlth/2)-1] = value&0xFF;

					}
					else{
						int k=0;
						while(eshead[k]->flag!=0)
							k++;
						strcpy(eshead[k]->symbol,ref[num]);
						eshead[k]->addr = CSADDR+addr+(mlth/2);
						eshead[k]->flag = 1;
					}
				}//end {M}
				
			}while(strncmp(str,"E",1));// end {while{} 'E'}
			if(!strncmp(str,"E",1)){
				unsigned int addr = 0x00;
				sscanf(str+1,"%6X",&addr);
			}
		}while(!feof(fp));
		CSADDR += cslth;
		for(int j=0;j<10;j++)
			free(ref[j]);
		free(ref);

		fclose(fp);
	}	

	return 0;
}

int is_bp(unsigned int value){
	for(int i=0;i<0x1077;i++){
		if(bp[i]==value){
			return 1;
		}
		else
			continue;
	}
	return 0;
}

int is_bp2(unsigned int value){
	for(int i=0;i<0x1077;i++){
		if(bp[i]==value){
			bp[i] = 0x1078;
			return 1;
		}
		else
			continue;
	}
	return 0;
}


int run(unsigned char **dump, unsigned int *PC, unsigned int *L, unsigned int *A, unsigned int *X, unsigned int *B, unsigned int *S, unsigned int *T){

	int opcode, ni, xbpe;
	char op[100];
	unsigned int cur_addr = *PC;
	int CC = 0,comp=0;
	do{
		cur_addr = *PC;
		opcode = (*dump)[cur_addr] & 252;
		strcpy(op,find_opcode_3(opcode));
	 	ni = (*dump)[cur_addr] & 3;
		xbpe = ((*dump)[cur_addr+1] & 0xF0)>>4;

		if(!strcmp(op,"CLEAR") || !strcmp(op,"TIXR") || !strcmp(op,"COMPR"))
			*PC += 2;
		else if(!strcmp(op,"JSUB"))
			*PC += 4;
		else
			*PC += 3;

		if(!strncmp(op,"ST",2)){
			if(op[2]=='L')
				RETADR = *L;
			else if(op[2]=='C')
				BUFFER = *X;
			else if(op[2]=='X')
				LENGTH = *X;
			else if(op[2]=='A'){

				if(cur_addr == 0x001D)
					BUFFER = *A;
				else if(cur_addr == 0x23)
					LENGTH = *A;
			}
				
		}
		else if(!strncmp(op,"LD",2)){
			if(op[2]=='B'){
				*B = (*dump)[cur_addr+2]+(((*dump)[cur_addr+1]&15)<<8)+(*PC);
			}
			else if(op[2]=='A'){
				if(ni==1)
					*A = (*dump)[cur_addr+2]+(((*dump)[cur_addr+1]&15)<<8);
				else
					*A = (*dump)[cur_addr+2]+(((*dump)[cur_addr+1]&15)<<8)+(*PC);
			}
			else if(op[2]=='T'){
				if(cur_addr == 0x103C){
					(*PC)++;
					*T = (*dump)[cur_addr+3]+(((*dump)[cur_addr+2])<<8)+(((*dump)[cur_addr+1]&15)<<16);
				}
				else if((xbpe&4) == 4){
					*T = LENGTH;
				}
				else
					*T = (*dump)[cur_addr+2]+(((*dump)[cur_addr+1]&15)<<8)+(*PC);
			}
			else if(op[2]=='C'){
				*A = ((*dump)[BUFFER++]);
			}
		}
		else if(!strcmp(op,"JSUB")){
			*L = (*PC);
			*PC = (*dump)[cur_addr+3]+(((*dump)[cur_addr+2])<<8)+(((*dump)[cur_addr+1]&15)<<16);
			cur_addr = *PC;
		}
		else if(!strcmp(op,"CLEAR")){
			int tmp = (*dump)[cur_addr+1];
			if(tmp == 0x10)
				*X = 0;
			else if(tmp == 0x00)
				*A = 0;
			else if(tmp == 0x40)
				*S = 0;
		}
		else if(!strcmp(op,"TIXR")){
			if(*X != *T)
				(*X)++;
			
		}
		else if(!strcmp(op,"RSUB")){
			*PC = *L;
		}
		else if(!strncmp(op,"J",1)){
			if(ni==2){
				*PC = RETADR;
				break;
			}
			else{
				if(op[1]=='L'){
					if(CC == 1 && (*T > *X))	
						*PC = (*dump)[cur_addr+2]+(((*dump)[cur_addr+1]&15)<<8)+(*PC)-0x1000;
				}
				else if(op[1]=='E'){
					if(CC == 0 && comp==1)
						*PC = (*dump)[cur_addr+2]+(((*dump)[cur_addr+1]&15)<<8)+(*PC);
				}
				else if(op[1]=='\0'){
					*L = *PC;
					*PC = (*dump)[cur_addr+2]+(((*dump)[cur_addr+1]&15)<<8)+(*PC);
				}
			}
		}
		else if(!strcmp(op,"RD")){
			LENGTH = 0;
			CC = 0;//'='
		}
		else if(!strcmp(op,"TD")){
			CC = 1;//'<'
		}
		else if(!strncmp(op,"COMP",4)){
			if(op[4]=='R'&&(*A==*S)){
				comp = 1; CC = 0;
			}
			else if(*A == (*dump)[cur_addr+2]+(((*dump)[cur_addr+1]&15)<<8)+(*PC))
				comp=1;
		}
	}while((is_bp(*PC)!=1)&&(*PC != 0x1077)&&(cur_addr<0x1077));

	printf("A : %06X  X: %06X\n",*A,*X);
	printf("L : %06X PC: %06X\n",*L,*PC);
	printf("B : %06X  S: %06X\n",*B,*S);
	printf("T : %06X\n",*T);
	if(is_bp2(*PC)==1)
		printf("\tStop at checkpoint[%X]\n",(*PC));
	else
		printf("\tEnd Program\n");
	
	return 0;
}

//main function
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

	//bp initialize
	for(int i=0;i<0x1077;i++)
		bp[i]= 0x100000;
	//run
	unsigned int PC = 0;
	unsigned int L = 0x1077;
	unsigned int A=0, X=0, B=0, S=0, T=0;

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
					if(value>=0x00 && value<=0xff && start_addr>=0x00000 && start_addr<=0xfffff && end_addr>=0x00000 && end_addr<=0xfffff && start_addr <= end_addr){
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
		//type filename
		else if(!strncmp(command,"type ",4)){
			int idx=0;
			char *cmd = (char*)malloc(sizeof(char)*strlen(command));
			strcpy(cmd,command);
			char *cmd_arr[2] = {NULL,};
			cmd_classifier(command,cmd_arr,&idx);
			
			if(idx == 2){
				//if file do not exists error message
				if(find_file(cmd_arr[1])==0){
					printf("ERROR:there is no such file\n");
					strcpy(cmd,"");
				}
				else{
					read_file(cmd_arr[1]);
				}
			}
			else{
				printf("ERROR:wrong command!! | type filename\n");
				strcpy(cmd,"");
			}

			for_history(cmd,tmp);
			for(int i=0;i<idx;i++)	
				cmd_arr[i] = NULL;
		}
		//assemble filename
		else if(!strncmp(command,"assemble ",9)){
			int idx=0;
			char *cmd = (char*)malloc(sizeof(char)*strlen(command));
			strcpy(cmd,command);
			char *cmd_arr[2] = {NULL,};
			cmd_classifier(command,cmd_arr,&idx);
			char *cmd2 = (char*)malloc(sizeof(char)*strlen(cmd_arr[1]));
			strcpy(cmd2,cmd_arr[1]);
			char *p = strtok(cmd_arr[1],".");
			char *filename = (char*)malloc(sizeof(char)*strlen(p));
			char *filetype = (char*)malloc(sizeof(char)*3);
			strcpy(filename,p);
			p = strtok(NULL," ");
			strcpy(filetype,p);

			if(idx == 2){
				if(strcmp(filetype,"asm")){
					printf("ERROR:filetype error\n");
					strcpy(cmd,"");
				}
				else{
					FILE *fp = fopen(cmd2,"r");
					if(fp == NULL){
						printf("ERROR:there is no such file!\n");
						continue;
					}
					else{
						assemble_file(fp,filename);
						printf(ANSI_COLOR_GREEN "Successfully" ANSI_COLOR_RESET " assemble %s.\n",cmd2);
					}
				}
			}
			else{
				printf("ERROR:wrong command!! | assemble filename\n");
				strcpy(cmd,"");
			}

			for_history(cmd,tmp);
			for(int i=0;i<idx;i++)	
				cmd_arr[i] = NULL;
		}
		//symbol
		else if(!strcmp(command,"symbol")){
			print_symbol();
			for_history(command,tmp);
		}
		//progaddr [address]
		else if(!strncmp(command,"progaddr ",9)){
			int idx=0;
			char *cmd = (char*)malloc(sizeof(char)*strlen(command));
			strcpy(cmd,command);

			idx = sscanf(command,"%*s %x\n",&PROG_ADDR);

			if(idx != 1){
				printf("ERROR:wrong command!! | progaddr [address] \n");
				strcpy(cmd,"");
			}
			else if(PROG_ADDR>0xfffff){
				printf("ERROR:wrong value!!\n");
				PROG_ADDR = 0x00000;
				strcpy(cmd,"");
			}
			for_history(cmd,tmp);
		}
		//linker loader
		else if(!strncmp(command,"loader ",7)){
			char file1[20],file2[20],file3[20];
			int idx = sscanf(command,"%*s %s %s %s",file1,file2,file3);
			if(idx > 3){
				printf("ERROR:wrong command!! | progaddr [address] \n");
				strcpy(command,"");
			}
			else{
				ll_pass1(file1,file2,file3);
				ll_pass2(file1,file2,file3,&dump);
			}
			strcpy(file1,"");strcpy(file2,"");strcpy(file3,"");
			for_history(command,tmp);
		}
		//break point
		else if(!strncmp(command,"bp",2)){
			if(!strcmp(command,"bp clear")){
				for(int i=0;i<0x1077;i++)
					bp[i]= 0x100000;
				printf("\t["ANSI_COLOR_GREEN"ok"ANSI_COLOR_RESET"] clear all breakpoints\n");
			}
			else if(command[3]=='\0'){
				printf("\tbreakpoint\n");
				printf("\t----------\n");
				for(int i=0;i<0x1077;i++){
					if(bp[i]!=0x100000)
						printf("\t%X\n",bp[i]);
				}
			}
			else{
				sscanf(command,"%*s %x",&bp[bp_idx]);
				if(bp[bp_idx]>0x1077){
					printf("ERROR::wrong value!\n");
					bp[bp_idx] = 0x100000;
					strcpy(command,"");
				}
				else
					printf("\t["ANSI_COLOR_GREEN "ok"ANSI_COLOR_RESET"] create breakpoint %x\n",bp[bp_idx++]);
			}
			for_history(command,tmp);
		}
		//run
		else if(!strcmp(command,"run")){
			run(&dump,&PC,&L,&A,&X,&B,&S,&T);
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
