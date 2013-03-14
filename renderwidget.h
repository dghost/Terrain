#ifndef RENDERWIDGET_H
#define RENDERWIDGET_H

#define DEBUG

#ifdef DEBUG
#define pdebug(x) qDebug() << x
#else
#define pdebug(x) do {} while (0)
#endif

#define BUFFER_OFFSET(bytes) ((GLubyte*) NULL + (bytes))

#include "gl_core_3_0.h"

#include <QGLWidget>
#include <QTime>
#include <QElapsedTimer>
#include <QGLShaderProgram>
#include <QApplication>
#include <QFocusEvent>

#include <QTimerEvent>
#include <QMouseEvent>
#include <QWheelEvent>

#define _USE_MATH_DEFINES
#include <math.h>

#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_operation.hpp>

#define BUFFER_OFFSET(bytes) ((GLubyte*) NULL + (bytes))

#define GL_RGBA32F                        0x8814
#define GL_RGBA16F                        0x881A
#define GL_R32F                           0x822E

typedef struct {

    struct {
      GLfloat x,y,z;
    } vertex;
    struct {
      GLfloat x,y,z;
    }  normal;

    struct {
      GLfloat u,v;
    }  texture;

} vertex_t;

typedef struct {
    GLuint textureHandle;
    GLuint frameBuffer;
    int width,height;
} texture_t;

typedef struct {
    vertex_t *mesh;
    GLuint *index;
    int vertexOffset;
    int normalOffset;
    int texOffset;
    int stride;
    int indexCount;
    GLuint vboID;
} mesh_t;

class RenderWidget : public QGLWidget
{
    Q_OBJECT
    
public:
    explicit RenderWidget(const QGLFormat& format,QGLWidget *parent = 0);
    ~RenderWidget();
    void toggleWireframe(void);
    void setPerlinScale(float scale);
    void setTimeScale(float timeScale);
    void setSubDivs(int subDivs);
    void setXLength(int length);
    void setYLength(int length);
    int FramesPerSecond(void);
protected:
    void paintGL();
    void initializeGL();
    void resizeGL(int width, int height);
    void mouseMoveEvent ( QMouseEvent * event );
    void mousePressEvent ( QMouseEvent * event );
    void mouseReleaseEvent ( QMouseEvent * event );
    void keyPressEvent(QKeyEvent *event);
    void timerEvent(QTimerEvent *event);
    void wheelEvent(QWheelEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void processInput(float timeSinceLastUpdate);
    void updateCamera();
    void drawFullScreenQuad(GLuint vert);
    void generateTexture(texture_t texStruct, QGLShaderProgram *shader);
    void generateFlatMesh(mesh_t &mesh, int width, int height, float scale = 1.0);
    void generateSphere(mesh_t &mesh, int width, int height, float radius = 1.0);
    void drawMesh(mesh_t &mesh, GLuint vert, GLuint norm, GLuint tex);
    void drawHUD(void);
    void initTexture(texture_t &texture, int width, int height);
    void focusInEvent(QFocusEvent *event);
    void focusOutEvent(QFocusEvent *event);

private:

    struct {
        bool drawTerrain;
        bool drawWater;
        bool blendWater;
    } _modes;

    struct {
        GLfloat x,y,z;
        GLfloat pan, tilt;
        GLfloat rotHoriz,rotVert;
        GLfloat radius;
        unsigned int mode;
        glm::mat4 viewMatrix;
        glm::mat4 projMatrix;
    } _camera;

    struct {
        GLint xLength;
        GLint yLength;
        GLint subDivs;
        float timeScale;
        float perlinScale;
        double timeElapsed;
        glm::vec2 texOffsets;
    } _mesh;

    struct {
        bool leftButtonHeldDown;
        int startX, startY;
        GLfloat origHoriz;
        GLfloat origVert;
    } _mouseStatus;

    struct {
        bool Q;
        bool W;
        bool E;
        bool A;
        bool S;
        bool D;
        bool Minus;
        bool Plus;
    } _keysDown;

    QElapsedTimer _runTime;
    QElapsedTimer _fpsTime;
    bool _wireFrame;
    bool _ortho;
    bool _textureLoaded;
    bool _paused;

    struct {
        bool enabled;
        QString oglVersion;
        QString resolution;
        QString textureUnits;
        QString maxSize;
        QString frameTime;
        QString numTris;
        QString sizes[8];
    } _hud;
    int _timerID;

    glm::vec3 lightPosition;
    glm::vec3 _lightMovement;
    mesh_t _flatMesh[8];
    int _groundMesh;
    int _waterMesh;
    int _gndTexture;
    int _wtrTexture;
    int _cldTexture;

    int _wtrShader;
    int _gndShader;

    texture_t _groundTexture[8];
    texture_t _cloudTexture[8];
    texture_t _waterTexture[8];

    QGLShaderProgram *_sky;
    QGLShaderProgram *_terrain;
    QGLShaderProgram *_clouds;
    QGLShaderProgram* _ground[2];
    QGLShaderProgram *_flow;
    QGLShaderProgram* _water[2];

    mesh_t _skysphere;

    struct {
        int fps;
        int sec0;
        int count;
    } _fpsInfo;



};

#endif // RENDERWIDGET_H
