
#include<GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include<math.h>
#include <iostream>
#include<bits/stdc++.h>
#include "BmpLoader.h"

using namespace std;
double Exval=500,Eyval=2533,Ezval=5500;
double Dxval=2,Dyval=0,Dzval=0;
double windowHeight=1000, windowWidth=1000;
double boat_z=0;

//double tx=1000;
double bus_x=6500;
double car1_tx=-6500;
double car2_tx=8000;
float angle = 0.0f;
bool flagScale=false;
bool ship_flag=false;
bool light1_flag=true;
bool light2_flag=true;
bool light3_flag=false;
bool light4_flag=false;
bool a1=true;
bool d1=true;
bool s1=true;
bool a2=true;
bool d2=true;
bool s2=true;
bool a3=true;
bool d3=true;
bool s3=true;



bool night=false;
double tx=180-60;
double tz=3500;



bool red=false;
bool green=true;




/* GLUT callback Handlers */

//const int L_funnel=32;
//const int nt_funnel = 60;				//number of slices along x-direction
//const int ntheta_funnel = 60;


int anglex= 0, angley = 0, anglez = 0;          //rotation angles
int window;
int wired=0;

int animat = 0;
const int dgre=3;

int clikd=0;

double ex=0, ey=0, ez=15, lx=0,ly=0,lz=0, hx=0,hy=1,hz=0;
float wcsClkDn[3],wcsClkUp[3];
///////////////////////////////
class point1
{
public:
    point1()
    {
        x=0;
        y=0;
    }
    int x;
    int y;
} clkpt[2];
int flag=0;
GLint viewport[4]; //var to hold the viewport info
GLdouble modelview[16]; //var to hold the modelview info
GLdouble projection[16]; //var to hold the projection matrix info

void scsToWcs(float sx,float sy, float wcsv[3] );
void processMouse(int button, int state, int x, int y);
void matColor(float kdr, float kdg, float kdb,  float shiny, int frnt_Back=0);
static void getNormal3p
(GLfloat x1, GLfloat y1,GLfloat z1, GLfloat x2, GLfloat y2,GLfloat z2, GLfloat x3, GLfloat y3,GLfloat z3);


void scsToWcs(float sx,float sy, float wcsv[3] )
{

    GLfloat winX, winY, winZ; //variables to hold screen x,y,z coordinates
    GLdouble worldX, worldY, worldZ; //variables to hold world x,y,z coordinates

    //glGetDoublev( GL_MODELVIEW_MATRIX, modelview ); //get the modelview info
    glGetDoublev( GL_PROJECTION_MATRIX, projection ); //get the projection matrix info
    glGetIntegerv( GL_VIEWPORT, viewport ); //get the viewport info

    winX = sx;
    winY = (float)viewport[3] - (float)sy;
    winZ = 0;

    //get the world coordinates from the screen coordinates
    gluUnProject( winX, winY, winZ, modelview, projection, viewport, &worldX, &worldY, &worldZ);
    wcsv[0]=worldX;
    wcsv[1]=worldY;
    wcsv[2]=worldZ;


}


//control points
long long nCr(int n, int r)
{
    if(r > n / 2) r = n - r; // because C(n, r) == C(n, n - r)
    long long ans = 1;
    int i;

    for(i = 1; i <= r; i++)
    {
        ans *= n - r + i;
        ans /= i;
    }

    return ans;
}

//polynomial interpretation for N points
void BezierCurve ( double t,  float xy[2],int L,GLfloat ctrlpoints[][3])
{
    double y=0;
    double x=0;
    t=t>1.0?1.0:t;
    for(int i=0; i<=L; i++)
    {
        int ncr=nCr(L,i);
        double oneMinusTpow=pow(1-t,double(L-i));
        double tPow=pow(t,double(i));
        double coef=oneMinusTpow*tPow*ncr;
        x+=coef*ctrlpoints[i][0];
        y+=coef*ctrlpoints[i][1];

    }
    xy[0] = float(x);
    xy[1] = float(y);

    //return y;
}

void funnelBezier(int L,int nt,int ntheta,GLfloat ctrlpoints[][3],double rotation_angle)
{
    int i, j;
    float x, y, z, r;				//current coordinates
    float x1, y1, z1, r1;			//next coordinates
    float theta;

    const float startx = 0, endx = ctrlpoints[L][0];
    //number of angular slices
    const float dx = (endx - startx) / nt;	//x step size
    const float dtheta = 2*rotation_angle / ntheta;		//angular step size

    float t=0;
    float dt=1.0/nt;
    float xy[2];
    BezierCurve( t,  xy,L,ctrlpoints);
    x = xy[0];
    r = xy[1];
    //rotate about z-axis
    float p1x,p1y,p1z,p2x,p2y,p2z;
    for ( i = 0; i < nt; ++i )  			//step through x
    {
        theta = 0;
        t+=dt;
        BezierCurve( t,  xy,L,ctrlpoints);
        x1 = xy[0];
        r1 = xy[1];

        //draw the surface composed of quadrilaterals by sweeping theta
        glBegin( GL_QUAD_STRIP );
        for ( j = 0; j <= ntheta; ++j )
        {
            theta += dtheta;
            double cosa = cos( theta );
            double sina = sin ( theta );
            y = r * cosa;
            y1 = r1 * cosa;	//current and next y
            z = r * sina;
            z1 = r1 * sina;	//current and next z

            //edge from point at x to point at next x
            glVertex3f (x, y, z);

            if(j>0)
            {
                getNormal3p(p1x,p1y,p1z,p2x,p2y,p2z,x, y, z);
            }
            else
            {
                p1x=x;
                p1y=y;
                p1z=z;
                p2x=x1;
                p2y=y1;
                p2z=z1;

            }
            glVertex3f (x1, y1, z1);

            //forms quad with next pair of points with incremented theta value
        }
        glEnd();
        x = x1;
        r = r1;
    } //for i

}

void boatBezier(int L,int nt,int ntheta,GLfloat ctrlpoints[][3],double rotation_angle)
{
    int i, j;
    float x, y, z, r;				//current coordinates
    float x1, y1, z1, r1;			//next coordinates
    float theta;

    const float startx = 0, endx = ctrlpoints[L][0];
    //number of angular slices
    const float dx = (endx - startx) / nt;	//x step size
    const float dtheta = 2*rotation_angle / ntheta;		//angular step size

    float t=0;
    float dt=1.0/nt;
    float xy[2];
    BezierCurve( t,  xy,L,ctrlpoints);
    x = xy[0];
    r = xy[1];
    //rotate about z-axis
    float p1x,p1y,p1z,p2x,p2y,p2z;
    for ( i = 0; i < nt; ++i )  			//step through x
    {
        theta = 0;
        t+=dt;
        BezierCurve( t,  xy,L,ctrlpoints);
        x1 = xy[0];
        r1 = xy[1];

        //draw the surface composed of quadrilaterals by sweeping theta
        glBegin( GL_QUAD_STRIP );
        for ( j = 0; j <= ntheta/2; ++j )
        {
            theta += dtheta;
            double cosa = cos( theta );
            double sina = sin ( theta );
            y = r * cosa;
            y1 = r1 * cosa;	//current and next y
            z = r * sina;
            z1 = r1 * sina;	//current and next z

            //edge from point at x to point at next x
            glVertex3f (x, y, z);

            if(j>0)
            {
                getNormal3p(p1x,p1y,p1z,p2x,p2y,p2z,x, y, z);
            }
            else
            {
                p1x=x;
                p1y=y;
                p1z=z;
                p2x=x1;
                p2y=y1;
                p2z=z1;

            }
            glVertex3f (x1, y1, z1);

            //forms quad with next pair of points with incremented theta value
        }
        glEnd();
        x = x1;
        r = r1;
    } //for i

}



static void resize(int width, int height)
{
    const float ar = (float) width / (float) height;

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //glFrustum(-ar, ar, -1.0, 1.0, 2.0, 100.0);
    // gluPerspective(60,ar, 2.0, 100.0);

    glOrtho(-8.0, 8.0, -8.0*(GLfloat)height/(GLfloat)width, 8.0*(GLfloat)height/(GLfloat)width, 2.0, 25.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity() ;
    gluLookAt(ex,ey,ez,lx,ly,lz,hx,hy,hz);

}




void update()
{
    if(tx>1600){
        tx=1600;
    }
    if(tx<-1600){
        tx=-1600;
    }
    if(tz>3500){
        tz=3500;
    }



    if (flagScale == true)
    {
        angle += 0.2f;
        if (angle > 60)
        {
            angle = 60;
        }


    }




    if (flagScale == false)
    {
        angle -= 0.2f;
        if (angle < 0)
        {
            angle = 0;
        }


    }
 if(ship_flag){

    if(green==true && bus_x>-10000){
          bus_x-=2;
    }
    else if(red==true && bus_x>-10000){

          if(bus_x==2000){
            bus_x=2000;
          }
          else{
            bus_x-=2;
          }

    }
 }
 if(ship_flag){

    if(green==true && car1_tx<10000){
          car1_tx+=2;
    }
    else if(red==true && car1_tx<10000){
          if(car1_tx==-2300){
            car1_tx=-2300;
          }
          else{
            car1_tx+=2;
          }

    }
 }

 if(ship_flag){

     if(green==true && car2_tx>-10000){
          car2_tx-=2;
    }
    else if(red==true && car2_tx>-10000){
          if(car2_tx==3000){
            car2_tx=3000;
          }
          else{
            car2_tx-=2;
          }

    }
 }

//    if(car1_tx<10000){
//
//        car1_tx++;
//    }
//
//    if(ship_flag==true){
//    if(tx>-5000){
//        tx-=0.5;
//    }
//    }

    if( tx >=-450 && tx<= 450 && tz<=2000 && tz>= -900-500 ){

if( bus_x<-2300 || bus_x>2000 && car1_tx<-2300|| car1_tx>2000 ){
        if(car2_tx<-2300 || car2_tx>3000){
        flagScale=true;
        red=true;
        green=false;

    }


    }

    else if(bus_x>-2300 && bus_x < 2000 && car1_tx> -2300 && car1_tx<2000 ){
         if(car2_tx>-2300 && car2_tx<3000){

       if(tz<=2000)
       {
           tz=2000;
       }
       else if(tz>=-1400){
            tz=-1400;
        }




    }

    }


    }
    else{
        flagScale=false;
        green=true;
        red=false;
    }





    glutPostRedisplay();


}


static GLfloat v_pyramid[5][3] =
{
//    {0.0, 0.0, 0.0},
//    {0.0, 0.0, 2.0},
//    {2.0, 0.0, 2.0},
//    {2.0, 0.0, 0.0},
{-0.5f, -0.5f, -0.5f},///4
{0.5f, -0.5f, -0.5f},///5
 {1.0f, 1.0f, -1.0f},///3
 {-1.0f, 1.0f, -1.0f},///2
{0, 4.5, 0.001}
};

static GLubyte p_Indices[4][3] =
{
    {4, 1, 2},
    {4, 2, 3},
    {4, 3, 0},
    {4, 0, 1}
};

static GLfloat v_rectangle[8][3] =
{
    {-1.0f, -1.0f, 1.0f},///0
    {1.0f, -1.0f, 1.0f},///1
    {1.0f, 1.0f, 1.0f},///2
    {-1.0f, 1.0f, 1.0f},///3

    {-0.5f, -0.5f, -0.5f},///4
    {-0.5f, 0.5f, -0.5f},///5
    {0.5f, 0.5f, -0.5f},///6
    {0.5f, -0.5f, -0.5f}///7


};




static GLfloat v_cube[8][3] =
{
    {-1.0f, -1.0f, 1.0f},///0
    {1.0f, -1.0f, 1.0f},///1
    {1.0f, 1.0f, 1.0f},///2
    {-1.0f, 1.0f, 1.0f},///3

    {-1.0f, -1.0f, -1.0f},///4
    {-1.0f, 1.0f, -1.0f},///5
    {1.0f, 1.0f, -1.0f},///6
    {1.0f, -1.0f, -1.0f}///7

};
static GLubyte quadindices[1][4] =
{
    {0, 1,2,3}
};

static GLubyte quadIndices[6][4] =
{
    {0, 1, 2,3},
    {7, 6, 2,1},
    {4, 5, 6,7},
    {4, 0, 3,5},
    {2,3,5,6},
    {1,0,4,7}



};


static void getNormal3p
(GLfloat x1, GLfloat y1,GLfloat z1, GLfloat x2, GLfloat y2,GLfloat z2, GLfloat x3, GLfloat y3,GLfloat z3)
{
    GLfloat Ux, Uy, Uz, Vx, Vy, Vz, Nx, Ny, Nz;

    Ux = x2-x1;
    Uy = y2-y1;
    Uz = z2-z1;

    Vx = x3-x1;
    Vy = y3-y1;
    Vz = z3-z1;

    Nx = Uy*Vz - Uz*Vy;
    Ny = Uz*Vx - Ux*Vz;
    Nz = Ux*Vy - Uy*Vx;

    glNormal3f(Nx,Ny,Nz);
}
void matColor(float kdr, float kdg, float kdb,  float shiny, int frnt_Back)
{

    const GLfloat mat_ambient[]    = { kdr*0.5,kdg*0.5,kdb*0.5,1 };
    const GLfloat mat_diffuse[]    = { kdr,kdg,kdb,1 };
    const GLfloat mat_specular[]   = { kdr,kdg,kdb,1 };
    const GLfloat high_shininess[] = { shiny };
    if(frnt_Back==0)
    {
        glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
        glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);
    }
    else if(frnt_Back==1)
    {
        glMaterialfv(GL_BACK, GL_AMBIENT,   mat_ambient);
        glMaterialfv(GL_BACK, GL_DIFFUSE,   mat_diffuse);
        glMaterialfv(GL_BACK, GL_SPECULAR,  mat_specular);
        glMaterialfv(GL_BACK, GL_SHININESS, high_shininess);
    }
    else if(frnt_Back==2)
    {
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   mat_ambient);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   mat_diffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  mat_specular);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, high_shininess);
    }

}

void drawpyramid(float a,float b,float c)
{
     GLfloat no_mat[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat mat_ambient[] = { 0.5*a, 0.5*b, 0.5*c, 1.0 };
    GLfloat mat_diffuse[] = { a, b, c, 1.0 };
    GLfloat mat_specular[] = { a, b, c, 1.0 };
    GLfloat mat_shininess[] = {60};

    glMaterialfv( GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv( GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv( GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv( GL_FRONT, GL_SHININESS, mat_shininess);

    glBegin(GL_TRIANGLES);
    for (GLint i = 0; i <4; i++)
    {

        getNormal3p(v_pyramid[p_Indices[i][0]][0], v_pyramid[p_Indices[i][0]][1], v_pyramid[p_Indices[i][0]][2],
                    v_pyramid[p_Indices[i][1]][0], v_pyramid[p_Indices[i][1]][1], v_pyramid[p_Indices[i][1]][2],
                    v_pyramid[p_Indices[i][2]][0], v_pyramid[p_Indices[i][2]][1], v_pyramid[p_Indices[i][2]][2]);

        glVertex3fv(&v_pyramid[p_Indices[i][0]][0]);glTexCoord2f(1,1);
        glVertex3fv(&v_pyramid[p_Indices[i][1]][0]);glTexCoord2f(1,0);
        glVertex3fv(&v_pyramid[p_Indices[i][2]][0]);glTexCoord2f(0,0);
    }
    glEnd();

    glBegin(GL_QUADS);
    for (GLint i = 0; i <1; i++)
    {
        //glColor3f(colors[4][0],colors[4][1],colors[4][2]);
        getNormal3p(v_pyramid[quadindices[i][0]][0], v_pyramid[quadindices[i][0]][1], v_pyramid[quadindices[i][0]][2],
                    v_pyramid[quadindices[i][1]][0], v_pyramid[quadindices[i][1]][1], v_pyramid[quadindices[i][1]][2],
                    v_pyramid[quadindices[i][2]][0], v_pyramid[quadindices[i][2]][1], v_pyramid[quadindices[i][2]][2]);

        glVertex3fv(&v_pyramid[quadindices[i][0]][0]);
        glVertex3fv(&v_pyramid[quadindices[i][1]][0]);
        glVertex3fv(&v_pyramid[quadindices[i][2]][0]);
        glVertex3fv(&v_pyramid[quadindices[i][3]][0]);

    }
    glEnd();


}

void drawroadcube(float a,float b,float c)
{
    //glColor3f(1,0,0);

 GLfloat no_mat[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat mat_ambient[] = { 0.5*a, 0.5*b, 0.5*c, 1.0 };
    GLfloat mat_diffuse[] = { a, b, c, 1.0 };
    GLfloat mat_specular[] = { a, b, c, 1.0 };
    GLfloat mat_shininess[] = {60};

    glMaterialfv( GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv( GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv( GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv( GL_FRONT, GL_SHININESS, mat_shininess);

    glBegin(GL_QUADS);
    for (GLint i = 0; i <6; i++)
    {

         glColor3f(a,b,c);

        getNormal3p(v_cube[quadIndices[i][0]][0], v_cube[quadIndices[i][0]][1], v_cube[quadIndices[i][0]][2],
                    v_cube[quadIndices[i][1]][0], v_cube[quadIndices[i][1]][1], v_cube[quadIndices[i][1]][2],
                    v_cube[quadIndices[i][2]][0], v_cube[quadIndices[i][2]][1], v_cube[quadIndices[i][2]][2]



                   );


        // setcolor();
        glVertex3fv(&v_cube[quadIndices[i][0]][0]);glTexCoord2f(0.5,0.5);
        glVertex3fv(&v_cube[quadIndices[i][1]][0]);glTexCoord2f(0.5,0);
        glVertex3fv(&v_cube[quadIndices[i][2]][0]);glTexCoord2f(0,0);
        glVertex3fv(&v_cube[quadIndices[i][3]][0]);glTexCoord2f(0,0.5);
    }
    glEnd();


}


void drawcube(float a,float b,float c)
{
    //glColor3f(1,0,0);

 GLfloat no_mat[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat mat_ambient[] = { 0.5*a, 0.5*b, 0.5*c, 1.0 };
    GLfloat mat_diffuse[] = { a, b, c, 1.0 };
    GLfloat mat_specular[] = { a, b, c, 1.0 };
    GLfloat mat_shininess[] = {60};

    glMaterialfv( GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv( GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv( GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv( GL_FRONT, GL_SHININESS, mat_shininess);

    glBegin(GL_QUADS);
    for (GLint i = 0; i <6; i++)
    {

         glColor3f(a,b,c);

        getNormal3p(v_cube[quadIndices[i][0]][0], v_cube[quadIndices[i][0]][1], v_cube[quadIndices[i][0]][2],
                    v_cube[quadIndices[i][1]][0], v_cube[quadIndices[i][1]][1], v_cube[quadIndices[i][1]][2],
                    v_cube[quadIndices[i][2]][0], v_cube[quadIndices[i][2]][1], v_cube[quadIndices[i][2]][2]



                   );


        // setcolor();
        glVertex3fv(&v_cube[quadIndices[i][0]][0]);glTexCoord2f(1,1);
        glVertex3fv(&v_cube[quadIndices[i][1]][0]);glTexCoord2f(1,0);
        glVertex3fv(&v_cube[quadIndices[i][2]][0]);glTexCoord2f(0,0);
        glVertex3fv(&v_cube[quadIndices[i][3]][0]);glTexCoord2f(0,1);
    }
    glEnd();


}


void drawrectangle()
{
    //glColor3f(1,0,0);

GLfloat no_mat[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat mat_ambient[] = { 0.5, 0.5, 0.5, 1.0 };
    GLfloat mat_diffuse[] = {  1, 1, 1, 1.0 };
    GLfloat mat_specular[] = { 1, 1, 1, 1.0 };
    GLfloat mat_shininess[] = {60};

    glMaterialfv( GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv( GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv( GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv( GL_FRONT, GL_SHININESS, mat_shininess);

    glBegin(GL_QUADS);
    for (GLint i = 0; i <6; i++)
    {

         //glColor3f(a,b,c);

        getNormal3p(v_rectangle[quadIndices[i][0]][0], v_rectangle[quadIndices[i][0]][1], v_rectangle[quadIndices[i][0]][2],
                    v_rectangle[quadIndices[i][1]][0], v_rectangle[quadIndices[i][1]][1], v_rectangle[quadIndices[i][1]][2],
                    v_rectangle[quadIndices[i][2]][0], v_rectangle[quadIndices[i][2]][1], v_rectangle[quadIndices[i][2]][2]



                   );


        // setcolor();
        glVertex3fv(&v_rectangle[quadIndices[i][0]][0]);glTexCoord2f(1,1);
        glVertex3fv(&v_rectangle[quadIndices[i][1]][0]);glTexCoord2f(1,0);
        glVertex3fv(&v_rectangle[quadIndices[i][2]][0]);glTexCoord2f(0,0);
        glVertex3fv(&v_rectangle[quadIndices[i][3]][0]);glTexCoord2f(0,1);
    }
    glEnd();


}

void tunnel(float a,float b,float c){

int L=32;
int nt=60;
int ntheta=60;
double rotation_angle=3.14159265389;
GLfloat ctrlpoints_funnel[L+1][3] =
{
{9.05,1.425,0},{8.425,1.4,0},
{7.625,1.375,0},{6.925,1.4,0},
{6.175,1.6,0},{5.3,1.95,0},
{4.325,2.425,0},{3.4,2.9,0},
{2.525,3.275,0},{1.425,3.575,0},
{0.475,3.75,0},{-0.425,3.825,0},
{-1.2,3.85,0},{-1.75,3.875,0},
{-1.75,3.475,0},{-1.7,2.975,0},
{-1.7,2.15,0},{-1.7,1.425,0},
{-1.7,0.7,0},{-1.775,-0.2,0},{-1.775,-0.8,0}


};

glPushMatrix();

glTranslated(0,-1,0);
    glRotatef( 90, 0.0, 0.0, 1.0);
   //
    glGetDoublev( GL_MODELVIEW_MATRIX, modelview ); //get the modelview info

    matColor(a,b,c,20);   // front face color
    matColor(a,b,c,20,1); // back face color
    glScaled(0.5,0.5,0.5);

    funnelBezier(L,nt,ntheta,ctrlpoints_funnel,rotation_angle);
    glPopMatrix();
}

void tire (float a,float b,float c){

int L=12;
int nt=60;
int ntheta=60;
double rotation_angle=3.14159265389;
GLfloat ctrlpoints_funnel[L+1][3] =
{
{-1.0815,3.25404,0},{-0.306167,3.23054,0},{0.328194,3.20705,0},{1.24449,3.18355,0},
{1.15051,2.66667,0},{0.539648,2.59618,0},{-0.212188,2.57269,0},{-0.917034,2.57269,0},
{-1.43392,2.57269,0},{-1.83333,2.73715,0},{-2.02129,2.99559,0},{-1.45,3.15,0},{-1.075,3.25,0}

};

glPushMatrix();

glTranslated(0,-1,0);
    glRotatef( 90, 0.0, 0.0, 1.0);
   //
    glGetDoublev( GL_MODELVIEW_MATRIX, modelview ); //get the modelview info

    matColor(a,b,c,20);   // front face color
    matColor(a,b,c,20,1); // back face color
    glScaled(0.5,0.5,0.5);

    funnelBezier(L,nt,ntheta,ctrlpoints_funnel,rotation_angle);
    glPopMatrix();

}

void wall()
{

/////ceiling
//    glPushMatrix();
//    glTranslated(2,6,-1);
//    glScaled(16,.2,13);
//    drawcube(0.529, 0.808, 0.922);
//    glPopMatrix();
//
//
//
//
///left




//    ///right wall
//    glPushMatrix();
//    glTranslated(18,-10,-1);
//    glRotated(270,0,0,1);
//
//    glScaled(16,.2,13);
//    drawcube(0.529, 0.808, 0.922);
//    glPopMatrix();
///backwall
 glEnable(GL_TEXTURE_2D);
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D,3);
    glTranslated(1.9,-9,-100);
    glRotated(180,1,0,0);

    glScaled(900,900,.2);
    drawcube(0.529, 0.808, 0.922);
    glPopMatrix();
     glDisable(GL_TEXTURE_2D);


      glEnable(GL_TEXTURE_2D);
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D,3);

    glTranslated(1000,400,0);
    glRotated(90,0,1,0);

    glScaled(900,900,.2);
    drawcube(0.529, 0.808, 0.922);
    glPopMatrix();
     glDisable(GL_TEXTURE_2D);


      glEnable(GL_TEXTURE_2D);
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D,3);

    glTranslated(-1000,400,0);
    glRotated(90,0,1,0);

    glScaled(900,900,.2);
    drawcube(0.529, 0.808, 0.922);
    glPopMatrix();
     glDisable(GL_TEXTURE_2D);


     ///floor
     glEnable(GL_TEXTURE_2D);

    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D,1);
    glTranslated(2,-26,-25);
    glRotated(180,0,0,1);

    glScaled(500,.2,500);
    drawcube(1.000, 1.000, 1.000);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);


}
void light1(){

    GLfloat no_light[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat light_ambient[]  = {1,1,1, 1.0};
    GLfloat light_diffuse[]  = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_position[] ={-50,50,0, 1.0};

    if(light1_flag)
        glEnable( GL_LIGHT0);
    else
        glDisable(GL_LIGHT0);
    glLightfv( GL_LIGHT0, GL_POSITION, light_position);


    if(a1)
    {
        glLightfv( GL_LIGHT0, GL_AMBIENT, light_ambient);
    }
    else
    {
        glLightfv( GL_LIGHT0, GL_AMBIENT, no_light);

    }

    if(d1)
    {
        glLightfv( GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    }
    else
    {
        glLightfv( GL_LIGHT0, GL_DIFFUSE, no_light);

    }
    if(s1)
    {
        glLightfv( GL_LIGHT0, GL_SPECULAR, light_specular);
    }
    else
    {
        glLightfv( GL_LIGHT0, GL_SPECULAR, no_light);

    }




}



void light2()
{
    GLfloat no_light[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat light_ambient[]  = {1,1,1,1.0};
    GLfloat light_diffuse[]  = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_position[] ={ 25,50,0, 1.0 };
    if(light2_flag)
        glEnable( GL_LIGHT1);
    else
        glDisable(GL_LIGHT1);
    glLightfv( GL_LIGHT1, GL_POSITION, light_position);


        if(a2)
    {
        glLightfv( GL_LIGHT1, GL_AMBIENT, light_ambient);
    }
    else
    {
        glLightfv( GL_LIGHT1, GL_AMBIENT, no_light);

    }

    if(d2)
    {
        glLightfv( GL_LIGHT1, GL_DIFFUSE, light_diffuse);
    }
    else
    {
        glLightfv( GL_LIGHT1, GL_DIFFUSE, no_light);

    }
    if(s2)
    {
        glLightfv( GL_LIGHT1, GL_SPECULAR, light_specular);
    }
    else
    {
        glLightfv( GL_LIGHT1, GL_SPECULAR, no_light);

    }




}
void light3()
{
     GLfloat no_light[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat light_ambient[]  = {1,1,1, 1.0};
    GLfloat light_diffuse[]  = { 1,1,1, 1.0 };
    GLfloat light_specular[] ={ 1,1,1, 1.0 };
    GLfloat light_position[] = { -1000,10,300, 1.0 };

    if(light3_flag)
        glEnable( GL_LIGHT2);
    else
        glDisable(GL_LIGHT2);

    glLightfv( GL_LIGHT2, GL_POSITION, light_position);



    if(a3)
    {
        glLightfv( GL_LIGHT2, GL_AMBIENT, light_ambient);
    }
    else
    {
        glLightfv( GL_LIGHT2, GL_AMBIENT, no_light);

    }

    if(d3)
    {
        glLightfv( GL_LIGHT2, GL_DIFFUSE, light_diffuse);
    }
    else
    {
        glLightfv( GL_LIGHT2, GL_DIFFUSE, no_light);

    }
    if(s3)
    {
        glLightfv( GL_LIGHT2, GL_SPECULAR, light_specular);
    }
    else
    {
        glLightfv( GL_LIGHT2, GL_SPECULAR, no_light);

    }



      GLfloat spot_direction[] = { -24+50,10,10 };
    glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, spot_direction);
    glLightf( GL_LIGHT2, GL_SPOT_CUTOFF, 90);




}
void light4()
{
     GLfloat no_light[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat light_ambient[]  = {1,1,1, 1.0};
    GLfloat light_diffuse[]  = {1,1,1, 1.0 };
    GLfloat light_specular[] ={1,1,1, 1.0 };
    GLfloat light_position[] = { 8,10,90, 1.0 };

     if(light4_flag)
        glEnable( GL_LIGHT3);
    else
        glDisable(GL_LIGHT3);

    glLightfv( GL_LIGHT3, GL_POSITION, light_position);

if(a3)
    {
        glLightfv( GL_LIGHT3, GL_AMBIENT, light_ambient);
    }
    else
    {
        glLightfv( GL_LIGHT3, GL_AMBIENT, no_light);

    }

    if(d3)
    {
        glLightfv( GL_LIGHT3, GL_DIFFUSE, light_diffuse);
    }
    else
    {
        glLightfv( GL_LIGHT3, GL_DIFFUSE, no_light);

    }
    if(s3)
    {
        glLightfv( GL_LIGHT3, GL_SPECULAR, light_specular);
    }
    else
    {
        glLightfv( GL_LIGHT3, GL_SPECULAR, no_light);

    }





      GLfloat spot_direction[] = { -24,10,10 };
    glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, spot_direction);
    glLightf( GL_LIGHT3, GL_SPOT_CUTOFF, 90);





}


void bridge()
{


glEnable(GL_TEXTURE_2D);
    glPushMatrix();
glBindTexture(GL_TEXTURE_2D,2);
    glTranslated(-4.86,4,0);
     glRotated(angle,0,0,1);
     glTranslated(2,0,0);
    glScaled(2.9,0.3,8.0);
    drawroadcube(1,1,1);
    glPopMatrix();
glDisable(GL_TEXTURE_2D);

glEnable(GL_TEXTURE_2D);
    glPushMatrix();
glBindTexture(GL_TEXTURE_2D,2);
    glTranslated(4.86,4,0);
     glRotated(-angle,0,0,1);
      glTranslated(-2,0,0);
    glScaled(2.9,0.3,8.0);

    drawroadcube(1,1,1);

    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
  glEnable(GL_TEXTURE_2D);glPushMatrix();glBindTexture(GL_TEXTURE_2D,2);
     glTranslated(-12.75,4,0);
    glScaled(7,0.3,8.0);
    drawroadcube(1,1,1);

    glPopMatrix();glDisable(GL_TEXTURE_2D);
  glEnable(GL_TEXTURE_2D);glPushMatrix();glBindTexture(GL_TEXTURE_2D,2);
     glTranslated(12.75,4,0);
    glScaled(7,0.3,8.0);
    drawroadcube(1,1,1);

    glPopMatrix();glDisable(GL_TEXTURE_2D);


     glEnable(GL_TEXTURE_2D);glPushMatrix();glBindTexture(GL_TEXTURE_2D,2);
     glTranslated(-26.8,4,0);
    glScaled(7,0.3,8.0);
    drawroadcube(1,1,1);

    glPopMatrix();glDisable(GL_TEXTURE_2D);

  glEnable(GL_TEXTURE_2D);glPushMatrix();glBindTexture(GL_TEXTURE_2D,2);
     glTranslated(26.7,4,0);
    glScaled(7,0.3,8.0);
    drawroadcube(1,1,1);

   glPopMatrix();glDisable(GL_TEXTURE_2D);



     glEnable(GL_TEXTURE_2D);glPushMatrix();glBindTexture(GL_TEXTURE_2D,2);
     glTranslated(-54,4,0);
    glScaled(20,0.3,8.0);
    drawroadcube(1,1,1);

    glPopMatrix();glDisable(GL_TEXTURE_2D);

  glEnable(GL_TEXTURE_2D);glPushMatrix();glBindTexture(GL_TEXTURE_2D,2);
     glTranslated(54,4,0);
    glScaled(20,0.3,8.0);
    drawroadcube(1,1,1);

    glPopMatrix();glDisable(GL_TEXTURE_2D);










    //table front leg
    //front
    //


      glEnable(GL_TEXTURE_2D);glPushMatrix();glBindTexture(GL_TEXTURE_2D,4);
    glTranslated(5.4,-2,-6.5);
    glScaled(0.5,6.1,0.5);
    drawcube(1,1,1);

     glPopMatrix();glDisable(GL_TEXTURE_2D);

       glEnable(GL_TEXTURE_2D);glPushMatrix();glBindTexture(GL_TEXTURE_2D,4);
    glTranslated(18,-2,-6.5);
    glScaled(0.5,6.1,0.5);
    drawcube(1,1,1);

     glPopMatrix();glDisable(GL_TEXTURE_2D);

         glEnable(GL_TEXTURE_2D);glPushMatrix();glBindTexture(GL_TEXTURE_2D,4);
    glTranslated(30,-2,-6.5);
    glScaled(0.5,6.1,0.5);
    drawcube(1,1,1);

     glPopMatrix();glDisable(GL_TEXTURE_2D);

      glEnable(GL_TEXTURE_2D);glPushMatrix();glBindTexture(GL_TEXTURE_2D,4);
    glTranslated(-5.4,-2,-6.5);
    glScaled(0.5,6.1,0.5);
    drawcube(1,1,1);

     glPopMatrix();glDisable(GL_TEXTURE_2D);


    glEnable(GL_TEXTURE_2D);glPushMatrix();glBindTexture(GL_TEXTURE_2D,4);
    glTranslated(-18,-2,-6.5);
    glScaled(0.5,6.1,0.5);
    drawcube(1,1,1);

     glPopMatrix();glDisable(GL_TEXTURE_2D);

       glEnable(GL_TEXTURE_2D);glPushMatrix();glBindTexture(GL_TEXTURE_2D,4);
    glTranslated(-30,-2,-6.5);
    glScaled(0.5,6.1,0.5);
    drawcube(1,1,1);

    glPopMatrix();glDisable(GL_TEXTURE_2D);
      glEnable(GL_TEXTURE_2D);glPushMatrix();glBindTexture(GL_TEXTURE_2D,4);
    glTranslated(-5.4,-2,6.8);
    glScaled(0.5,6.1,0.5);
    drawcube(1,1,1);

     glPopMatrix();glDisable(GL_TEXTURE_2D);


        glEnable(GL_TEXTURE_2D);glPushMatrix();glBindTexture(GL_TEXTURE_2D,4);
    glTranslated(-18,-2,6.5);
    glScaled(0.5,6.1,0.5);
    drawcube(1,1,1);

     glPopMatrix();glDisable(GL_TEXTURE_2D);


        glEnable(GL_TEXTURE_2D);glPushMatrix();glBindTexture(GL_TEXTURE_2D,4);
    glTranslated(-30,-2,6.5);
    glScaled(0.5,6.1,0.5);
    drawcube(1,1,1);

     glPopMatrix();glDisable(GL_TEXTURE_2D);

      glEnable(GL_TEXTURE_2D);glPushMatrix();glBindTexture(GL_TEXTURE_2D,4);
    glTranslated(5.4,-2,6.8);
    glScaled(0.5,6.1,0.5);
    drawcube(1,1,1);

     glPopMatrix();glDisable(GL_TEXTURE_2D);

        glEnable(GL_TEXTURE_2D);glPushMatrix();glBindTexture(GL_TEXTURE_2D,4);
    glTranslated(18,-2,6.5);
    glScaled(0.5,6.1,0.5);
    drawcube(1,1,1);

     glPopMatrix();glDisable(GL_TEXTURE_2D);

       glEnable(GL_TEXTURE_2D);glPushMatrix();glBindTexture(GL_TEXTURE_2D,4);
    glTranslated(30,-2,6.5);
    glScaled(0.5,6.1,0.5);
    drawcube(1,1,1);

     glPopMatrix();glDisable(GL_TEXTURE_2D);





}

void car1(){

 glEnable(GL_TEXTURE_2D);glPushMatrix(); glBindTexture(GL_TEXTURE_2D,12);
glScaled(100,30,70);
drawcube(1,1,1);
glPopMatrix();glDisable(GL_TEXTURE_2D);

 glEnable(GL_TEXTURE_2D);glPushMatrix(); glBindTexture(GL_TEXTURE_2D,13);
glTranslated(27,60,0);
glScaled(70,30,70);
drawcube(1,1,1);
glPopMatrix();glDisable(GL_TEXTURE_2D);

 glEnable(GL_TEXTURE_2D);glPushMatrix(); glBindTexture(GL_TEXTURE_2D,8);
glTranslated(27,60,0);
glRotated(90,0,1,0);
glScaled(80,20,20);
drawcube(0,0,1);
glPopMatrix();glDisable(GL_TEXTURE_2D);






glEnable(GL_TEXTURE_2D);glPushMatrix(); glBindTexture(GL_TEXTURE_2D,11);
glTranslated(40,-22,70);
glScaled(1.5,1.5,1.5);

glutSolidTorus(4,20,20,20);
glPopMatrix();glDisable(GL_TEXTURE_2D);

 glEnable(GL_TEXTURE_2D);glPushMatrix(); glBindTexture(GL_TEXTURE_2D,11);
glTranslated(-40,-22,70);
glScaled(1.5,1.5,1.5);
glutSolidTorus(4,20,20,20);
glPopMatrix();glDisable(GL_TEXTURE_2D);

 glEnable(GL_TEXTURE_2D);glPushMatrix(); glBindTexture(GL_TEXTURE_2D,11);
glTranslated(-40,-22,-70.5);
glScaled(1.5,1.5,1.5);
glutSolidTorus(4,20,20,20);
glPopMatrix();glDisable(GL_TEXTURE_2D);

 glEnable(GL_TEXTURE_2D);glPushMatrix(); glBindTexture(GL_TEXTURE_2D,11);
glTranslated(40,-22,-70.5);
glScaled(1.5,1.5,1.5);
glutSolidTorus(4,20,20,20);
glPopMatrix();glDisable(GL_TEXTURE_2D);



}

void sun(float a,float b,float c){

int L=33;
int nt=60;
int ntheta=40;
double rotation_angle=180;
GLfloat ctrlpoints_sun[L+1][3] =
{
 {3.9699,4.75771,0},{4.74523,4.07636,0},
 {5.56755,3.32452,0},{6.08443,2.99559,0},
 {6.36637,2.78414,0},{6.38987,2.14978,0},
 {6.43686,1.39794,0},{6.41336,0.763583,0},
 {6.43686,0.152716,0},{6.43686,-0.50514,0},
 {6.46035,-1.20999,0},{6.46035,-1.67988,0},
 {6.46035,-2.24376,0},{6.46035,-2.45521,0},
 {6.46035,-2.54919,0},{6.10793,-2.54919,0},
 {5.75551,-2.54919,0},{5.3091,-2.57269,0},
 {4.81571,-2.59618,0},{4.48678,-2.61968,0},
 {4.11087,-2.61968,0},{3.92291,-2.61968,0},
 {3.87592,-2.61968,0},{3.87592,-2.29075,0},
 {3.87592,-1.72687,0},{3.87592,-1.04552,0},{3.87592,-0.36417,0},{3.87592,0.223201,0},
 {3.89941,0.881057,0},{3.87592,1.37445,0},{3.92291,2.19677,0},{3.92291,2.80764,0},{3.9699,3.32452,0},
 {3.9464,3.60646,0}

};

glPushMatrix();

glTranslated(500,2000,-2500);
    glRotatef( 90, 0.0, 1.0, 0.0);
   //
    glGetDoublev( GL_MODELVIEW_MATRIX, modelview ); //get the modelview info

    matColor(a,b,c,20);   // front face color
    matColor(a,b,c,20,1); // back face color
    glScaled(100,100,100);

    funnelBezier(L,nt,ntheta,ctrlpoints_sun,rotation_angle);
    glPopMatrix();


}

void wing(float a,float b,float c);
void boat(float a,float b,float c){

int L=18;
int nt=60;
int ntheta=40;
double rotation_angle=3.14159265389;;
GLfloat ctrlpoints_boat[L+1][3] =

 {
{-5.34824,0.0587375,0},{-5.1289,0.55213,0},{-4.73408,1.20999,0},{-3.83477,1.96182,0},{-3.04514,2.43172,0},
{-1.86068,2.99559,0},{-0.89557,3.39501,0},{-0.0401301,3.67695,0},{1.05659,4.00587,0},{1.99976,4.09985,0},
{2.46038,4.09985,0},{2.50425,3.62996,0},{2.61393,2.94861,0},{2.61393,2.24376,0},{2.65779,1.51542,0},{2.70166,0.763583,0},
{2.70166,-0.0587371,0},{2.67973,-0.505139,0},{2.59199,-0.904552,0}
};





glPushMatrix();
glTranslated(-4500,-250,2500);
glRotatef( -90, 0, 1, 0);
    glRotatef( 90, 1, 0, 0);
   //
    glGetDoublev( GL_MODELVIEW_MATRIX, modelview ); //get the modelview info

    matColor(0.545, 0.271, 0.075,20);   // front face color
    matColor(1.000, 1.000, 0.000,20,1); // back face color
    glScaled(100,100,100);

   boatBezier(L,nt,ntheta,ctrlpoints_boat,rotation_angle);


    glPopMatrix();



}
void wing(float a,float b,float c){

int L=24;
int nt=20;
int ntheta=2;
double rotation_angle=3.14159265389;;
GLfloat ctrlpoints_boat[L+1][3] =

 {
{3.95,5.725,0},{3.475,5.625,0},{3,5.45,0},{2.525,5.2,0},{2.225,4.85,0},{1.95,4.4,0},{1.825,3.8,0},{1.75,2.875,0},
{1.775,2.1,0},{1.775,1.175,0},{1.8,0.375,0},{1.8,0.375,0},{1.8,0.275,0},{1.825,-0.15,0},{1.825,-0.75,0},{1.925,-1.3,0},
{1.975,-2.025,0},{2.05,-2.725,0},{2.075,-3.3,0},{2.15,-3.875,0},{2.35,-4.425,0},{2.8,-5,0},{3.35,-5.275,0},{3.8,-5.475,0},
{4.2,-5.625,0}

};

glPushMatrix();
glTranslated(-500,-200,2100);
glRotatef( -90, 0, 1, 0);
    glRotatef( 90, 1, 0, 0);
   //
    glGetDoublev( GL_MODELVIEW_MATRIX, modelview ); //get the modelview info

    matColor(a,b,c,20);   // front face color
    matColor(a,b,c,20,1); // back face color
    glScaled(70,50,50);

   funnelBezier(L,nt,ntheta,ctrlpoints_boat,rotation_angle);
    glPopMatrix();




}
void complete_boat(){
glPushMatrix();
glTranslated(-4000,-70,110);
glScaled(1,1,1);
wing(0.502, 0.502, 0.000);
glPopMatrix();
boat(0.741, 0.718, 0.420);

}

void car2(){

glEnable(GL_TEXTURE_2D);glPushMatrix(); glBindTexture(GL_TEXTURE_2D,12);
glScaled(100,30,70);
drawcube(1,1,1);
glPopMatrix();glDisable(GL_TEXTURE_2D);

glEnable(GL_TEXTURE_2D);glPushMatrix(); glBindTexture(GL_TEXTURE_2D,13);
glTranslated(110-5-10-40-50,75,0);
glScaled(90,50,70);
glColor3f(0,1,0);
glRotated(90,1,0,0);
drawrectangle();
glPopMatrix();glDisable(GL_TEXTURE_2D);

glEnable(GL_TEXTURE_2D);glPushMatrix(); glBindTexture(GL_TEXTURE_2D,8);
glTranslated(27-20,60,0);
glRotated(90,0,1,0);
glScaled(70,20,20);
drawcube(0,0,1);
glPopMatrix();glDisable(GL_TEXTURE_2D);




glEnable(GL_TEXTURE_2D);glPushMatrix(); glBindTexture(GL_TEXTURE_2D,11);
glTranslated(40,-22,70);
glScaled(1.5,1.5,1.5);
glutSolidTorus(4,20,20,20);
glPopMatrix();glDisable(GL_TEXTURE_2D);

glEnable(GL_TEXTURE_2D);glPushMatrix(); glBindTexture(GL_TEXTURE_2D,11);
glTranslated(-40,-22,70);
glScaled(1.5,1.5,1.5);
glutSolidTorus(4,20,20,20);
glPopMatrix();glDisable(GL_TEXTURE_2D);

glEnable(GL_TEXTURE_2D);glPushMatrix(); glBindTexture(GL_TEXTURE_2D,11);
glTranslated(-40,-22,-70.5);
glScaled(1.5,1.5,1.5);

glutSolidTorus(4,20,20,20);
glPopMatrix();glDisable(GL_TEXTURE_2D);

glEnable(GL_TEXTURE_2D);glPushMatrix(); glBindTexture(GL_TEXTURE_2D,11);
glTranslated(40,-22,-70.5);
glScaled(1.5,1.5,1.5);

glutSolidTorus(4,20,20,20);
glPopMatrix();glDisable(GL_TEXTURE_2D);



}




void bus(){



glEnable(GL_TEXTURE_2D);glPushMatrix(); glBindTexture(GL_TEXTURE_2D,14);
glScaled(100,70,70);
glTranslated(0,0.8,0);
drawcube(1,1,1);
glPopMatrix();glDisable(GL_TEXTURE_2D);

 glEnable(GL_TEXTURE_2D);glPushMatrix(); glBindTexture(GL_TEXTURE_2D,8);
glTranslated(40,80,80);
glScaled(20,20,20);
drawcube(1,1,1);
glPopMatrix();glDisable(GL_TEXTURE_2D);
glEnable(GL_TEXTURE_2D);glPushMatrix(); glBindTexture(GL_TEXTURE_2D,8);
glTranslated(-40,80,80);
glScaled(20,20,20);
drawcube(1,1,1);
glPopMatrix();glDisable(GL_TEXTURE_2D);





glEnable(GL_TEXTURE_2D);glPushMatrix(); glBindTexture(GL_TEXTURE_2D,11);
glTranslated(40,-22,70);
glScaled(1.5,1.5,1.5);
glutSolidTorus(4,20,20,20);
glPopMatrix();glDisable(GL_TEXTURE_2D);

glEnable(GL_TEXTURE_2D);glPushMatrix(); glBindTexture(GL_TEXTURE_2D,11);
glTranslated(-40,-22,70);
glScaled(1.5,1.5,1.5);
glutSolidTorus(4,20,20,20);
glPopMatrix();glDisable(GL_TEXTURE_2D);

glEnable(GL_TEXTURE_2D);glPushMatrix(); glBindTexture(GL_TEXTURE_2D,11);
glTranslated(-40,-22,-70.5);
glScaled(1.5,1.5,1.5);
glutSolidTorus(4,20,20,20);
glPopMatrix();glDisable(GL_TEXTURE_2D);

glEnable(GL_TEXTURE_2D);glPushMatrix(); glBindTexture(GL_TEXTURE_2D,11);
glTranslated(40,-22,-70.5);
glScaled(1.5,1.5,1.5);

glutSolidTorus(4,20,20,20);
glPopMatrix();glDisable(GL_TEXTURE_2D);



}




void traffic(){


glEnable(GL_TEXTURE_2D);glPushMatrix();glBindTexture(GL_TEXTURE_2D,5);
glScaled(5,15,7);
drawcube(1,1,1);
glPopMatrix();glDisable(GL_TEXTURE_2D);


glPushMatrix();
//glColor3f(1,0,0);
glTranslated(0,10,10);
glScaled(2,2,2);
GLfloat no_light_1[] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat mat_ambient_1[] = { 1*0.3,0,0, 1.0 };
    GLfloat mat_diffuse_1[] = { 1,0,0, 1.0 };
    GLfloat mat_specular_1[] = { 1,0,0, 1.0 };
    GLfloat mat_shininess_1[] = {60};
     if(red){
    glMaterialfv( GL_FRONT, GL_AMBIENT, mat_ambient_1);
    glMaterialfv( GL_FRONT, GL_DIFFUSE, mat_diffuse_1);
    glMaterialfv( GL_FRONT, GL_SPECULAR, mat_specular_1);
    glMaterialfv( GL_FRONT, GL_SHININESS, mat_shininess_1);}
    else{
         glMaterialfv( GL_FRONT, GL_AMBIENT, mat_ambient_1);
    glMaterialfv( GL_FRONT, GL_DIFFUSE, no_light_1);
    glMaterialfv( GL_FRONT, GL_SPECULAR, no_light_1);
    glMaterialfv( GL_FRONT, GL_SHININESS, mat_shininess_1);

    }
glutSolidSphere(2,100,100);
glPopMatrix();

glPushMatrix();
//glColor3f(0,1,0);
glTranslated(0,-5,10);
glScaled(2,2,2);
GLfloat no_light_2[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat mat_ambient_2[] = { 0,1*0.3,0, 1.0 };
    GLfloat mat_diffuse_2[] = { 0,1,0, 1.0 };
    GLfloat mat_specular_2[] = { 0,1,0, 1.0 };
    GLfloat mat_shininess_2[] = {60};

 if(green){
    glMaterialfv( GL_FRONT, GL_AMBIENT, mat_ambient_2);
    glMaterialfv( GL_FRONT, GL_DIFFUSE, mat_diffuse_2);
    glMaterialfv( GL_FRONT, GL_SPECULAR, mat_specular_2);
    glMaterialfv( GL_FRONT, GL_SHININESS, mat_shininess_2);}
    else{
         glMaterialfv( GL_FRONT, GL_AMBIENT, mat_ambient_2);
    glMaterialfv( GL_FRONT, GL_DIFFUSE, no_light_2);
    glMaterialfv( GL_FRONT, GL_SPECULAR, no_light_2);
    glMaterialfv( GL_FRONT, GL_SHININESS, mat_shininess_2);



    }



glutSolidSphere(2,100,100);
glPopMatrix();


}
void ship()
{     ///base
	glEnable(GL_TEXTURE_2D);glPushMatrix(); glBindTexture(GL_TEXTURE_2D,6);
	glTranslated(0,4,0);
	glScalef(7,3,7);
     glRotated(-90,1,0,0);
     glColor3f(1,1,1);
	drawrectangle();
	glPopMatrix();glDisable(GL_TEXTURE_2D);

	glEnable(GL_TEXTURE_2D);glPushMatrix(); glBindTexture(GL_TEXTURE_2D,6);
	glTranslated(0,1.76,6);
	glRotated(60,1,0,0);

	glScaled(7,3.5,4);
	//glColor3f(1,0,0);

	drawpyramid(1,1,1);
glPopMatrix();glDisable(GL_TEXTURE_2D);
glEnable(GL_TEXTURE_2D);glPushMatrix(); glBindTexture(GL_TEXTURE_2D,6);
//glTranslated(0,0,0);
glRotated(180,0,1,0);
	glTranslated(0,1.76,6);
	glRotated(60,1,0,0);

	glScaled(7,3.5,4);
	//glColor3f(1,0,0);

	drawpyramid(1,1,1);
glPopMatrix();glDisable(GL_TEXTURE_2D);
///upper base
	glEnable(GL_TEXTURE_2D);glPushMatrix(); glBindTexture(GL_TEXTURE_2D,9);
	glTranslated(0,8,0);
	glScaled(5,2,5);
	drawcube(1,1,1);
	glPopMatrix();glDisable(GL_TEXTURE_2D);

	glEnable(GL_TEXTURE_2D);glPushMatrix(); glBindTexture(GL_TEXTURE_2D,8);
	glTranslated(0,8,0);
	glScaled(5.5,1,1);
	drawcube(0.275, 0.510, 0.706);
	glPopMatrix();glDisable(GL_TEXTURE_2D);

	glEnable(GL_TEXTURE_2D);glPushMatrix(); glBindTexture(GL_TEXTURE_2D,8);
	glRotated(90,0,1,0);
	glTranslated(0,8,0);
	glScaled(5.5,1,1);
	drawcube(0.275, 0.510, 0.706);
	glPopMatrix();glDisable(GL_TEXTURE_2D);

	glEnable(GL_TEXTURE_2D);
	glPushMatrix();glBindTexture(GL_TEXTURE_2D,7);
	glTranslated(0,12.9,1.95);
	//glRotated(90,0,0,1);
	glScaled(1,1,0.75);

	tunnel(0.663, 0.663, 0.663);
//	drawcube(1,1,1);
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);

	glEnable(GL_TEXTURE_2D);glPushMatrix(); glBindTexture(GL_TEXTURE_2D,7);
	glTranslated(0,13,-2);
	//glRotated(90,0,0,1);
	glScaled(1,1.75,1);
	tunnel(0.502, 0.502, 0.502);
	glPopMatrix();glDisable(GL_TEXTURE_2D);


	glEnable(GL_TEXTURE_2D);glPushMatrix(); glBindTexture(GL_TEXTURE_2D,10);
	glTranslated(0,10,0);
	glScaled(3,1.5,3);
	drawcube(1,1,1);
	glPopMatrix();glDisable(GL_TEXTURE_2D);




}


void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

if(wired)
    {
        glPolygonMode( GL_FRONT, GL_LINE ) ;
        glPolygonMode( GL_BACK, GL_LINE ) ;

    }
    else
    {
        glPolygonMode( GL_FRONT,GL_FILL ) ;
        glPolygonMode( GL_BACK, GL_FILL ) ;
    }
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glFrustum(-5,5,-5,5, 4, 100000);
    //gluPerspective(60,1,5,100);
    //glOrtho(-5,5,-5,5, 4, 50);

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    gluLookAt(Exval,Eyval,Ezval,Dxval,Dyval,Dzval, 0,1,0);

    glViewport(0, 0, windowHeight, windowWidth);


    glPushMatrix();
    glScaled(50,25,50);
    wall();
    glPopMatrix();


    glPushMatrix();
    glTranslated(1,-200,-200);
    glScaled(200,50,80);
    bridge();
    glPopMatrix();

    glPushMatrix();
   glTranslated(bus_x,230,10);
    glScaled(4,4,1);
    bus();

    glPopMatrix();

    glPushMatrix();
   glTranslated(car1_tx,230,-450);
    glScaled(4,4,4);
    glRotated(180,0,1,0);
    car1();

    glPopMatrix();


    glPushMatrix();
   glTranslated(car2_tx,230,10);
    glScaled(4,4,4);
    car2();

    glPopMatrix();



glPushMatrix();
glTranslated(tx,-753,tz);
glScaled(50,50,50);
ship();
glPopMatrix();

glPushMatrix();
glTranslated(0,-9,boat_z);
complete_boat();
glPopMatrix();

glPushMatrix();
glTranslated(1500,250,-800);
glScaled(10,20,10);
traffic();
glPopMatrix();

glPushMatrix();
glTranslated(-1500,250,-800);
glScaled(10,20,10);
traffic();
glPopMatrix();

if(night==false){
glPushMatrix();

sun(1.000, 1.000, 0.000);
glPopMatrix();
}

 light1();

light2();
    light3();
    light4();



    glFlush();
    glutSwapBuffers();


}

void myKeyboardFunc( unsigned char key, int x, int y )
{
    switch ( key )
    {
       case '+':
        Ezval+=3;
        break;

    case '-':
        Ezval-=3;
        break;

    case 'p':
        Exval+=3;
        break;

    case 'o':
        Exval-=3;
        break;

    case 'y':
        Eyval+=3;
        break;

    case 'u':
        Eyval-=3;
        break;

    case 'P':
        Dxval+=3;
        break;

    case 'O':
        Dxval-=3;
        break;

    case 'Y':
        Dyval+=3;
        break;

    case 'U':
        Dyval-=3;
        break;
    case 'r':
          Dzval+=3;

        break;
    case 't':
         Dzval-=3;
        break;
    case 'q':
        boat_z-=3;
        break;
    case 'Q':
        boat_z+=3;
        break;
case 'w':
        wired=!wired;
        break;

    case 'd':

night=true;
 Dxval=2;
    Dyval=0;
    Dzval=0;
    Eyval=2533;
    Ezval=5500;
    Exval=500;
     bus_x=6500;
 car1_tx=-6500;
 car2_tx=8000;
 tx=180-60;
tz=3500;

       light1_flag=false;
 light2_flag=false;
light3_flag=true;
 light4_flag=true;
 red=false;
 green=true;
        break;

        case 'D':

night=false;
 Dxval=2;
    Dyval=0;
    Dzval=0;
    Eyval=2533;
    Ezval=5500;
    Exval=500;
     bus_x=6500;
 car1_tx=-6500;
 car2_tx=8000;
 tx=180-60;
tz=3500;

       light1_flag=true;
 light2_flag=true;
light3_flag=false;
 light4_flag=false;
 red=false;
 green=true;
        break;

//        case 's':
//       flagScale= !flagScale;
//        if (!flagScale)
//            flagScale=false;
//        else
//            flagScale=true;
//        break;

          case 'a':
              ship_flag=!ship_flag;
             // ship_flag=true;
        break;


case 'i':
    tz-=6;

    break;
case 'm':
        tz+=6;

        break;
case 'j':
tx-=6;

break;
case 'l':
tx+=6;

break;

    case'z':
    Dxval=0;
    Dyval=-500;
    Dzval=500;
    Eyval=-250;
    Ezval=5500-30;
    break;

    case'Z':
    Dxval=2;
    Dyval=0;
    Dzval=0;
    Eyval=2533;
    Ezval=5500;
    break;

    case'x':
    Dxval=0;
    Dyval=0;
    Dzval=0;
    Eyval=2000;
Ezval=5500-4095;
Exval=2;
bus_x=2000-2055;
car1_tx=-2300+500;
car2_tx=2000-500;



    break;
case'X':
    Dxval=2;
    Dyval=0;
    Dzval=0;
    Eyval=2533;
    Ezval=5500;
    Exval=500;
     bus_x=6500;
 car1_tx=-6500;
 car2_tx=8000;



    break;



 case'c':
    Dxval=-4000;
    Dyval=-70;
    Dzval=2000;
    Eyval=1200;
Ezval=5500-4095-500;
Exval=-3500;
break;

 case '1':
        light1_flag = !light1_flag;
        break;

    case '2':
        light2_flag = !light2_flag;
        break;

case '3':
        a1 = !a1;
       break;

    case '4':
        d1=!d1;
        break;
    case '5':
        s1=!s1;

        case '6':
        a2= !a2;
       break;

    case '7':
        d2=!d2;
        break;
    case '8':
        s2=!s2;

        case '9':
        a3= !a3;
       break;

    case '0':
        d3=!d3;
        break;
    case '[':
        s3=!s3;
         case ']':
       light3_flag=!light3_flag;
       light4_flag=!light4_flag;
break;


        break;
   case 27:	// Escape key
        exit(1);
    }
    glutPostRedisplay();
}




void LoadTexture(const char*filename,unsigned int ID)
{
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_2D, ID);
    glPixelStorei(GL_UNPACK_ALIGNMENT, ID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    BmpLoader bl(filename);
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, bl.iWidth, bl.iHeight, GL_RGB, GL_UNSIGNED_BYTE, bl.textureData );
}



int main (int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

cout<<"press a : start and stop animation"<<endl;
    cout<<"press d: night mode"<<endl;
    cout<<"press D: day mode"<<endl;
    cout<<"press w: wired mode"<<endl;
    cout<<"move boat front : q \t move boat back : Q"<<endl;
    cout<<"move ship front : i \t move boat back : m"<<endl;
    cout<<"move ship right: l \t move ship left: j"<<endl;
    cout<<"focus ship: z \t reset: Z"<<endl;
    cout<<"focus vehicles : x \t reset: X"<<endl;
    cout<<"focus boat: c \t reset: X"<<endl;
        cout<<"light 1 on-off : 1 \t light 2 on-off: 2"<<endl;
            cout<<"light 1 ambient on-off : 3 \t light 1 diffuse on-off:4 \t light 1 specular on-off: 5"<<endl;
                cout<<"light 2 ambient on-off : 6 \t light 2 diffuse on-off:7\t light 2 specular on-off: 8"<<endl;
 cout<<"light 3 and 4 on-off : ]"<<endl;
            cout<<"light 3 and 4 ambient on-off : 9  \t light 3 and 4 diffuse on-off:0 \t light 3 and 4 specular on-off: ["<<endl;


    cout<<"Eye position"<<endl;
    cout<<"zoom in: - \t zoom out: +"<<endl;
    cout<<"move camera right: p \t move camera left: o"<<endl;
    cout<<"move camera up: u \t move camera left: y"<<endl<<endl;

    cout<<"reference point directions"<<endl;
    cout<<"reference point X directions: (for  positive):P  \t for (neg):O"<<endl;
    cout<<"reference point Y directions: (for  positive):Y  \t for (neg):U"<<endl;
    cout<<"reference point Z directions: (for  positive):r \t for (neg):t"<<endl;




    glutInitWindowPosition(0,0);
    glutInitWindowSize(windowHeight, windowWidth);
    glutCreateWindow("1607110");

   LoadTexture("C:\\Users\\HP\\Desktop\\1607110_Vertical lift bridge with traffic control simulation\\sea.bmp",1);
    LoadTexture("C:\\Users\\HP\\Desktop\\1607110_Vertical lift bridge with traffic control simulation\\road.bmp",2);
    LoadTexture("C:\\Users\\HP\\Desktop\\1607110_Vertical lift bridge with traffic control simulation\\sky.bmp",3);
 LoadTexture("C:\\Users\\HP\\Desktop\\1607110_Vertical lift bridge with traffic control simulation\\piller.bmp",4);
 LoadTexture("C:\\Users\\HP\\Desktop\\1607110_Vertical lift bridge with traffic control simulation\\wood.bmp",5);
  LoadTexture("C:\\Users\\HP\\Desktop\\1607110_Vertical lift bridge with traffic control simulation\\iron.bmp",6);
  LoadTexture("C:\\Users\\HP\\Desktop\\1607110_Vertical lift bridge with traffic control simulation\\funnel1.bmp",7);
  LoadTexture("C:\\Users\\HP\\Desktop\\1607110_Vertical lift bridge with traffic control simulation\\window.bmp",8);
LoadTexture("C:\\Users\\HP\\Desktop\\1607110_Vertical lift bridge with traffic control simulation\\upper_base.bmp",9);
LoadTexture("C:\\Users\\HP\\Desktop\\1607110_Vertical lift bridge with traffic control simulation\\top.bmp",10);
LoadTexture("C:\\Users\\HP\\Desktop\\1607110_Vertical lift bridge with traffic control simulation\\wheel.bmp",11);
LoadTexture("C:\\Users\\HP\\Desktop\\1607110_Vertical lift bridge with traffic control simulation\\car_base.bmp",12);
LoadTexture("C:\\Users\\HP\\Desktop\\1607110_Vertical lift bridge with traffic control simulation\\car_top.bmp",13);
LoadTexture("C:\\Users\\HP\\Desktop\\1607110_Vertical lift bridge with traffic control simulation\\bus.bmp",14);
//LoadTexture("C:\\Users\\HP\\Desktop\\1607110_Vertical lift bridge with traffic control simulation\\city.bmp",15);







   glShadeModel( GL_SMOOTH );
    glEnable( GL_DEPTH_TEST );
    glEnable(GL_NORMALIZE);
    glEnable(GL_LIGHTING);
 glutReshapeFunc(resize);
    glutKeyboardFunc(myKeyboardFunc);
    glutDisplayFunc(display);
glutIdleFunc(update);

    glutMainLoop();

    return 0;
}

