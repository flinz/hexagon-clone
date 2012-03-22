#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <deque>
#include <GLUT/glut.h>
s
const int circPoints = 30;
float gamespeed = 1.;

const float pi = acos(-1);


int lastFrameTime = 0;
float boxX = 0.0f;
float xpoz = 0, zpoz = 0;
float orient = 0;
float radius = .3;
float width = .03;

int dead = 0;

GLboolean leftPressed = false;
GLboolean rightPressed = false;


//
//  SimpleBaddie Class
//

class SimpleBaddie {
public:
    float orient;
    float radius;
    float width;
    float spacing;    

    SimpleBaddie(float, float, float, float);
    void update(float);
    void draw(void);
    int collide(float, float);
};

SimpleBaddie::SimpleBaddie(float a_orient, float a_rad, float a_width,float a_spacing) {
    orient = a_orient;
    radius = a_rad;
    width = a_width;
    spacing = a_spacing;
}

void SimpleBaddie::update(float passedTime) {
    radius -= passedTime*gamespeed;
    //orient += passedTime*64.;
}

int SimpleBaddie::collide(float pl_orient, float pl_radius) {
    if (pl_radius < radius) return 0;
    else if (abs(pl_orient-orient) < 2*spacing) return -1;
    return 1;
}

void SimpleBaddie::draw(void) {
    
    glPushMatrix();
    glRotatef(orient,0,0,1);    
    
    glColor3f(1.0,1.0,0.0);
    
    int leftout = int(spacing/360.*circPoints);
    
    glBegin( GL_TRIANGLE_STRIP );
    for( int i=0; i<circPoints-2*leftout+1; i++ )
    {
        float angle = (i+leftout) * (2*pi/circPoints); // use 360 instead of 2.0*PI if
        
        float x = cos( angle );  // you use d_cos and d_sin
        float y = sin( angle );
        glVertex3f( x*(radius-width), y*(radius-width), 0);
        glVertex3f( x*(radius+width), y*(radius+width), 0);
    }
    glEnd();
    glPopMatrix();    
}

std::deque<SimpleBaddie> baddies;

void init(void) 
{
    glClearColor (0.0, 0.0, 0.0, 0.0); 
    glEnable(GL_DEPTH_TEST);
    glShadeModel (GL_SMOOTH);
}

void drawPlayerCircle(float orient)
{   
    glPushMatrix();
    glRotatef(orient,0,0,1);    
    
    glColor3f(1.0,1.0,0.0);

    glBegin( GL_TRIANGLE_STRIP );
    for( int i=0; i<circPoints+1; i++ )
    {
        float angle = i * (2*pi/circPoints); // use 360 instead of 2.0*PI if
        
        float x = cos( angle );  // you use d_cos and d_sin
        float y = sin( angle );
        glVertex3f( x*(radius-width), y*(radius-width), 0);
        glVertex3f( x*(radius+width), y*(radius+width), 0);
    }
    glEnd();
    
    int incr = 4;
    
    glBegin (GL_TRIANGLES);
    float angle = (-incr) * (2*pi/circPoints);
    glVertex3f( cos(angle)*(radius+width), sin(angle)*(radius+width), 0);
    
    glVertex3f(1.*(radius+radius/2.), 0.*(radius+radius/2.), 0);

    angle = (+incr) * (2*pi/circPoints);
    glVertex3f( cos(angle)*(radius+width), sin(angle)*(radius+width), 0);
    glEnd();

    glPopMatrix();  
}

void display(void)
{
    
    if(dead>0) {
        glClearColor (1.0, 0.0, 0.0, 0.0);
        dead = (dead+1)%200;
    }
    else
        glClearColor (0.0, 0.0, 0.0, 0.0); 
    
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity ();
    
    // We will draw things a little deeper into the screen
    glTranslatef(0,0,-6);
    
    // iterate over baddies
    for (int i=0; i < baddies.size(); i++) {
        SimpleBaddie &b = baddies[i];
        b.draw();
    }
    
    // Draw player circle
    drawPlayerCircle(orient);
    glutSwapBuffers();

}

void reshape (int w, int h)
{
    glViewport (0, 0, (GLsizei) w, (GLsizei) h); 
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    gluPerspective(60.0, (GLfloat) w/(GLfloat) h, 1.0, 20.0);
    glMatrixMode (GL_MODELVIEW);
}

void idle(void)
{    
    
    while(baddies.size() < 2) {
            baddies.push_back(SimpleBaddie((float) rand()/RAND_MAX * 360., 5.+(float) rand()/RAND_MAX * 1.5, .05,40.+(float) rand()/RAND_MAX * 20. ));
    }
    
    if (lastFrameTime == 0)
    {
        lastFrameTime = glutGet(GLUT_ELAPSED_TIME);
    }
    
    int now = glutGet(GLUT_ELAPSED_TIME);
    int elapsedMilliseconds = now - lastFrameTime;
    float elapsedTime = elapsedMilliseconds / 1000.0f;
    lastFrameTime = now;
    
    float rate = 2.*elapsedTime*360.;
    orient += (leftPressed - rightPressed) * rate;
    
    // iterate over baddies
    for (int i=0; i < baddies.size(); i++) {
        SimpleBaddie &b = baddies[i];
        b.update(elapsedTime);
    }
    
    if(baddies.size()>0) {
        SimpleBaddie &b = baddies[0];
        int col = b.collide(orient,radius);
        if (col==-1) {
            baddies.pop_front();
        } else if ( col== 1) {
            dead = 1;
            baddies.pop_front();
        }
    }
    
    glutPostRedisplay();
}

void normKeys(unsigned char key, int x, int y)
{
    switch (key) {
        case 27: // ESC key
            exit(0);
            break; 
    }
}

void specialKeys(int key, int x, int y) {
    
	switch(key) {
		case GLUT_KEY_LEFT :
            leftPressed = true;
            break;
		case GLUT_KEY_RIGHT :
            rightPressed = true;
            break;
	}
}

void specialKeysUp(int key, int x, int y) {
    
	switch(key) {
		case GLUT_KEY_LEFT :
            leftPressed = false;
            break;
		case GLUT_KEY_RIGHT :
            rightPressed = false;
            break;
	}
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    
    glutCreateWindow("Fun game");
    
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);
    glutKeyboardFunc(normKeys);
    glutSpecialFunc(specialKeys);
    glutSpecialUpFunc(specialKeysUp);
    
    baddies.push_back(SimpleBaddie((float) rand()/RAND_MAX * 360., 4.+(float) rand()/RAND_MAX * 1.5, .05,40.+(float) rand()/RAND_MAX * 10. ));
    
        baddies.push_back(SimpleBaddie((float) rand()/RAND_MAX * 360., 6.5+(float) rand()/RAND_MAX * 1.5, .05,40.+(float) rand()/RAND_MAX * 20. ));
    
    glutMainLoop();
    return EXIT_SUCCESS;
}

