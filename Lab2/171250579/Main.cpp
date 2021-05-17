/*
@author: Edwin Xu
@Date:2019/11/13
@Note: just ASCII
*/
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <iostream>
#include <vector>

#define IMG_PATH "a.img"

using namespace std;

//myPrint in NASM
extern "C" {void myPrint(char *c,int len,int color);}
//print func using myPrint()
char chars[513]; 
void print(string s,int color){
	const char * c = s.c_str();
	strcpy(chars,s.c_str());
	myPrint(chars,s.length(),color);
}



//Part1: struct and util functions 
struct File
{
	string filename;
	int size;
	int firstCluster;
	File(string n, int s, int cluster) {
		filename = n;
		size = s;
		firstCluster = cluster;
	}
	File(){
		size = 0;
		filename = "";
		firstCluster = 0;
	}
};
struct Directory
{
	string dirName ;
	//子目录
	vector <Directory>subDirs ;
	//当前文件夹的文件
	vector <File>subFiles ;
	int dirNum=0;
	int fileNum=0;

	void addDir(Directory d) {
		this->subDirs.push_back(d);
		dirNum++;
	}
	void addFile(string name,int size,int cluster) {
		this->subFiles.push_back(File(name,size,cluster));
		fileNum++;
	}

	Directory() {
		dirName = "";
	}
	Directory(string n) {
		this->dirName = n;
	}

	//判断有没有son这个儿子
	int hasSon(string son) {
		for (int i = 0; i < dirNum; i++) {
			if (son.compare(subDirs[i].dirName) == 0) {
				return 1;
			}
		}
		return 0;
	}
	//判断有没有这个file, 并返回cluster
	int hasFile(string file) {
		for (int i = 0; i < fileNum; i++) {
			if (file.compare(subFiles[i].filename) == 0) {
				return subFiles[i].firstCluster; //存在文件，返回cluster
			}
		}
		return -1; //不存在文件，返回-1
	}

};


vector<string> path; //当前已经遍历的目录

string getPath() {
	string res = "/";
	for (int i = 0; i < path.size(); i++) {
		res += (path[i] + "/");
	}
	return res ;
}

void ls_display(Directory d) {
	if (d.dirName.length()>0) path.push_back(d.dirName); //if not root dir
	print(getPath()+":\n",0);
	if(d.dirName.length()>0) print(".  ..  ",1);
	for (int i = 0; i < d.dirNum; i++) {
		print(d.subDirs[i].dirName+"  ",1);
	}
	for (int i = 0; i < d.fileNum; i++) {
		print(d.subFiles[i].filename+"  ",0);
	}
	print("\n",0);
	for (int i = 0; i < d.dirNum; i++) {
		ls_display(d.subDirs[i]);
	}
	if (d.dirName.length() > 0)
	{
		path.pop_back();
	}
}

void ls_l_display(Directory d) {
	if (d.dirName.length() > 0) path.push_back(d.dirName); //if not root dir
	print(getPath() +" " + to_string(d.dirNum) + " " + to_string(d.fileNum)+":\n",0);
	if (d.dirName.length() > 0) print( ".\n..\n",1);
	for (int i = 0; i < d.dirNum; i++) {
		print(d.subDirs[i].dirName,1);
		print("  " + to_string(d.subDirs[i].dirNum) + " " + to_string(d.subDirs[i].fileNum)+"\n",0);
	}
	for (int i = 0; i < d.fileNum; i++) {
		print(d.subFiles[i].filename +"  " + to_string(d.subFiles[i].size)+"\n",0);
	}
	print("\n",0);
	for (int i = 0; i < d.dirNum; i++) {
		ls_l_display(d.subDirs[i]);
	}
	if (d.dirName.length() > 0) path.pop_back();
}


/*
	get dir node by dir name (note: name is unique)
*/
Directory getDirectory(Directory beginDir,string dirName) {
	if (beginDir.dirName.compare(dirName) == 0) {
		return beginDir; //get it, the current dir is what i want
	}
	//fond in the subdir:
	for (int i = 0; i < beginDir.dirNum; i++) {
		Directory temp_dir= getDirectory(beginDir.subDirs[i], dirName);
		if (temp_dir.dirName.length() != 0)return temp_dir;
	}
	return Directory();
}




//Part2: BPB and FAT operations

typedef unsigned char u8;	//1B
typedef unsigned short u16;	//2B
typedef unsigned int u32;	//4B

int  BytsPerSec;	//Bytes of every sector
int  SecPerClus;	//sectors of a cluster
int  RsvdSecCnt;	//Boot记录占用的扇区数
int  NumFATs;	//FAT tables Num
int  RootEntCnt;	//max files Num of root directory
int  FATSz;	//num of FAT sectors 

#pragma pack (1) /*指定按1字节对齐*/

//offset is 11 bytes
struct BPB {
	u16  BPB_BytsPerSec;	//每扇区字节数
	u8   BPB_SecPerClus;	//每簇扇区数
	u16  BPB_RsvdSecCnt;	//Boot记录占用的扇区数
	u8   BPB_NumFATs;	//FAT表个数
	u16  BPB_RootEntCnt;	//根目录最大文件数
	u16  BPB_TotSec16;
	u8   BPB_Media;
	u16  BPB_FATSz16;	//FAT扇区数
	u16  BPB_SecPerTrk;
	u16  BPB_NumHeads;
	u32  BPB_HiddSec;
	u32  BPB_TotSec32;	//如果BPB_FATSz16为0，该值为FAT扇区数
};
//BPB over，length is 25 bytes

//root directory entry
struct RootEntry {
	char DIR_Name[11];
	u8   DIR_Attr;		//file attr
	char reserved[10];
	u16  DIR_WrtTime;
	u16  DIR_WrtDate;
	u16  DIR_FstClus;	//start cluster No
	u32  DIR_FileSize;
};
//根目录条目结束，32字节
#pragma pack () /*取消指定对齐，恢复缺省对齐*/

struct subEntry {
	char DIR_Name[11];
	u8  DIR_Attr;
	char reserved[10];
	u16  DIR_WrTime;
	u16  DIR_WrDate;
	u16  DIR_FstClus;
	u32  DIR_FileSize;
};

#pragma pack () /*取消指定对齐，恢复缺省对齐*/


void fillBPB(struct BPB* bpb_ptr);	//载入BPB
void printFiles(struct RootEntry* rootEntry_ptr, Directory* rootnode);	//打印文件名，这个函数在打印目录时会调用下面的printChildren
void printChildren(char * directory, int startClus,Directory * d);	//打印目录及目录下子文件名
int  getFATValue(int num);	//读取num号FAT项所在的两个字节，并从这两个连续字节中取出FAT项的值，
void printFileContent(int startClus);
int getFATValue(int num);

Directory root;
FILE* fat12;  //global variable

void init() {
	struct BPB bpb;
	struct BPB* bpb_ptr = &bpb;

	//载入BPB
	fillBPB(bpb_ptr);

	//init all global variables
	BytsPerSec = bpb_ptr->BPB_BytsPerSec;
	SecPerClus = bpb_ptr->BPB_SecPerClus;
	RsvdSecCnt = bpb_ptr->BPB_RsvdSecCnt;
	NumFATs = bpb_ptr->BPB_NumFATs;
	RootEntCnt = bpb_ptr->BPB_RootEntCnt;
	if (bpb_ptr->BPB_FATSz16 != 0) {
		FATSz = bpb_ptr->BPB_FATSz16;
	}
	else {
		FATSz = bpb_ptr->BPB_TotSec32;
	}

	struct RootEntry rootEntry;
	struct RootEntry* rootEntry_ptr = &rootEntry;

	//print in cursion
	printFiles(rootEntry_ptr,&root);
}
//erase the spaces in the biginning and end
string& trim(std::string &s)
{
	if (s.empty()) return s;
	s.erase(0, s.find_first_not_of(" "));
	s.erase(s.find_last_not_of(" ") + 1);
	return s;
}

string ls_l_resolve(string command) {
	const char * cmd1 = command.c_str();
	int len = command.length();

	//处理 -lll这种多个l的情况
	int _l_beginIndex = 0;
	for (int i = 0; i < len-1; i++) {
		if (cmd1[i] == '-'&&cmd1[i + 1] == 'l') {
			_l_beginIndex = i + 1;
			break;
		}
	}
	int l_endIndex = _l_beginIndex;
	for (int i = _l_beginIndex + 1; i < len; i++) {
		if (cmd1[i] != 'l') {
			l_endIndex = i;
			break;
		}
	}
	if (_l_beginIndex != l_endIndex) {
		command = command.substr(0, _l_beginIndex+1) + command.substr(l_endIndex,len-l_endIndex);
	}
	//cout << "comm:" << command << endl;
	const char * cmd = command.c_str();
	len = command.length();

	int beginIndex = 0, endIndex = 0;
	int hasBegun = 0; //是否已经进入目录的 beginIndex

	//form1：ls path -l
	if (cmd[len - 1] == 'l'&&cmd[len - 2] == '-') {
		for (int i = 3; i < len; i++) {
			if (cmd[i] != ' '&&!hasBegun) {
				beginIndex = i;
				hasBegun = 1;
				//break;
			}
			if (hasBegun && (cmd[i] == ' ' || cmd[i] == '-')) {
				endIndex = i;
				break;
			}
		}
	}
	else {
		//form2：ls -l path
		for (int i = 3; i < len; i++) {
			if (cmd[i] == '-'&&cmd[i + 1] == 'l') {
				beginIndex = i + 1;
				break;
			}
		}
		for (int i = beginIndex+1; i < len; i++) {
			if (cmd[i] != ' ') {
				beginIndex = i;
				break;
			}
		}
		endIndex = len;
	}
	string res = command.substr(beginIndex, endIndex - beginIndex);
	return res;
}
//to upper
string upper(string s) {
	string res;
	const char* c = s.c_str();
	for (int i = 0; i < s.length(); i++) {
		if (c[i] > 96&&c[i]<127)res += (c[i] - 32);
		else res += c[i];
	}
	return res;
}
//tranform path like '/a/b/c', return a vector
vector<string> getPathVector(string path) {
	vector<string> pathVec;
	const char* pathChars = path.c_str();
	int len = path.length();
	int beginIndex = 0;
	if (pathChars[0] == '/')beginIndex++;
	char temp[20];//temp char arr
	for (int i = beginIndex; i < len; i++) {
		if (pathChars[i] == '/'||i==len-1) {
			pathVec.push_back(path.substr(beginIndex,(i==len-1)?len:i-beginIndex));
			beginIndex = i + 1;
		}
	}
	return pathVec;
}

//得到路径中的目录
Directory locateDirectory(string path) {
	path = upper(path);
	vector<string> pathVector = getPathVector(path);
	vector <Directory> dirArr;

	for (int i = 0; i < pathVector.size(); i++) {
		dirArr.push_back(getDirectory(root, pathVector[i]));
	}
	
	Directory res = dirArr[dirArr.size() - 1];
	int isOK = 1;
	if(dirArr.size()==1){
		if(dirArr[0].dirName.length()==0)isOK=0;
	}
	else{
		for (int i = 0; i < pathVector.size() - 1; i++) {
			if (!dirArr[i].hasSon(dirArr[i + 1].dirName)) {
				isOK = 0;
				break;
			}
		}
	}
	if (!isOK) res.dirNum = -1; //如果路径不正确，将返回目录节点的dirName置-1
	return res;
}

void Main() {
	string inp;
	while (1)
	{
		print(">",0);
		getline(cin, inp);
		inp = trim(inp);

		const char* c_inp = inp.c_str();
		
		int has_ = 0;
		for(int i =0;i<inp.length();i++){
			if(c_inp[i]=='-'){
				has_ = 1;
				break;
			}
		}
		
		if (inp.compare("exit") == 0) {
			print( "Bye!\n",0);
			break;
		}
		if (inp.length() >= 2 && c_inp[0] == 'l'&&c_inp[1] == 's') {
			if (!inp.compare("ls")) {
				ls_display(root);
			}
			else if(!inp.substr(0,2).compare("ls")&&!has_){
				string path = inp.substr(2,inp.length()-2);
				path = trim(path);
				Directory dir = locateDirectory(path);
				ls_display(dir);
			}
			else if(!inp.compare("ls -l")) {
				ls_l_display(root);
			}
			else if(!inp.substr(0,5).compare("ls -L")) {
				print("your command is invalid!\n",0);
			}
			else {
				string path = ls_l_resolve(inp);  
				//cout << "path: " << path << endl;
				
				if (path.length() == 0) {
					print("The path is invalid！ Try again!\n",0);
				}
				else {
					Directory dir = locateDirectory(path);
					if (dir.dirNum==-1) {
						print("The path is invalid！ Try again!\n",0);
					}
					else {
						ls_l_display(dir);
					}
				}
			}
		}
		else if (!inp.substr(0,3).compare("cat")) {
			int i;
			for (i = 3; i < inp.length(); i++) {
				if (c_inp[i] != ' ')break;  //取出cat后面的space
			}
			//取出文件和目录:
			int endIndex = inp.length() - 1;
			for (int i = endIndex; i >= 3; i--) {
				if (c_inp[i] == '/') {
					endIndex = i;
					break;
				}
			}

			string path = inp.substr(i, endIndex - i);
			string fileName = inp.substr(endIndex + 1, inp.length() - endIndex - 1);

			if (endIndex == inp.length() - 1) {
				path = "";
				fileName = inp.substr(i, inp.length()-i);
			}
			//cout << "path: " << path << " file: " << fileName << endl;
			Directory dir;
			if (path.length() == 0) { //path is empty, so it's root entry
				dir = root;
			}
			else {
				dir = locateDirectory(path);
			}
			if (dir.dirNum == -1) {
				print("The path is invalid！\n",0);
			}
			else {
				int cluster = dir.hasFile(upper(fileName));
				if (cluster==-1) {
					print("The file is not exist!\n",0);
				}
				else {
					if (cluster >= 2) 
						printFileContent(cluster);
				}

			}
		}
		else if(inp.length()>0){
			string s = "Wrong Command! Try Again!\n";
			print(s,0);
		}
	}
}


void fillBPB( struct BPB* bpb_ptr) {
	int check;
	//BPB从偏移11个字节处开始
	check = fseek(fat12, 11, SEEK_SET);
	if (check == -1)
		print("fseek in fillBPB failed!\n",0);

	//BPB长度为25字节
	check = fread(bpb_ptr, 1, 25, fat12);
	if (check != 25)
		print("fread in fillBPB failed!\n",0);
}


void printFiles(struct RootEntry* rootEntry_ptr, Directory* root) {
	int base = (RsvdSecCnt + NumFATs * FATSz) * BytsPerSec;	//根目录首字节的偏移数
	int check;
	char realName[12];	//暂存将空格替换成点后的文件名

	//依次处理根目录中的各个条目
	int i;
	for (i = 0; i < RootEntCnt; i++) {

		check = fseek(fat12, base, SEEK_SET);
		if (check == -1)
			print("fseek in printFiles failed!\n",0);

		check = fread(rootEntry_ptr, 1, 32, fat12);
		if (check != 32)
			print("fread in printFiles failed!\n",0);
		base += 32;
		if (rootEntry_ptr->DIR_Name[0] == '\0') continue;	//empty entry

		//filliter ilvalid
		int j;
		int boolean = 0;
		for (j = 0; j < 11; j++) {
			if (!(((rootEntry_ptr->DIR_Name[j] >= 48) && (rootEntry_ptr->DIR_Name[j] <= 57)) ||
				((rootEntry_ptr->DIR_Name[j] >= 65) && (rootEntry_ptr->DIR_Name[j] <= 90)) ||
				((rootEntry_ptr->DIR_Name[j] >= 97) && (rootEntry_ptr->DIR_Name[j] <= 122)) ||
				(rootEntry_ptr->DIR_Name[j] == ' '))) {
				boolean = 1;	//invalid char
				break;
			}
		}
		if (boolean == 1) continue;	//not object

		int k;
		if ((rootEntry_ptr->DIR_Attr & 0x10) == 0) {
			//it's a file 
			int tempLong = -1;
			for (k = 0; k < 11; k++) {
				if (rootEntry_ptr->DIR_Name[k] != ' ') {
					tempLong++;
					realName[tempLong] = rootEntry_ptr->DIR_Name[k];
				}
				else {
					tempLong++;
					realName[tempLong] = '.';
					while (rootEntry_ptr->DIR_Name[k] == ' ') k++;
					k--;
				}
			}
			tempLong++;
			realName[tempLong] = '\0';	//filename is realName

			root->addFile(realName,rootEntry_ptr->DIR_FileSize,rootEntry_ptr->DIR_FstClus);
			//output fileName:
			//printf("FileName: %s\n", realName);
		}
		else {
			//it's a dir
			int tempLong = -1;
			for (k = 0; k < 11; k++) {
				if (rootEntry_ptr->DIR_Name[k] != ' ') {
					tempLong++;
					realName[tempLong] = rootEntry_ptr->DIR_Name[k];
				}
				else {
					tempLong++;
					realName[tempLong] = '\0';
					break;
				}
			}	//dir name is realName

			Directory d = Directory(realName);
			root->addDir(d);

			//priint recurisively
			printChildren(realName, rootEntry_ptr->DIR_FstClus, &root->subDirs[root->dirNum-1]);
		}
	}
}


void printChildren(char * directory, int startClus,Directory* d) {
	//offset of No.2 cluster(the first cluster)
	int dataBase = BytsPerSec * (RsvdSecCnt + FATSz * NumFATs + (RootEntCnt * 32 + BytsPerSec - 1) / BytsPerSec);
	char fullName[240];	//file /dir fullname
	int strLength = strlen(directory);
	strcpy(fullName, directory);
	fullName[strLength] = '/';
	strLength++;
	fullName[strLength] = '\0';
	char* fileName = &fullName[strLength];

	int currentClus = startClus;
	int value = 0;
	int ifOnlyDirectory = 0;

	while (value < 0xFF8) { //value <FF8H
		value = getFATValue(currentClus);
		if (value < 0) {
			break;
		}
		if (value == 0xFF7) {
			print("Bad cluster，failed!\n",0);
			break;
		}

		char* str = (char*)malloc(SecPerClus*BytsPerSec);	//暂存从簇中读出的数据
		char* content = str;

		int startByte = dataBase + (currentClus - 2)*SecPerClus*BytsPerSec;
		int check;
		check = fseek(fat12, startByte, SEEK_SET);
		if (check == -1)
			print("fseek in printChildren failed!\n",0);

		check = fread(content, 1, SecPerClus*BytsPerSec, fat12);
		if (check != SecPerClus * BytsPerSec)
			print("fread in printChildren failed!\n",0);

		//solve date of content,the regular entry structure is as same as root dir entry.
		int count = SecPerClus * BytsPerSec;	//bytes of a cluster
		int loop = 0;
		while (loop < count) {
			int i;
			char tempName[120];	//暂存替换空格为点后的文件名
			if (content[loop] == '\0') {
				loop += 32;
				continue;
			}	//empty entry

				//filiter the invaild things
			int j;
			int boolean = 0;
			for (j = loop; j < loop + 11; j++) {
				if (!(((content[j] >= 48) && (content[j] <= 57)) ||
					((content[j] >= 65) && (content[j] <= 90)) ||
					((content[j] >= 97) && (content[j] <= 122)) ||
					(content[j] == ' '))) {
					boolean = 1;	//invaild char
					break;
				}
			}
			if (boolean == 1) {
				loop += 32;
				continue;
			}	//not object file

			if ((content[loop + 11] & 0x10) == 0) {
				//File :
				int k;
				int tempLong = -1;
				for (k = 0; k < 11; k++) {
					if (content[loop + k] != ' ') {
						tempLong++;
						tempName[tempLong] = content[loop + k];
					}
					else {
						tempLong++;
						tempName[tempLong] = '.';
						while (content[loop + k] == ' ') k++;
						k--;
					}
				}
				tempLong++;
				tempName[tempLong] = '\0';	

				int a = content[loop+28],
					b = content[loop + 29],
					c = content[loop + 30],
					d1 = content[loop + 31];
				a = a >= 0 ? a : 256 + a;//处理负数
				b = b >= 0 ? b: 256 + b;
				c = c >= 0 ? c : 256 + c;
				d1 = d1 >= 0 ? d1 : 256 + d1;

				int size = a + 256*b + 256*256*c + 256*256*256*d1;

				int x = content[loop + 26];
				int y = content[loop + 27];
				//cout << "cluster of : " << fullName<<"/"<<tempName<<" :" <<y * 256 + x << endl;
				d->addFile(tempName,size, y * 256 + x);
				strcpy(fileName, tempName);
				//printf("%s\n", fullName);
				ifOnlyDirectory = 1;
			}
			else {
				//dir
				int k;
				int tempLong = -1;

				for (k = 0; k < 11; k++) {
					if (content[loop + k] != ' ') {
						tempLong++;
						tempName[tempLong] = content[loop + k];
					}
					else {
						while (content[loop + k] == ' ') k++;
						k--;
					}
				}
				tempLong++;
				tempName[tempLong] = '\0';
				strcpy(fileName, tempName);
				ifOnlyDirectory == 1;

				Directory dir = Directory(tempName);
				d->addDir(dir);

				int a = content[loop + 26];
				int b = content[loop + 27];
				printChildren(fullName, b*256+a, &d->subDirs[d->dirNum-1]);
			}
			loop += 32;
		}
		free(str);
		currentClus = value;
	};
	//if (ifOnlyDirectory == 0)
	//	printf("%s\n", fullName);	//空目录的情况下，输出目录
}

//输出文件内容：
void printFileContent(int startClus) {
	int dataBase = BytsPerSec * (RsvdSecCnt + FATSz * NumFATs + (RootEntCnt * 32 + BytsPerSec - 1) / BytsPerSec);
	int check;
	int curclus = startClus;
	int value = 0;
	while (value < 0xFF8) {//4088
		value = getFATValue(curclus);
		if (value < 0) {
			break;
		}
		if (value == 0xFF7) {
			print( "bad cluster\n",0);
		}
		char temp[513];
		int startByte = dataBase + 512 * (curclus - 2);
		check = fseek(fat12, startByte, SEEK_SET);
		if (check == -1)print( "Failed!\n",0);
		check = fread(temp, 1, 512, fat12);
		temp[512] = '\0';
		print(temp,0);
		print("\n",0);
		curclus = value;
	}
}


int getFATValue(int num) {
	int fatBase = RsvdSecCnt * BytsPerSec;
	int fatPos = fatBase + num * 3 / 2;
	int type = 0;
	if (num % 2 == 0) {
		type = 0; //even
	}
	else {
		type = 1; //odd
	}
	u16 bytes;
	u16* bytes_ptr = &bytes;
	int check;
	check = fseek(fat12, fatPos, SEEK_SET);
	if (check == -1) {
		print("fseek in getFATValue failed!\n",0);
		return -1;
	}
	check = fread(bytes_ptr, 1, 2, fat12);
	if (check != 2) {
		print("fread in getFATValue failed!\n",0);
		return -1;
	}
	//u16为short，结合存储的小端顺序和FAT项结构可以得到
	//type为0的话，取byte2的低4位和byte1构成的值，type为1的话，取byte2和byte1的高4位构成的值
	if (type == 0) {
		bytes = bytes << 4;
		bytes = bytes >> 4;
	}
	else {
		bytes = bytes >> 4;
	}
	return bytes;
}



// Main, entrance of this program
int main() {
	fat12 = fopen(IMG_PATH, "rb");	//open the fat12 image
	init(); //init, build the file tree
	Main(); //command solve and execute
	fclose(fat12); //clost the file stream in the last。
	return 0;
}
