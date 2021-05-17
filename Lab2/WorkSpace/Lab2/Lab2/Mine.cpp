//
///*
//	�Լ��ķ�����Ϊʲô��һ������һ�������Ķ���
//
//*/
//
//
//
///*�ļ�����֧�����ļ����š�*/
//#include <cstdio>
//#include <cstdlib>
//#include <cstring>
//#include <string>
//#include "Directory.h"
//#include <iostream>
//
//using namespace std;
//
//#pragma warning(disable:4996)
//
//typedef unsigned char u8;	//1�ֽ�
//typedef unsigned short u16;	//2�ֽ�
//typedef unsigned int u32;	//4�ֽ�
//
//
//int  BytsPerSec;	//ÿ�����ֽ���
//int  SecPerClus;	//ÿ��������
//int  RsvdSecCnt;	//Boot��¼ռ�õ�������
//int  NumFATs;	//FAT�����
//int  RootEntCnt;	//��Ŀ¼����ļ���
//int  FATSz;	//FAT������
//
//
//#pragma pack (1) /*ָ����1�ֽڶ���*/
//
////ƫ��11���ֽ�
//struct BPB {
//	u16  BPB_BytsPerSec;	//ÿ�����ֽ���
//	u8   BPB_SecPerClus;	//ÿ��������
//	u16  BPB_RsvdSecCnt;	//Boot��¼ռ�õ�������
//	u8   BPB_NumFATs;	//FAT�����
//	u16  BPB_RootEntCnt;	//��Ŀ¼����ļ���
//	u16  BPB_TotSec16;
//	u8   BPB_Media;
//	u16  BPB_FATSz16;	//FAT������
//	u16  BPB_SecPerTrk;
//	u16  BPB_NumHeads;
//	u32  BPB_HiddSec;
//	u32  BPB_TotSec32;	//���BPB_FATSz16Ϊ0����ֵΪFAT������
//};
////BPB���˽���������25�ֽ�
//
////��Ŀ¼��Ŀ
//struct RootEntry {
//	char DIR_Name[11];  //8+3
//	u8   DIR_Attr;		//�ļ�����
//	char reserved[10];
//	u16  DIR_WrtTime;
//	u16  DIR_WrtDate;
//	u16  DIR_FstClus;	//��ʼ�غ�
//	u32  DIR_FileSize;
//};
////��Ŀ¼��Ŀ������32�ֽ�
//
//#pragma pack () /*ȡ��ָ�����룬�ָ�ȱʡ����*/
//
//Directories root ;
//
//
//
//
//void fillBPB(FILE * fat12, struct BPB* bpb_ptr);	//����BPB
////��ӡ�ļ�������������ڴ�ӡĿ¼ʱ����������printChildren
//void printFiles(FILE * fat12, struct RootEntry* rootEntry_ptr);
////��ӡĿ¼��Ŀ¼�����ļ���
//void printChildren(FILE * fat12, int startClus, Directories *curDir);
////��ȡnum��FAT�����ڵ������ֽڣ����������������ֽ���ȡ��FAT���ֵ��
//int  getFATValue(FILE * fat12, int num);
//
//char * getName(char* name);
//string getStrName(char* name);
//char res[12];
//
//void init() {
//	FILE* fat12;
//	fat12 = fopen("e.img", "rb");	//��FAT12��ӳ���ļ�
//
//	struct BPB bpb;
//	struct BPB* bpb_ptr = &bpb;
//
//	//����BPB
//	fillBPB(fat12, bpb_ptr);
//
//	//��ʼ������ȫ�ֱ���
//	BytsPerSec = bpb_ptr->BPB_BytsPerSec;
//	SecPerClus = bpb_ptr->BPB_SecPerClus;
//	RsvdSecCnt = bpb_ptr->BPB_RsvdSecCnt;
//	NumFATs = bpb_ptr->BPB_NumFATs;
//	RootEntCnt = bpb_ptr->BPB_RootEntCnt;
//	if (bpb_ptr->BPB_FATSz16 != 0) {
//		FATSz = bpb_ptr->BPB_FATSz16;
//	}
//	else {
//		FATSz = bpb_ptr->BPB_TotSec32;
//	}
//
//	struct RootEntry rootEntry;
//	struct RootEntry* rootEntry_ptr = &rootEntry;
//
//	//��ӡ�ļ���
//	printFiles(fat12, rootEntry_ptr);
//
//	fclose(fat12);
//
//
//}
//
//void Main() {
//	string inp;
//	while (1)
//	{
//		cout << ">";
//		getline(cin, inp);
//		//cout << "inp: " << inp << endl;
//		if (inp.compare("exit") == 0) {
//			cout << "Bye!" << endl;
//			break;
//		}
//		else if (inp.compare("ls")==0) {
//			ls_display(root);
//		}
//		else if (!inp.compare("ls -l")) {
//
//		}
//		else if (!inp.compare("cat path")) {
//
//		}
//		else if(inp.length()>0){
//			cout << "Wrong Command! Try Again!\n";
//		}
//
//	}
//}
//
//
//
//int main() {
//
//	
//	//char c[] = {'a', 'b','c',' ',' ',' ',' ',' ',' ' ,' ' ,' ','\0'};
//	//cout << getName(c) << endl;
//	//system("pause");
//	//return 0;
//
//	init();
//	display(root);
//
//	printf( "---------------Please Input Commands------------------\n");
//	
//	Main();
//
//
//	system("pause");
//
//}
//
//
//
//
//void fillBPB(FILE* fat12, struct BPB* bpb_ptr) {
//	int check;
//
//	//BPB��ƫ��11���ֽڴ���ʼ
//	check = fseek(fat12, 11, SEEK_SET);
//	if (check == -1)
//		printf("fseek in fillBPB failed!");
//
//	//BPB����Ϊ25�ֽ�
//	check = fread(bpb_ptr, 1, 25, fat12);
//	if (check != 25)
//		printf("fread in fillBPB failed!");
//}
//
//;
//void printFiles(FILE * fat12, struct RootEntry* rootEntry_ptr) {
//	int base = (RsvdSecCnt + NumFATs * FATSz) * BytsPerSec;	//��Ŀ¼���ֽڵ�ƫ����
//	int check;
//	//char realName[12];	//�ݴ潫�ո��滻�ɵ����ļ���
//
//	//���δ����Ŀ¼�еĸ�����Ŀ
//	int i;
//	for (i = 0; i < RootEntCnt; i++) {//edwin���б�Ҫ����ȫ�����224����
//
//		check = fseek(fat12, base, SEEK_SET); //edwin: ��λ��
//		if (check == -1)
//			printf("fseek in printFiles failed!");
//
//		check = fread(rootEntry_ptr, 1, 32, fat12); //edwin:��ȡһ����Ŀ¼
//		if (check != 32)
//			printf("fread in printFiles failed!");
//
//		base += 32; //edwin:һ����Ŀ¼32λ
//
//		if (rootEntry_ptr->DIR_Name[0] == '\0') { //8+3,8�е�һλ�գ�ȫ��
//			continue;	//����Ŀ�����
//		}
//
//		
//		if ((rootEntry_ptr->DIR_Attr & 0x10) == 0) { //1Byte�����ԣ���5λ��ʾ�ǲ����ļ�
//			//�ļ�����Ϊ��5λ��0����Ŀ¼�����ļ���
//			char * realName;
//			realName = getName(rootEntry_ptr->DIR_Name);
//			//����ļ�
//			printf("File: %s\n", realName);
//			string a = realName;
//			root.addFile(a);
//		}
//		else {
//			//Ŀ¼
//			char * realName;
//			realName= getName(rootEntry_ptr->DIR_Name);
//
//			printf("dir: %s\n", realName);
//
//			string a = realName;
//			root.addDir(Directories(a));
//
//			//root.show();
//			//���Ŀ¼�����ļ�
//			printChildren(fat12, rootEntry_ptr->DIR_FstClus, &root.subDirs[root.dirNum - 1]);
//		}
//	}
//}
//
//
//
///*
//@ char * directory: ��Ŀ¼����
//void printChildren(FILE * fat12, char * directory, int startClus, Directories *curDir)
//*/
//void printChildren(FILE * fat12, int startClus, Directories *curDir) {
//	struct RootEntry rootEntry;
//	struct RootEntry* rootEntry_ptr = &rootEntry;
//
//	//printf("��ǰ��: %d\n", startClus);
//
//	//�������ĵ�һ���أ���2�Ŵأ���ƫ���ֽ�
//	int dataBase = BytsPerSec * (RsvdSecCnt + FATSz * NumFATs + (RootEntCnt * 32 + BytsPerSec - 1) / BytsPerSec);
//	dataBase += (512 * (startClus -2));
//
//	dataBase += 64;//??
//
//	//printf("ƫ��:%d\n", dataBase);
//
//	fseek(fat12, dataBase, SEEK_SET); //edwin: ��λ��
//	fread(rootEntry_ptr, 1, 32, fat12); //edwin:��ȡһ����Ŀ¼
//	
//
//	if ((rootEntry_ptr->DIR_Attr & 0x10) == 0) {
//		//�ļ���
//		/*cout << "rootEntry_ptr->DIR_Name:  " << rootEntry_ptr->DIR_Name[0] << endl;*/
//
//		char res1[22];
//		int temp = 0;
//		for (int i = 0; i < 8; i++) {
//			if (rootEntry_ptr->DIR_Name[i] == 32) {
//				break;
//			}
//			res1[temp] = rootEntry_ptr->DIR_Name[i];
//			temp++;
//		}
//		res1[temp] = '.';
//		temp++;
//		int t = temp;
//		for (int i = 8; i < 11; i++) {
//			if (rootEntry_ptr->DIR_Name[i] == 32) {
//				break;
//			}
//			res1[temp] = rootEntry_ptr->DIR_Name[i];
//			temp++;
//		}
//		if (temp == t)temp--;
//		cout << "temp: " << temp << endl;
//		res1[temp] = '\0';
//
//		string filename = res1;
//		cout << "----File:  " << res1 << endl;
//		string a = filename;
//		//curDir->addFile(a);
//		curDir->addFile(filename);
//	}
//	else {
//		//Ŀ¼���ݹ�
//		//char *dirname = getName(rootEntry_ptr->DIR_Name);
//		//string dirname = getStrName(rootEntry_ptr->DIR_Name);
//	/*	cout <<"rootEntry_ptr->DIR_Name[0] : "<< rootEntry_ptr->DIR_Name[0] << endl;
//		if (rootEntry_ptr->DIR_Name[0] == 0xE5) {
//			cout << "Empty " << endl;
//		}*/
//		int temp = 0;
//		char res2[22];
//		for (int i = 0; i < 8; i++) {
//			if (rootEntry_ptr->DIR_Name[i] == 32) {
//				break;
//			}
//			res2[temp] = rootEntry_ptr->DIR_Name[i];
//			temp++;
//		}
//		res2[temp] = '.';
//		temp++;
//		int t = temp;
//		for (int i = 8; i < 11; i++) {
//			if (rootEntry_ptr->DIR_Name[i] == 32) {
//				break;
//			}
//			res2[temp] = rootEntry_ptr->DIR_Name[i];
//			temp++;
//		}
//		if (temp == t)temp--;
//		res2[temp] = '\0';
//
//		string dirname = res2;
//		cout << "----Dir:  " << res2 << endl;
//
//		string a = dirname;
//		Directories d = Directories(a);
//		printf("curDir : %s \n", curDir->dirName.c_str());
//		curDir->addDir(d);
//		
//		//����Ҫ�ҵ����е���Ŀ¼����˳��һ��һ���ݹ��ȥ
//		int offset = BytsPerSec * (RsvdSecCnt + FATSz * NumFATs + (RootEntCnt * 32 + BytsPerSec - 1) / BytsPerSec);
//		offset += (512 * (rootEntry_ptr->DIR_FstClus - 2));
//
//		for (int i = 0; i < 16; i++) {
//			struct RootEntry entry;
//			struct RootEntry* entry_ptr = &entry;
//			fseek(fat12, offset, SEEK_SET); //edwin: ��λ��
//			fread(entry_ptr, 1, 32, fat12); //edwin:��ȡһ����Ŀ¼
//			int fstClus = getFATValue(fat12, entry_ptr->DIR_FstClus);
//			if (fstClus == 0xFF7) {
//				printf("���أ���ȡʧ��!\n");
//				break;
//			}
//
//			printChildren(fat12, fstClus, &curDir->subDirs[curDir->dirNum - 1]);
//
//		}
//		
//
//		printChildren(fat12, rootEntry_ptr->DIR_FstClus,&curDir->subDirs[curDir->dirNum-1]);
//		//���ҳ������ļ�����ӡ����
//		
//
//		//printChildren( fat12, directory, startClus);
//	}
//
//}
//
//
//
//int  getFATValue(FILE * fat12, int num) {//�غ�
//	//FAT1��ƫ���ֽ�
//	int fatBase = RsvdSecCnt * BytsPerSec;
//	//FAT���ƫ���ֽ�
//	int fatPos = fatBase + num * 3 / 2;
//	//��żFAT���ʽ��ͬ��������д�����0��FAT�ʼ
//	int type = 0;
//	if (num % 2 == 0) {
//		type = 0;
//	}
//	else {
//		type = 1;
//	}
//
//	//�ȶ���FAT�����ڵ������ֽ�
//	u16 bytes;
//	u16* bytes_ptr = &bytes;
//	int check;
//	check = fseek(fat12, fatPos, SEEK_SET);
//	if (check == -1)
//		printf("fseek in getFATValue failed!");
//
//	check = fread(bytes_ptr, 1, 2, fat12);
//	if (check != 2)
//		printf("fread in getFATValue failed!");
//
//	//u16Ϊshort����ϴ洢��Сβ˳���FAT��ṹ���Եõ�
//	//typeΪ0�Ļ���ȡbyte2�ĵ�4λ��byte1���ɵ�ֵ��typeΪ1�Ļ���ȡbyte2��byte1�ĸ�4λ���ɵ�ֵ
//	if (type == 0) {
//		return bytes << 4;
//	}
//	else {
//		return bytes >> 4;
//	}
//
//}
//
//
//char * getName(char* name) {
//	int temp = 0;
//	for (int i = 0; i < 8; i++) {
//		if (name[i] == 32) {
//			break;
//		}
//		res[temp] = name[i];
//		temp++;
//	}
//	res[temp] = '.';
//	temp++;
//	int t = temp;
//	for (int i = 8; i < 11; i++) {
//		if (name[i] == 32) {
//			break;
//		}
//		res[temp] = name[i];
//		temp++;
//	}
//	if (temp == t)temp--;
//	res[temp] = '\0';
//	return res;
//}
//
//string getStrName(char* name) {
//	int temp = 0;
//	for (int i = 0; i < 8; i++) {
//		if (name[i] == 32) {
//			break;
//		}
//		res[temp] = name[i];
//		temp++;
//	}
//	res[temp] = '.';
//	temp++;
//	int t = temp;
//	for (int i = 8; i < 11; i++) {
//		if (name[i] == 32) {
//			break;
//		}
//		res[temp] = name[i];
//		temp++;
//	}
//	if (temp == t)temp--;
//	res[temp] = '\0';
//	string str = res;
//	return str;
//}