/*
operatingRecordTable.h

操作记录表——记录执行的操作：
Operating Record Table
ORT
--------------------------|------------
|操作   | 字符  |   isTAb      isNewLine
--------------------------|-------------
|写入1  | char |    1/0         1/0 
--------------------------|-------------
|删除0  | char |   1/0          1/0 
--------------------------|------------


*/


/*++++++++++++++++++++EDWIN XU+++++2019/11/27+++++++++++++++++++*/

#ifndef _EDWINXU_OPERATINGRECORD_H_
#define _EDWINXU_OPERATINGRECORD_H_


#define OR_TABLE_SIZE	1000	/* OR table size */

/* ORT 
都是char 1:插入； 0：删除
是只增不减的——一直在操作

*/
typedef struct OperatingRecordTable
{
	char	table[OR_TABLE_SIZE][4];	/* 表格：*/
	int	count ;	/* 已经填充了多少行  其实这个就是一个指针，始终指向最有一个*/
	
	//OperatingRecordTable(){
	//	this.count=0;
	//}
	
	//void add(char a, char b, char isTab,char isnewline);
	//	table[count][0]=a;
	//	table[count][1]=b;
	//	table[count][2]=isTab;
	//	count++;
	//}
	//char getCurChar(){
	//	return table[count];
	//}

}ORT;


#endif
