abc
	subabc
		1.txt
aaa
	2.txt
3.txt



https://blog.csdn.net/qq_39654127/article/details/88429461#main-toc


递归 ok
打印 
cat 
ls  -l ok





First Logical Clouster
指向的簇存放所有子目录、簇的目录




之前是我自己写：
	//过滤非目标文件
			int j;
			int boolean = 0;
			for (j = loop; j < loop + 11; j++) {
				if (!(((content[j] >= 48) && (content[j] <= 57)) ||
					((content[j] >= 65) && (content[j] <= 90)) ||
					((content[j] >= 97) && (content[j] <= 122)) ||
					(content[j] == ' '))) {
					boolean = 1;	//非英文及数字、空格
					break;
				}
			}
			if (boolean == 1) {
				loop += 32;
				continue;
			}	//非目标文件不输出
把这个删掉了
会报错
一个你找不到原因的错误
正是因为这个错误搞了我几个小时



ls -l 1.txt


