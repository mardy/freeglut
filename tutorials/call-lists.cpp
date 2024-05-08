#include <GL/gl.h>
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>

GLfloat redDiffuseMaterial[] = {1.0, 0.0, 0.0, 1.0 };
GLfloat white[] = { 1.0, 1.0, 1.0, 1.0 };
GLuint texture; //the array for our texture
GLuint squareList, teapotList;

GLfloat angle = 0.0;

//function to load the RAW file

GLuint LoadTexture( const char * filename, int width, int height )
{
    GLuint texture;
    unsigned char * data;
    FILE * file;

    //The following code will read in our RAW file
    file = fopen( filename, "rb" );
    if ( file == NULL ) return 0;
    data = (unsigned char *)malloc( width * height * 3 );
    fread( data, width * height * 3, 1, file );
    fclose( file );

    glGenTextures( 1, &texture ); //generate the texture with the loaded data
    glBindTexture( GL_TEXTURE_2D, texture ); //bind the texture to it’s array
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE ); //set texture environment parameters

    //here we are setting what textures to use and when. The MIN filter is which quality to show
    //when the texture is near the view, and the MAG filter is which quality to show when the texture
    //is far from the view.

    //The qualities are (in order from worst to best)
    //GL_NEAREST
    //GL_LINEAR
    //GL_LINEAR_MIPMAP_NEAREST
    //GL_LINEAR_MIPMAP_LINEAR

    //And if you go and use extensions, you can use Anisotropic filtering textures which are of an
    //even better quality, but this will do for now.
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    //Here we are setting the parameter to repeat the texture instead of clamping the texture
    //to the edge of our shape.
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

    //Generate the texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    free( data ); //free the texture
    return texture; //return whether it was successfull
}

void FreeTexture( GLuint texture )
{
  glDeleteTextures( 1, &texture );
}

void square (void) {
    glBindTexture( GL_TEXTURE_2D, texture ); //bind our texture to our shape
    glBegin (GL_QUADS);
    glTexCoord2d(0.0,0.0); glVertex2d(-1.0,-1.0); //with our vertices we have to assign a texcoord
    glTexCoord2d(1.0,0.0); glVertex2d(+1.0,-1.0); //so that our texture has some points to draw to
    glTexCoord2d(1.0,1.0); glVertex2d(+1.0,+1.0);
    glTexCoord2d(0.0,1.0); glVertex2d(-1.0,+1.0);
    glEnd();

//This is how texture coordinates are arranged
//
//  0,1   —–   1,1
//       |     |
//       |     |
//       |     |
//  0,0   —–   1,0

// With 0,0 being the bottom left and 1,1 being the top right.

// Now the point of using the value 0,1 instead of 0,10, is so that it is mapping 1 texture to the
// coordinates. Changing that to 10 would then try to map 10 textures to the one quad. Which because
// I have the repeat parameter set in our texture, it would draw 10 across and 10 down, if we had
// it clamped, we would be still drawing 1. The repeat function is good for things like
// brick walls.
}

#define SQUARE_LIST 1
void build_lists(void) {
    squareList = glGenLists(3);
    teapotList = squareList + 1;

#if SQUARE_LIST
    glNewList(squareList, GL_COMPILE);
    glColor3f(0.0, 1.0, 1.0);
    square();
    glEndList();
#endif

    glNewList(teapotList, GL_COMPILE_AND_EXECUTE);
    glColor3f(0.0, 0.0, 1.0);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, redDiffuseMaterial);
#if 1
    glutSolidTeapot(1);
#else
    square();
#endif
    glEndList();
}

void display (void) {
    glClearColor (0.0,0.0,0.0,1.0);
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt (0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
#if SQUARE_LIST
    glEnable( GL_TEXTURE_2D );
    glPushMatrix();
    glRotatef( angle, 1.0f, 1.0f, 1.0f );
    glCallList(squareList);
    glPopMatrix();
    glDisable( GL_TEXTURE_2D );
#endif

#if 0
    glPushMatrix();
    glTranslatef(0.0, 2.0, 0.0);
    square();
    glPopMatrix();
#endif

#if 0
    glTranslatef(2.0, 0.0, 0.0);
    glEnableClientState(GL_VERTEX_ARRAY);
    float vertices[] = {
        -1.0, -1.0, 0.0,
         1.0, -1.0, 0.0,
         1.0,  1.0, 0.0,
        -1.0,  1.0, 0.0,
    };
    float colors[] = {
        1.0, 1.0, 0.0,
        0.0, 1.0, 0.0,
        0.0,  1.0, 1.0,
        1.0,  0.0, 1.0,
    };
    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glColorPointer(3, GL_FLOAT, 0, colors);
    glDrawArrays(GL_QUADS, 0, 4);
    glDisableClientState(GL_VERTEX_ARRAY);
#else
    glEnable (GL_LIGHTING);
    glEnable (GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
    glTranslatef(2.0, 0.0, 0.0);
    glPushMatrix();
    glRotatef( angle, 1.0f, 1.0f, 1.0f );
    glCallList(teapotList);
    glPopMatrix();
    glDisable(GL_LIGHTING);
#endif

    glutSwapBuffers();
    angle ++;
}
void reshape (int w, int h) {
    glViewport (0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    gluPerspective (60, (GLfloat)w / (GLfloat)h, 1.0, 100.0)
;
    glMatrixMode (GL_MODELVIEW);
}

int main (int argc, char **argv) {
    setenv("OPENGX_DEBUG", "2", 1);
    glutInit (&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize (500, 500);
    glutInitWindowPosition (100, 100);
    glutCreateWindow ("A basic OpenGL Window");
    glutDisplayFunc (display);
    glutIdleFunc (display);
    glutReshapeFunc (reshape);

    //Load our texture
    texture = LoadTexture( "texture.raw", 256, 256 );

    glEnable (GL_DEPTH_TEST);
    build_lists();
    glutMainLoop ();

    //Free our texture
    FreeTexture( texture );

    return 0;
}
