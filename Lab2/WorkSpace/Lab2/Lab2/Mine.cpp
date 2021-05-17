//
///*
//	自己的分析：为什么不一个扇区一个扇区的读。
//
//*/
//
//
//
///*文件名不支持中文及符号。*/
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
//typedef unsigned char u8;	//1字节
//typedef unsigned short u16;	//2字节
//typedef unsigned int u32;	//4字节
//
//
//int  BytsPerSec;	//每扇区字节数
//int  SecPerClus;	//每簇扇区数
//int  RsvdSecCnt;	//Boot记录占用的扇区数
//int  NumFATs;	//FAT表个数
//int  RootEntCnt;	//根目录最大文件数
//int  FATSz;	//FAT扇区数
//
//
//#pragma pack (1) /*指定按1字节对齐*/
//
////偏移11个字节
//struct BPB {
//	u16  BPB_BytsPerSec;	//每扇区字节数
//	u8   BPB_SecPerClus;	//每簇扇区数
//	u16  BPB_RsvdSecCnt;	//Boot记录占用的扇区数
//	u8   BPB_NumFATs;	//FAT表个数
//	u16  BPB_RootEntCnt;	//根目录最大文件数
//	u16  BPB_TotSec16;
//	u8   BPB_Media;
//	u16  BPB_FATSz16;	//FAT扇区数
//	u16  BPB_SecPerTrk;
//	u16  BPB_NumHeads;
//	u32  BPB_HiddSec;
//	u32  BPB_TotSec32;	//如果BPB_FATSz16为0，该值为FAT扇区数
//};
////BPB至此结束，长度25字节
//
////根目录条目
//struct RootEntry {
//	char DIR_Name[11];  //8+3
//	u8   DIR_Attr;		//文件属性
//	char reserved[10];
//	u16  DIR_WrtTime;
//	u16  DIR_WrtDate;
//	u16  DIR_FstClus;	//开始簇号
//	u32  DIR_FileSize;
//};
////根目录条目结束，32字节
//
//#pragma pack () /*取消指定对齐，恢复缺省对齐*/
//
//Directories root ;
//
//
//
//
//void fillBPB(FILE * fat12, struct BPB* bpb_ptr);	//载入BPB
////打印文件名，这个函数在打印目录时会调用下面的printChildren
//void printFiles(FILE * fat12, struct RootEntry* rootEntry_ptr);
////打印目录及目录下子文件名
//void printChildren(FILE * fat12, int startClus, Directories *curDir);
////读取num号FAT项所在的两个字节，并从这两个连续字节中取出FAT项的值，
//int  getFATValue(FILE * fat12, int num);
//
//char * getName(char* name);
//string getStrName(char* name);
//char res[12];
//
//void init() {
//	FILE* fat12;
//	fat12 = fopen("e.img", "rb");	//打开FAT12的映像文件
//
//	struct BPB bpb;
//	struct BPB* bpb_ptr = &bpb;
//
//	//载入BPB
//	fillBPB(fat12, bpb_ptr);
//
//	//初始化各个全局变量
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
//	//打印文件名
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
//	//BPB从偏移11个字节处开始
//	check = fseek(fat12, 11, SEEK_SET);
//	if (check == -1)
//		printf("fseek in fillBPB failed!");
//
//	//BPB长度为25字节
//	check = fread(bpb_ptr, 1, 25, fat12);
//	if (check != 25)
//		printf("fread in fillBPB failed!");
//}
//
//;
//void printFiles(FILE * fat12, struct RootEntry* rootEntry_ptr) {
//	int base = (RsvdSecCnt + NumFATs * FATSz) * BytsPerSec;	//根目录首字节的偏移数
//	int check;
//	//char realName[12];	//暂存将空格替换成点后的文件名
//
//	//依次处理根目录中的各个条目
//	int i;
//	for (i = 0; i < RootEntCnt; i++) {//edwin：有必要遍历全部表项（224）吗？
//
//		check = fseek(fat12, base, SEEK_SET); //edwin: 定位流
//		if (check == -1)
//			printf("fseek in printFiles failed!");
//
//		check = fread(rootEntry_ptr, 1, 32, fat12); //edwin:读取一个根目录
//		if (check != 32)
//			printf("fread in printFiles failed!");
//
//		base += 32; //edwin:一个根目录32位
//
//		if (rootEntry_ptr->DIR_Name[0] == '\0') { //8+3,8中第一位空，全空
//			continue;	//空条目不输出
//		}
//
//		
//		if ((rootEntry_ptr->DIR_Attr & 0x10) == 0) { //1Byte的属性，第5位表示是不是文件
//			//文件（因为第5位是0，非目录，即文件）
//			char * realName;
//			realName = getName(rootEntry_ptr->DIR_Name);
//			//输出文件
//			printf("File: %s\n", realName);
//			string a = realName;
//			root.addFile(a);
//		}
//		else {
//			//目录
//			char * realName;
//			realName= getName(rootEntry_ptr->DIR_Name);
//
//			printf("dir: %s\n", realName);
//
//			string a = realName;
//			root.addDir(Directories(a));
//
//			//root.show();
//			//输出目录及子文件
//			printChildren(fat12, rootEntry_ptr->DIR_FstClus, &root.subDirs[root.dirNum - 1]);
//		}
//	}
//}
//
//
//
///*
//@ char * directory: 父目录？？
//void printChildren(FILE * fat12, char * directory, int startClus, Directories *curDir)
//*/
//void printChildren(FILE * fat12, int startClus, Directories *curDir) {
//	struct RootEntry rootEntry;
//	struct RootEntry* rootEntry_ptr = &rootEntry;
//
//	//printf("当前簇: %d\n", startClus);
//
//	//数据区的第一个簇（即2号簇）的偏移字节
//	int dataBase = BytsPerSec * (RsvdSecCnt + FATSz * NumFATs + (RootEntCnt * 32 + BytsPerSec - 1) / BytsPerSec);
//	dataBase += (512 * (startClus -2));
//
//	dataBase += 64;//??
//
//	//printf("偏移:%d\n", dataBase);
//
//	fseek(fat12, dataBase, SEEK_SET); //edwin: 定位流
//	fread(rootEntry_ptr, 1, 32, fat12); //edwin:读取一个根目录
//	
//
//	if ((rootEntry_ptr->DIR_Attr & 0x10) == 0) {
//		//文件：
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
//		//目录：递归
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
//		//首先要找到所有的子目录，按顺序一个一个递归进去
//		int offset = BytsPerSec * (RsvdSecCnt + FATSz * NumFATs + (RootEntCnt * 32 + BytsPerSec - 1) / BytsPerSec);
//		offset += (512 * (rootEntry_ptr->DIR_FstClus - 2));
//
//		for (int i = 0; i < 16; i++) {
//			struct RootEntry entry;
//			struct RootEntry* entry_ptr = &entry;
//			fseek(fat12, offset, SEEK_SET); //edwin: 定位流
//			fread(entry_ptr, 1, 32, fat12); //edwin:读取一个根目录
//			int fstClus = getFATValue(fat12, entry_ptr->DIR_FstClus);
//			if (fstClus == 0xFF7) {
//				printf("坏簇，读取失败!\n");
//				break;
//			}
//
//			printChildren(fat12, fstClus, &curDir->subDirs[curDir->dirNum - 1]);
//
//		}
//		
//
//		printChildren(fat12, rootEntry_ptr->DIR_FstClus,&curDir->subDirs[curDir->dirNum-1]);
//		//再找出所有文件，打印出来
//		
//
//		//printChildren( fat12, directory, startClus);
//	}
//
//}
//
//
//
//int  getFATValue(FILE * fat12, int num) {//簇号
//	//FAT1的偏移字节
//	int fatBase = RsvdSecCnt * BytsPerSec;
//	//FAT项的偏移字节
//	int fatPos = fatBase + num * 3 / 2;
//	//奇偶FAT项处理方式不同，分类进行处理，从0号FAT项开始
//	int type = 0;
//	if (num % 2 == 0) {
//		type = 0;
//	}
//	else {
//		type = 1;
//	}
//
//	//先读出FAT项所在的两个字节
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
//	//u16为short，结合存储的小尾顺序和FAT项结构可以得到
//	//type为0的话，取byte2的低4位和byte1构成的值，type为1的话，取byte2和byte1的高4位构成的值
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