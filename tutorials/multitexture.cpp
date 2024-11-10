#include <GL/gl.h>
#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

GLuint checker_tex, circle_tex, circle2_tex;

GLfloat angle = 0.0;

static const uint8_t checker_tex_data[] = {
    0x00, 0xff,
    0xff, 0x00,
};

static const uint16_t circle_tex_data[] = {
    0x0000, 0x0000, 0xfff8, 0xffff, 0xffff, 0xfff8, 0x0000, 0x0000,
    0x0000, 0xfff8, 0xffff, 0xf00f, 0xf00f, 0xffff, 0xfff8, 0x0000,
    0xfff8, 0xffff, 0xf00f, 0xf008, 0xf008, 0xf00f, 0xffff, 0xfff8,
    0xffff, 0xf00f, 0xf008, 0xf004, 0xf004, 0xf008, 0xf00f, 0xffff,
    0xffff, 0xf00f, 0xf008, 0xf004, 0xf004, 0xf008, 0xf00f, 0xffff,
    0xfff8, 0xffff, 0xf00f, 0xf008, 0xf008, 0xf00f, 0xffff, 0xfff8,
    0x0000, 0xfff8, 0xffff, 0xf00f, 0xf00f, 0xffff, 0xfff8, 0x0000,
    0x0000, 0x0000, 0xfff8, 0xffff, 0xffff, 0xfff8, 0x0000, 0x0000,
};

static const uint16_t circle2_tex_data[] = {
    0x0000, 0x0f08, 0x0f0c, 0x0f0f, 0x0f0f, 0x0f0c, 0x0f08, 0x0000,
    0x0f88, 0x8f88, 0x8f4c, 0x8f0f, 0x8f0f, 0x8f4c, 0x8f88, 0x0f88,
    0x0ff8, 0xfff8, 0xff8c, 0xff0f, 0xff0f, 0xff8c, 0xfff8, 0x0ff8,
    0x0fff, 0xff0f, 0x888c, 0x00f8, 0x00f8, 0x888c, 0xff0f, 0x0fff,
    0x0fff, 0xff0f, 0x888c, 0x00f8, 0x00f8, 0x888c, 0xff0f, 0x0fff,
    0x0ff8, 0xfff8, 0xff8c, 0xff0f, 0xff0f, 0xff8c, 0xfff8, 0x0ff8,
    0x0f88, 0x8f88, 0x8f4c, 0x8f0f, 0x8f0f, 0x8f4c, 0x8f88, 0x0f88,
    0x0000, 0x0f08, 0x0f0c, 0x0f0f, 0x0f0f, 0x0f0c, 0x0f08, 0x0000,
};

void setup_textures() {
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glGenTextures(1, &circle_tex);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, circle_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 8, 8, 0, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4, circle_tex_data);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenTextures(1, &circle2_tex);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, circle2_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 8, 8, 0, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4, circle2_tex_data);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenTextures(1, &checker_tex);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, checker_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 2, 2, 0, GL_ALPHA, GL_UNSIGNED_BYTE, checker_tex_data);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    /* Setup multi-texture environment */
    glActiveTexture(GL_TEXTURE0);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    glActiveTexture(GL_TEXTURE1);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
    glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_INTERPOLATE);    // Interpolate RGB with RGB
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_PREVIOUS);
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_TEXTURE);
    // GL_CONSTANT refers to the call we make with glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, mycolor)
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_RGB, GL_CONSTANT);
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_RGB, GL_SRC_COLOR);
    // --------------------
    glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_INTERPOLATE);   // Interpolate ALPHA with ALPHA
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA, GL_PREVIOUS);
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_ALPHA, GL_TEXTURE);
    // GL_CONSTANT refers to the call we make with glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, mycolor)
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_ALPHA, GL_CONSTANT);
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA);
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_ALPHA, GL_SRC_ALPHA);
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_ALPHA, GL_SRC_ALPHA);

    glActiveTexture(GL_TEXTURE2);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
    // Sample RGB, multiply by previous texunit result
    glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_REPLACE);   // Modulate RGB with RGB
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_PREVIOUS);
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
    // Sample ALPHA, multiply by previous texunit result
    glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_MODULATE);  // Modulate ALPHA with ALPHA
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA, GL_PREVIOUS);
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_ALPHA, GL_TEXTURE);
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA);
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_ALPHA, GL_SRC_ALPHA);
}

void FreeTexture()
{
    glDeleteTextures(1, &checker_tex);
    glDeleteTextures(1, &circle_tex);
    glDeleteTextures(1, &circle2_tex);
}

void square (void) {
    glBindTexture(GL_TEXTURE_2D, checker_tex); //bind our texture to our shape
    //glRotatef( angle, 1.0f, 1.0f, 1.0f );
    glBegin (GL_QUADS);
    glTexCoord2f(0.0,0.0);
//    glMultiTexCoord2f(GL_TEXTURE1, 0.0, 0.0);
    glMultiTexCoord2f(GL_TEXTURE2, 0.0, 0.0);
    glVertex2f(-2.0,-1.0);

    glTexCoord2f(1.0,0.0);
//    glMultiTexCoord2f(GL_TEXTURE1, 1.0, 0.0);
    glMultiTexCoord2f(GL_TEXTURE2, 1.0, 0.0);
    glVertex2f(+2.0,-1.0);

    glTexCoord2f(1.0,1.0);
//    glMultiTexCoord2f(GL_TEXTURE1, 1.0,1.0);
    glMultiTexCoord2f(GL_TEXTURE2, 1.0,1.0);
    glVertex2f(+2.0,+1.0);

    glTexCoord2f(0.0,1.0);
//    glMultiTexCoord2f(GL_TEXTURE1, 0.0,1.0);
    glMultiTexCoord2f(GL_TEXTURE2, 0.0,1.0);
    glVertex2f(-2.0,+1.0);

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
    static int time0 = glutGet(GLUT_ELAPSED_TIME);
    int time1 = glutGet(GLUT_ELAPSED_TIME);
    float elapsed = (time1 - time0) / 1000.0;

    glClearColor (0.0,1.0,0.0,1.0);
    glClear (GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    gluLookAt (0.0, 0.0, 3.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    glMatrixMode(GL_TEXTURE);

    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, circle_tex);
    glLoadIdentity();
    float scale = (1 + sinf(elapsed / 10)) * 5;
    glScalef(scale, scale, 1.0);

    glActiveTexture(GL_TEXTURE1);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
    glBindTexture(GL_TEXTURE_2D, circle2_tex);
    glLoadIdentity();
    glScalef(scale, scale, 1.0);
    float mycolor[4];
    mycolor[0]=mycolor[1]=mycolor[2]=
    mycolor[3]=(1 + sinf(elapsed)) / 2.0;                         // Set the blend factor with this
    glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, mycolor);


    glActiveTexture(GL_TEXTURE2);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, checker_tex);

    glLoadIdentity();
    glTranslatef(sinf(elapsed), cosf(elapsed), 1.0);

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
    glutInitWindowSize (640, 480);
    glutInitWindowPosition (100, 100);
    glutCreateWindow ("A basic OpenGL Window");
    glutDisplayFunc (display);
    glutIdleFunc (display);
    glutJoystickFunc (joystick, 10);
    glutReshapeFunc (reshape);

    setup_textures();
    glutMainLoop();

    //Free our texture
    FreeTexture();

    return 0;
}
