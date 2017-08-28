/*
@author Tirthankar Sarkar
*/
#include<graphics.h>
#include<conio.h>
#include<stdio.h>
#include<dos.h>
#include<stdlib.h>
#undef abs
#include<math.h>

/* Symbolic Constants for Line Status. */
#define LINE_DRAWN 'l'
#define NO_LINE 'n'

/* Symbolic Constants for Box Status.*/
#define EMPTY 'e'
#define COMPLETE 'c'

/* Symbolic Constants for Line Alignment.*/
#define HORIZONTAL 'h'
#define VERTICAL 'v'

/* Symbolic Constants for Keys.*/
#define LEFT 75
#define RIGHT 77
#define UP 72
#define DOWN 80
#define ENTER 28
#define ESCAPE 1

/* Symbolic Constants for Enter Status for return key.*/
#define EVEN 0
#define ODD 1

/* Symbolic Constants for Game Status.*/
#define WIN 1
#define NO_RESULT 0
#define TIE -1

/* Symbolic Constants for Flags used.*/
#define OFF 0
#define ON 1

/*       For a 5x8 grid    */
char HORIZ_LINE[5][7];
char VERT_LINE[4][8];
char BOX[4][7];

int SCORE_1,SCORE_2;

/* ...............................Function Prototypes......................*/
void Initiate_Game(void);
void Play_Area(void);
int getkey(void);
void Draw_Vertices(void);
int Check_For_Box(int,int,int,char);
int Check_Win(void);
void End_Game(void);
void SHOW_CURRENT_POS(int,int,int);
void HIDE_PREV_POS(int,int);
void Highlight_Selected_Vertex(int,int,int);
void Draw_Line(int,int,int,int,int);

int main()
{
   int gd=DETECT,gm;
   int i,j;
   initgraph(&gd,&gm,"C:\\TurboC3\\BGI");
   //Initialize all boxes as empty...
   for(i=0;i<4;++i)
     for(j=0;j<7;++j)
	 BOX[i][j]=EMPTY;
   //Initialize all Horizontal lines as not drawn...
   for(i=0;i<5;++i)
     for(j=0;j<7;++j)
	 HORIZ_LINE[i][j]=NO_LINE;
   //Initialize all Vertical lines as not drawn...
   for(i=0;i<4;++i)
     for(j=0;j<8;++j)
	 VERT_LINE[i][j]=NO_LINE;
   //Initialize Game...
   Initiate_Game();
   return 0;
}


int getkey(void)
{
    union REGS i,o;
    while(!kbhit());
    i.h.ah=0;
    int86(22,&i,&o);
    return(o.h.ah);
}

void Draw_Vertices(void)
{
   setfillstyle(SOLID_FILL,LIGHTGRAY);
   for(int i=0;i<5;++i)
      for(int j=0;j<8;++j)
	  fillellipse(100+50*j,100+50*i,5,5);
}

void Initiate_Game(void)
{
    Draw_Vertices();
    Play_Area();
    char LINE_ALIGN;
    int flag,stat,scancode,Enter_Stat,Game_Stat,PLAYER,IS_BOX,Player_Color;
    int X1=100,Y1=100,X2,Y2,CURRENT_X,CURRENT_Y;
    int Diff_X,Diff_Y,dx,dy;
    int small_X,small_Y,i,j;
    CURRENT_X=X1;
    CURRENT_Y=Y1;
    PLAYER=1;
    SCORE_1=SCORE_2=0;
    Enter_Stat=EVEN;
    Game_Stat=NO_RESULT;
    while(Game_Stat==NO_RESULT)
    {
	if(PLAYER==1) Player_Color=GREEN;
	else Player_Color=BROWN;

	SHOW_CURRENT_POS(CURRENT_X,CURRENT_Y,Player_Color);
	setcolor(WHITE);

	scancode=getkey();
	switch(scancode)
	{
	    case UP: if(CURRENT_Y!=100) CURRENT_Y-=50;
		       else CURRENT_Y=300;
		     break;

	    case DOWN: if(CURRENT_Y!=300) CURRENT_Y+=50;
			 else CURRENT_Y=100;
		       break;

	    case RIGHT: if(CURRENT_X!=450) CURRENT_X+=50;
			  else CURRENT_X=100;
			break;

	    case LEFT:  if(CURRENT_X!=100) CURRENT_X-=50;
			  else CURRENT_X=450;
			break;

	    case ENTER: if(Enter_Stat==EVEN) Enter_Stat=ODD;
			  else if(Enter_Stat==ODD) Enter_Stat=EVEN;
			switch(Enter_Stat)
			{
			    case ODD: X1=CURRENT_X;
				      Y1=CURRENT_Y;
				      Highlight_Selected_Vertex(X1,Y1,Player_Color);
				      break;

			    case EVEN: X2=CURRENT_X;
				       Y2=CURRENT_Y;

				       dx=X1-X2;
				       dy=Y1-Y2;
				       //Absolute difference of coordinates
				       Diff_X=abs(dx);
				       Diff_Y=abs(dy);

				       //Turn flag off which checks whether
				       //line chosen is already drawn or not.
				       flag=OFF;
				       //Another flag to check whether box
				       //is completed or not.If yes,then
				       //don't swap turns.
				       IS_BOX=OFF;

				       if(Diff_X<=50 && Diff_Y<=50 &&
					      Diff_X!=Diff_Y)
				       {
					  /*determine whether Horizontal or
					    vertical line was drawn*/
					  if(Diff_X!=0)
					  {
					      //Horizontal line case.
					      if(X1<X2) small_X=X1;
					      else small_X=X2;
					      j= (small_X-100)/50;
					      i= (Y2-100)/50;
					      if(HORIZ_LINE[i][j]==NO_LINE)
					      {
						 Highlight_Selected_Vertex(X2,Y2,Player_Color);
						 HORIZ_LINE[i][j]=LINE_DRAWN;
						 Draw_Line(X1,Y1,X2,Y2,Player_Color);
						 LINE_ALIGN=HORIZONTAL;
						 if(X2>X1)
						 IS_BOX=Check_For_Box(X2,Y2,PLAYER,LINE_ALIGN);
						 else
						 IS_BOX=Check_For_Box(X1,Y2,PLAYER,LINE_ALIGN);
					      }
					      else
					      {
						 Enter_Stat=EVEN;
						 flag=ON;
					      }
					  }
					  else
					  {
					     //Vertical line drawn.
					     if(Y1<Y2) small_Y=Y1;
					     else small_Y=Y2;
					     i= (small_Y-100)/50;
					     j= (X2-100)/50;
					     if(VERT_LINE[i][j]==NO_LINE)
					     {
						Highlight_Selected_Vertex(X2,Y2,Player_Color);
						VERT_LINE[i][j]=LINE_DRAWN;
						Draw_Line(X1,Y1,X2,Y2,Player_Color);
						LINE_ALIGN=VERTICAL;
						if(Y2>Y1)
						IS_BOX=Check_For_Box(X2,Y2,PLAYER,LINE_ALIGN);
						else
						IS_BOX=Check_For_Box(X2,Y1,PLAYER,LINE_ALIGN);
					     }
					     else
					     {
						Enter_Stat=EVEN;
						flag=ON;
					     }
					  }
					  //Check for WIN
					  Game_Stat=Check_Win();
					  //if game isn't over then
					  //Switch Player Turn...
					  if(Game_Stat!=WIN || Game_Stat!=TIE)
					    if(!flag && !IS_BOX)
					    {
					      if(PLAYER==1) PLAYER=2;
						else if(PLAYER==2) PLAYER=1;
					    }
				       }
				       else Enter_Stat=ODD;
				       break;
			}
			break;

	    case ESCAPE: End_Game();
			 exit(0);
			 break;
	}
    }
    End_Game();
}

int Check_For_Box(int X,int Y,int PL,char L_A)
{
     int x,y,i,j,flag=OFF,Is_Box=OFF;

     if(L_A==HORIZONTAL)// apply algos ULD & DLU
     {
	  //---------------------ALGO__ULD-----------------------
		  y=Y-50;
		  i=(Y-100)/50;
		  j=(X-100)/50;
		  if(y>=100)
		     if(VERT_LINE[i-1][j]==LINE_DRAWN)
		       flag=ON;

		  if(flag==ON)
		    if(HORIZ_LINE[i-1][j-1]!=LINE_DRAWN)
		       flag=OFF;

		  if(flag==ON)
		     if(VERT_LINE[i-1][j-1]!=LINE_DRAWN)
			 flag=OFF;

		  if(flag==ON)
		  {
		      if(BOX[i-1][j-1]==EMPTY)
		      {
			  BOX[i-1][j-1]=COMPLETE;
			  Is_Box=ON;
			  if(PL==1) SCORE_1++;
			  else SCORE_2++;
		      }
		  }
		  //--------------------ALGO__DLU--------------------------
		  y=Y+50;
		  flag=OFF;

		  if(y<=300)
		    if(VERT_LINE[i][j]==LINE_DRAWN)
		       flag=ON;

		  if(flag==ON)
		     if(HORIZ_LINE[i+1][j-1]!=LINE_DRAWN)
			 flag=OFF;

		  if(flag==ON)
		     if(VERT_LINE[i][j-1]!=LINE_DRAWN)
			  flag=OFF;

		  if(flag==ON)
		  {
		       if( BOX[i][j-1]==EMPTY )
		       {
			   BOX[i][j-1]=COMPLETE;
			   Is_Box=ON;
			   if(PL==1) SCORE_1++;
			   else SCORE_2++;
		       }
		  }

     }
     else{  // VERTICAL Line case... apply algos RUL & LUR

	    //---------------------ALGO__RUL-----------------
	    x=X+50;
	    i=(Y-100)/50;
	    j=(X-100)/50;
	    if(x<=450)
	    {
		if(HORIZ_LINE[i][j]==LINE_DRAWN)
		   flag=ON;

		if(flag==ON)
		   if(VERT_LINE[i-1][j+1]!=LINE_DRAWN)
		      flag=OFF;

		if(flag==ON)
		   if(HORIZ_LINE[i-1][j]!=LINE_DRAWN)
		      flag=OFF;

		if(flag==ON)
		  if(BOX[i-1][j]==EMPTY)
		  {
			BOX[i-1][j]=COMPLETE;
			Is_Box=ON;
			if(PL==1) SCORE_1++;
			else SCORE_2++;
		  }
	    }

	    // ----------------------------ALGO__LUR-----------------------
	    x=X-50;
	    flag=OFF;
	    if(x>=100)
	    {
		if(HORIZ_LINE[i][j-1]==LINE_DRAWN)
		   flag=ON;

		if(flag==ON)
		   if(VERT_LINE[i-1][j-1]!=LINE_DRAWN)
		      flag=OFF;

		if(flag==ON)
		  if(HORIZ_LINE[i-1][j-1]!=LINE_DRAWN)
		     flag=OFF;

		if(flag==ON)
		   if(BOX[i-1][j-1]==EMPTY)
		   {
		      BOX[i-1][j-1]=COMPLETE;
		      Is_Box=ON;
		      if(PL==1) SCORE_1++;
		      else SCORE_2++;
		   }
	    }
	 }
     return(Is_Box);
}

int Check_Win(void)
{
    int Boxes=0,i,j,Stat;

    Stat=NO_RESULT;

    //Count number of Completed Boxes...
    for(i=0;i<4;++i)
      for(j=0;j<7;++j)
	if(BOX[i][j]==COMPLETE)
	   Boxes++;

    //Check Game status as per number of completed boxes.
    if(Boxes==28 && SCORE_1!=SCORE_2)
       Stat=WIN;
    else if(Boxes==28 && SCORE_1==SCORE_2)
       Stat=TIE;

    return(Stat);
}

void End_Game(void)
{
     closegraph();
     printf("\n\n\t\tSCORE of PLAYER_1: %d",SCORE_1);
     printf("\n\n\t\tSCORE of PLAYER_2: %d",SCORE_2);
     if(SCORE_1>SCORE_2)
     printf("\n\n\t\tPLAYER 1 WINS!!!");
     else if(SCORE_1==SCORE_2)
     printf("\n\n\t\tMATCH TIED...");
     else printf("\n\n\t\tPLAYER_2 WINS!!!");
     printf("\n\tPRESS ANY KEY...");
     getch();
}

void HIDE_PREV_POS(int I,int J)
{
   setfillstyle(SOLID_FILL,LIGHTGRAY);
   fillellipse(100+50*J,100+50*I,5,5);
}

void SHOW_CURRENT_POS(int X,int Y,int CL)
{
    static int i=0,j=0;
    HIDE_PREV_POS(i,j);
    i=(Y-100)/50;
    j=(X-100)/50;
    setfillstyle(SOLID_FILL,CL);
    fillellipse(100+50*j,100+50*i,3,3);
}

void Highlight_Selected_Vertex(int x,int y,int cl)
{
    setcolor(cl);
    setfillstyle(SOLID_FILL,cl);
    fillellipse(x,y,6,6);
}

void Draw_Line(int x1,int y1,int x2,int y2,int cl)
{
     int i,j;
     setcolor(cl);
     line(x1,y1,x2,y2);
     setcolor(WHITE);
     i=(y1-100)/50;
     j=(x1-100)/50;
     HIDE_PREV_POS(i,j);
     i=(y2-100)/50;
     j=(x2-100)/50;
     HIDE_PREV_POS(i,j);
}


void Play_Area(void)
{
    /*------------------------- Developer's Box--------------------------*/
    setcolor(GREEN);
    rectangle(95,360,455,460);
    setcolor(LIGHTGRAY);
    outtextxy(135,380,"Game Coded, Designed & Developed by:");
    outtextxy(135,400,"------------------------------------");
    setcolor(DARKGRAY);
    settextstyle(GOTHIC_FONT,HORIZ_DIR,5);
    outtextxy(120,395,"Tirthankar Sarkar");

    /*-----------------------Game's Name at the top-----------------------*/
    setcolor(BROWN);
    outtextxy(75,30,"VERTEX RELOADED");

    /*-----------------------Game's Control Box----------------------------*/
    setcolor(BROWN);
    rectangle(480,95,638,200);

    setcolor(LIGHTGRAY);
    settextstyle(DEFAULT_FONT,HORIZ_DIR,1);
    outtextxy(530,105,"CONTROLS");
    outtextxy(530,115,"--------");
    outtextxy(485,130,"Arrow Keys:Movement");
    outtextxy(485,150,"Enter Key:Selection");
    outtextxy(485,170,"Escape Key:Quit");

    /*---------------------------Game Progress Box------------------------*/
    setcolor(GREEN);
    rectangle(480,220,638,320);

    setcolor(LIGHTGRAY);
    outtextxy(510,230,"Game Progress");
    outtextxy(510,240,"-------------");
    outtextxy(490,260,"Player 1:");
    //itoa(SCORE_1,SC1,10);
    outtextxy(490,280,"Player 2:");

    /*---------------------------Game Message/Status Box-------------------*/

    setcolor(BROWN);
    rectangle(480,360,638,460);

    setcolor(LIGHTGRAY);
    outtextxy(510,370,"Game Status");
    outtextxy(510,380,"-----------");
    setcolor(WHITE);
}
