#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>

// #include <windows.h>
#include<bits/stdc++.h>
using namespace std;

#include<GL/glut.h>
#define pi (2*acos(0.0))

double cameraHeight;
double cameraAngle;
int drawgrid;
int drawaxes;
double angle;
double lenOfSquare=100;
double centerCircleRadius =80;
bool pause =false;

int bubbleUp = 0;


double bubbleRadius= 15;
double scalar=1;
clock_t t;


struct point
{
	double x,y,z;
	point(){
	}
	point(double p,double q,double r){
		x=p;
		y=q;
		z=r;
	}
	void print()
	{
		cout<<"t "<< x << " " << y << " " << z << endl;
	}
	
};
point bubbles[5];
point velocity[5];
bool insideCircle[5];


point sub(point a, point b)
{
	point temp;
	temp.x = a.x - b.x;
	temp.y = a.y - b.y;
	temp.z = a.z - b.z;
	return temp;
}
point add(point a, point b)
{
	point temp;
	temp.x = a.x + b.x;
	temp.y = a.y + b.y;
	temp.z = a.z + b.z;
	return temp;
}
point withScalar(point a ,point vel)
{
    a.x= a.x+scalar*vel.x;
    a.y= a.y+scalar*vel.y;
    return a;

}
double getDistance(point a ){
    return sqrt(a.x*a.x+a.y*a.y);
}
double getPointDistance(point a,point b)
{
    return sqrt(  (a.x-b.x) *(a.x-b.x)+ (a.y-b.y)*(a.y-b.y));
}
bool collisionChecker(point a ,point b)
{
    if (getPointDistance(a,b)<2*bubbleRadius)
        return true;
    return false;
}
bool checkInsideCircle(point a){
    if (getDistance(a) <=fabs(centerCircleRadius-bubbleRadius))
        return true;
    return false;
}

point reflection(point center, point v)
{
    // printf("\n\n\n");
    // cout<<  "center is ";center.print();
    // cout<< "v is ";v.print();

    point result;
    point temp = add(center,v);

    // cout<<  "after adding ";temp.print();

    double dist=  getDistance(temp);

    // cout<<"dist is "<<dist<<endl;

    temp.x= temp.x/(-1.0*dist);  //negative means origin -point
    temp.y= temp.y/(-1.0*dist);
    
    // cout<< "to the origin ";temp.print();

    double dotProduct =v.x*temp.x +v.y*temp.y;
    result.x=v.x-2.0 * (dotProduct) * temp.x;
    result.y=v.y-2.0 * (dotProduct ) * temp.y;
    result.z=0;

    // cout<<"result is " ;result.print();

    return result;
   
}
point bubbleReflectionFormula(point vel, point normal)
{
    point result;
    
    double dist =getDistance(normal);
    normal.x= normal.x/(1.0*dist);  //negative means origin -point
    normal.y= normal.y/(1.0*dist);
    
    double dotProduct = vel.x*normal.x +vel.y*normal.y;
    result.x=vel.x-2.0 * (dotProduct) * normal.x;
    result.y=vel.y-2.0 * (dotProduct ) * normal.y;
    
    return result;


}
void bubbleCollision(point  bub1, point bub2 , int index1,int index2 )
{
    if(collisionChecker(bub1,bub2))
        return;
    point p1=add(bub1,velocity[index1]);
    point p2=add(bub2,velocity[index2]);
    if(collisionChecker(p1,p2))
    {
        cout<<" bubble  " << index1 <<  "and  bubble "<<index2<<" will collide"<<endl;
        cout<<"dist is ";getPointDistance(bub1,bub2); cout<<endl;
        bub1.print();
        

        point normal1=sub(p1,p2);
        point normal2=sub(p2,p1);
    
        velocity[index1]= bubbleReflectionFormula(velocity[index1],normal1);
        
        velocity[index2]= bubbleReflectionFormula(velocity[index2],normal2);
    
    

    }
}
void drawAxes()
{
	if(drawaxes==1)
	{
		glBegin(GL_LINES);{
			glColor3f(1.0, 0, 0);

			glVertex3f( 100,0,0);
			glVertex3f(-100,0,0);

			glColor3f(0, 1.0, 0);
			glVertex3f(0,-100,0);
			glVertex3f(0, 100,0);

			glColor3f(0, 0, 1);
			glVertex3f(0,0, 100);
			glVertex3f(0,0,-100);
		}glEnd();
	}
}


void drawGrid()
{
	int i;
	if(drawgrid==1)
	{
		glColor3f(0.6, 0.6, 0.6);	//grey
		glBegin(GL_LINES);{
			for(i=-8;i<=8;i++){

				if(i==0)
					continue;	//SKIP the MAIN axes

				//lines parallel to Y-axis
				glVertex3f(i*10, -90, 0);
				glVertex3f(i*10,  90, 0);

				//lines parallel to X-axis
				glVertex3f(-90, i*10, 0);
				glVertex3f( 90, i*10, 0);
			}
		}glEnd();
	}
}

void drawSquare(double a)
{
    //glColor3f(1.0,0.0,0.0);
    glBegin(GL_LINES);{
        glVertex3f( a, a,0);
		glVertex3f( a,-a,0);


        glVertex3f( a, a,0);
		glVertex3f( -a,a,0);


        glVertex3f( -a, -a,0);
		glVertex3f( a,-a,0);
    
        glVertex3f( -a, -a,0);
		glVertex3f( -a,a,0);
    }glEnd();

	// glBegin(GL_QUADS);{
	// 	glVertex3f( a, a,2);
	// 	glVertex3f( a,-a,2);
	// 	glVertex3f(-a,-a,2);
	// 	glVertex3f(-a, a,2);
	// }glEnd();
}


void drawCircle(double radius,int segments)
{
    int i;
    struct point points[100];
    // glColor3f(0.7,0.7,0.7);
    //generate points
    for(i=0;i<=segments;i++)
    {
        points[i].x=radius*cos(((double)i/(double)segments)*2*pi);
        points[i].y=radius*sin(((double)i/(double)segments)*2*pi);
    }
    //draw segments using generated points
    for(i=0;i<segments;i++)
    {
        glBegin(GL_LINES);
        {
			glVertex3f(points[i].x,points[i].y,0);
			glVertex3f(points[i+1].x,points[i+1].y,0);
        }
        glEnd();
    }
}

void drawCone(double radius,double height,int segments)
{
    int i;
    double shade;
    struct point points[100];
    //generate points
    for(i=0;i<=segments;i++)
    {
        points[i].x=radius*cos(((double)i/(double)segments)*2*pi);
        points[i].y=radius*sin(((double)i/(double)segments)*2*pi);
    }
    //draw triangles using generated points
    for(i=0;i<segments;i++)
    {
        //create shading effect
        if(i<segments/2)shade=2*(double)i/(double)segments;
        else shade=2*(1.0-(double)i/(double)segments);
        glColor3f(shade,shade,shade);

        glBegin(GL_TRIANGLES);
        {
            glVertex3f(0,0,height);
			glVertex3f(points[i].x,points[i].y,0);
			glVertex3f(points[i+1].x,points[i+1].y,0);
        }
        glEnd();
    }
}


void drawSphere(double radius,int slices,int stacks)
{
	struct point points[100][100];
	int i,j;
	double h,r;
	//generate points
	for(i=0;i<=stacks;i++)
	{
		h=radius*sin(((double)i/(double)stacks)*(pi/2));
		r=radius*cos(((double)i/(double)stacks)*(pi/2));
		for(j=0;j<=slices;j++)
		{
			points[i][j].x=r*cos(((double)j/(double)slices)*2*pi);
			points[i][j].y=r*sin(((double)j/(double)slices)*2*pi);
			points[i][j].z=h;
		}
	}
	//draw quads using generated points
	for(i=0;i<stacks;i++)
	{
        glColor3f((double)i/(double)stacks,(double)i/(double)stacks,(double)i/(double)stacks);
		for(j=0;j<slices;j++)
		{
			glBegin(GL_QUADS);{
			    //upper hemisphere
				glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
                //lower hemisphere
                glVertex3f(points[i][j].x,points[i][j].y,-points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,-points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,-points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,-points[i+1][j].z);
			}glEnd();
		}
	}
}
void drawProblems()
{
    glColor3f(0,1,0);
    drawSquare(lenOfSquare);

    glColor3f(1,0,0);
    drawCircle(centerCircleRadius,80);
    for (int i=0 ;i< bubbleUp;i++)
    {
        glPushMatrix();
        glTranslatef(bubbles[i].x,bubbles[i].y,bubbles[i].z);
        drawCircle(bubbleRadius,80);
        glPopMatrix();
    }
    

}

void drawSS()
{
    glColor3f(1,0,0);
    drawSquare(20);

    glRotatef(angle,0,0,1);
    glTranslatef(110,0,0);
    glRotatef(2*angle,0,0,1);
    glColor3f(0,1,0);
    drawSquare(15);

    glPushMatrix();
    {
        glRotatef(angle,0,0,1);
        glTranslatef(60,0,0);
        glRotatef(2*angle,0,0,1);
        glColor3f(0,0,1);
        drawSquare(10);
    }
    glPopMatrix();

    glRotatef(3*angle,0,0,1);
    glTranslatef(40,0,0);
    glRotatef(4*angle,0,0,1);
    glColor3f(1,1,0);
    drawSquare(5);
}

void keyboardListener(unsigned char key, int x,int y){
	switch(key){

		case 'p':
            pause = !pause;
			break;


		default:
			break;
	}
}


void specialKeyListener(int key, int x,int y){
	switch(key){
		case GLUT_KEY_DOWN:		//down arrow key
			if(scalar-0.1 >0)
			{
				scalar-=0.1;
			}
			break;
		case GLUT_KEY_UP:		// up arrow key
			if(scalar+0.1<2){
				scalar+=0.1;
			}
            break;

		case GLUT_KEY_RIGHT:
			cameraAngle += 0.03;
			break;
		case GLUT_KEY_LEFT:
			cameraAngle -= 0.03;
			break;

		case GLUT_KEY_PAGE_UP:
			break;
		case GLUT_KEY_PAGE_DOWN:
			break;

		case GLUT_KEY_INSERT:
			break;

		case GLUT_KEY_HOME:
			break;
		case GLUT_KEY_END:
			break;

		default:
			break;
	}
}


void mouseListener(int button, int state, int x, int y){	//x, y is the x-y of the screen (2D)
	switch(button){
		case GLUT_LEFT_BUTTON:
			// if(state == GLUT_DOWN){		// 2 times?? in ONE click? -- solution is checking DOWN or UP
			// 	drawaxes=1-drawaxes;
			// }
			break;

		case GLUT_RIGHT_BUTTON:
			//........
			break;

		case GLUT_MIDDLE_BUTTON:
			//........
			break;

		default:
			break;
	}
}



void display(){

	//clear the display
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0,0,0,0);	//color black
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/********************
	/ set-up camera here
	********************/
	//load the correct matrix -- MODEL-VIEW matrix
	glMatrixMode(GL_MODELVIEW);

	//initialize the matrix
	glLoadIdentity();

	//now give three info
	//1. where is the camera (viewer)?
	//2. where is the camera looking?
	//3. Which direction is the camera's UP direction?

	//gluLookAt(100,100,100,	0,0,0,	0,0,1);
	//gluLookAt(200*cos(cameraAngle), 200*sin(cameraAngle), cameraHeight,		0,0,0,		0,0,1);
	gluLookAt(0,0,200,	0,0,0,	0,1,0);


	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);


	/****************************
	/ Add your objects from here
	****************************/
	//add objects

	drawAxes();
	drawGrid();

    //glColor3f(1,0,0);
    //drawSquare(10);

    drawProblems();
    //drawCircle(30,24);

    //drawCone(20,50,24);

	//drawSphere(30,24,20);




	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}

void animate(){
    bool checker =false;
    if(!pause){
        int t2 = (int) (clock()- t);
        if (t2>10000  && bubbleUp<5)
        {
            // cout<< t2 <<" is up" <<endl;
            t=clock();

            bubbleUp++; 
        }
        for (int i=0;i<bubbleUp;i++)
        {
            

            if(insideCircle[i]){
                for (int  j=0 ;j<bubbleUp  ;j++)
                {
                    if (insideCircle[j] && j!=i){

                        bubbleCollision(bubbles[i],bubbles[j],i,j);
                        // while(collisionChecker(bubbles[i],bubbles[j]))
                        // {
                        //     checker=true;
                        //     bubbles[i].x +=scalar*velocity[i].x;
                        //     bubbles[i].y +=scalar*velocity[i].y;

                        // } 
                    }
                    
                }
            }
			point temp;
            temp.x = bubbles[i].x+ scalar*velocity[i].x;
            temp.y = bubbles[i].y+scalar*velocity[i].y;
            temp.z=0;
            if( fabs(temp.x) > fabs(lenOfSquare-bubbleRadius)){
                velocity[i].x= (- velocity[i].x);
            }
            if( fabs(temp.y) > fabs(lenOfSquare-bubbleRadius)){
                velocity[i].y= (- velocity[i].y);
            }
            if (checkInsideCircle(bubbles[i]) && !checkInsideCircle(temp))// circle is inside and  after padding it will go outside
                                                                          //so collision occurs on the wall
            {
                // cout<<i<< "will be reflected "<<endl;
                // cout<<i<<" bubble will be inside " <<temp.x << "  temp.y"<<endl;
                velocity[i] =reflection(bubbles[i],velocity[i]);
            }
            
            bubbles[i].x +=scalar*velocity[i].x;
            bubbles[i].y +=scalar*velocity[i].y;
            // if(insideCircle[i]==true  && !checkInsideCircle(bubbles[i]))
            // {
            //     insideCircle[i] =true;
            //     continue;
            // }
                
            insideCircle[i]=checkInsideCircle(bubbles[i]);
            



        }
    }
	//codes for any changes in Models, Camera
    
    
	glutPostRedisplay();
}
void generateRandom()
{
    srand(time(0));

    for(int i=0;i<5;i++)
    {
        int temp=rand() % 88+1;
        cout<< "degree is "<< temp<<endl;
        velocity[i].x= cos( (double) temp  *pi/ 180.0);
        velocity[i].y= sin( (double) temp  *pi/ 180.0);
        velocity[i].z=0;
        

        bubbles[i].x=-lenOfSquare+bubbleRadius;
        bubbles[i].y=-lenOfSquare+bubbleRadius;
        bubbles[i].z=0;
    }
}
void init(){
	//codes for initialization
	drawgrid=0;
	drawaxes=0;
	cameraHeight=150.0;
	cameraAngle=1.0;
	angle=0;
    for (int i=0;i< 5;i++){
        insideCircle[i]=false;
    }



	//clear the screen
	glClearColor(0,0,0,0);

	/************************
	/ set-up projection here
	************************/
	//load the PROJECTION matrix
	glMatrixMode(GL_PROJECTION);

	//initialize the matrix
	glLoadIdentity();

	//give PERSPECTIVE parameters
	gluPerspective(80,	1,	1,	1000.0);
	//field of view in the Y (vertically)
	//aspect ratio that determines the field of view in the X direction (horizontally)
	//near distance
	//far distance
}

int main(int argc, char **argv){
    generateRandom();
	glutInit(&argc,argv);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color

	glutCreateWindow("My OpenGL Program");
    float r2 = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/10));

	init();

	glEnable(GL_DEPTH_TEST);	//enable Depth Testing

	glutDisplayFunc(display);	//display callback function
    t=clock();

	glutIdleFunc(animate);		//what you want to do in the idle time (when no drawing is occuring)

	glutKeyboardFunc(keyboardListener);
	glutSpecialFunc(specialKeyListener);
	glutMouseFunc(mouseListener);

	glutMainLoop();		//The main loop of OpenGL

	return 0;
}