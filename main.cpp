
#include "renderwidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    pdebug("Debugging enabled");
    QApplication a(argc, argv);
   QGLFormat glFormat = QGLFormat(QGL::DoubleBuffer | QGL::DirectRendering | QGL::DeprecatedFunctions);
   // QGLFormat glFormat = QGLFormat(QGL::DoubleBuffer | QGL::DirectRendering);

    glFormat.setVersion( 3, 0 );
//    glFormat.setDepthBufferSize(32);
    glFormat.setProfile( QGLFormat::CompatibilityProfile ); // Requires >=Qt-4.8.0
   // glFormat.setProfile(QGLFormat::CoreProfile);
    RenderWidget w(glFormat);
    w.show();
    w.resize(1280,720);
    w.move(0,0);
//    w.showFullScreen();
    w.setWindowTitle("Procedural Terrain");
    return a.exec();
}
