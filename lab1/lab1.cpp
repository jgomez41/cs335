//Job Gomez
//CS335
//hw1
//cs335 Spring 2015 Lab-1
//This program demonstrates the use of OpenGL and XWindows
//Assignment is to modify this program.
//You will follow along with your instructor.
//Elements to be learned in this lab...
//. general animation framework
//. animation loop
//. object definition and movement
//. collision detection
//. mouse/keyboard interaction
//. object constructor
//. coding style
//. defined constants
//. use of static variables
//. dynamic memory allocation
//. simple opengl components
//. git
//
//elements we will add to program...
//. Game constructor
//. multiple particles
//. gravity
//. collision detection
//. more objects
//
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cmath>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
extern "C" {
#include "fonts.h"
}

#define WINDOW_WIDTH  1000
#define WINDOW_HEIGHT 800

#define MAX_PARTICLES 9999
#define GRAVITY 0.1

#define rnd() (double)rand() / (double)RAND_MAX
#define RADIUS 150 
//X Windows variables
Display *dpy;
Window win;
GLXContext glc;

//Structures

struct Vec {
	float x, y, z;
};

struct Shape {
	float width, height;
	float radius;
	Vec center;
};

struct Particle {
	Shape s;
	Vec velocity;
};

struct Game {
	Shape box;
	Shape box2;
	Shape box3;
	Shape box4;
	Shape box5;
	Shape circle;
	Particle particle[MAX_PARTICLES];
	int n;
	int Mousex;
	int Mousey;
};

//Function prototypes
void initXWindows(void);
void init_opengl(void);
void cleanupXWindows(void);
void check_mouse(XEvent *e, Game *game);
int check_keys(XEvent *e, Game *game);
void movement(Game *game);
void render(Game *game);
bool bubbler = false;

int main(void)
{
	int done=0;
	srand(time(NULL));
	initXWindows();
	init_opengl();
	//declare game object
	Game game;
	game.n=0;
	//declare a box shape
	//Gordon's Example
	game.box.width = 100;
	game.box.height = 10;
	game.box.center.x = 320;
	game.box.center.y = 400;
	
	//Box 2 (second to first)
	game.box2.width = 100;
	game.box2.height = 10;
	game.box2.center.x = 220;
	game.box2.center.y = 500;
	//Box 3 (first)
	game.box3.width = 100;
	game.box3.height = 10;
	game.box3.center.x = 120;
	game.box3.center.y = 600;
	
	//bottom box	
	game.box4.width = 100;
	game.box4.height = 10;
	game.box4.center.x = 420;
	game.box4.center.y = 300;
        //very bottom box
	game.box5.width = 100;
	game.box5.height = 10;
	game.box5.center.x = 520;
	game.box5.center.y = 200;
	
	//circle
	game.circle.radius = RADIUS; 
	game.circle.center.x = 685;
	game.circle.center.y = 1.0;	

	//start animation
	while(!done) {
		while(XPending(dpy)) {

			XEvent e;
			XNextEvent(dpy, &e);
			check_mouse(&e, &game);
			done = check_keys(&e, &game);
		}
		movement(&game);
		render(&game);
		glXSwapBuffers(dpy, win);
	}
	cleanupXWindows();
	return 0;
}

void set_title(void)
{
	//Set the window title bar.
	XMapWindow(dpy, win);
	XStoreName(dpy, win, "Waterfall Model");
}

void cleanupXWindows(void) {
	//do not change
	XDestroyWindow(dpy, win);
	XCloseDisplay(dpy);
}

void initXWindows(void) {
	//do not change
	GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
	int w=WINDOW_WIDTH, h=WINDOW_HEIGHT;
	dpy = XOpenDisplay(NULL);
	if (dpy == NULL) {
		std::cout << "\n\tcannot connect to X server\n" << std::endl;
		exit(EXIT_FAILURE);
	}
	Window root = DefaultRootWindow(dpy);
	XVisualInfo *vi = glXChooseVisual(dpy, 0, att);
	if(vi == NULL) {
		std::cout << "\n\tno appropriate visual found\n" << std::endl;
		exit(EXIT_FAILURE);
	} 
	Colormap cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);
	XSetWindowAttributes swa;
	swa.colormap = cmap;
	swa.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask |
							ButtonPress | ButtonReleaseMask |
							PointerMotionMask |
							StructureNotifyMask | SubstructureNotifyMask;
	win = XCreateWindow(dpy, root, 0, 0, w, h, 0, vi->depth,
					InputOutput, vi->visual, CWColormap | CWEventMask, &swa);
	set_title();
	glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
	glXMakeCurrent(dpy, win, glc);
}

void init_opengl(void)
{
	//OpenGL initialization
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	//Initialize matrices
	glMatrixMode(GL_PROJECTION); glLoadIdentity();
	glMatrixMode(GL_MODELVIEW); glLoadIdentity();
	//Set 2D mode (no perspective)
	glOrtho(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT, -1, 1);
	//Set the screen background color
	glClearColor(0.1, 0.1, 0.1, 1.0);
}

void makeParticle(Game *game, int x, int y) {
	if (game->n >= MAX_PARTICLES)
		return;
	//std::cout << "makeParticle() " << x << " " << y << std::endl;
	//position of particle
	Particle *p = &game->particle[game->n];
	p->s.center.x = x;
	p->s.center.y = y;
	p->velocity.y = rnd()-4.0;
	p->velocity.x = rnd()-1.0;
	game->n++;
}

void check_mouse(XEvent *e, Game *game)
{
	static int savex = 0;
	static int savey = 0;
	static int n = 0;
	
	if (e->type == ButtonRelease) {
		return;
	}
	if (e->type == ButtonPress) {
	    if (e->xbutton.button==1) {
		//Left button was pressed
		int y = WINDOW_HEIGHT - e->xbutton.y;
		for (int i = 0; i<10; i++)
		    makeParticle(game, e->xbutton.x, y);
		return;
	    }
	    if (e->xbutton.button==3) {
		//Right button was pressed
		return;
	    }
	}
	//Did the mouse move?
	if (savex != e->xbutton.x || savey != e->xbutton.y) {
	    savex = e->xbutton.x;
	    savey = e->xbutton.y;
	    if (++n < 10)
	    	return;
	    int y = WINDOW_HEIGHT - e->xbutton.y;
	    for (int i = 0; i<10; i++)	
		makeParticle(game, e->xbutton.x, y);
	}
}

int check_keys(XEvent *e, Game *game)
{
	//Was there input from the keyboard?
	if (e->type == KeyPress) {
	    int key = XLookupKeysym(&e->xkey, 0);
	    if (key == XK_Escape) {
		return 1;
	    }
            //You may check other keys here.
	    if(key == XK_b) 
	    	bubbler ^= 1;
	    		
	}
	return 0;
}

void movement(Game *game)
{
	Particle *p;

	if (game->n <= 0)
		return;


	for (int i = 0; i<game->n; i++) {
		p = &game->particle[i];
		p->s.center.x += p->velocity.x;
		p->s.center.y += p->velocity.y;
	
		p->velocity.y -= 0.5; //gravity
	
		//check for collision with shapes...
		Shape *s;
		s = &game->box;
		if(p->s.center.y >= s->center.y - (s->height) &&
		   p->s.center.y <= s->center.y + (s->height) &&
		   p->s.center.x >= s->center.x - (s->width) &&
		   p->s.center.x <= s->center.x + (s->width)) {
		   p->velocity.y *= rnd()* -0.5;
		   p->velocity.x += rnd()*0.05;
		}
		
		Shape *s2;
		s2 = &game->box2;
		if(p->s.center.y >= s2->center.y - (s2->height) &&
		   p->s.center.y <= s2->center.y + (s2->height) &&
		   p->s.center.x >= s2->center.x - (s2->width) &&
		   p->s.center.x <= s2->center.x + (s2->width)) {
		   p->velocity.y *= rnd()* -0.5;
		   p->velocity.x += rnd()*0.05;
		}	
		
		Shape *s3;
		s3 = &game->box3;
		if(p->s.center.y >= s3->center.y - (s3->height) &&
		   p->s.center.y <= s3->center.y + (s3->height) &&
		   p->s.center.x >= s3->center.x - (s3->width) &&
		   p->s.center.x <= s3->center.x + (s3->width)) {
		   p->velocity.y *= rnd()* -0.5;
		   p->velocity.x += rnd()*0.05;

		}
		
		Shape *s4;
		s4 = &game->box4;
		if(p->s.center.y >= s4->center.y - (s4->height) &&
		   p->s.center.y <= s4->center.y + (s4->height) &&
		   p->s.center.x >= s4->center.x - (s4->width) &&
		   p->s.center.x <= s4->center.x + (s4->width)) {
		   p->velocity.y *= rnd()* -0.5;
		   p->velocity.x += rnd()*0.05;

		}			
		
		Shape *s5;
		s5 = &game->box5;
		if(p->s.center.y >= s5->center.y - (s5->height) &&
		   p->s.center.y <= s5->center.y + (s5->height) &&
		   p->s.center.x >= s5->center.x - (s5->width) &&
		   p->s.center.x <= s5->center.x + (s5->width)) {
		   p->velocity.y *= rnd()* -0.5;
		   p->velocity.x += rnd()*0.05;

		}
		
		//Circle
			
		Shape *circle;
		circle = &game->circle;
		double xPoint, yPoint, xCircle, yCircle;

		if(p->s.center.x >= circle->center.x - RADIUS  &&
	           p->s.center.x <= circle->center.x + RADIUS  &&
		   p->s.center.y <  RADIUS + 3) {

		   
	    	   xPoint = p->s.center.x;
       		   yPoint = p->s.center.y;
		   xCircle = circle->center.x;
		   yCircle = circle->center.y;

		   double distance = sqrt(pow(xPoint - xCircle, 2) + pow(yPoint - yCircle, 2));
		   if(distance - RADIUS <= 0) {
    		   	if(p->s.center.x < circle->center.x - 5) {
			    p->velocity.x = sin(-(1.0 + rnd() * 0.1));
			}
			else 
			    p->velocity.y = cos(rnd() * 0.1);
	 	        p->velocity.y *= -0.1;
		        p->s.center.y += 2;
			
			
		   }
		}
		
		//check for off-screen
		if ((p->s.center.y < 0.0) || (p->s.center.y > WINDOW_HEIGHT)) {
			memcpy(&game->particle[i], &game->particle[game->n -1],	sizeof(Particle));
		    	game->n--;
		}
	}
}

void render(Game *game)
{
	float w, h;
	glClear(GL_COLOR_BUFFER_BIT);
	//Draw shapes...
	//Draw Circle
	
	Shape *circle;
	glColor3ub(90, 140, 90);
	circle = &game->circle;
	glPushMatrix();
	glTranslatef(circle->center.x, circle->center.y, circle->center.z);
	double x1, x2, y1, y2;
	x1 = 0.0;
	y1 = 0.0;
	
	glBegin(GL_TRIANGLE_FAN);
	glVertex2i(x1, y1);
	double pi = 3.14159265;
		for(double i = 0.0; i <= 360.0; i += 1.0) {
		    x2 = x1 + sin(i * (pi/100)) * RADIUS;
		    y2 = y1 + cos(i * (pi/100)) * RADIUS;	
		    glVertex2i(x2, y2);
		}
	glEnd();
	glPopMatrix();
	
	//draw box
	//Gordon's box
	Shape *s;
	glColor3ub(90,140,90);
	s = &game->box;
	glPushMatrix();
	glTranslatef(s->center.x, s->center.y, s->center.z);
	w = s->width;
	h = s->height;
	glBegin(GL_QUADS);
		glVertex2i(-w,-h);
		glVertex2i(-w, h);
		glVertex2i( w, h);
		glVertex2i( w,-h);
	glEnd();
	glPopMatrix();
	
	//Second box
	Shape *s2;
	glColor3ub(90,140,90);
	s2 = &game->box2;
	glPushMatrix();
	glTranslatef(s2->center.x, s2->center.y, s2->center.z);
	w = s2->width;
	h = s2->height;
	glBegin(GL_QUADS);
		glVertex2i(-w,-h);
		glVertex2i(-w, h);
		glVertex2i( w, h);
		glVertex2i( w,-h);
	glEnd();
	glPopMatrix();
	
	//First box
	Shape *s3;
	glColor3ub(90,140,90);
	s3 = &game->box3;
	glPushMatrix();
	glTranslatef(s3->center.x, s3->center.y, s3->center.z);
	w = s3->width;
	h = s3->height;
	glBegin(GL_QUADS);
		glVertex2i(-w,-h);
		glVertex2i(-w, h);
		glVertex2i( w, h);
		glVertex2i( w,-h);
	glEnd();
	glPopMatrix();
	//bottom box
	Shape *s4;
	glColor3ub(90,140,90);
	s4 = &game->box4;
	glPushMatrix();
	glTranslatef(s4->center.x, s4->center.y, s4->center.z);
	w = s4->width;
	h = s4->height;
	glBegin(GL_QUADS);
		glVertex2i(-w,-h);
		glVertex2i(-w, h);
		glVertex2i( w, h);
		glVertex2i( w,-h);
	glEnd();
	glPopMatrix();
	//Very bottom box
	Shape *s5;
	glColor3ub(90,140,90);
	s5 = &game->box5;
	glPushMatrix();
	glTranslatef(s5->center.x, s5->center.y, s5->center.z);
	w = s5->width;
	h = s5->height;
	glBegin(GL_QUADS);
		glVertex2i(-w,-h);
		glVertex2i(-w, h);
		glVertex2i( w, h);
		glVertex2i( w,-h);
	glEnd();
	glPopMatrix();
	

	glPushMatrix();
	glColor3ub(150,160,220);

	int randColorWater = 0;
	for(int i = 0; i < game->n; i++){

		if(randColorWater == 100)
			randColorWater = 0;
		//glColor3ub(150 ,160 ,255);// BLUE water
		glColor3ub(0+randColorWater ,0+randColorWater ,255);// looks best 
		Vec *c = &game->particle[i].s.center;
		w = 2;
		h = 2;
		glBegin(GL_QUADS);
		glVertex2i(c->x-w, c->y-h);
		glVertex2i(c->x-w, c->y+h);
		glVertex2i(c->x+w, c->y+h);
		glVertex2i(c->x+w, c->y-h);
		glEnd();
		glPopMatrix();	 
		randColorWater+= 10; 
	}
	Rect r;
	glEnable(GL_TEXTURE_2D);

    	r.bot = 585;
    	r.left = 100;
    	ggprint8b(&r, 16, 0x00FFFF00, "Press B for bubbler");

	r.bot = 490;
	r.left = 100;
	r.center = 1;
	ggprint16(&r, 16, 0x00FFFF00, "Requirements");
	r.bot = 428;
	r.left = 150;
	ggprint16(&r, 16, 0x00FFFF00, "Design");
	
	r.bot-= 45;
	r.left +=70;
	ggprint16(&r, 16, 0x00FFFF00, "Coding");

	r.bot -= 45;
	r.left += 50;
	ggprint16(&r, 16, 0x00FFFF00, "Testing");

	r.bot -= 40;
	r.left +=70;
	ggprint16(&r, 16, 0x00FFFF00, "Maintenance");
	
	r.bot = 200;
	r.left = 120;
	ggprint16(&r, 16, 0x00FFFF00, "The Waterfall Model");	
	
	if (bubbler) {
	    for (int i =0; i<10; i++)
		makeParticle(game, 175, 750);
	} 
	
}


