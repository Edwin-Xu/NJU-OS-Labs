/*
console 中的内容

还是需要另外一个输入表，这个表：
25X80X2
列X行X[ASCII，isTAB]

为什么不用一个特殊的char值表示TAB，如 0
那就二维数组：
列X行


为什么不直接遍历显存？？？？
对啊，这个也没必要

*/
#ifndef _EDWINXU_OPERATINGRECORD_H_
#define _EDWINXU_OPERATINGRECORD_H_

#define ROWS	1000
#define COLS  1000
//#define ATTRS	2  //属性数量为2：[ASCII，isTAB]

typedef struct ConsoleContent{
	char cc[ROWS][COLS];
	

}CC;




#endif
