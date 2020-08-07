#include <iostream>
#include <vector>
#include <string.h>
#include <fstream>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdlib.h>
#include <assert.h>

#include "triangulate.h"

#define ACTION 1
#define POLYGON 2
#define LINE_COLOR 3
#define FILL_COLOR 4
#define CLIPPING 5
#define EXTRUDE 4

#define WHITE 1
#define BLACK 2
#define RED 3
#define BLUE_GREEN 4
#define PURPLE 5
#define BLUE 6
#define GREEN 7
#define ORANGE 8
#define VIOLET 9
#define DARK_GRAY 10
#define BRONZE 11
#define BABY_BLUE 12
#define CYAN 13
#define LILAC 14
#define DARK_BLUE 15
#define BROWN 16

using namespace std;

/**********prototypes & global variables *********/
int polygCount = 0;
int clipIn=0;
int mainWindow;
int  mouse_flag;
int key_flag = 0;
int i,j,numberOfLists=0; 
int action_menu, main_menu, line_color_menu, fill_color_menu;
int actionFlag=0;//flag for if statement to create polygon
int lineColorFlag=0;
int fillColorFlag=0;
int menuFlag=0; //menu status
GLsizei ysize=499;
bool status =false;
//int countPosition = 1;
int position =0;
int counter=1; // the number of every vertice of the clipper window
int pointsChosen=0;
int color_counter=0;
struct rgb
{
  float r;
  float g;
  float b;
};

struct rgb color, colorLine;

int th = 0; // these two variables change when the user press the special keys (they are for rotation the 3dshape)
int ph = 0;



std:: vector<struct rgb > colorMap;
std:: vector<struct rgb > colorMapPol;

Vector2dVector vertices; // vector with vertices of polygon
std:: vector<Vector2dVector > vert_shapes; // vector with all polygons
std:: vector<Vector2dVector > vert_triangl; // vector with the triangles of every polygon
int clipper_size = 4; //size of clipper window
int clipper_points[4][2]; //this is the clipper window
float extrudeLength; // this is the length  of extrusion that user give

void display();
void processMainMenu(int option);
void processActionMenu(int option);
void processLineColorMenu(int option);
void processFillColorMenu(int option);
void createPopupMenus();
void processMenuStatus(int status, int x, int y);
void mouse(int button,int state,int x, int y);
void keyboard(unsigned char key, int x, int y);
void makePolygon(int x,int y);
bool linesIntersect(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4);
bool clockwise(int x1, int y1, int x2, int y2, int x3, int y3);
int intesect_point_x(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4);
int intesect_point_y(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4);
void clipping(Vector2dVector &polygon, int x1, int y1, int x2, int y2);// make the new points whith intersect with the clipper
void hodg_suth_algorithm(vector<Vector2dVector >  &polygon, int clipper_points[][2], int clipper_size);
void clip_window(int x, int y);

void windowSpecial(int key,int x,int y);

/*************************************************/
/*********TRIANGILUATION****************/

static const float EPSILON=0.0000000001f;

float Triangulate::Area(const Vector2dVector &contour)
{

  int n = contour.size();

  float A=0.0f;

  for(int p=n-1,q=0; q<n; p=q++)
  {
    A+= contour[p].GetX()*contour[q].GetY() - contour[q].GetX()*contour[p].GetY();
  }
  return A*0.5f;
}

   /*
     InsideTriangle decides if a point P is Inside of the triangle
     defined by A, B, C.
   */
bool Triangulate::InsideTriangle(float Ax, float Ay,
                      float Bx, float By,
                      float Cx, float Cy,
                      float Px, float Py)

{
  float ax, ay, bx, by, cx, cy, apx, apy, bpx, bpy, cpx, cpy;
  float cCROSSap, bCROSScp, aCROSSbp;

  ax = Cx - Bx;  ay = Cy - By;
  bx = Ax - Cx;  by = Ay - Cy;
  cx = Bx - Ax;  cy = By - Ay;
  apx= Px - Ax;  apy= Py - Ay;
  bpx= Px - Bx;  bpy= Py - By;
  cpx= Px - Cx;  cpy= Py - Cy;

  aCROSSbp = ax*bpy - ay*bpx;
  cCROSSap = cx*apy - cy*apx;
  bCROSScp = bx*cpy - by*cpx;

  return ((aCROSSbp >= 0.0f) && (bCROSScp >= 0.0f) && (cCROSSap >= 0.0f));
};

bool Triangulate::Snip(const Vector2dVector &contour,int u,int v,int w,int n,int *V)
{
  int p;
  float Ax, Ay, Bx, By, Cx, Cy, Px, Py;

  Ax = contour[V[u]].GetX();
  Ay = contour[V[u]].GetY();

  Bx = contour[V[v]].GetX();
  By = contour[V[v]].GetY();

  Cx = contour[V[w]].GetX();
  Cy = contour[V[w]].GetY();

  if ( EPSILON > (((Bx-Ax)*(Cy-Ay)) - ((By-Ay)*(Cx-Ax))) ) return false;

  for (p=0;p<n;p++)
  {
    if( (p == u) || (p == v) || (p == w) ) continue;
    Px = contour[V[p]].GetX();
    Py = contour[V[p]].GetY();
    if (InsideTriangle(Ax,Ay,Bx,By,Cx,Cy,Px,Py)) return false;
  }

  return true;
}

bool Triangulate::Process(const Vector2dVector &contour,Vector2dVector &result)
{
  /* allocate and initialize list of Vertices in polygon */

  int n = contour.size();
  if ( n < 3 ) return false;

  int *V = new int[n];

  /* we want a counter-clockwise polygon in V */

  if ( 0.0f < Area(contour) )
    for (int v=0; v<n; v++) V[v] = v;
  else
    for(int v=0; v<n; v++) V[v] = (n-1)-v;

  int nv = n;

  /*  remove nv-2 Vertices, creating 1 triangle every time */
  int count = 2*nv;   /* error detection */

  for(int m=0, v=nv-1; nv>2; )
  {
    /* if we loop, it is probably a non-simple polygon */
    if (0 >= (count--))
    {
      //** Triangulate: ERROR - probable bad polygon!
      return false;
    }

    /* three consecutive vertices in current polygon, <u,v,w> */
    int u = v  ; if (nv <= u) u = 0;     /* previous */
    v = u+1; if (nv <= v) v = 0;     /* new v    */
    int w = v+1; if (nv <= w) w = 0;     /* next     */

    if ( Snip(contour,u,v,w,nv,V) )
    {
      int a,b,c,s,t;

      /* true names of the vertices */
      a = V[u]; b = V[v]; c = V[w];

      /* output Triangle */
      result.push_back( contour[a] );
      result.push_back( contour[b] );
      result.push_back( contour[c] );

      m++;

      /* remove v from remaining polygon */
      for(s=v,t=v+1;t<nv;s++,t++) V[s] = V[t]; nv--;

      /* resest error detection counter */
      count = 2*nv;
    }
  }



  delete V;

  return true;
}
/*********MAIN()****************/
int main(int argc, char *argv[])
{
    
     // init GLUT and create window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
    glutInitWindowSize (600, 500); // Set the width and height of the window  
    glutInitWindowPosition (600, 250); 
    glutCreateWindow("Polygons Editor");
    createPopupMenus();


    glutDisplayFunc(display);       // Register callback handler for window re-paint event
    glutMouseFunc(mouse);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(windowSpecial);
    
    
    glutMainLoop();
    

    return 0;
}



/*************display*********/

void display() {
   // Set "clearing" or background color
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  glOrtho(0, 600, 0, 500, -500, 500);
   glClearColor (1.0, 1.0, 1.0, 1.0);  /* make the background white */
           // Clear the color buffer (background)
    glClear(GL_COLOR_BUFFER_BIT |GL_DEPTH_BUFFER_BIT); 
  

   if(mouse_flag==1){ // Print the polygon

       

        for(int i=0; i<vert_shapes.size();i++){

        
            Vector2dVector temp_v;
            Triangulate::Process(vert_shapes[i], temp_v);

            int tcount = temp_v.size()/3;
    
            for(int j=0; j < tcount; j++){ 
               glColor3f(colorMapPol[i].r, colorMapPol[i].g, colorMapPol[i].b);     
              glBegin (GL_TRIANGLES);
              glVertex2d (temp_v[j*3+0].GetX(), temp_v[j*3+0].GetY());
              glVertex2d (temp_v[j*3+1].GetX(), temp_v[j*3+1].GetY());
              glVertex2d (temp_v[j*3+2].GetX(), temp_v[j*3+2].GetY());
              glEnd ();
            }
            vert_triangl.push_back(temp_v); // this vector has in every pos has the triangles of every polygon
            glDisable(GL_DEPTH_TEST);


            glLineWidth(2.5);
             glColor3f(colorMap[i].r, colorMap[i].g, colorMap[i].b);
            glBegin (GL_LINE_LOOP);

            for(int j=0;j<vert_shapes[i].size();j++){     
                glVertex2i (vert_shapes[i][j].GetX(), vert_shapes[i][j].GetY());
            }  
            glEnd ();
            glEnable(GL_DEPTH_TEST);
          



        }
        
   }         
    
   if(actionFlag==EXTRUDE){

    
      mouse_flag = 100;
     glEnable(GL_DEPTH_TEST);
      glRotatef(ph,1,0,0);
      glRotatef(th,0,1,0);

      	//bottom edge
        for(int i=0; i < vert_triangl.size();i++){
          glColor3f(colorMapPol[i].r, colorMapPol[i].g, colorMapPol[i].b);
          glBegin (GL_TRIANGLES);
          for(int j = 0; j < vert_triangl[i].size(); j++){
              
              glVertex3d (vert_triangl[i][j].GetX(), vert_triangl[i][j].GetY(), 0.0);
              
            }
          glEnd ();  
        }    

        for(int i =0; i < vert_shapes.size(); i++){
            //edges
          glColor3f(colorMap[i].r, colorMap[i].g, colorMap[i].b);
            
          for(int k=0; k < vert_shapes[i].size() ; k++){
            glBegin( GL_QUADS );
              if(k != vert_shapes[i].size() - 1){
           
                glVertex3i (vert_shapes[i][k].GetX(), vert_shapes[i][k].GetY(), 0.0);
                glVertex3i (vert_shapes[i][k+1].GetX(), vert_shapes[i][k+1].GetY(), 0.0);
                glVertex3i (vert_shapes[i][k+1].GetX(), vert_shapes[i][k+1].GetY(), extrudeLength);
                glVertex3i (vert_shapes[i][k].GetX(), vert_shapes[i][k].GetY(), extrudeLength);
              }else{
                glVertex3i (vert_shapes[i][k].GetX(), vert_shapes[i][k].GetY(), 0.0);
                glVertex3i (vert_shapes[i][0].GetX(), vert_shapes[i][0].GetY(), 0.0);
                glVertex3i (vert_shapes[i][0].GetX(), vert_shapes[i][0].GetY(), extrudeLength);
                glVertex3i (vert_shapes[i][k].GetX(), vert_shapes[i][k].GetY(), extrudeLength);

              }
                  
            
            glEnd();
          } 
        }
            //top edge

        for(int i=0; i < vert_triangl.size();i++){
          glColor3f(colorMapPol[i].r, colorMapPol[i].g, colorMapPol[i].b);
          glBegin (GL_TRIANGLES);
          for(int j = 0; j < vert_triangl[i].size(); j++){
              
              glVertex3d (vert_triangl[i][j].GetX(), vert_triangl[i][j].GetY(), extrudeLength);
              
            }
          glEnd ();  
        }               

    


   } 

   if(key_flag==1){ //here is the code of the triangulation(when the user press T)
      
      	        for(int i=0; i<vert_shapes.size();i++){

			            Vector2dVector temp_v;
			            Triangulate::Process(vert_shapes[i], temp_v);

			            int tcount = temp_v.size()/3;
			    
			            for(int j=0; j < tcount; j++){ 
			               glColor3f(colorMapPol[i].r, colorMapPol[i].g, colorMapPol[i].b);     
			              glBegin (GL_TRIANGLES);
			              glVertex2d (temp_v[j*3+0].GetX(), temp_v[j*3+0].GetY());
			              glVertex2d (temp_v[j*3+1].GetX(), temp_v[j*3+1].GetY());
			              glVertex2d (temp_v[j*3+2].GetX(), temp_v[j*3+2].GetY());
			              glEnd ();
			            }
			        }

            glDisable(GL_DEPTH_TEST);
     
        for(int i=0; i < vert_shapes.size(); i++){
          Vector2dVector temp_v;
          Triangulate::Process(vert_shapes[i], temp_v);

          int tcount = temp_v.size()/3;
    
          for(int j=0; j < tcount; j++){     
              cout << temp_v[j*3+j].GetX() <<","<< temp_v[j*3+j].GetY() <<endl;
              glColor3d (0.0, 1.0, 0.0);          /* initial drawing color is green */
              glLineWidth (1.5);  
              glBegin (GL_LINE_LOOP);
              glVertex2d (temp_v[j*3+0].GetX(), temp_v[j*3+0].GetY());
              glVertex2d (temp_v[j*3+1].GetX(), temp_v[j*3+1].GetY());
              glVertex2d (temp_v[j*3+2].GetX(), temp_v[j*3+2].GetY());
              glEnd ();
          } 
        }                 
   }  
  
  

   glFlush();

   glutSwapBuffers();
} 

/**********procesMainMenu*********/

void processMainMenu(int option){
    /*nothing do here
    everything in submenus*/
}


/******processActionMenu*********/

void processActionMenu(int option){

    switch (option){
    
    case ACTION:
        exit(0);
        //actionFlag=1;
        break;
    case POLYGON:
        actionFlag = POLYGON; 
        break;
        //glutDetachMenu (GLUT_RIGHT_BUTTON);       
    case CLIPPING:
        clipIn = 1;
        actionFlag=CLIPPING;

        break;
    case EXTRUDE:
        if(clipIn==0){
          if(vert_shapes.size()==1){
            actionFlag=EXTRUDE;
            printf("Give the length of extrusion: ");
            scanf("%f", &extrudeLength);
          }  
        }  
        break;
    default :
        break;  
    }

}
/**********processLineColorMenu*****/
void processLineColorMenu(int option){
  switch (option){

    case WHITE :
          colorLine.r = 1.0f;
          colorLine.g = 1.0f;
          colorLine.b = 1.0f;
          //glColor3f(1.0f, 1.0f, 1.0f);//white
          lineColorFlag = WHITE;

          break;

    case BLACK :
          colorLine.r = 0.0f;
          colorLine.g = 0.0f;
          colorLine.b = 0.0f;
          //glColor3f(0.0f, 0.0f, 0.0f);//Black
          lineColorFlag = BLACK;

          break;
    case RED :
          colorLine.r = 1.0f;
          colorLine.g = 0.0f;
          colorLine.b = 0.0f;    
          //glColor3f(1.0f, 0.0f, 0.0f);//red
          lineColorFlag = RED;

          break;
    case BLUE_GREEN :
          colorLine.r = 0.0f;
          colorLine.g = 0.5f;
          colorLine.b = 0.5f;    
          //glColor3f(0.0f, 0.5f, 0.5f);//Blue-Green
          lineColorFlag = BLUE_GREEN;

          break;
    case PURPLE :
          colorLine.r = 1.0f;
          colorLine.g = 0.0f;
          colorLine.b = 1.0f;
          //glColor3f(1.0f, 0.0f, 1.0f);//Purple
         lineColorFlag = PURPLE;

         break; 
    case BLUE :
          colorLine.r = 0.0f;
          colorLine.g = 0.0f;
          colorLine.b = 1.0f;
          //glColor3f(0.0f, 0.0f, 1.0f);//Blue
          lineColorFlag = BLUE;

          break;  
    case GREEN :
          colorLine.r = 0.0f;
          colorLine.g = 1.0f;
          colorLine.b = 0.0f;    
         // glColor3f(0.0f, 1.0f, 0.0f);//Green
          lineColorFlag = GREEN;

          break;
    case ORANGE :
          colorLine.r = 1.0;
          colorLine.g = 0.5f;
          colorLine.b = 0.0f;
          //glColor3f(1.0f, 0.5f, 0.0f);//Orange
          lineColorFlag = ORANGE;

          break;

    case VIOLET :
          colorLine.r = 0.5f;
          colorLine.g = 0.5f;
          colorLine.b = 0.5f;
          //glColor3f(0.5f, 0.5f, 0.5f);//Violet
          lineColorFlag = VIOLET;

          break;

    case DARK_GRAY :
          colorLine.r = 0.1f;
          colorLine.g = 0.1f;
          colorLine.b = 0.1f;
          //glColor3f(0.1f, 0.1f, 0.1f);//Dark grey
          lineColorFlag = DARK_GRAY;
          
          break;
    case BRONZE :
          colorLine.r = 0.1f;
          colorLine.g = 0.1f;
          colorLine.b = 0.0f;
         // glColor3f(0.1f, 0.1f, 0.0f);//Bronze
          lineColorFlag = BRONZE;
          
          break;
    case BABY_BLUE :
          colorLine.r = 0.0f;
          colorLine.g = 0.5f;
          colorLine.b = 1.0f;
          //glColor3f(0.0f, 0.5f, 1.0f);//baby Blue
          lineColorFlag = BABY_BLUE;
          
          break;
    case CYAN :
          colorLine.r = 0.5f;
          colorLine.g = 1.0f;
          colorLine.b = 1.0f;
          //glColor3f(0.5f, 1.0f, 1.0f);//cyan
          lineColorFlag = CYAN;
          
          break;
    case LILAC :
          colorLine.r = 2.0f;
          colorLine.g = 0.5f;
          colorLine.b = 1.0f;
          //glColor3f(2.0f, 0.5f, 1.0f);//Lilac
          lineColorFlag = LILAC;

          break;
    case DARK_BLUE :
          colorLine.r = 0.0f;
          colorLine.g = 0.1f;
          colorLine.b = 0.1f;
          //glColor3f(0.0f, 0.1f, 0.1f);//Dark blue
          lineColorFlag = DARK_BLUE;

          break;
    case BROWN :
          colorLine.r = 0.1f;
          colorLine.g = 0.0f;
          colorLine.b = 0.0f;
          //glColor3f(0.1f, 0.0f, 0.0f);//Brown
          lineColorFlag = BROWN;

          break;
    default : //black color
        colorLine.r = 0.0f;
        colorLine.g = 0.0f;
        colorLine.b = 0.0f;
        lineColorFlag = 0;
        break;           
  }

}
/**********processFillColorMenu*****/
void processFillColorMenu(int option){
switch (option){

    case WHITE :
          color.r = 1.0f;
          color.g = 1.0f;
          color.b = 1.0f;    
         // glColor3f(1.0f, 1.0f, 1.0f);//white
          fillColorFlag = WHITE;

          break;

    case BLACK :
          color.r = 0.0f;
          color.g = 0.0f;
          color.b = 0.0f;
          //glColor3f(0.0f, 0.0f, 0.0f);//Black
          fillColorFlag = BLACK;

          break;
    case RED :
          color.r = 1.0f;
          color.g = 0.0f;
          color.b = 0.0f;    
          //glColor3f(1.0f, 0.0f, 0.0f);//red
          fillColorFlag = RED;

          break;
    case BLUE_GREEN :
          color.r = 0.0f;
          color.g = 0.5f;
          color.b = 0.5f;    
          //glColor3f(0.0f, 0.5f, 0.5f);//Blue-Green
          fillColorFlag = BLUE_GREEN;

          break;
    case PURPLE :
          color.r = 1.0f;
          color.g = 0.0f;
          color.b = 1.0f;
          //glColor3f(1.0f, 0.0f, 1.0f);//Purple
         fillColorFlag = PURPLE;

         break; 
    case BLUE :
          color.r = 0.0f;
          color.g = 0.0f;
          color.b = 1.0f;
          //glColor3f(0.0f, 0.0f, 1.0f);//Blue
          fillColorFlag = BLUE;

          break;  
    case GREEN :
          color.r = 0.0f;
          color.g = 1.0f;
          color.b = 0.0f;    
         // glColor3f(0.0f, 1.0f, 0.0f);//Green
          fillColorFlag = GREEN;

          break;
    case ORANGE :
          color.r = 1.0;
          color.g = 0.5f;
          color.b = 0.0f;
          //glColor3f(1.0f, 0.5f, 0.0f);//Orange
          fillColorFlag = ORANGE;

          break;

    case VIOLET :
          color.r = 0.5f;
          color.g = 0.5f;
          color.b = 0.5f;
          //glColor3f(0.5f, 0.5f, 0.5f);//Violet
          fillColorFlag = VIOLET;

          break;

    case DARK_GRAY :
          color.r = 0.1f;
          color.g = 0.1f;
          color.b = 0.1f;
          //glColor3f(0.1f, 0.1f, 0.1f);//Dark grey
          fillColorFlag = DARK_GRAY;
          
          break;
    case BRONZE :
          color.r = 0.1f;
          color.g = 0.1f;
          color.b = 0.0f;
         // glColor3f(0.1f, 0.1f, 0.0f);//Bronze
          fillColorFlag = BRONZE;
          
          break;
    case BABY_BLUE :
          color.r = 0.0f;
          color.g = 0.5f;
          color.b = 1.0f;
          //glColor3f(0.0f, 0.5f, 1.0f);//baby Blue
          fillColorFlag = BABY_BLUE;
          
          break;
    case CYAN :
          color.r = 0.5f;
          color.g = 1.0f;
          color.b = 1.0f;
          //glColor3f(0.5f, 1.0f, 1.0f);//cyan
          fillColorFlag = CYAN;
          
          break;
    case LILAC :
          color.r = 2.0f;
          color.g = 0.5f;
          color.b = 1.0f;
          //glColor3f(2.0f, 0.5f, 1.0f);//Lilac
          fillColorFlag = LILAC;

          break;
    case DARK_BLUE :
          color.r = 0.0f;
          color.g = 0.1f;
          color.b = 0.1f;
          //glColor3f(0.0f, 0.1f, 0.1f);//Dark blue
          fillColorFlag = DARK_BLUE;

          break;
    case BROWN :
          color.r = 0.1f;
          color.g = 0.0f;
          color.b = 0.0f;
          //glColor3f(0.1f, 0.0f, 0.0f);//Brown
          fillColorFlag = BROWN;

          break;
    default :
          color.r = 1.0f;
          color.g = 1.0f;
          color.b = 1.0f; 
          fillColorFlag = 0;
        break;        
  }
}



/*******createPopupMenus*******/

void createPopupMenus() {

    action_menu = glutCreateMenu (processActionMenu);
        glutAddMenuEntry("Exit",1);
        glutAddMenuEntry("POLYGON",2);
        glutAddMenuEntry("CLIPPING",5);
        glutAddMenuEntry("EXTRUDE", 4);
    
    line_color_menu = glutCreateMenu (processLineColorMenu);
        glutAddMenuEntry("WHITE", WHITE);
        glutAddMenuEntry("BLACK", BLACK);
        glutAddMenuEntry("RED", RED);
        glutAddMenuEntry("BLUE_GREEN", BLUE_GREEN);
        glutAddMenuEntry("PURPLE", PURPLE);
        glutAddMenuEntry("BLUE", BLUE);
        glutAddMenuEntry("GREEN", GREEN);
        glutAddMenuEntry("ORANGE", ORANGE);
        glutAddMenuEntry("VIOLET", VIOLET);
        glutAddMenuEntry("DARK_GRAY", DARK_GRAY);
        glutAddMenuEntry("BRONZE", BRONZE);
        glutAddMenuEntry("BABY_BLUE", BABY_BLUE);
        glutAddMenuEntry("CYAN", CYAN);
        glutAddMenuEntry("LILAC", LILAC);
        glutAddMenuEntry("DARK_BLUE", DARK_BLUE);
        glutAddMenuEntry("BROWN", BROWN);

    fill_color_menu = glutCreateMenu (processFillColorMenu);
        glutAddMenuEntry("WHITE", WHITE);
        glutAddMenuEntry("BLACK", BLACK);
        glutAddMenuEntry("RED", RED);
        glutAddMenuEntry("BLUE_GREEN", BLUE_GREEN);
        glutAddMenuEntry("PURPLE", PURPLE);
        glutAddMenuEntry("BLUE", BLUE);
        glutAddMenuEntry("GREEN", GREEN);
        glutAddMenuEntry("ORANGE", ORANGE);
        glutAddMenuEntry("VIOLET", VIOLET);
        glutAddMenuEntry("DARK_GRAY", DARK_GRAY);
        glutAddMenuEntry("BRONZE", BRONZE);
        glutAddMenuEntry("BABY_BLUE", BABY_BLUE);
        glutAddMenuEntry("CYAN", CYAN);
        glutAddMenuEntry("LILAC", LILAC);
        glutAddMenuEntry("DARK_BLUE", DARK_BLUE);
        glutAddMenuEntry("BROWN", BROWN);

    main_menu = glutCreateMenu (processMainMenu);       
        glutAddSubMenu ("ACTION",action_menu);
        glutAddSubMenu("LINE_COLOR", line_color_menu);
        glutAddSubMenu("FILL_COLOR", fill_color_menu); 
        
    glutAttachMenu (GLUT_RIGHT_BUTTON);
    // this will allow us to know if the menu is active
    glutMenuStatusFunc(processMenuStatus);  
    
}



/**************processMenuStatus*********/
void processMenuStatus(int status, int x, int y) {

    if (status == GLUT_MENU_IN_USE)
        menuFlag = 1;
    else
        menuFlag = 0;
}

/************mouse**********************/
void mouse(int button,int state,int x, int y){
     
    if(actionFlag == POLYGON){ // actionFlag is equal to POLYGON(2) when the user choose the coice POLYGON
      
        if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN)){
            makePolygon(x,y);
            glutDetachMenu (GLUT_RIGHT_BUTTON); // deactivate menu so when the user will press right click the polygon gonna appear
            

        }if((button == GLUT_RIGHT_BUTTON) ) {
                     
            if(vertices.empty() == true)
            pointsChosen= 0;
                
            else{
                vert_shapes.push_back(vertices);
                    
                vertices.clear(); 
                pointsChosen=0;

              if(fillColorFlag==0){  // user doesnt choose fill color so the color is white by default 
                color.r = 1.0f;
                color.g = 1.0f;
                color.b = 1.0f;
                glColor3f (color.r, color.g, color.b);
                colorMapPol.push_back(color);                

              }if(lineColorFlag==0){ // user doesnt choose line color so the color is black by default   
                colorLine.r = 0.0f;
                colorLine.g = 0.0f;
                colorLine.b = 0.0f;
                glColor3f (colorLine.r, colorLine.g, colorLine.b);
                colorMap.push_back(colorLine);
              
              }  

                for(int k = 1; k < 17; k++){
                  if(fillColorFlag==k){
                  glColor3f (color.r, color.g, color.b);
                  colorMapPol.push_back(color);
                  fillColorFlag = 0;
                
                  }if(lineColorFlag==k){
                    glColor3f (colorLine.r, colorLine.g, colorLine.b);
                    colorMap.push_back(colorLine);
                    lineColorFlag = 0;
                  }
                }

                mouse_flag=1; 
                //actionFlag=100;  

                glutAttachMenu (GLUT_RIGHT_BUTTON);
            }            
                        
            
            
        }   
    
    }
    if (actionFlag==CLIPPING){
      vector<Vector2dVector > temp_shapes;
      temp_shapes = vert_shapes;

        if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN)){
            
            clip_window(x, y);
            if(counter>2){
                
                for(int i=0; i<temp_shapes.size();i++){ 
                    for(int j=0; j <clipper_size; j++ ){
                      
                      int k = (j+1) % clipper_size;

                      clipping(temp_shapes[i], clipper_points[j][0], 
                        clipper_points[j][1], clipper_points[k][0], 
                        clipper_points[k][1] );

                      
                    }
                 
                }
                vert_shapes = temp_shapes;       
                 mouse_flag=1;
                counter=1;
                actionFlag=100;
          }    
        }     
          
    }
  glutPostRedisplay();   
}
/**************keyboard**************/
void keyboard(unsigned char key, int x, int y){
  glutPostRedisplay();
    switch(key){
        case 't':
        case 'T':
        // for key_flag = 1 the program make the triangulation of the polygon 
         if( key_flag==0){
         	
            key_flag=1; 
            mouse_flag = 100;
         }else{
          key_flag=0;
          mouse_flag=1; 
         } 
    	default :
        break;  


    }
    glutPostRedisplay();
}

/**********windowSpecial**********/
void windowSpecial(int key,int x,int y)
{
  /*  Right arrow key - increase azimuth by 3 degrees */
  if (key == GLUT_KEY_RIGHT) th += 3;
  /*  Left arrow key - decrease azimuth by 3 degrees */
  else if (key == GLUT_KEY_LEFT) th -= 3;
  /*  Up arrow key - increase elevation by 3 degrees */
  else if (key == GLUT_KEY_UP) ph += 3;
  /*  Down arrow key - decrease elevation by 3 degrees */
  else if (key == GLUT_KEY_DOWN) ph -= 3;

  /*  Keep angles to +/-360 degrees */
  th %= 360;
  ph %= 360;


  glutPostRedisplay();
}


/****************makePolygon***********/
void makePolygon(int x,int y){

    
   switch(pointsChosen)
            { 
            case 0:
              int xi,yi;         
                xi = x;
                yi = ysize - y;
                //check from the 3d line if intersects. 3d line only with 1st line etc
                if(vertices.size() > 2){
                    for(int i = 0; i < (vertices.size()-2); i++){ 
                        status = linesIntersect(vertices[i].GetX(), vertices[i].GetY(), vertices[i+1].GetX(), vertices[i+1].GetY(), vertices.back().GetX(),vertices.back().GetY(), xi, yi);
                        if(status == true){
                            
                            printf("Error. The polygon is intersected. Try again\n");

                            actionFlag = 100; 
                            
                            break;
                    
                        }
                    }if(status == false){
                        vertices.push_back(Vector2d(xi,yi));
                        pointsChosen=0;
                        break;

                    }else{
                        
                        vertices.clear();

                        break;
                    }
               }else{
                    vertices.push_back(Vector2d(xi,yi));
                    pointsChosen=0;
                    break;
                }
          default :
                break;
                
            }    
             
}    


/***************linesIntersect*******/
//we have to lines one is the ((x1,y1),(x2,y2)) and the other ((x3,y3),(x4,y4))
bool linesIntersect(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4){
    bool c1,c2,c3,c4;
    c1 =clockwise(x1, y1, x2, y2, x3, y3);
    c2 =clockwise(x1, y1, x2, y2, x4, y4);
    c3 =clockwise(x3, y3, x4, y4, x1, y1);
    c4 =clockwise(x3, y3, x4, y4, x2, y2);

    if(c1 != c2 && c3 != c4) return true; // the lines are intersecting
    else return false;
}

/***************clockwise************/
bool clockwise(int x1, int y1, int x2, int y2, int x3, int y3){
    int value;
    bool status;
    value = (y2 - y1) * (x3 - x2) - (x2 - x1) * (y3 - y2); 

    if(value>0) return true; //the orientation of the points is clockwise
    else return false; // the orientation of the points is counter clockwise
}

int intesect_point_x(int x1, int y1, int x2, int y2, 
        int x3, int y3, int x4, int y4) 
{ 
  int num = (x1*y2 - y1*x2) * (x3-x4) - 
      (x1-x2) * (x3*y4 - y3*x4); 
  int den = (x1-x2) * (y3-y4) - (y1-y2) * (x3-x4); 
  return num/den; 
}
int intesect_point_y(int x1, int y1, int x2, int y2, 
        int x3, int y3, int x4, int y4) 
{ 
  int num = (x1*y2 - y1*x2) * (y3-y4) - 
      (y1-y2) * (x3*y4 - y3*x4); 
  int den = (x1-x2) * (y3-y4) - (y1-y2) * (x3-x4); 
  return num/den; 
}


void clipping(Vector2dVector &polygon, int x1, int y1, int x2, int y2){// make thw new points which intersect with the clipper

  Vector2dVector new_points;
  
  for (int i = 0 ; i < polygon.size(); i++){
    
    int k = (i+1) % polygon.size();
    int ix = polygon[i].GetX();
    int iy = polygon[i].GetY();
    int kx = polygon[k].GetX();
    int ky = polygon[k].GetY();

    int ipos = (x2-x1) * (iy-y1) - (y2-y1) * (ix-x1); 
    int kpos = (x2-x1) * (ky-y1) - (y2-y1) * (kx-x1);
    
    //case 1 both inside
    if(ipos > 0 && kpos > 0){
       new_points.push_back(Vector2d(kx, ky));
    }
    //case 2 only second inside
    else if (ipos <= 0 && kpos > 0){

      new_points.push_back(Vector2d(intesect_point_x(x1, y1, x2, y2, ix, iy, kx, ky), intesect_point_y(x1, y1, x2, y2, ix, iy, kx, ky) ));
    
      new_points.push_back(Vector2d(kx, ky));
    }
    //case 3 only first inside
    else if (ipos > 0 && kpos <= 0){
      new_points.push_back(Vector2d(intesect_point_x(x1, y1, x2, y2, ix, iy, kx, ky), intesect_point_y(x1, y1, x2, y2, ix, iy, kx, ky) ));

    }
    //case 4 nothing inside
    else{
      // dont do anything
    }

  }
    polygon.clear();
    // change all values of polygon to new_points
    polygon = new_points;

  

}




void clip_window(int x, int y){

    if(counter==1){
        //first point
        clipper_points[0][0] = x;
        clipper_points[0][1] = ysize - y;
        counter++;
    }
    else if(counter==2){
        //third point
        clipper_points[2][0] = x;
        clipper_points[2][1] = ysize - y;
        //second point
        clipper_points[1][0] = clipper_points[0][0];
        clipper_points[1][1] = clipper_points[2][1];
        //fourth point
        clipper_points[3][0] = clipper_points[2][0];
        clipper_points[3][1] = clipper_points[0][1];

        counter++;
    }
        

} 
