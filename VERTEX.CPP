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

/* Symbolic Constants for checking box algorithms.*/
#define ULD 1
#define DLU 2
#define RUL 3
#define LUR 4

/*       For a 5x8 grid    */
char HORIZ_LINE[5][7];
char VERT_LINE[4][8];
char BOX[4][7];

int SCORE_1,SCORE_2,ODD_STAT_X=-100,ODD_STAT_Y=-100;

/* ...............................Function Prototypes......................*/
void Initiate_Game(void);
void Play_Area(void);
int getkey(void);
void Draw_Vertices(void);
int Check_For_Box(int,int,int,char);
int Check_Win(void);
void End_Game(void);
void SHOW_CURRENT_POS(int,int,int,int);
void HIDE_PREV_POS(int,int);
void Highlight_Selected_Vertex(int,int,int);
void Draw_Line(int,int,int,int,int);
void Show_Score(void);
void Whose_Turn(int);
void Show_Box(int,int,int,int,int);
void Box_Sound(int);
int Check_Vertex_For_X1_Y1(int,int);

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
    int Check_X1_Y1;
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

	SHOW_CURRENT_POS(CURRENT_X,CURRENT_Y,Player_Color,Check_X1_Y1);
	setcolor(WHITE);

	Show_Score();
	Whose_Turn(PLAYER);

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
			    case ODD: X1=ODD_STAT_X=CURRENT_X;
				      Y1=ODD_STAT_Y=CURRENT_Y;
				      Check_X1_Y1=Check_Vertex_For_X1_Y1(X1,Y1);
				      if(Check_X1_Y1==OFF)
				      {
					 Highlight_Selected_Vertex(X1,Y1,Player_Color);
					 sound(800);
					 delay(50);
					 nosound();
				      }
				      else Enter_Stat=EVEN;
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
					  sound(800);
					  delay(50);
					  nosound();
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
				       else if(Diff_X==0 && Diff_Y==0)
					    {
						Enter_Stat=EVEN;
						Check_X1_Y1=ON;
						sound(800);
						delay(50);
						nosound();
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
     int x,y,i,j,flag=OFF,Is_Box=OFF,ALGO;

     if(L_A==HORIZONTAL)// apply algos ULD & DLU
     {
	  //---------------------ALGO__ULD-----------------------
		  ALGO=ULD;
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
			  Show_Box(i,j,PL,L_A,ALGO);
			  Box_Sound(PL);
		      }
		  }
		  //--------------------ALGO__DLU--------------------------
		  ALGO=DLU;
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
			   Show_Box(i,j,PL,L_A,ALGO);
			   Box_Sound(PL);
		       }
		  }

     }
     else{  // VERTICAL Line case... apply algos RUL & LUR

	    //---------------------ALGO__RUL-----------------
	    ALGO=RUL;
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
			Show_Box(i,j,PL,L_A,ALGO);
			Box_Sound(PL);
		  }
	    }

	    // ----------------------------ALGO__LUR-----------------------
	    ALGO=LUR;
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
		      Show_Box(i,j,PL,L_A,ALGO);
		      Box_Sound(PL);
		   }
	    }
	 }
     return(Is_Box);
}

int Check_Vertex_For_X1_Y1(int X1,int Y1)
{
    int i,j,VERTEX_STAT=OFF;
    i=(Y1-100)/50;
    j=(X1-100)/50;

    // Check for Upper VERTICAL Line.
    if((i-1)>=0)
    {
	 if(VERT_LINE[i-1][j]==LINE_DRAWN)
	   VERTEX_STAT=ON;
    }
    else VERTEX_STAT=ON;//This else clause removes i=0 rows' vertex problem.

    // Check for Lower VERTICAL Line.
    if((i+1)<=4 && VERTEX_STAT==ON)
      if(VERT_LINE[i][j]==NO_LINE)
	VERTEX_STAT=OFF;

    // Check for Left HORIZONTAL Line.
    if((j-1)>=0 && VERTEX_STAT==ON)
      if(HORIZ_LINE[i][j-1]==NO_LINE)
	VERTEX_STAT=OFF;

    // Check for Right HORIZONTAL Line.
    if((j+1)<=7 && VERTEX_STAT==ON)
      if(HORIZ_LINE[i][j]==NO_LINE)
	VERTEX_STAT=OFF;

    return(VERTEX_STAT);
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
     textcolor(GREEN);
     printf("\n\n\n\n\n\t\t\t  ");
     cprintf("Score of PLAYER_1: %d",SCORE_1);
     textcolor(BROWN);
     printf("\n\n\t\t\t  ");
     cprintf("Score of PLAYER_2: %d",SCORE_2);
     printf("\n\n\n\n\t\t\t  ");
     if(SCORE_1>SCORE_2)
     {
	textcolor(GREEN);
	cprintf("PLAYER 1 WINS!!!");
     }
     else if(SCORE_1==SCORE_2)
     {
	textcolor(YELLOW);
	cprintf("MATCH TIED...");
     }
     else {
	      textcolor(BROWN);
	      cprintf("PLAYER_2 WINS!!!");
	  }
     _setcursortype(_NOCURSOR);
     printf("\n\n\n\n\t\t\t  PRESS ANY KEY...");
     getch();
}

void HIDE_PREV_POS(int I,int J)
{
   setfillstyle(SOLID_FILL,LIGHTGRAY);
   fillellipse(100+50*J,100+50*I,5,5);
}

void SHOW_CURRENT_POS(int X,int Y,int CL,int CHK_X1_Y1)
{
    static int i=0,j=0;

    //Hide Previous position.
      HIDE_PREV_POS(i,j);

    //Redraw selected/highlighted first coordinate if erased & if valid.
    if( (100+50*j)==ODD_STAT_X && (100+50*i)==ODD_STAT_Y && CHK_X1_Y1==OFF)
    {
	  setfillstyle(SOLID_FILL,CL);
	  fillellipse(ODD_STAT_X,ODD_STAT_Y,5,5);
	  setcolor(WHITE);
    }

    //Show the current position/coordinates.
    i=(Y-100)/50;
    j=(X-100)/50;
    setfillstyle(SOLID_FILL,CL);
    fillellipse(100+50*j,100+50*i,5,5);
}

void Highlight_Selected_Vertex(int x,int y,int cl)
{
    setcolor(cl);
    setfillstyle(SOLID_FILL,cl);
    fillellipse(x,y,5,5);
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
    outtextxy(75,30,"VORTEX");
    setcolor(GREEN);
    outtextxy(265,30,"RELOADED");

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
    setcolor(GREEN);
    outtextxy(490,260,"Player 1:");
    setcolor(BROWN);
    outtextxy(490,280,"Player 2:");

    /*---------------------------Game Message/Status Box-------------------*/
    rectangle(480,360,638,460);

    setcolor(LIGHTGRAY);
    outtextxy(510,370,"Game Status");
    outtextxy(510,380,"-----------");
    setcolor(WHITE);
}

void Show_Score(void)
{
     char*s1,*s2;
     itoa(SCORE_1,s1,10);
     itoa(SCORE_2,s2,10);
     setfillstyle(SOLID_FILL,BLACK);
     bar(563,258,630,290);
     settextstyle(DEFAULT_FONT,HORIZ_DIR,1);
     setcolor(DARKGRAY);
     outtextxy(565,260,s1);
     outtextxy(585,260,"boxes.");
     outtextxy(565,280,s2);
     outtextxy(585,280,"boxes.");
}

void Whose_Turn(int P)
{
     setfillstyle(SOLID_FILL,BLACK);
     bar(484,398,635,430);
     if(P==1)
     {
	setcolor(GREEN);
	outtextxy(495,400,"Player 1's turn.");
     }
     else {
	      setcolor(BROWN);
	      outtextxy(495,400,"Player 2's turn.");
	  }
    if(SCORE_1>SCORE_2)
    {
	 setcolor(GREEN);
	 outtextxy(484,420,"Player 1 is leading");
    }
    else if(SCORE_1==SCORE_2)
    {
	 setcolor(DARKGRAY);
	 outtextxy(484,420,"Heading towards tie");
    }
    else
    {
	 setcolor(BROWN);
	 outtextxy(484,420,"Player 2 is leading");
    }
    setcolor(WHITE);
}

void Show_Box(int I,int J,int Player,int L_A,int algo)
{
    int x,y,Color;
    x=100+50*J;
    y=100+50*I;
    if(Player==1) Color=GREEN;
    else Color=BROWN;

    settextstyle(GOTHIC_FONT,HORIZ_DIR,3);
    setcolor(Color);

    if(L_A==HORIZONTAL)
    {
       if(algo==ULD)
       {
	  x-=30;
	  y-=30;
       }
       else if(algo==DLU)
       {
	  x-=30;
	  y+=20;
       }
    }
    if(L_A==VERTICAL)
    {
       if(algo==RUL)
       {
	  x+=20;
	  y-=30;
       }
       else if(algo==LUR)
       {
	  x-=30;
	  y-=30;
       }
    }
    if(Player==1)
      outtextxy(x,y,"1");
    else outtextxy(x,y,"2");
}

void Box_Sound(int PL)
{
    int i;
    if(PL==1)
	 for(i=200;i<=1000;i=i+20)
	 {
	     sound(i);
	     delay(25);
	 }

    else for(i=1000;i>=200;i=i-20)
	 {
	     sound(i);
	     delay(25);
	 }
    nosound();
}
