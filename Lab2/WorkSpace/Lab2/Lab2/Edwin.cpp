/*�ļ�����֧�����ļ����š�*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<iostream>
#include<string>
#include<iostream>
#include "Directory.h"
using namespace std;

#pragma warning(disable:4996);

using namespace std;
typedef unsigned char u8;	//1�ֽ�
typedef unsigned short u16;	//2�ֽ�
typedef unsigned int u32;	//4�ֽ�


int  BytsPerSec;	//ÿ�����ֽ���
int  SecPerClus;	//ÿ��������
int  RsvdSecCnt;	//Boot��¼ռ�õ�������
int  NumFATs;	//FAT�����
int  RootEntCnt;	//��Ŀ¼����ļ���
int  FATSz;	//FAT������


#pragma pack (1) /*ָ����1�ֽڶ���*/

//ƫ��11���ֽ�
struct BPB {
	u16  BPB_BytsPerSec;	//ÿ�����ֽ���
	u8   BPB_SecPerClus;	//ÿ��������
	u16  BPB_RsvdSecCnt;	//Boot��¼ռ�õ�������
	u8   BPB_NumFATs;	//FAT�����
	u16  BPB_RootEntCnt;	//��Ŀ¼����ļ���
	u16  BPB_TotSec16;
	u8   BPB_Media;
	u16  BPB_FATSz16;	//FAT������
	u16  BPB_SecPerTrk;
	u16  BPB_NumHeads;
	u32  BPB_HiddSec;
	u32  BPB_TotSec32;	//���BPB_FATSz16Ϊ0����ֵΪFAT������
};
//BPB���˽���������25�ֽ�

//��Ŀ¼��Ŀ
struct RootEntry {
	char DIR_Name[11];
	u8   DIR_Attr;		//�ļ�����
	char reserved[10];
	u16  DIR_WrtTime;
	u16  DIR_WrtDate;
	u16  DIR_FstClus;	//��ʼ�غ�
	u32  DIR_FileSize;
};
//��Ŀ¼��Ŀ������32�ֽ�
#pragma pack () /*ȡ��ָ�����룬�ָ�ȱʡ����*/

struct subEntry {
	char DIR_Name[11];
	u8  DIR_Attr;
	char reserved[10];
	u16  DIR_WrTime;
	u16  DIR_WrDate;
	u16  DIR_FstClus;
	u32 DIR_FileSize;

};

#pragma pack () /*ȡ��ָ�����룬�ָ�ȱʡ����*/


void fillBPB(struct BPB* bpb_ptr);	//����BPB
void printFiles(struct RootEntry* rootEntry_ptr, Directory* rootnode);	//��ӡ�ļ�������������ڴ�ӡĿ¼ʱ����������printChildren
void printChildren(char * directory, int startClus,Directory * d);	//��ӡĿ¼��Ŀ¼�����ļ���
int  getFATValue(int num);	//��ȡnum��FAT�����ڵ������ֽڣ����������������ֽ���ȡ��FAT���ֵ��
void printFileContent(int startClus);
int getFATValue(int num);

Directory root;
FILE* fat12;

void init() {
	struct BPB bpb;
	struct BPB* bpb_ptr = &bpb;

	//����BPB
	fillBPB(bpb_ptr);

	//��ʼ������ȫ�ֱ���
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

	//��ӡ�ļ���
	printFiles(rootEntry_ptr,&root);

}
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

	//���� -lll���ֶ��l�����
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
	cout << "comm:" << command << endl;
	const char * cmd = command.c_str();
	len = command.length();

	int beginIndex = 0, endIndex = 0;
	int hasBegun = 0; //�Ƿ��Ѿ�����Ŀ¼�� beginIndex

	//��ʽ1��ls path -l
	if (cmd[len - 1] == 'l'&&cmd[len - 2] == '-') {
		//cout << "ls path -l" << endl;

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
		//��ʽ2��ls -l path
		//cout << "ls -l path" << endl;

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

	//int hasObliqueLine = 0;
	//for (int i = 0; i < res.length()-1; i++) {
	//	if (res.substr(i, 1).compare("\\\\") == 0) {
	//		hasObliqueLine = 1;
	//		break;
	//	}	
	//}
	//if (!hasObliqueLine) {
	//	cout << "has no ObliqueLine" << endl;
	//	return "";
	//}
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

//�õ�·���е�Ŀ¼
Directory locateDirectory(string path) {
	path = upper(path);
	vector<string> pathVector = getPathVector(path);
	vector <Directory> dirNameArr;

	for (int i = 0; i < pathVector.size(); i++) {
		dirNameArr.push_back(getDirectory(root, pathVector[i]));
	}
	int isOK = 1;
	for (int i = 0; i < pathVector.size() - 1; i++) {
		if (!dirNameArr[i].hasSon(dirNameArr[i + 1].dirName)) {
			isOK = 0;
			break;
		}
	}
	Directory res = dirNameArr[dirNameArr.size() - 1];
	if (!isOK) res.dirNum = -1; //���·������ȷ��������Ŀ¼�ڵ��dirName��-1
	return res;
}

void Main() {
	string inp;
	while (1)
	{
		cout << ">";
		getline(cin, inp);
		inp = trim(inp);

		const char* c_inp = inp.c_str();
		if (inp.compare("exit") == 0) {
			cout << "Bye!" << endl;
			break;
		}
		else if (inp.length() >= 2 && c_inp[0] == 'l'&&c_inp[1] == 's') {
			if (!inp.compare("ls")) {
				ls_display(root);
			}
			else if(!inp.compare("ls -l")) {
				ls_l_display(root);
			}
			else {
				string path = ls_l_resolve(inp);  
				//cout << "path: " << path << endl;
				
				if (path.length() == 0) {
					cout << "The path is invalid��" << endl;
				}
				else {
					Directory dir = locateDirectory(path);
					if (dir.dirNum==-1) {
						cout << "The path is invalid��" << endl;
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
				if (c_inp[i] != ' ')break;  //ȡ��cat�����space
			}
			//ȡ���ļ���Ŀ¼:
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
			if (path.length() == 0) { //pathΪ�վ��Ǹ�Ŀ¼
				dir = root;
			}
			else {
				dir = locateDirectory(path);
			}
			if (dir.dirNum == -1) {
				cout << "The path is invalid��" << endl;
			}
			else {
				int cluster = dir.hasFile(upper(fileName));
				if (cluster==-1) {
					cout << "The file is not exist!" << endl;
				}
				else {
					if (cluster >= 2) 
						printFileContent(cluster);
				}

			}
		}
		else if(inp.length()>0){
			cout << "Wrong Command! Try Again!\n";
		}
	}
}


int main() {
	fat12 = fopen("imgs/a.img", "rb");	//��FAT12��ӳ���ļ�
	init();
	Main();
	fclose(fat12); //����������رա�
	system("pause");
}

void fillBPB( struct BPB* bpb_ptr) {
	int check;
	//BPB��ƫ��11���ֽڴ���ʼ
	check = fseek(fat12, 11, SEEK_SET);
	if (check == -1)
		printf("fseek in fillBPB failed!");

	//BPB����Ϊ25�ֽ�
	check = fread(bpb_ptr, 1, 25, fat12);
	if (check != 25)
		printf("fread in fillBPB failed!");
}


void printFiles(struct RootEntry* rootEntry_ptr, Directory* root) {
	int base = (RsvdSecCnt + NumFATs * FATSz) * BytsPerSec;	//��Ŀ¼���ֽڵ�ƫ����
	int check;
	char realName[12];	//�ݴ潫�ո��滻�ɵ����ļ���

	//���δ����Ŀ¼�еĸ�����Ŀ
	int i;
	for (i = 0; i < RootEntCnt; i++) {

		check = fseek(fat12, base, SEEK_SET);
		if (check == -1)
			printf("fseek in printFiles failed!");

		check = fread(rootEntry_ptr, 1, 32, fat12);
		if (check != 32)
			printf("fread in printFiles failed!");
		base += 32;
		if (rootEntry_ptr->DIR_Name[0] == '\0') continue;	//����Ŀ�����

		//���˷�Ŀ���ļ�
		int j;
		int boolean = 0;
		for (j = 0; j < 11; j++) {
			if (!(((rootEntry_ptr->DIR_Name[j] >= 48) && (rootEntry_ptr->DIR_Name[j] <= 57)) ||
				((rootEntry_ptr->DIR_Name[j] >= 65) && (rootEntry_ptr->DIR_Name[j] <= 90)) ||
				((rootEntry_ptr->DIR_Name[j] >= 97) && (rootEntry_ptr->DIR_Name[j] <= 122)) ||
				(rootEntry_ptr->DIR_Name[j] == ' '))) {
				boolean = 1;	//��Ӣ�ļ����֡��ո�
				break;
			}
		}
		if (boolean == 1) continue;	//��Ŀ���ļ������

		int k;
		if ((rootEntry_ptr->DIR_Attr & 0x10) == 0) {
			//�ļ�
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
			realName[tempLong] = '\0';	//����Ϊֹ�����ļ�����ȡ�����ŵ���realName��

			root->addFile(realName,rootEntry_ptr->DIR_FileSize,rootEntry_ptr->DIR_FstClus);
			//int cl = rootEntry_ptr->DIR_FstClus;
			//cout << "cluster of : " <<rootEntry_ptr->DIR_Name<<": "<<cl << endl;
			//����ļ�
			//printf("FileName: %s\n", realName);
		}
		else {
			//Ŀ¼
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
			}	//����Ϊֹ����Ŀ¼����ȡ�����ŵ���realName

			Directory d = Directory(realName);
			root->addDir(d);
			//strcpy(subNode->Filename, realName);

			//���Ŀ¼�����ļ�
			printChildren(realName, rootEntry_ptr->DIR_FstClus, &root->subDirs[root->dirNum-1]);
		}
	}
}


void printChildren(char * directory, int startClus,Directory* d) {
	//�������ĵ�һ���أ���2�Ŵأ���ƫ���ֽ�
	int dataBase = BytsPerSec * (RsvdSecCnt + FATSz * NumFATs + (RootEntCnt * 32 + BytsPerSec - 1) / BytsPerSec);
	char fullName[240];	//����ļ�·����ȫ��
	int strLength = strlen(directory);
	strcpy(fullName, directory);
	fullName[strLength] = '/';
	strLength++;
	fullName[strLength] = '\0';
	char* fileName = &fullName[strLength];

	int currentClus = startClus;
	int value = 0;
	int ifOnlyDirectory = 0;

	while (value < 0xFF8) {
		value = getFATValue(currentClus);
		if (value < 0) {
			break;
		}
		if (value == 0xFF7) {
			printf("���أ���ȡʧ��!\n");
			break;
		}

		char* str = (char*)malloc(SecPerClus*BytsPerSec);	//�ݴ�Ӵ��ж���������
		char* content = str;

		int startByte = dataBase + (currentClus - 2)*SecPerClus*BytsPerSec;
		int check;
		check = fseek(fat12, startByte, SEEK_SET);
		if (check == -1)
			printf("fseek in printChildren failed!");

		check = fread(content, 1, SecPerClus*BytsPerSec, fat12);
		if (check != SecPerClus * BytsPerSec)
			printf("fread in printChildren failed!");

		//����content�е�����,���δ��������Ŀ,Ŀ¼��ÿ����Ŀ�ṹ���Ŀ¼�µ�Ŀ¼�ṹ��ͬ
		int count = SecPerClus * BytsPerSec;	//ÿ�ص��ֽ���
		int loop = 0;
		while (loop < count) {
			int i;
			char tempName[120];	//�ݴ��滻�ո�Ϊ�����ļ���
			if (content[loop] == '\0') {
				loop += 32;
				continue;
			}	//����Ŀ�����


				//���˷�Ŀ���ļ�
			int j;
			int boolean = 0;
			for (j = loop; j < loop + 11; j++) {
				if (!(((content[j] >= 48) && (content[j] <= 57)) ||
					((content[j] >= 65) && (content[j] <= 90)) ||
					((content[j] >= 97) && (content[j] <= 122)) ||
					(content[j] == ' '))) {
					boolean = 1;	//��Ӣ�ļ����֡��ո�
					break;
				}
			}
			if (boolean == 1) {
				loop += 32;
				continue;
			}	//��Ŀ���ļ������

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
				a = a >= 0 ? a : 256 + a;//������
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
	//	printf("%s\n", fullName);	//��Ŀ¼������£����Ŀ¼
}

//����ļ����ݣ�
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
			cout << "bad cluster\n";
		}
		char temp[513];
		int startByte = dataBase + 512 * (curclus - 2);
		check = fseek(fat12, startByte, SEEK_SET);
		if (check == -1)cout << "Failed!\n";
		check = fread(temp, 1, 512, fat12);
		temp[512] = '\0';
		cout << temp<<endl;
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
		printf("fseek in getFATValue failed!");
		return -1;
	}

	check = fread(bytes_ptr, 1, 2, fat12);
	if (check != 2) {
		printf("fread in getFATValue failed!");
		return -1;
	}

	//u16Ϊshort����ϴ洢��С��˳���FAT��ṹ���Եõ�
	//typeΪ0�Ļ���ȡbyte2�ĵ�4λ��byte1���ɵ�ֵ��typeΪ1�Ļ���ȡbyte2��byte1�ĸ�4λ���ɵ�ֵ
	if (type == 0) {
		bytes = bytes << 4;
		bytes = bytes >> 4;
	}
	else {
		bytes = bytes >> 4;
	}
	return bytes;
}