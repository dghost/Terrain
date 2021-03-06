#include "renderwidget.h"


void RenderWidget::keyPressEvent(QKeyEvent *event)
{
    if (!event->isAutoRepeat())
    {
        if(event->key() == Qt::Key_Escape)
        {
            if (_timerID != 0)
                killTimer(_timerID);
            qApp->processEvents();
            close();
        }


        if(event->key() == Qt::Key_Space)
        {
            _paused = !_paused;
        }
        if(event->key() == Qt::Key_Q)
        {
            _keysDown.Q = true;
        }
        if(event->key() == Qt::Key_W)
        {
            if(event->modifiers().testFlag(Qt::ControlModifier))
            {
                _modes.drawWater = !_modes.drawWater;
            } else {

                _keysDown.W = true;

            }
        }


        if(event->key() == Qt::Key_E)
        {
            _keysDown.E = true;
        }
        if(event->key() == Qt::Key_A)
        {
            _keysDown.A = true;
        }
        if(event->key() == Qt::Key_S)
        {
            _keysDown.S = true;
        }
        if(event->key() == Qt::Key_D)
        {
            _keysDown.D = true;
        }

        if(event->key() == Qt::Key_Plus)
        {
            _keysDown.Plus = true;
        }
        if(event->key() == Qt::Key_Minus)
        {
            _keysDown.Minus = true;
        }
        if (event->key() == Qt::Key_F)
        {
            if (isFullScreen())
            {
                showNormal();
            } else
            {
                showFullScreen();
            }
        }

        if(event->key() == Qt::Key_H)
        {
            _hud.enabled = !_hud.enabled;
        }


        if(event->key() == Qt::Key_B)
        {
            _modes.blendWater = !_modes.blendWater;
        }

        if(event->key() == Qt::Key_R  && event->modifiers().testFlag(Qt::ControlModifier))
        {
            for (int i = 0 ; i < 7 ; i++)
            {
                generateTexture(_groundTexture[i],_terrain);
            }
        }

        if(event->key() == Qt::Key_T  && event->modifiers().testFlag(Qt::ControlModifier))
        {
            _modes.drawTerrain = !_modes.drawTerrain;
        }

        if(event->key() == Qt::Key_X)
        {
            _wireFrame = !_wireFrame;
        }
        if(event->key() == Qt::Key_BracketLeft)
        {
            if (_groundMesh > 0)
                _groundMesh--;
        }

        if(event->key() == Qt::Key_BracketRight)
        {
            if (_groundMesh < 6)
                _groundMesh++;
        }

        if(event->key() == Qt::Key_Semicolon)
        {
            if (_waterMesh > 0)
                _waterMesh--;
        }

        if(event->key() == Qt::Key_Apostrophe)
        {
            if (_waterMesh < 6)
                _waterMesh++;
        }
        if(event->key() == Qt::Key_O)
        {
            if (_gndTexture > 0)
                _gndTexture--;
        }

        if(event->key() == Qt::Key_P)
        {
            if (_gndTexture < 6)
                _gndTexture++;
        }
        if(event->key() == Qt::Key_K)
        {
            if (_wtrTexture > 0)
                _wtrTexture--;
        }

        if(event->key() == Qt::Key_L)
        {
            if (_wtrTexture < 6)
                _wtrTexture++;
        }
        if(event->key() == Qt::Key_Comma)
        {
            if (_cldTexture > 0)
                _cldTexture--;
        }

        if(event->key() == Qt::Key_Period)
        {
            if (_cldTexture < 6)
                _cldTexture++;
        }

        if(event->key() == Qt::Key_1)
        {
            if (_wtrShader < 1)
                _wtrShader++;
            else
                _wtrShader = 0;
        }
        if(event->key() == Qt::Key_2)
        {
            if (_gndShader)
                _gndShader = 0;
            else
                _gndShader = 1;
        }
    }

}

void RenderWidget::keyReleaseEvent(QKeyEvent *event)
{

    if (!event->isAutoRepeat())
    {
        if(event->key() == Qt::Key_Q)
        {
            _keysDown.Q = false;
        }
        if(event->key() == Qt::Key_W)
        {
            _keysDown.W = false;
        }
        if(event->key() == Qt::Key_E)
        {
            _keysDown.E = false;
        }
        if(event->key() == Qt::Key_A)
        {
            _keysDown.A = false;
        }
        if(event->key() == Qt::Key_S)
        {
            _keysDown.S = false;
        }
        if(event->key() == Qt::Key_D)
        {
            _keysDown.D = false;
        }
        if(event->key() == Qt::Key_Plus)
        {
            _keysDown.Plus = false;
        }
        if(event->key() == Qt::Key_Minus)
        {
            _keysDown.Minus = false;
        }
    }
}

// event to handle scrolling with scroll wheel
void RenderWidget::wheelEvent(QWheelEvent *event)
{
    float numDegrees = event->delta() / 8;
    float numSteps = numDegrees / 15;

    if (event->orientation() == Qt::Vertical) {
#ifdef __APPLE__
        // compensate for apple's inverted scrolling
        _camera.radius += numSteps * 2.0;
#else
        // everyone else behaves correctly
        _camera.radius -= numSteps * 2.0;
#endif
    }
    event->accept();
}

void RenderWidget::processInput(float timeSinceLastUpdate)
{

    if (_hasFocus)
    {
    _camera.pan  -= (QCursor::pos().x() - _mouseStatus.startX) / 2.0;
    _camera.tilt -= (QCursor::pos().y() - _mouseStatus.startY) / 2.0;
    QCursor::setPos(width()/2,height()/2);
    _mouseStatus.startX = width()/2;
    _mouseStatus.startY = height()/2;
    }
    float perFrame = timeSinceLastUpdate / 1000.0;
    if (_camera.mode == 0)
    {
        if ((_keysDown.Q || _keysDown.Minus) && (!_keysDown.E && !_keysDown.Plus))
        {
            _camera.radius += 100 * perFrame;
        } else if ((_keysDown.E || _keysDown.Plus) && (!_keysDown.Q && !_keysDown.Minus))
        {
            _camera.radius -= 100 * perFrame;
        }
        if (_keysDown.W && !_keysDown.S)
        {
            _camera.rotVert -= 45  * perFrame;
        } else if (_keysDown.S && !_keysDown.W)
        {
            _camera.rotVert += 45 * perFrame;
        }
        if (_keysDown.A && !_keysDown.D)
        {
            _camera.rotHoriz += 90 * perFrame;
        } else if (_keysDown.D && !_keysDown.A)
        {
            _camera.rotHoriz -= 90 * perFrame;
        }
    } else 	if (_camera.mode == 1)
    {
        if (_keysDown.Q && !_keysDown.E)
        {
            _camera.z -= 75 * perFrame;
        } else if (_keysDown.E && !_keysDown.Q)
        {
            _camera.z += 75 * perFrame;
        }
        if (_keysDown.W && !_keysDown.S)
        {
            _camera.x -= sin(_camera.pan * M_PI / 180) * 75 * perFrame;
            _camera.y -= -cos(_camera.pan * M_PI / 180) * 75 * perFrame;


        } else if (_keysDown.S && !_keysDown.W)
        {
            _camera.x += sin(_camera.pan * M_PI / 180) * 75 * perFrame;
            _camera.y += -cos(_camera.pan * M_PI / 180) * 75 * perFrame;
        }
        if (_keysDown.A && !_keysDown.D)
        {
            //   _camera.pan += 90  * perFrame;

            _camera.x -= cos(_camera.pan * M_PI / 180) * 75 * perFrame;
            _camera.y -= sinf(_camera.pan * M_PI / 180) * 50 * perFrame;


        } else if (_keysDown.D && !_keysDown.A)
        {
            //  _camera.pan -= 90 * perFrame;

            _camera.x += cos(_camera.pan * M_PI / 180) * 75 * perFrame;
            _camera.y += sinf(_camera.pan * M_PI / 180) * 75 * perFrame;

        }
    }

}



bool RenderWidget::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == qApp)
    {
        if (event->type() == QEvent::ApplicationActivate)
        {
                pdebug("Window gained focus");
                setCursor(QCursor(Qt::BlankCursor));
                _hasFocus = true;
                setMouseTracking(true);
                grabMouse();
                QCursor::setPos(width()/2,height()/2);
                _mouseStatus.startX = width()/2;
                _mouseStatus.startY = height()/2;
        } else if (event->type() == QEvent::ApplicationDeactivate)
        {
                pdebug("Window lost focus");
                setCursor(QCursor(Qt::ArrowCursor));
                _hasFocus = false;
               setMouseTracking(false);
               releaseMouse();
        }
    }
    return QGLWidget::eventFilter(watched,event);
}
