#include <GL/gl.h>
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>

GLuint texture; //the array for our texture

GLfloat angle = 0.0;

//function to load the RAW file

GLuint LoadTexture(int width, int height )
{
    GLuint texture;
    uint8_t * data;
    FILE * file;

    //The following code will read in our RAW file
    data = (unsigned char *)malloc( width * height * 4 );
    uint8_t r=0, g=0, b=0, a =255;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < height; x++) {
            uint8_t *pixel = data + (y * width + x) * 3;
            if (x > width / 2) {
                if (y > height / 2) {
                    r = 0;
                    g = 255;
                    b = 0;
                } else {
                    r = 0;
                    g = 0;
                    b = 255;
                }
            } else {
                r = 255;
                g = 0;
                b = 0;
            }
            a = y;
            pixel[0] = r;
            pixel[1] = g;
            pixel[2] = b;
            //printf("pixel: 0x%02x%02x\n", pixel[0], pixel[1]);
        }
    }

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

    int time0 = glutGet(GLUT_ELAPSED_TIME);
    //Generate the texture
    for (int i = 0; i < 1000; i++) {
        glGenTextures( 1, &texture ); //generate the texture with the loaded data
        glBindTexture( GL_TEXTURE_2D, texture ); //bind the texture to it’s array
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glDeleteTextures(1, &texture);
    }
    int elapsed = glutGet(GLUT_ELAPSED_TIME) - time0;
    printf("Elapsed time: %d\n", elapsed);
    glGenTextures( 1, &texture );
    glBindTexture( GL_TEXTURE_2D, texture );
    int digit = 0;
    for (int y = 0; y < height; y++) {
        if (y % 32 == 0) {
            digit = elapsed % 10;
            elapsed /= 10;
        }
        for (int x = 0; x < height; x++) {
            uint8_t *pixel = data + (y * width + x) * 3;
            if (x / 16 < digit && (x % 16 != 0) && (y %32 != 0)) {
                r = 255;
                g = 255;
                b = 0;
            } else if (x > 250&& (y %32 > 1)) {
                r = 0;
                g = 0;
                b = 255;
            } else {
                r = 0;
                g = 0;
                b = 0;
            }
            pixel[0] = r;
            pixel[1] = g;
            pixel[2] = b;
            //printf("pixel: 0x%02x%02x\n", pixel[0], pixel[1]);
        }
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    free( data ); //free the texture
    return texture; //return whether it was successfull
}

void FreeTexture( GLuint texture )
{
  glDeleteTextures( 1, &texture );
}

void square (void) {
    glBindTexture( GL_TEXTURE_2D, texture ); //bind our texture to our shape
    //glRotatef( angle, 1.0f, 1.0f, 1.0f );
    glBegin (GL_QUADS);
    glTexCoord2d(0.0,0.0); glVertex2d(-2.0,-1.0); //with our vertices we have to assign a texcoord
    glTexCoord2d(1.0,0.0); glVertex2d(+2.0,-1.0); //so that our texture has some points to draw to
    glTexCoord2d(1.0,1.0); glVertex2d(+2.0,+1.0);
    glTexCoord2d(0.0,1.0); glVertex2d(-2.0,+1.0);
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

void display (void) {
    glClearColor (0.0,0.0,0.0,1.0);
    glClear (GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    glEnable( GL_TEXTURE_2D );
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    gluLookAt (0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    square();
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

void joystick(unsigned int buttons, int axis0, int axis1, int axis2)
{
    static unsigned int old = 0;

    unsigned int pressed = (buttons ^ old) & buttons;
    old = buttons;

    if (pressed & 0x1) exit(0);
}
int main (int argc, char **argv) {
    setenv("OPENGX_DEBUG", "2", 1);
    glutInit (&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize (500, 500);
    glutInitWindowPosition (100, 100);
    glutCreateWindow ("A basic OpenGL Window");
    glutDisplayFunc (display);
    glutIdleFunc (display);
    glutJoystickFunc (joystick, 10);
    glutReshapeFunc (reshape);

    //Load our texture
    texture = LoadTexture(256, 256 );

    glutMainLoop ();

    //Free our texture
    FreeTexture( texture );

    return 0;
}
