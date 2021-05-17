/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            keyboard.c
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
#include "proto.h"
#include "keyboard.h"
#include "keymap.h"
#include "myConst.h"
#include "operatingRecordTable.h"

PRIVATE KB_INPUT	kb_in;

PRIVATE	int	code_with_E0;
PRIVATE	int	shift_l;	/* l shift state */
PRIVATE	int	shift_r;	/* r shift state */
PRIVATE	int	alt_l;		/* l alt state	 */
PRIVATE	int	alt_r;		/* r left state	 */
PRIVATE	int	ctrl_l;		/* l ctrl state	 */
PRIVATE	int	ctrl_r;		/* l ctrl state	 */
PRIVATE	int	caps_lock;	/* Caps Lock	 */
PRIVATE	int	num_lock;	/* Num Lock	 */
PRIVATE	int	scroll_lock;	/* Scroll Lock	 */
PRIVATE	int	column;

PRIVATE int	caps_lock;	/* Caps Lock	 */
PRIVATE int	num_lock;	/* Num Lock	 */
PRIVATE int	scroll_lock;	/* Scroll Lock	 */

PRIVATE u8	get_byte_from_kbuf();
PRIVATE void    set_leds();
PRIVATE void    kb_wait();
PRIVATE void    kb_ack();



PUBLIC char* getCurChar(ORT* p_ort){
//count表示表项的数量
	if(p_ort->count==0)return p_ort;
	p_ort->count--;
	return p_ort->table[p_ort->count];
}



void setCursor(CONSOLE *curCon){
	int position = curCon->cursor;
	disable_int();
	out_byte(CRTC_ADDR_REG, CURSOR_H);
	out_byte(CRTC_DATA_REG, (position >> 8) & 0xFF);
	out_byte(CRTC_ADDR_REG, CURSOR_L);
	out_byte(CRTC_DATA_REG, position & 0xFF);
	enable_int();
}



/*======================================================================*
                            keyboard_handler
 *======================================================================*/
PUBLIC void keyboard_handler(int irq)
{
	u8 scan_code = in_byte(KB_DATA);

	if (kb_in.count < KB_IN_BYTES) {
		*(kb_in.p_head) = scan_code;
		kb_in.p_head++;
		if (kb_in.p_head == kb_in.buf + KB_IN_BYTES) {
			kb_in.p_head = kb_in.buf;
		}
		kb_in.count++;
	}
}


/*======================================================================*
                           init_keyboard
*======================================================================*/
PUBLIC void init_keyboard()
{
	kb_in.count = 0;
	kb_in.p_head = kb_in.p_tail = kb_in.buf;

	shift_l	= shift_r = 0;
	alt_l	= alt_r   = 0;
	ctrl_l	= ctrl_r  = 0;

	caps_lock   = 0;
	num_lock    = 1;
	scroll_lock = 0;

	set_leds();

        put_irq_handler(KEYBOARD_IRQ, keyboard_handler);/*设定键盘中断处理程序*/
        enable_irq(KEYBOARD_IRQ);                       /*开键盘中断*/
}



//Edwin:
	SearchMode =1;
	LOCK = FALSE;

/*======================================================================*
                           keyboard_read
*======================================================================*/
PUBLIC void keyboard_read(TTY* p_tty,int* P1, int *P2,int *P2_LOCK,ORT* p_ort)
{


	u8	scan_code;
	char	output[2];
	int	make;	/* 1: make;  0: break. */

	u32	key = 0;/* 用一个整型来表示一个键。比如，如果 Home 被按下，
			 * 则 key 值将为定义在 keyboard.h 中的 'HOME'。
			 */
	u32*	keyrow;	/* 指向 keymap[] 的某一行 */

	if(kb_in.count > 0){
		code_with_E0 = 0;

		scan_code = get_byte_from_kbuf();

		//如果已经按下回车开始搜索，就屏蔽其他输入，但是并没有，只是让第二次也可以回车了。|||现在有用了：
		///在tty.c里面设置了：
										 //if(SearchMode==1)  //如果是搜索模式下，就不要输出这个换行
										//put_key(p_tty, '\n');
						//居然就可以了
										
		if(*P2_LOCK == TRUE && scan_code != 0x01){ 
			return ; 
		}


		//set P2:
		if(SearchMode==0&&scan_code==0x1C&&*P2_LOCK==FALSE){
			*P2 = (&console_table[0])->cursor;
			//printf("+");
			*P2_LOCK=TRUE;
			//屏蔽所有的输入：
			//*KEYBOARD_LOCK = TRUE;
			
			//-------------------->开始搜索匹配：
			//                [*P1, *P2)
			char pattern[80];
			for(int i=*P1;i<*P2;i++){
				u8* p_vmem = (u8*)(V_MEM_BASE + i * 2); //光标也是相对与显存的基址，只不过2单位算1
				//disp_int(*p_vmem);
				pattern[i-*P1]=*p_vmem;
			}
			int len = *P2-*P1;
			pattern[len]='\0';
			//printf(pattern);  //       get Pattern
			
			//搜索：
			CONSOLE *curCon = &console_table[0];
			for(int i = V_MEM_BASE+curCon->original_addr*2;i<=V_MEM_BASE + *P1 * 2-len*2;i+=2){//易错
				u8* p_vmem = (u8*)(i);
				int isOK = TRUE;
				for(int j=0;j<len;j++){
					 if(*(p_vmem+j*2)!=pattern[j]){
					 	 isOK = FALSE;
					 	 break;
					 }
				}
				if(isOK==TRUE){
					//匹配：
					for(int j=0;j<len;j++){
					  *(p_vmem+j*2+1) = FONT_COLOR_AQUA; //change font color: 青色
					}
					i+=(len*2-2);//易错
				}
			}
			
			}else{
				*P2_LOCK = FALSE;
			}
			
			
			
			
			
			//撤销：
			u8* p;
			CONSOLE *curCon = &console_table[0];
			int begin,end,cur_org,count=0;
			if (scan_code == 0x1D) {	//Ctrl
				 scan_code = get_byte_from_kbuf();
		     if(scan_code==0x2C){ // z
		     		//printf("111111");
		     
		     		char * arr = getCurChar(p_ort);
		     		
		     		//disp_int(p_ort->count);
		     		//disp_int(arr[0]);
		     		//disp_int(arr[1]);
		     		//disp_int(arr[2]);
		     		//disp_int(arr[3]);
		     		
		     		
		     		if(arr[0]==1){
		     			//增加，需删除
		     			p = (u8*)(V_MEM_BASE + curCon->cursor * 2); 
		     			if(arr[2]==0&&arr[3]==0){
		     				*(p-2)=' ';
		     				curCon->cursor--;
		     				setCursor(curCon);
		     			}
		     			else if(arr[2]==1)//tab
		     			
		     			{
		     				*(p-2)=' ';
		     				*(p-4)=' ';
		     				*(p-6)=' ';
		     				*(p-8)=' ';
		     				curCon->cursor-=4;
		     				setCursor(curCon);
		     			}
		     			else if(arr[3]==1){//\n
								cur_org = curCon->cursor -curCon->original_addr;
								if(cur_org%80==0){
										while(*(p-2)==0){
											count++;
											if(count==81)break; //防止一次删掉多个空行
											*(p-2) = ' ';
											curCon->cursor--;
											p = (u8*)(V_MEM_BASE+curCon->cursor*2);
										}
				   			}
				   			setCursor(curCon);
		     			}
		     		}
		     		else if(arr[0]==0){
		     			//删除，需增加
		     			p = (u8*)(V_MEM_BASE + curCon->cursor * 2); 
		     			if(arr[2]==0&&arr[3]==0){	
		     			//正常情况，增加一个字符
		     				*(p)=arr[1];
		     				curCon->cursor++;
		     				setCursor(curCon);
		     			}
		     			else if(arr[2]==1){
		     				//刚才删除了一个tab
		     				in_process(p_tty, 0);
								in_process(p_tty, 0);
								in_process(p_tty, 0);
								in_process(p_tty, 0);
		     			}
		     			else if(arr[3]==1){
		     				//刚才删除一个换行
		     					out_char(&console_table[0],'\n');
		     				}
		     			
		     			
		     		}
       		
       		
       		return;
       }
		 }
		

	
	
	

		/* 下面开始解析扫描码 */
		//Edwin:
		if(scan_code ==0x01){
			key = ESC;	
			SearchMode=(SearchMode==0)?1:0;
			
			if(SearchMode==1) //退出正常模式
			{
				CONSOLE *curCon = &console_table[0];
				//从ESC退出，清除颜色
				
				//把锁取消
				*P2_LOCK = FALSE;
			
				
				for(int i = V_MEM_BASE+curCon->original_addr*2;i<V_MEM_BASE + *P1 * 2;i+=2)
				{
					u8* p_vmem = (u8*)(i);
					*(p_vmem+1) = DEFAULT_CHAR_COLOR; //change font color: 青色
				}
				
				//清除输入的pattern
				u8* p_vmem = (u8*)(V_MEM_BASE + *P1 * 2);
				for(int i=0;i<*P2-*P1;i++)
				{
					*(p_vmem+2*i)=' ';
					*(p_vmem+2*i+1)=DEFAULT_CHAR_COLOR;
					curCon->cursor--;
					
					//光标重置：
					setCursor(curCon);

				}
				
				
			}
			
			
			
		}

		if (scan_code == 0x0F){
			key = TAB;
		}

		
		
		else if (scan_code == 0xE1) {
			int i;
			u8 pausebrk_scode[] = {0xE1, 0x1D, 0x45,
					       0xE1, 0x9D, 0xC5};
			int is_pausebreak = 1;
			for(i=1;i<6;i++){
				if (get_byte_from_kbuf() != pausebrk_scode[i]) {
					is_pausebreak = 0;
					break;
				}
			}
			if (is_pausebreak) {
				key = PAUSEBREAK;
			}
		}
		else if (scan_code == 0xE0) {
			scan_code = get_byte_from_kbuf();

			/* PrintScreen 被按下 */
			if (scan_code == 0x2A) {
				if (get_byte_from_kbuf() == 0xE0) {
					if (get_byte_from_kbuf() == 0x37) {
						key = PRINTSCREEN;
						make = 1;
					}
				}
			}
			/* PrintScreen 被释放 */
			if (scan_code == 0xB7) {
				if (get_byte_from_kbuf() == 0xE0) {
					if (get_byte_from_kbuf() == 0xAA) {
						key = PRINTSCREEN;
						make = 0;
					}
				}
			}
			/* 不是PrintScreen, 此时scan_code为0xE0紧跟的那个值. */
			if (key == 0) {
				code_with_E0 = 1;
			}
		}
		if ((key != PAUSEBREAK) && (key != PRINTSCREEN)) {
			/* 首先判断Make Code 还是 Break Code */
			make = (scan_code & FLAG_BREAK ? 0 : 1);

			/* 先定位到 keymap 中的行 */
			keyrow = &keymap[(scan_code & 0x7F) * MAP_COLS];

			column = 0;

			int caps = shift_l || shift_r;
			if (caps_lock) {
				if ((keyrow[0] >= 'a') && (keyrow[0] <= 'z')){
					caps = !caps;
				}
			}
			if (caps) {
				column = 1;
			}

			if (code_with_E0) {
				column = 2;
			}

			key = keyrow[column];

			switch(key) {
			case SHIFT_L:
				shift_l = make;
				break;
			case SHIFT_R:
				shift_r = make;
				break;
			case CTRL_L:
				ctrl_l = make;
				break;
			case CTRL_R:
				ctrl_r = make;
				break;
			case ALT_L:
				alt_l = make;
				break;
			case ALT_R:
				alt_l = make;
				break;
			case CAPS_LOCK:
				if (make) {
					caps_lock   = !caps_lock;
					set_leds();
				}
				break;
			case NUM_LOCK:
				if (make) {
					num_lock    = !num_lock;
					set_leds();
				}
				break;
			case SCROLL_LOCK:
				if (make) {
					scroll_lock = !scroll_lock;
					set_leds();
				}
				break;
			default:
				break;
			}

			if (make) { /* 忽略 Break Code */
	
				int pad = 0;

				/* 首先处理小键盘 */
				if ((key >= PAD_SLASH) && (key <= PAD_9)) {
					pad = 1;
					switch(key) {
					case PAD_SLASH:
						key = '/';
						break;
					case PAD_STAR:
						key = '*';
						break;
					case PAD_MINUS:
						key = '-';
						break;
					case PAD_PLUS:
						key = '+';
						break;
					case PAD_ENTER:
						key = ENTER;
						break;
					default:
						if (num_lock &&
						    (key >= PAD_0) &&
						    (key <= PAD_9)) {
							key = key - PAD_0 + '0';
						}
						else if (num_lock &&
							 (key == PAD_DOT)) {
							key = '.';
						}
						else{
							switch(key) {
							case PAD_HOME:
								key = HOME;
								break;
							case PAD_END:
								key = END;
								break;
							case PAD_PAGEUP:
								key = PAGEUP;
								break;
							case PAD_PAGEDOWN:
								key = PAGEDOWN;
								break;
							case PAD_INS:
								key = INSERT;
								break;
							case PAD_UP:
								key = UP;
								break;
							case PAD_DOWN:
								key = DOWN;
								break;
							case PAD_LEFT:
								key = LEFT;
								break;
							case PAD_RIGHT:
								key = RIGHT;
								break;
							case PAD_DOT:
								key = DELETE;
								break;
							default:
								break;
							}
						}
						break;
					}
				}
				

				key |= shift_l	? FLAG_SHIFT_L	: 0;
				key |= shift_r	? FLAG_SHIFT_R	: 0;
				key |= ctrl_l	? FLAG_CTRL_L	: 0;
				key |= ctrl_r	? FLAG_CTRL_R	: 0;
				key |= alt_l	? FLAG_ALT_L	: 0;
				key |= alt_r	? FLAG_ALT_R	: 0;
				key |= pad      ? FLAG_PAD      : 0;

				//Edwin:
				if(key==TAB){
					
					//a tab = 4 spaces
					in_process(p_tty, 0);
					in_process(p_tty, 0);
					in_process(p_tty, 0);
					in_process(p_tty, 0);
				}
				else{
					in_process(p_tty, key);
				}
				
			}
		}
	}
	
	
}




/*======================================================================*
			    get_byte_from_kbuf
 *======================================================================*/
PRIVATE u8 get_byte_from_kbuf()       /* 从键盘缓冲区中读取下一个字节 */
{
        u8 scan_code;

        while (kb_in.count <= 0) {}   /* 等待下一个字节到来 */

        disable_int();
        scan_code = *(kb_in.p_tail);
        kb_in.p_tail++;
        if (kb_in.p_tail == kb_in.buf + KB_IN_BYTES) {
                kb_in.p_tail = kb_in.buf;
        }
        kb_in.count--;
        enable_int();

	return scan_code;
}

/*======================================================================*
				 kb_wait *======================================================================*/
PRIVATE void kb_wait()	/* 等待 8042 的输入缓冲区空 */
{
	u8 kb_stat;

	do {
		kb_stat = in_byte(KB_CMD);
	} while (kb_stat & 0x02);
}


/*======================================================================*
				 kb_ack
 *======================================================================*/
PRIVATE void kb_ack()
{
	u8 kb_read;

	do {
		kb_read = in_byte(KB_DATA);
	} while (kb_read =! KB_ACK);
}

/*======================================================================*
				 set_leds
 *======================================================================*/
PRIVATE void set_leds()
{
	u8 leds = (caps_lock << 2) | (num_lock << 1) | scroll_lock;

	kb_wait();
	out_byte(KB_DATA, LED_CODE);
	kb_ack();

	kb_wait();
	out_byte(KB_DATA, leds);
	kb_ack();
}

