/*
 NeHe (nehe.gamedev.net) OpenGL tutorial series
 GLUT port.in 2001 by milix (milix_gr@hotmail.com)
 Most comments are from the original tutorials found in NeHe.
 For VC++ users, create a Win32 Console project and link
 the program with glut32.lib, glu32.lib, opengl32.lib, glaux.lib
*/
#include <stdio.h>              // Standard C/C++ Input-Output
#include <GL/glut.h>            // The GL Utility Toolkit (GLUT) Header
#include "Bmp.h"

#define WCX          640        // Window Width
#define WCY          480        // Window Height
#define TEXTURES_NUM 3          // We Have 3 Textures

// A Structure For RGB Bitmaps
typedef struct _RGBIMG {
    GLuint   w;                 // Image's Width
    GLuint   h;                 // Image's Height
    GLubyte* data;              // Image's Data (Pixels)
} RGBIMG;

// Global Variables
bool    g_gamemode;             // GLUT GameMode ON/OFF
bool    g_fullscreen;           // Fullscreen Mode ON/OFF (When g_gamemode Is OFF)
bool    g_light = false;        // Lighting ON/OFF
GLfloat g_xrot = 0.0f;          // X Rotation
GLfloat g_yrot = 0.0f;          // Y Rotation
GLfloat g_xspeed = 0.0f;        // X Rotation Speed
GLfloat g_yspeed = 0.0f;        // Y Rotation Speed
GLfloat g_z = -5.0f;            // Depth Into The Screen
GLfloat g_lightAmbient[]  = { 0.5f, 0.5f, 0.5f, 1.0f }; // Ambient Light
GLfloat g_lightDiffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f }; // Fiffuse Light
GLfloat g_lightPosition[] = { 0.0f, 0.0f, 2.0f, 1.0f }; // Light Source Position
GLuint  g_filter;               // Which Filter To Use
GLuint  g_texid[TEXTURES_NUM];  // Our Textures' Id List
int     g_part1;                // Start Of Disc
int     g_part2;                // End Of Disc
int     g_p1 = 0;               // Increase 1
int     g_p2 = 1;               // Increase 2
GLuint  g_object = 0;           // Which Object To Draw
GLUquadricObj* g_quadratic;     // Storage For Our Quadratic Objects
GLuint  texture[6];             // Storage For 6 Textures (NEW)


// Load the respective BMP textures(NEW)
bool LoadGLTextures()                           // Load Bitmaps And Convert To Textures
{
    Image::Bmp TextureImage[2];
    if (!TextureImage[0].read("data/BG.bmp")) return false;
    if (!TextureImage[1].read("data/Reflect.bmp")) return false;

    glGenTextures(6, &texture[0]);          // Create Three Textures

    for (int loop=0; loop<=1; loop++)
    {
        // Create Nearest Filtered Texture
        glBindTexture(GL_TEXTURE_2D, texture[loop]);        // Gen Tex 0 And 1
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[loop].getWidth(), TextureImage[loop].getHeight(),
            0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[loop].getDataRGB());

        // Create Linear Filtered Texture
        glBindTexture(GL_TEXTURE_2D, texture[loop+2]);      // Gen Tex 2 and 3
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[loop].getWidth(), TextureImage[loop].getHeight(),
            0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[loop].getDataRGB());

        // Create MipMapped Texture
        glBindTexture(GL_TEXTURE_2D, texture[loop+4]);      // Gen Tex 4 and 5
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
        gluBuild2DMipmaps(GL_TEXTURE_2D, 3, TextureImage[loop].getWidth(), TextureImage[loop].getHeight(),
            GL_RGB, GL_UNSIGNED_BYTE, TextureImage[loop].getDataRGB());
    }
    return true;
}

// Our GL Specific Initializations. Returns true On Success, false On Fail.
bool init(void)
{
    if (!LoadGLTextures())                              // Jump To Texture Loading Routine (NEW)
    {
        return false;                                   // If Texture Didn't Load Return FALSE (NEW)
    }

    glEnable(GL_TEXTURE_2D);                            // Enable Texture Mapping
    glShadeModel(GL_SMOOTH);                            // Enable Smooth Shading
    glClearColor(0.0f, 0.0f, 0.0f, 0.5f);               // Black Background
    glClearDepth(1.0f);                                 // Depth Buffer Setup
    glEnable(GL_DEPTH_TEST);                            // Enables Depth Testing
    glDepthFunc(GL_LEQUAL);                             // The Type Of Depth Testing To Do
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);          // Pixel Storage Mode To Byte Alignment
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    glLightfv(GL_LIGHT1, GL_AMBIENT, g_lightAmbient);  // Setup The Ambient Light
    glLightfv(GL_LIGHT1, GL_DIFFUSE, g_lightDiffuse);  // Setup The Diffuse Light
    glLightfv(GL_LIGHT1, GL_POSITION,g_lightPosition); // Position The Light
    glEnable(GL_LIGHT1);                               // Enable Ligh

    g_quadratic = gluNewQuadric();                     // Create A Pointer To The Quadric Object (Return 0 If No Memory)
    if (g_quadratic == 0) return false;
    gluQuadricNormals(g_quadratic, GLU_SMOOTH);        // Create Smooth Normals
    gluQuadricTexture(g_quadratic, GL_TRUE);           // Create Texture Coords

    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);    // Set The Texture Generation Mode For S To Sphere Mapping ( NEW )
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);    // Set The Texture Generation Mode For T To Sphere Mapping ( NEW )

    return true;
}

// (NEW)
GLvoid glDrawCube()
{
        glBegin(GL_QUADS);
        // Front Face
        glNormal3f( 0.0f, 0.0f, 0.5f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
        // Back Face
        glNormal3f( 0.0f, 0.0f,-0.5f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
        // Top Face
        glNormal3f( 0.0f, 0.5f, 0.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
        // Bottom Face
        glNormal3f( 0.0f,-0.5f, 0.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
        // Right Face
        glNormal3f( 0.5f, 0.0f, 0.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
        // Left Face
        glNormal3f(-0.5f, 0.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
    glEnd();
}

// Our Rendering Is Done Here
void render(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);         // Clear The Screen And The Depth Buffer
    glLoadIdentity();                           // Reset The View

    glTranslatef(0.0f,0.0f,g_z);

    glEnable(GL_TEXTURE_GEN_S);                     // Enable Texture Coord Generation For S ( NEW )
    glEnable(GL_TEXTURE_GEN_T);                     // Enable Texture Coord Generation For T ( NEW )

    glBindTexture(GL_TEXTURE_2D, texture[g_filter+(g_filter+1)]);       // This Will Select A Sphere Map
    glPushMatrix();
    glRotatef(g_xrot,1.0f,0.0f,0.0f);
    glRotatef(g_yrot,0.0f,1.0f,0.0f);
    switch(g_object)
    {
    case 0:
        glDrawCube();
        break;
    case 1:
        glTranslatef(0.0f,0.0f,-1.5f);                  // Center The Cylinder
        gluCylinder(g_quadratic,1.0f,1.0f,3.0f,32,32);          // A Cylinder With A Radius Of 0.5 And A Height Of 2
        break;
    case 2:
        gluSphere(g_quadratic,1.3f,32,32);              // Sphere With A Radius Of 1 And 16 Longitude/Latitude Segments
        break;
    case 3:
        glTranslatef(0.0f,0.0f,-1.5f);                  // Center The Cone
        gluCylinder(g_quadratic,1.0f,0.0f,3.0f,32,32);          // Cone With A Bottom Radius Of .5 And Height Of 2
        break;
    };

    glPopMatrix();
    glDisable(GL_TEXTURE_GEN_S);                        // Disable Texture Coord Generation ( NEW )
    glDisable(GL_TEXTURE_GEN_T);                        // Disable Texture Coord Generation ( NEW )

    glBindTexture(GL_TEXTURE_2D, texture[g_filter*2]);          // This Will Select The BG Texture ( NEW )
    glPushMatrix();
        glTranslatef(0.0f, 0.0f, -24.0f);
        glBegin(GL_QUADS);
            glNormal3f( 0.0f, 0.0f, 1.0f);
            glTexCoord2f(0.0f, 0.0f); glVertex3f(-13.3f, -10.0f,  10.0f);
            glTexCoord2f(1.0f, 0.0f); glVertex3f( 13.3f, -10.0f,  10.0f);
            glTexCoord2f(1.0f, 1.0f); glVertex3f( 13.3f,  10.0f,  10.0f);
            glTexCoord2f(0.0f, 1.0f); glVertex3f(-13.3f,  10.0f,  10.0f);
        glEnd();

    glPopMatrix();

    g_xrot+=g_xspeed;
    g_yrot+=g_yspeed;


    // Swap The Buffers To Become Our Rendering Visible
    glutSwapBuffers ( );
}

// Our Reshaping Handler (Required Even In Fullscreen-Only Modes)
void reshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);     // Select The Projection Matrix
    glLoadIdentity();                // Reset The Projection Matrix
    // Calculate The Aspect Ratio And Set The Clipping Volume
    if (h == 0) h = 1;
    gluPerspective(45, (float)w/(float)h, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);      // Select The Modelview Matrix
    glLoadIdentity(); // Reset The Modelview Matrix
}


// Our Keyboard Handler (Normal Keys)
void keyboard(unsigned char key, int x, int y)
{
    switch (key) {
        case 27:
            exit(0);
        break;
        case 'L': case 'l':
            g_light = !g_light;
            if (!g_light) glDisable(GL_LIGHTING);
            else glEnable(GL_LIGHTING);
        break;
        case 'F': case 'f':
            g_filter += 1;
            if (g_filter > 2) g_filter = 0;
        break;

        case ' ':
            //sp=TRUE;
            g_object++;
            if(g_object>3)
                g_object=0;
        break;

        default:
        break;
    }
}

// Our Keyboard Handler For Special Keys (Like Arrow Keys And Function Keys)
void special_keys(int a_keys, int x, int y)
{
    switch (a_keys) {
        case GLUT_KEY_PAGE_UP:
            g_z -= 0.02f;
        break;
        case GLUT_KEY_PAGE_DOWN:
            g_z += 0.02f;
        break;
        case GLUT_KEY_UP:
            g_xspeed -= 0.01f;
        break;
        case GLUT_KEY_DOWN:
            g_xspeed += 0.01f;
        break;
        case GLUT_KEY_RIGHT:
            g_yspeed += 0.01f;
        break;
        case GLUT_KEY_LEFT:
            g_yspeed -= 0.01f;
        break;
        default:
        break;
    }
}

void joystickCB(unsigned int buttons, int axis0, int axis1, int axis2)
{
    static unsigned int old = 0;

    unsigned int pressed = (buttons ^ old) & buttons;
    old = buttons;

    g_xspeed += axis0 * 0.00001f;
    g_yspeed += axis1 * 0.00001f;

    if (pressed & 0x1) keyboard(' ', 0, 0);
    if (pressed & 0x2) keyboard('l', 0, 0);
    if (pressed & 0x4) keyboard('f', 0, 0);
    if (pressed & 0x40) keyboard(27, 0, 0);
}

// Main Function For Bringing It All Together.
int main(int argc, char** argv)
{
    setenv("OPENGX_DEBUG", "texture", 1);
    glutInit(&argc, argv);                           // GLUT Initializtion
    glutInitDisplayMode(GLUT_DEPTH | GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(WCX, WCY);                // Window Size If We Start In Windowed Mode
    glutCreateWindow("NeHe's OpenGL Framework"); // Window Title
    if (!init()) {                                   // Our Initialization
        fputs("Initialization failed!\n", stderr);
        return -1;
    }
    glutDisplayFunc(render);                         // Register The Display Function
    glutReshapeFunc(reshape);                        // Register The Reshape Handler
    glutKeyboardFunc(keyboard);                      // Register The Keyboard Handler
    glutSpecialFunc(special_keys);                   // Register Special Keys Handler
    glutJoystickFunc(joystickCB, 10);
    glutIdleFunc(render);                            // We Render In Idle Time
    glutMainLoop();                                  // Go To GLUT Main Loop
    return 0;
}
