#include <vector>
#include <string>
#include <iostream>
/*
	之前使用char*,一直不行，但是现在我改成string就好了
*/
using namespace std;
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

void display(Directory d) {
	printf("Dir: %s\n", d.dirName.c_str());
	for (int i = 0; i < d.dirNum; i++) {
		display(d.subDirs[i]);
	}
	for (int i = 0; i < d.fileNum; i++) {
		printf("File %d :%s\n", i + 1, d.subFiles[i].filename.c_str());
	}
}

vector<string> path; //当前已经遍历的目录

string getPath() {
	string res = "/";
	for (int i = 0; i < path.size(); i++) {
		res += (path[i] + "/");
	}
	return res ;
}

void ls_display(Directory d) {
	if (d.dirName.length()>0) path.push_back(d.dirName); //如果不是根目录

	printf("%s:\n",getPath().c_str());

	if(d.dirName.length()>0) printf(".  ..  ");
	for (int i = 0; i < d.dirNum; i++) {
		printf("%s  ", d.subDirs[i].dirName.c_str());
	}
	for (int i = 0; i < d.fileNum; i++) {
		printf("%s  ",  d.subFiles[i].filename.c_str());
	}
	printf("\n");
	for (int i = 0; i < d.dirNum; i++) {
		ls_display(d.subDirs[i]);
	}
	if (d.dirName.length() > 0)
	{
		path.pop_back();
	}
}

void ls_l_display(Directory d) {
	if (d.dirName.length() > 0) path.push_back(d.dirName); //如果不是根目录
	cout << getPath() << " " << d.dirNum << " " << d.fileNum<<":"<<endl;
	if (d.dirName.length() > 0) cout << "." << endl << ".." << endl;
	for (int i = 0; i < d.dirNum; i++) {
		cout << d.subDirs[i].dirName << "  " << d.subDirs[i].dirNum << " " << d.subDirs[i].fileNum << endl;;
	}
	for (int i = 0; i < d.fileNum; i++) {
		cout << d.subFiles[i].filename << "  " << d.subFiles[i].size << endl;
	}
	cout << endl;
	for (int i = 0; i < d.dirNum; i++) {
		ls_l_display(d.subDirs[i]);
	}
	if (d.dirName.length() > 0) path.pop_back();
}


/*
	根据目录名获取目录节点
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
