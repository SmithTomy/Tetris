#include <iostream>
#include <sys/time.h>
#include <stdlib.h>
#include "curses.h"
#include <unistd.h>
#include <winsock.h>

using namespace std;

void swap(int &a, int &b){
	int t=a;
	a = b;
	b = t;
}
int getrand(int min, int max)
{
	return(min+rand()%(max-min+1));
}

WINDOW *create_newwin(int height,int width,int starty,int startx);
void destory_win(WINDOW *local_win);

int game_win_height=30;	
int game_win_width=45;

int hint_win_height=10;
int hint_win_width=20;
WINDOW * game_win, *hint_win ,*score_win;
int key;

class Piece
{
public:
	int score;
	int shape; 

	int head_x;
	int head_y;   //位置坐标
	int size_h;
	int size_w;   //当前方块的size
    //下一个方块的形状和size
    int next_shape;
	int next_size_h;
	int next_size_w;

	int box_shape[4][4];
	int next_box_shape[4][4];

	int box_map[30][45];

	bool game_over;

public:
	void initial();
	void set_shape(int &cshape, int box_shape[][4],int &size_w, int & size_h);

	void score_next();
	void judge();
	void move();
	void rotate();
	bool isaggin();
	bool exsqr(int row);

	
};

int main()
{

	initscr();
	//raw();
	cbreak();
	noecho();
	curs_set(0);
	keypad(stdscr,TRUE);

	refresh();
	
	game_win = create_newwin(game_win_height, game_win_width, 0,0);
	wborder(game_win, '*', '*', '*', '*', '*', '*', '*', '*');
	wrefresh(game_win);
    /**
     *  printw(string) //在stdscr的当前光标位置打印字符串string
        mvprintw(y,x,string)  //将字符串string打印在坐标(y,x)处
        wprintw(win,string)  //在窗口win的当前光标位置打印字符串string
        mvwprintw(win,y,x,string)  //将光标移动到窗口win的(y,x)处然后打印字符串string
     */
	hint_win = create_newwin(hint_win_height, hint_win_width, 0, game_win_width+10);
	mvprintw(0, game_win_width+10+2,"%s","Next");
	refresh();//调用 refresh() 函数，只更新 stdscr 上的缓冲区。

	score_win = create_newwin(hint_win_height, hint_win_width, 20, game_win_width+10);
	mvprintw(20, game_win_width+10+2,"%s","Score");
	refresh();



	Piece* pp = new Piece;
	pp->initial();


	while(1)
	{
		pp->move();
		if(pp->game_over)
			break;
	}

	destory_win(game_win);
	destory_win(hint_win);
	destory_win(score_win);
	delete pp;
	system("clear");

	int row,col;
	getmaxyx(stdscr,row,col);
	mvprintw(row/2,col/2 ,"%s","GAMER OVER ! \n ");
	mvprintw(row/2+2,col/2-2 ,"%s","Wait 5s to return tthe erminal ! \n ");
	refresh();

	sleep(5);
	endwin();
	return 0;
}
//建立窗口
WINDOW *create_newwin(int height, int width, int starty, int startx)
{
	WINDOW *local_win;
	local_win = newwin(height, width, starty, startx);
	box(local_win,0,0);  // box() 函数在已经定义的窗口外围画上边框。
	wrefresh(local_win);
	return local_win;
}

void destory_win(WINDOW *local_win)
{
	wborder(local_win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
	wrefresh(local_win);
	delwin(local_win);
}


void Piece::initial()
{
	score=0;
	game_over=false;
	for(int i =0;i<game_win_height;i++)
		for(int j=0;j<game_win_width;j++){
			if(i==0 || i==game_win_height-1 || j==0 || j==game_win_width-1){
				box_map[i][j]=1;
			}
			else
				box_map[i][j]=0;
		}

	srand((unsigned)time(0));
	shape=getrand(0,6);
	set_shape(shape,box_shape,size_w,size_h);

	next_shape=getrand(0,6);
	set_shape(next_shape,next_box_shape,next_size_w,next_size_h);

	for(int i =0;i<4;i++)
		for(int j=0;j<4;j++)
			if(next_box_shape[i][j]==1){
				mvwaddch(hint_win,(hint_win_height-size_h)/2+i,(hint_win_width-size_w)/2+j,'#');
				wrefresh(hint_win);
			}
				

	mvwprintw(score_win, hint_win_height/2,hint_win_width/2-2,"%d",score);
	wrefresh(score_win);
}
/**
 * 如何实现根据设定好的宽高，展现方块？？
 * @param cshape
 * @param shape
 * @param size_w
 * @param size_h
 */
void Piece::set_shape(int &cshape, int shape[][4],int &size_w,int &size_h)
{
	int i,j;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++)
			shape[i][j]=0;
	switch(cshape)
	{
        //定义7种形状，
		case 0:	  //第一种形状：I型**** 用二维数组元素 1，0两种标志表示形状
			size_h=1;
			size_w=4;	
			shape[0][0]=1;
			shape[0][1]=1;
			shape[0][2]=1;
			shape[0][3]=1;
			break;
		case 1:
            /**  J型：
             *        *
             *        * * *
             * */
			size_h=2;
			size_w=3;
			shape[0][0]=1;
			shape[1][0]=1;
			shape[1][1]=1;
			shape[1][2]=1;
			break;
		case 2:
            /** L型
             *        *
             *    * * *
             */
			size_h=2;
			size_w=3;	
			shape[0][2]=1;
			shape[1][0]=1;
			shape[1][1]=1;
			shape[1][2]=1;
			break;
		case 3:
            /**  s型
             *     * *
             *   * *
             *
             */
			size_h=2;
			size_w=3;
			shape[0][1]=1;
			shape[0][2]=1;
			shape[1][0]=1;
			shape[1][1]=1;
			break;

		case 4:
        /**  Z型
         *    * *
         *      * *
         *
         */
			size_h=2;
			size_w=3;
			shape[0][0]=1;
			shape[0][1]=1;
			shape[1][1]=1;
			shape[1][2]=1;
			break;

		case 5:
		/** O型
		 *  * *
		 *  * *
		 */
			size_h=2;
			size_w=2;
			shape[0][0]=1;
			shape[0][1]=1;
			shape[1][0]=1;
			shape[1][1]=1;
			break;
        /** T型
         *     *
         *   * * *
         */
		case 6:	
			size_h=2;
			size_w=3;
			shape[0][1]=1;
			shape[1][0]=1;
			shape[1][1]=1;
			shape[1][2]=1;
			break;
	}

	head_x=game_win_width/2;
	head_y=1;

	if(isaggin())    /* GAME OVER ! */
		game_over=true;

}


void Piece::rotate()  //旋转函数
{
	int temp[4][4]={0};
	int temp_piece[4][4]={0};  //备份数组，防止旋转失败丢失初始形状
	int i,j,tmp_size_h,tmp_size_w;

	tmp_size_w=size_w;
	tmp_size_h=size_h;

	for(int i=0; i<4;i++)
		for(int j=0;j<4;j++)
			temp_piece[i][j]=box_shape[i][j]; //备份一下当前的方块，如果旋转失败则返回到当前的形状


	for(i=0;i<4;i++)
		for(j=0;j<4;j++)
			temp[j][i]=box_shape[i][j];    //斜对角线对称
	i=size_h;
	size_h=size_w;
	size_w=i;
	for(i=0;i<size_h;i++)
		for(j=0;j<size_w;j++)
			box_shape[i][size_w-1-j]=temp[i][j];  //左右对称

    /*如果旋转以后重合，则返回到备份的数组形状*/
	if(isaggin()){
		for(int i=0; i<4;i++)
			for(int j=0;j<4;j++)
				box_shape[i][j]=temp_piece[i][j];
		size_w=tmp_size_w;
		size_h=tmp_size_h;
	}
        /*如果旋转成功，那么在屏幕上进行显示*/
	else{
		for(int i=0; i<4;i++)
			for(int j=0;j<4;j++){
				if(temp_piece[i][j]==1){
					mvwaddch(game_win,head_y+i,head_x+j,' ');  //移动到game_win窗口的某个坐标处打印字符
					wrefresh(game_win);
				}
			}
		for(int i=0; i<size_h;i++)
			for(int j=0;j<size_w;j++){
				if(this->box_shape[i][j]==1){
					mvwaddch(game_win,head_y+i,head_x+j,'#');  //移动到game_win窗口的某个坐标处打印字符
					wrefresh(game_win);
				}
		}

	}
}
/**
 *
 */
void Piece::move(){
	 fd_set set;
	 FD_ZERO(&set);
	 FD_SET(0, &set);
	      
	 struct timeval timeout;
	 timeout.tv_sec = 0;
	 timeout.tv_usec= 500000;
	  //设置等待按键时间
  	if (select(1, &set, NULL, NULL, &timeout) == 0){
          //这里程序流转到了什么功能，也就是说下面代码的作用是什么？
		head_y++;
		if(isaggin()){
			head_y--;
			for(int i=0;i<size_h;i++)
				for(int j=0;j<size_w;j++)
					if(box_shape[i][j]==1)
						box_map[head_y+i][head_x+j]=1;
			score_next();
		}
		else{
			for(int i=size_h-1; i>=0;i--)
				for(int j=0;j<size_w;j++){
					if(this->box_shape[i][j]==1){
						mvwaddch(game_win,head_y-1+i,head_x+j,' ');//移动到game_win窗口的某个坐标处打印字符
						mvwaddch(game_win,head_y+i,head_x+j,'#');

					}
				}
			wrefresh(game_win);
		}

  	}
      //得到输入的 key 值，通过判断不同的 key 值进行向左、右、下、旋转等操作。
	if (FD_ISSET(0, &set)) {
	        while ((key = getch()) == -1) ;

		if(key==KEY_LEFT){
			head_x--;
			if(isaggin())
				head_x++; //undo
			else{
				for(int i=0; i<size_h;i++)
					for(int j=0;j<size_w;j++){
						if(this->box_shape[i][j]==1){
							mvwaddch(game_win,head_y+i,head_x+j+1,' ');
							mvwaddch(game_win,head_y+i,head_x+j,'#');

						}
					}
				wrefresh(game_win);
			}
		}

		if(key==KEY_RIGHT){
			head_x++;
			if(isaggin())
				head_x--;
			else{
				for(int i=0; i<size_h;i++)
					for(int j=size_w-1;j>=0;j--){
						if(this->box_shape[i][j]==1){
							mvwaddch(game_win,head_y+i,head_x+j-1,' ');
							mvwaddch(game_win,head_y+i,head_x+j,'#');

						}
					}
				wrefresh(game_win);
			}
		}

		if(key==KEY_DOWN){ //下按键
			head_y++;
			if(isaggin()){
				head_y--;
                //地图上对应的box设置为已被占用，用1表示，0表示未被占用
				for(int i=0;i<size_h;i++)
					for(int j=0;j<size_w;j++)
						if(box_shape[i][j]==1)
							box_map[head_y+i][head_x+j]=1;

				score_next();
				
			}//移动的机制是什么？改变数组元素的0，1值来实现方块的移动吗？
			else{
				for(int i=size_h-1; i>=0;i--)
					for(int j=0;j<size_w;j++){
						if(this->box_shape[i][j]==1){
							mvwaddch(game_win,head_y-1+i,head_x+j,' ');
							mvwaddch(game_win,head_y+i,head_x+j,'#');

						}
					}
				wrefresh(game_win);
			}
		}

		if(key==KEY_UP)
			rotate();

		if(head_x+size_w+1>game_win_width)
			head_x=game_win_width-size_w-1;
		if(head_x<1)
			head_x=1;
	}
}
//判断重复函数，每次移动或旋转之后要进行判断的函数，函数返回真则不能行动，返回假则可以进行下一步。
/**
 * 首先长宽高遍历，对box_shape[i][j]==1也就是显示格子的元素进行以下判断：
 *          1.if(head_y+i > game_win_height-2) 方块长度超过窗体长度-2
 *          2.if(head_x+j > game_win_width-2 || head_x+i-1<0)  方块宽度超过窗体宽度，或者宽度小于1
 *          3.if(box_map[head_y+i][head_x+j]==1)  //与已占用的box重合
 * @return
 */
bool Piece::isaggin(){
	for(int i=0;i<size_h;i++)
		for(int j=0;j<size_w;j++){
			if(box_shape[i][j]==1){
				if(head_y+i > game_win_height-2)   //下面出界
					return true;
				if(head_x+j > game_win_width-2 || head_x+i-1<0)  //左右出界
					return true;
				if(box_map[head_y+i][head_x+j]==1)  //与已占用的box重合
					return true ;
			}
		}
	return false;
}
//判断当前行是否空
bool Piece::exsqr(int row){
	for(int j=1;j<game_win_width-1;j++)
		if(box_map[row][j]==1)
			return true;
	return false;
}
//层满函数：对方块已满的行进行消除，每当一个方块向下移动停止后都需要进行判断
void Piece::judge(){
	int i,j;
	int line=0; //层数
	bool full;
	for(i=1;i<game_win_height-1;i++){
		full=true;
		for(j=1;j<game_win_width-1;j++){
			if(box_map[i][j]==0)  //存在未被占用的box
				full=false;  //说明本层未满
		}
		if(full){ //如果层满
			line++; //行满+1
			score+=50; //分数加50
			for(j=1;j<game_win_width-1;j++)
				box_map[i][j]=0;  //把该层清空(标记为未被占用)
		}
	}

	//行高遍历
    /*上面判断完后 看line的值,如果非 0 说明有层已满需要进行消除*/
	if(line!=0){
	for(i=game_win_height-2;i>=2;i--){
		int s=i;
		if(exsqr(i)==0){
			while(s>1 && exsqr(--s)==0);
			for(j=1;j<game_win_width-1;j++){
				box_map[i][j]=box_map[s][j]; //上层下移
				box_map[s][j]=0; //上层清空
			}
		}
	}
    /*清空和移动标记完成以后就要屏幕刷新了，重新打印game_win*/
	for(int i=1;i<game_win_height-1;i++)
			for(int j=1;j<game_win_width-1;j++){
				if(box_map[i][j]==1){
					mvwaddch(game_win,i,j,'#'); //移动到game_win窗口的某个坐标处打印字符
					wrefresh(game_win);
				}
				else{
					mvwaddch(game_win,i,j,' ');
					wrefresh(game_win);
				}
			}
	}
}			

void Piece::score_next(){
	score+=10;
	judge();

	mvwprintw(score_win, hint_win_height/2,hint_win_width/2-2,"%d",score);
	wrefresh(score_win);


	set_shape(next_shape,box_shape,size_w,size_h);

	this->next_shape=getrand(0,6);
	set_shape(next_shape,next_box_shape,next_size_w,next_size_h);


	for(int i =1;i<hint_win_height-1;i++)
		for(int j=1;j<hint_win_width-1;j++){
			mvwaddch(hint_win, i, j,' ');
			wrefresh(hint_win);
		}
	for(int i =0;i<4;i++)
		for(int j=0;j<4;j++)
			if(next_box_shape[i][j]==1){
				mvwaddch(hint_win,(hint_win_height-size_h)/2+i,(hint_win_width-size_w)/2+j,'#');
				wrefresh(hint_win);
		}
}
