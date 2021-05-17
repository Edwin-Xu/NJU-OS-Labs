
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            main.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "type.h"
#include "const.h"
#include "protect.h"
#include "proto.h"
#include "string.h"
#include "proc.h"
#include "global.h"

//EDWIN
//信号量
SEMOPHORE customers, mutex;
SEMOPHORE barber_available;
SEMOPHORE cut_ready, cut_start, cut_end;
SEMOPHORE customer_leave;

SEMOPHORE rw, mymutex, w, rLimit;
static int readcount = 1; 
//同时的读者限制
#define READER_LIMIT 3
//读者、写者优先  READ_FIRST 
const int rw_priority = WRITE_FIRST;//const


//初始化信号量
void init_semophore(SEMOPHORE *s, int value)
{
	s->value = value;
	s->queue_start = 0;
	s->queue_end = 0;
}

//int to char*
char* myitoa(int num,char* str,int radix)
{
    char index[]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";//索引表
    unsigned unum;
    int i=0,j,k;
 
    if(radix==10&&num<0)//要转换成十进制数并且是负数
    {
        unum=(unsigned)-num;//将num的绝对值赋给unum
        str[i++]='-';//在字符串最前面设置为'-'号，并且索引加1
    }
    else unum=(unsigned)num;
    do
    {
        str[i++]=index[unum%(unsigned)radix];
        unum/=radix;//unum去掉最后一位
 
    }while(unum);//直至unum为0退出循环
 
    str[i]='\0';
    //将顺序调整过来
    if(str[0]=='-') k=1;
    else k=0;//不是负数，全部都要调整
 
    char temp;//临时变量，交换两个值时用到
    for(j=k;j<=(i-1)/2;j++)
    {
        temp=str[j];//头部赋值给临时变量
        str[j]=str[i-1+k-j];//尾部赋值给头部
        str[i-1+k-j]=temp;
    }
 
    return str;//返回转换后的字符串
 
}

void printInt(int a){
	char * res;
	myitoa(a,res,10);
	myPrint_F(res);
}


/*======================================================================*
                            kernel_main
 *======================================================================*/
PUBLIC int kernel_main()
{
	

	TASK *p_task = task_table;
	PROCESS *p_proc = proc_table;
	char *p_task_stack = task_stack + STACK_SIZE_TOTAL;
	u16 selector_ldt = SELECTOR_LDT_FIRST;
	int i;
	for (i = 0; i < NR_TASKS; i++)
	{
		strcpy(p_proc->p_name, p_task->name); // name of the process
		p_proc->pid = i;					  // pid

		p_proc->ldt_sel = selector_ldt;

		memcpy(&p_proc->ldts[0], &gdt[SELECTOR_KERNEL_CS >> 3],
			   sizeof(DESCRIPTOR));
		p_proc->ldts[0].attr1 = DA_C | PRIVILEGE_TASK << 5;
		memcpy(&p_proc->ldts[1], &gdt[SELECTOR_KERNEL_DS >> 3],
			   sizeof(DESCRIPTOR));
		p_proc->ldts[1].attr1 = DA_DRW | PRIVILEGE_TASK << 5;
		p_proc->regs.cs = ((8 * 0) & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
		p_proc->regs.ds = ((8 * 1) & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
		p_proc->regs.es = ((8 * 1) & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
		p_proc->regs.fs = ((8 * 1) & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
		p_proc->regs.ss = ((8 * 1) & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
		p_proc->regs.gs = (SELECTOR_KERNEL_GS & SA_RPL_MASK) | RPL_TASK;

		p_proc->regs.eip = (u32)p_task->initial_eip;
		p_proc->regs.esp = (u32)p_task_stack;
		p_proc->regs.eflags = 0x1202; /* IF=1, IOPL=1 */

		p_proc->wake_tick = 0;
		p_proc->waiting_semophore = 0;
		p_proc->ticks = TIME_SLICE;

		p_task_stack -= p_task->stacksize;
		p_proc++;
		p_task++;
		selector_ldt += 1 << 3;
	}

	k_reenter = 0;
	ticks = 0;


	
	
	//edwin:
	init_semophore(&rw,1); 
	init_semophore(&mymutex,1); 
	init_semophore(&w,1);
	init_semophore(&rLimit,READER_LIMIT);	
	

	// clear screen and reset cursor

	u8 *base = (u8 *)V_MEM_BASE;
	for (int i = 0; i < V_MEM_SIZE; i += 2)
	{
		base[i] = ' ';
		base[i + 1] = DEFAULT_CHAR_COLOR;
	}
	disp_pos = 0;

	p_proc_ready = proc_table;

	/* 初始化 8253 PIT */
	out_byte(TIMER_MODE, RATE_GENERATOR);
	out_byte(TIMER0, (u8)(TIMER_FREQ / HZ));
	out_byte(TIMER0, (u8)((TIMER_FREQ / HZ) >> 8));

	put_irq_handler(CLOCK_IRQ, clock_handler); /* 设定时钟中断处理程序 */
	enable_irq(CLOCK_IRQ);					   /* 让8259A可以接收时钟中断 */

	restart();

	while (1){}
}



void myPrint_reader(char* content) {
	int color=0x0A;
	disp_color_str(content, color);
	disp_str("   ");
}
void myPrint_writer(char* content) {
	int color=0x0C;
	disp_color_str(content, color);
	disp_str("   ");
}
void myPrint_F(char* content) {
	int color=0x0E;
	disp_color_str(content, color);
	disp_str("   ");
}


void reader_i(int i,int delay){

	char *start = "RA_comming";
	char *reading = "RA_Reading";
	char *over = "RA_over";
	if (i==1) {
	  start = "RB_comming";
	  reading = "RB_Reading";
		over = "RB_over";
	}
	else if (i==2) {
	  start = "RC_comming";
	  reading = "RC_Reading";
		over = "RC_over";
	}
	
	myPrint_reader(start);

	if(rw_priority==READ_FIRST){
		sem_p(&rLimit);
	
		sem_p(&mymutex);
		if(readcount==1){
			sem_p(&rw);
		}
		
		readcount++;
		sem_v(&mymutex);

		myPrint_reader(reading);
		milli_delay(delay* 1000 /HZ);
		myPrint_reader(over);

		sem_p(&mymutex);
		readcount--;
		
		if(readcount==1){
			sem_v(&rw);
		}
		sem_v(&mymutex);
		
		sem_v(&rLimit);
	}
	
	else {
		sem_p(&rLimit);
		
		sem_p(&w);//互斥写
		sem_p(&mymutex);
		if(readcount==1){
			sem_p(&rw);
		}
		
		readcount++;
		sem_v(&mymutex);
		sem_v(&w);
	
		myPrint_reader(reading);
		milli_delay(delay* 1000 /HZ);
		myPrint_reader(over);
		
		sem_p(&mymutex);
		readcount--;

		if(readcount==1){
			sem_v(&rw);
		}
		sem_v(&mymutex);
		
		sem_v(&rLimit);
	}
}

void writer_i(int i,int delay){
	char *start = "WD_comming";
	char *writing = "WD_Writing";
	char *over = "WD_over";
	if (i==1) {
	  start = "WE_comming";
	  writing = "WE_Writing";
		over = "WE_over";
	}
	
	myPrint_writer(start);
	if(rw_priority==READ_FIRST){
			sem_p(&rw);

			myPrint_writer(writing);
			milli_delay(delay* 1000 /HZ);
			myPrint_writer(over);
			
			sem_v(&rw);
	}
	else{
		sem_p(&w);//在无写进程时请求进入
		sem_p(&rw);//互斥访问共享file, 即有无读者
		
		myPrint_writer(writing);
		milli_delay(delay* 1000 /HZ);
		myPrint_writer(over);
		
		sem_v(&rw);
		sem_v(&w);
	}
}


//reader
void A(){
	
	while (1){
		reader_i(0,2);
	}
}
//reader
void B(){
	
	while (1){
		reader_i(1,3);
	}
}
//reader
void C(){

	while (1){
		reader_i(2,3);
	}
}
//writer
void D(){
	while (1){
		writer_i(0,3);
	}
}
//writer
void E(){
	while (1){
		writer_i(1,4);
	}
}
//regular
void F(){
	while (1){
		if(readcount==1){
			myPrint_F("writing");
		}
		else{
			myPrint_F("reading:");
			printInt(readcount-1);
		}
		
		process_sleep(1* 1000 /HZ);
	}
}





