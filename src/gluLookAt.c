
#include "common.h"

void mygluLookAt(GLdouble eyex, GLdouble eyey, GLdouble eyez, GLdouble centerx, GLdouble centery, GLdouble centerz, GLdouble upx, GLdouble upy, GLdouble upz)
{
    float forward[3], side[3], up[3];

    // Compute the forward vector
    forward[0] = centerx - eyex;
    forward[1] = centery - eyey;
    forward[2] = centerz - eyez;
    float forwardLen = sqrt(forward[0] * forward[0] + forward[1] * forward[1] + forward[2] * forward[2]);
    if (forwardLen != 0.0f) {
        forward[0] /= forwardLen;
        forward[1] /= forwardLen;
        forward[2] /= forwardLen;
    }

    // Compute the side vector
    float sideX = forward[1] * upz - forward[2] * upy;
    float sideY = forward[2] * upx - forward[0] * upz;
    float sideZ = forward[0] * upy - forward[1] * upx;
    float sideLen = sqrt(sideX * sideX + sideY * sideY + sideZ * sideZ);
    if (sideLen != 0.0f) {
        side[0] = sideX / sideLen;
        side[1] = sideY / sideLen;
        side[2] = sideZ / sideLen;
    }

    // Compute the up vector
    up[0] = side[1] * forward[2] - side[2] * forward[1];
    up[1] = side[2] * forward[0] - side[0] * forward[2];
    up[2] = side[0] * forward[1] - side[1] * forward[0];

    // Build the transformation matrix
    GLfloat m[4][4] = {
        { side[0], up[0], -forward[0], 0 },
        { side[1], up[1], -forward[1], 0 },
        { side[2], up[2], -forward[2], 0 },
        { 0, 0, 0, 1 }
    };
    glMultMatrixf(&m[0][0]);

    // Apply the translation to the camera position
    glTranslated(-eyex, -eyey, -eyez);
}
