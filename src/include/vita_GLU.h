#ifndef VITA_GLU
#define VITA_GLU

#include <vitaGL.h>

typedef struct GLUquadric GLUquadric;
typedef GLUquadric GLUquadricObj;
#define GLU_SMOOTH                         100000
#define GLU_FLAT                           100001
#define GLU_NONE                           100002
#define GLU_ERROR                          100103
#define GLU_OUTSIDE                        100020
#define GLU_INSIDE                         100021
#define GLU_POINT                          100010
#define GLU_LINE                           100011
#define GLU_FILL                           100012
#define GLU_SILHOUETTE                     100013
#define GLU_INVALID_ENUM                   100900
#define GLU_INVALID_VALUE                  100901
#define GLU_INVALID_OPERATION              100904

#define GLU_FALSE                          0
#define GLU_TRUE                           1
extern void gluSphere(GLUquadric *qobj, GLdouble radius, GLint slices, GLint stacks);
extern GLUquadric * gluNewQuadric(void);
extern void gluQuadricNormals(GLUquadric *qobj, GLenum normals);
extern void gluQuadricTexture(GLUquadric *qobj, GLboolean textureCoords);
extern void gluQuadricOrientation(GLUquadric *qobj, GLenum orientation);
extern void gluQuadricDrawStyle(GLUquadric *qobj, GLenum drawStyle);
#endif
