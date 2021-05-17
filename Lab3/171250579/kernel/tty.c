
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                               tty.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "type.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "keyboard.h"
#include "proto.h"
#include "myConst.h"
#include "operatingRecordTable.h"

#define TTY_FIRST	(tty_table)
#define TTY_END		(tty_table + NR_CONSOLES)


//按下ESC时,记录当前光标在显存的位置 P1
//按下回车时,记录当前光标位置P2
int P1=0;
int P2=0;
int P1_LOCK = FALSE; //用于寻找P1P2中锁定，避免重复赋值
int P2_LOCK = FALSE;

	ORT ort;  // 
	ORT* p_ort = &ort;

PRIVATE void init_tty(TTY* p_tty);
PRIVATE void tty_do_read(TTY* p_tty);
PRIVATE void tty_do_write(TTY* p_tty);
PRIVATE void put_key(TTY* p_tty, u32 key);

/*======================================================================*
                           task_tty

Edwin: Who Call This Function???????????
 *======================================================================*/
 
 
PUBLIC void add(ORT* p_ort,char a, char b, char isTab,char isNewLine){
	p_ort->table[p_ort->count][0]=a;
	p_ort->table[p_ort->count][1]=b;
	p_ort->table[p_ort->count][2]=isTab;
	p_ort->table[p_ort->count][3]=isNewLine;
	p_ort->count++;
}

 
 
PUBLIC void task_tty()
{
	TTY*	p_tty;
	
	//Edwin:


	p_ort->count=0;
	
	
	int  curTicks = get_ticks();
	//End
	
	
	init_keyboard();

	for (p_tty=TTY_FIRST;p_tty<TTY_END;p_tty++) {
		init_tty(p_tty);
	}
	select_console(0);
	
	//
	CONSOLE *curCon = &console_table[0];
	
	while (1) {
		for (p_tty=TTY_FIRST;p_tty<TTY_END;p_tty++) {
		
			tty_do_read(p_tty);
			tty_do_write(p_tty);

			//Edwin:
			if(SearchMode==1){//正常模式
					//清幕>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
					//这里是清屏幕：和输入缓冲区无关，是Console
					//我现在只有一个console：
				//CONSOLE *curCon = &console_table[0];
				//每个TTY都有一个Console，所以这里严格来说时p_tty->p_console
				
				if((get_ticks()-curTicks)/HZ>=CLEAN_SCREEN_PERIOD)//myConst.h
				{ 	
					//把ort清空
					p_ort->count=0;
				
				
					u8* p_vmem;
					while (curCon->cursor > curCon->original_addr) 		//
					{ 											//original_addr：当前控制台对应显存位置
						p_vmem = (u8*)(V_MEM_BASE + curCon->cursor * 2);
						curCon->cursor--;
						*(p_vmem-2) = ' ';
						*(p_vmem-1) = DEFAULT_CHAR_COLOR;
					}
					//把光标移到最初位置：左上角
					int position = curCon->cursor;
					disable_int();
					out_byte(CRTC_ADDR_REG, CURSOR_H);
					out_byte(CRTC_DATA_REG, (position >> 8) & 0xFF);
					out_byte(CRTC_ADDR_REG, CURSOR_L);
					out_byte(CRTC_DATA_REG, position & 0xFF);
					enable_int();
					
					curTicks = get_ticks();
				}
				//清幕<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

				P1_LOCK = FALSE;
			}
			if(SearchMode==0){
				//是SearchMode，需要更新curTicks的值。
				curTicks = get_ticks();
				
				//记录当前光标在显存的位置 P1 >>>>>>>>>>>>>>>>>>>>>>>>>>
				if(P1_LOCK==FALSE)
				{
	//这里时不断执行的，所以一旦赋值了P1，后面就不要赋值了，直到退出ESC时P1被设置为0
					P1 = curCon->cursor;
					P1_LOCK=TRUE;
				}

			}
			//End
			
		}
	}
}

/*======================================================================*
			   init_tty
 *======================================================================*/
PRIVATE void init_tty(TTY* p_tty)
{
	p_tty->inbuf_count = 0;
	p_tty->p_inbuf_head = p_tty->p_inbuf_tail = p_tty->in_buf;

	init_screen(p_tty);
}

/*======================================================================*
				in_process
 *======================================================================*/
PUBLIC void in_process(TTY* p_tty, u32 key)
{
        char output[2] = {'\0', '\0'};
        
        CONSOLE *curCon = &console_table[0];
        u8* p;
        char temp;

        if (!(key & FLAG_EXT)) {
        //FLAG_EXT:功能区、数字区(方形)、控制字符
        	
					put_key(p_tty, key);
					if(key=='\n')	add(p_ort,1,0,0,1);
					else if(key==0)add(p_ort,1,0,1,0);
					else add(p_ort,1,0,0,0);
					
        }
        else {
                int raw_code = key & MASK_RAW;
                switch(raw_code) {
                case ENTER:
                	if(SearchMode==1)  //如果是搜索模式下，就不要输出这个换行
										put_key(p_tty, '\n');
										//添加
									  add(p_ort,1,0 ,0,1);
									break;
                case BACKSPACE:
									
									//删除
									p =(u8*)(V_MEM_BASE+curCon->cursor*2);
									temp = (u8)*(p-2);		
									if(temp==0){
									//tab
									
										//注意：这里是删除，但是不知道前一个是不是Tab,或换行
										//上一个是0，当前光标是在行首还是非行首，行首就认为是\n(其实也可能是tab)，非行首就是tab          (有一点小问题)
										if((curCon->cursor - curCon->original_addr)%80==0){
												add(p_ort,0,0,0,1);//行首
												
										}else{
												add(p_ort,0,0,1,0);
										}
										
									}			
									else{
										add(p_ort,0,temp,0,0);//
									}
									
									put_key(p_tty, '\b'); //退格
									break;
                case UP:
                        if ((key & FLAG_SHIFT_L) || (key & FLAG_SHIFT_R)) {
														scroll_screen(p_tty->p_console, SCR_DN);
                        }
									break;
								case DOWN:
									if ((key & FLAG_SHIFT_L) || (key & FLAG_SHIFT_R)) {
										scroll_screen(p_tty->p_console, SCR_UP);
									}
									break;
								case F1:
								case F2:
								case F3:
								case F4:
								case F5:
								case F6:
								case F7:
								case F8:
								case F9:
								case F10:
								case F11:
								case F12:
									/* Alt + F1~F12 */
									if ((key & FLAG_ALT_L) || (key & FLAG_ALT_R)) {
										select_console(raw_code - F1);
									}
									break;
							  default:
												          break;
								}
						}
}

/*======================================================================*
			      put_key
*======================================================================*/
PRIVATE void put_key(TTY* p_tty, u32 key)
{
	if (p_tty->inbuf_count < TTY_IN_BYTES) {
		*(p_tty->p_inbuf_head) = key; //把key放到缓冲区中
		p_tty->p_inbuf_head++;
		if (p_tty->p_inbuf_head == p_tty->in_buf + TTY_IN_BYTES) {
			p_tty->p_inbuf_head = p_tty->in_buf;
		}
		p_tty->inbuf_count++;
	}
}


/*======================================================================*
			      tty_do_read
 *======================================================================*/
PRIVATE void tty_do_read(TTY* p_tty)
{
	if (is_current_console(p_tty->p_console)) {
		//edwin:
		//if(KEYBOARD_LOCK==FALSE){ 不能屏蔽所有输入啊！！！
			keyboard_read(p_tty,&P1,&P2,&P2_LOCK,  p_ort);
		//}
	}
	

}


/*======================================================================*
			      tty_do_write
 *======================================================================*/
PRIVATE void tty_do_write(TTY* p_tty)
{
	if (p_tty->inbuf_count) {
		char ch = *(p_tty->p_inbuf_tail);
		p_tty->p_inbuf_tail++;
		if (p_tty->p_inbuf_tail == p_tty->in_buf + TTY_IN_BYTES) {
			p_tty->p_inbuf_tail = p_tty->in_buf;
		}
		p_tty->inbuf_count--;

		out_char(p_tty->p_console, ch);
	}
}

/*======================================================================*
                              tty_write
*======================================================================*/
PUBLIC void tty_write(TTY* p_tty, char* buf, int len)
{
        char* p = buf;
        int i = len;

        while (i) {
                out_char(p_tty->p_console, *p++);
                i--;
        }
}

/*======================================================================*
                              sys_write
*======================================================================*/
PUBLIC int sys_write(char* buf, int len, PROCESS* p_proc)
{
        tty_write(&tty_table[p_proc->nr_tty], buf, len);
        return 0;
}

