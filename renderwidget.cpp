#include "renderwidget.h"

const GLdouble pi = 3.1415926535897932384626433832795;

RenderWidget::RenderWidget(const QGLFormat& format, QGLWidget *parent) :
    QGLWidget(format, parent)
{
    _modes.drawTerrain = true;
    _modes.drawWater = true;
    _modes.blendWater = true;
    // initialize variables
    _wireFrame = false;
    _ortho = true;
    _textureLoaded = false;
    _hud.enabled = true;
    for (int i = 0 ; i < 10 ; i++)
    {
        _flatMesh[i].mesh = NULL;
        _flatMesh[i].index = NULL;
        _flatMesh[i].vboID = 0;
    }
    _skysphere.mesh = NULL;
    _skysphere.index = NULL;
    _skysphere.vboID = 0;
    _quad.mesh = NULL;
    _quad.index = NULL;
    _quad.vboID = 0;
    _timerID = 0;
    _keysDown.Q = false;
    _keysDown.W = false;
    _keysDown.E = false;
    _keysDown.A = false;
    _keysDown.S = false;
    _keysDown.D = false;
    _keysDown.Plus = false;
    _keysDown.Minus = false;

    _paused = false;

    _groundMesh = 3;
    _waterMesh = 0;
    _gndTexture = 5;
    _wtrTexture = 4;
    _cldTexture = 3;

    _wtrShader = 1;
    _gndShader = 1;

    _camera.x = 0;
    _camera.y = 0;
    _camera.z = 50.0;
    _camera.tilt = 0;
    _camera.pan = 0;
    _camera.radius = 20.0f;
    _camera.rotHoriz = 225;
    _camera.rotVert = 45;
    _camera.mode = 1;
    _mesh.xLength = 200;
    _mesh.yLength = 200;
    _mesh.subDivs = 1.0;
    _mesh.perlinScale = 10.0;
    _mesh.timeScale = 0.1f;
    _mesh.timeElapsed = (float) (QTime::currentTime()).msec();
    _mesh.texOffsets = glm::vec2(0.0);

    _fpsInfo.fps = 0;
    _fpsInfo.count = 0;
    _fpsInfo.sec0 = 0;

    setCursor(QCursor(Qt::BlankCursor));

    lightPosition = glm::vec3(1500,0,1500);
    _lightMovement = glm::vec3(50,0,0);

    _hasFocus = isActiveWindow();

    if (qApp)
        qApp->installEventFilter(this);
}

RenderWidget::~RenderWidget()
{
    makeCurrent();

    for (int i = 0 ; i < 5 ; i++)
    {
        if (_flatMesh[i].mesh != NULL)
            free(_flatMesh[i].mesh);

        if (_flatMesh[i].index != NULL)
            free(_flatMesh[i].index);
        glDeleteBuffers(1,&_groundTexture[i].frameBuffer);
        glDeleteTextures(1,&_groundTexture[i].textureHandle);
        glDeleteBuffers(1,&_cloudTexture[i].frameBuffer);
        glDeleteTextures(1,&_cloudTexture[i].textureHandle);
        glDeleteBuffers(1,&_waterTexture[i].frameBuffer);
        glDeleteTextures(1,&_waterTexture[i].textureHandle);
    }

    delete _sky;
    delete _terrain;
    delete _clouds;
    delete _flow;

    for (int i = 0 ; i < 2 ; i++)
    {
        delete _water[i];
    }


    for (int i = 0 ; i < 2 ; i++)
    {
        delete _ground[i];
    }
    if (_skysphere.mesh != NULL)
        free(_skysphere.mesh);
    if (_skysphere.index != NULL)
        free(_skysphere.index);

    doneCurrent();
    setCursor(QCursor(Qt::ArrowCursor));
}


// handle resizing the view
void RenderWidget::resizeGL(int width, int height)
{
    if (height == 0) {
        height = 1;
    }

    _hud.resolution = QString("Resolution: %1 x %2").arg(width).arg(height);
    pdebug(_hud.resolution);
    glViewport(0, 0, width, height);

    GLfloat fW, fH;
    // 60 degree FOV and fixed depth of 1-5000
    fH = tan( 60.0 / 360 * pi );
    fW = fH * (float) width / (float) height;

    _camera.projMatrix = glm::frustum(-fW, fW, -fH, fH, 1.0f, 5000.0f);



}

void RenderWidget::initializeGL()
{
#ifndef __APPLE__
    ogl_LoadFunctions();
#endif
    /* capture some debug info */
    _hud.oglVersion = QString("OGL: %1 GLSL: %2").arg((char *) glGetString(GL_VERSION)).arg((char *) glGetString(GL_SHADING_LANGUAGE_VERSION));
    pdebug(_hud.oglVersion);

    int MaxVertexTextureImageUnits;
    glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &MaxVertexTextureImageUnits);
    int MaxCombinedTextureImageUnits;
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &MaxCombinedTextureImageUnits);
    int MaxFragmnetTextureImageUnits;
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS,&MaxFragmnetTextureImageUnits);

    _hud.textureUnits = QString("Max TIUS - Frag: %1 Vert: %2 Comb: %3").arg(MaxFragmnetTextureImageUnits).arg(MaxVertexTextureImageUnits).arg(MaxCombinedTextureImageUnits);
    pdebug(_hud.textureUnits);

    GLint dims[2];
    glGetIntegerv(GL_MAX_VIEWPORT_DIMS, &dims[0]);

    _hud.maxSize = QString("Max viewport: %1 x %2").arg(dims[0]).arg(dims[1]);
    pdebug(_hud.maxSize);

    /* generate shaders */
    pdebug("Compiling Sky shader...");
    _sky = new ShaderProgram();
    _sky->addVertex("Sky.vert");
    _sky->addFragment("Sky.frag");
    _sky->link();

    pdebug("Compiling Ground shader...");
    _ground[0] = new ShaderProgram();
    _ground[0]->addVertex("Ground.vert");
    _ground[0]->addFragment("GroundNoCaustic.frag");
    _ground[0]->addTesselationControl("Ground.tessc");
    _ground[0]->addTesselationEvaluation("Ground.tesse");
    _ground[0]->link();

    _ground[1] = new ShaderProgram();
    _ground[1]->addVertex("Ground.vert");
    _ground[1]->addFragment("Ground.frag");
    _ground[1]->addTesselationControl("Ground.tessc");
    _ground[1]->addTesselationEvaluation("Ground.tesse");
    _ground[1]->link();

    pdebug("Compiling Water shader...");

    _water[0] = new ShaderProgram();
    _water[0]->addVertex("tesselate.vert");
    _water[0]->addFragment("WaterFast.frag");
    _water[0]->addTesselationControl("Water.tessc");
    _water[0]->addTesselationEvaluation("Water.tesse");
    _water[0]->link();

    _water[1] = new ShaderProgram();
    _water[1]->addVertex("tesselate.vert");
    _water[1]->addFragment("Water.frag");
    _water[1]->addTesselationControl("Water.tessc");
    _water[1]->addTesselationEvaluation("Water.tesse");
    _water[1]->link();

    pdebug("Compiling flow shader...");
    _flow = new ShaderProgram();
    _flow->addVertex("quad.vert");
    _flow->addFragment("flow.frag");
    _flow->link();

    pdebug("Compiling cloud shader...");
    _clouds = new ShaderProgram();
    _clouds->addVertex("quad.vert");
    _clouds->addFragment("clouds.frag");
    _clouds->link();

    pdebug("Compiling terrain shader...");
    _terrain = new ShaderProgram();
    _terrain->addVertex("quad.vert");
    _terrain->addFragment("terrain.frag");
    _terrain->link();


    int texSize = 64;
    int meshSize = 8;
    for (int i = 0; i < 7 ; i++)
    {
        _hud.texSizes[i] = QString("%1 x %1").arg(texSize);
        _hud.meshSizes[i] = QString("%1 x %1").arg(meshSize);
        pdebug(QString("Generating flat mesh of size %1").arg(meshSize));
        generateFlatMesh(_flatMesh[i],meshSize,meshSize,2048.0 / meshSize);

        pdebug(QString("Generating textures of size %1").arg(texSize));
        initTexture(_waterTexture[i],texSize,texSize);
        initTexture(_groundTexture[i],texSize,texSize);
        initTexture(_cloudTexture[i],texSize,texSize);
        texSize *= 2;
        meshSize *= 2;
    }

    pdebug("Generating sphere mesh...");
    generateSphere(_skysphere,16,16,1.0);

    pdebug("Generating full screen quad mesh...");
    generateQuad(_quad);

    // standard opengl enables
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);
    glClearColor(0.0,0.0,0.0,0.0);

    pdebug("Generating textures...");
    /* initialize textures for FBO's */


    /* generate the initial ground value */
    pdebug("Generating ground textures");
    for (int i = 0 ; i < 8 ; i++)
    {
        generateTexture(_groundTexture[i],_terrain);
    }
    generateTexture(_waterTexture[_wtrTexture],_flow);
    generateTexture(_cloudTexture[_cldTexture],_clouds);

    grabMouse();
    QCursor::setPos(width()/2,height()/2);
    _mouseStatus.startX = width()/2;

    setMouseTracking(true);

    // start tracking time
    _runTime.start();
    _fpsTime.start();
    _textureTime.start();
    // generate meshes

    _timerID = startTimer(0);


}


void RenderWidget::timerEvent ( QTimerEvent * event )
{
    // calculate the time elapsed for the perlin waves
    qint64 timeElapsed = _runTime.nsecsElapsed();
    float msElapsedSinceRender = timeElapsed / 1000000.0;
    qint64 texTimeElapsed = _textureTime.nsecsElapsed();
    float msElapsedSinceTexture = texTimeElapsed / 1000000.0;

    if (msElapsedSinceTexture >= 33.3)
    {
        _textureTime.start();
        if (!_paused)
        {
            // if not paused update the textures
            if (_modes.drawWater)
                generateTexture(_waterTexture[_wtrTexture],_flow);
            generateTexture(_cloudTexture[_cldTexture],_clouds);
        }
    }

    if (msElapsedSinceRender >= 0.0)
    {
        _runTime.start();


        processInput(msElapsedSinceRender);
        if (!_paused)
        {
            // if not paused update the timing info
            float timePassed = _mesh.timeScale * msElapsedSinceRender / 1000.0;
            _mesh.timeElapsed += timePassed;
            _mesh.texOffsets += glm::vec2(0.1,0.02) * timePassed;

            //     if (lightPosition.x > 1500 || lightPosition.x < -1500) {

            //         _lightMovement = glm::vec3(-_lightMovement.x,-_lightMovement.y,-_lightMovement.z);
            //     }

            //     lightPosition += _lightMovement;

            lightPosition.x = 1500 * cos(_mesh.timeElapsed);
            lightPosition.z = 1500 * fabs(sin(_mesh.timeElapsed));
        }


        if (isVisible())
            updateGL();

    }
    event->accept();

}



// render path
void RenderWidget::paintGL()
{
    QElapsedTimer renderTimer;
    renderTimer.start();

    // standard setup

    // update the camera position
    updateCamera();

    int trisRendered = 0;



    // draw the mesh in a series of strips

    if (_wireFrame)
    {
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    } else {
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,_cloudTexture[_cldTexture].textureHandle);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D,_groundTexture[_gndTexture].textureHandle);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D,_waterTexture[_wtrTexture].textureHandle);



    if (_modes.drawTerrain)
    {
        if (_flatMesh[_groundMesh].mesh != NULL && _flatMesh[_groundMesh].index != NULL)
        {
            int index = _gndShader;
            // glBindBuffer(GL_ARRAY_BUFFER,_flatMesh[_groundMesh].vboID);


            /* draw the terrain */


            _ground[index]->bind();
            GLint t0Loc = _ground[index]->uniformLocation("cloudTexture");
            GLint t1Loc = _ground[index]->uniformLocation("groundTexture");
            GLint t2Loc = _ground[index]->uniformLocation("waterTexture");
            GLint lPos = _ground[index]->uniformLocation("light_pos");
            GLint vPos = _ground[index]->uniformLocation("viewMatrix");
            GLint pPos = _ground[index]->uniformLocation("projMatrix");
            GLint sPos = _water[index]->uniformLocation("screen_size");
            GLuint vnPos = _ground[index]->attributeLocation("inNormal");
            GLuint vtPos = _ground[index]->attributeLocation("inTexCoord");
            GLuint vvPos = _ground[index]->attributeLocation("inVertex");

            glUniform1i(t0Loc,0);
            glUniform1i(t1Loc,1);
            glUniform1i(t2Loc,2);
            glUniform3fv(lPos,1,glm::value_ptr(lightPosition));
            glUniformMatrix4fv(pPos,1,0,glm::value_ptr(_camera.projMatrix));
            glUniformMatrix4fv(vPos,1,0,glm::value_ptr(_camera.viewMatrix));
            glUniform2f(sPos,width(),height());
            drawTessMesh(_flatMesh[_groundMesh],vvPos,vnPos,vtPos);
            _ground[index]->release();
            trisRendered += _flatMesh[_groundMesh].indexCount / 3;


        }


        /* draw the water */
    }
    if (_modes.drawWater)
    {
        if (_flatMesh[_waterMesh].mesh != NULL && _flatMesh[_waterMesh].index != NULL)
        {
            int index = _wtrShader;
            //glBindBuffer(GL_ARRAY_BUFFER,_flatMesh[_waterMesh].vboID);
            if (_modes.blendWater && !_wireFrame)
            {
                glEnable(GL_BLEND);
                glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            }

            _water[index]->bind();
            GLint t0Loc = _water[index]->uniformLocation("cloudTexture");
            GLint t1Loc = _water[index]->uniformLocation("groundTexture");
            GLint t2Loc = _water[index]->uniformLocation("waterTexture");
            GLint lPos = _water[index]->uniformLocation("light_pos");
            GLint vPos = _water[index]->uniformLocation("viewMatrix");
            GLint pPos = _water[index]->uniformLocation("projMatrix");
            GLint sPos = _water[index]->uniformLocation("screen_size");
            GLuint vnPos = _water[index]->attributeLocation("inNormal");
            GLuint vtPos = _water[index]->attributeLocation("inTexCoord");
            GLuint vvPos = _water[index]->attributeLocation("inVertex");

            glUniform1i(t0Loc,0);
            glUniform1i(t1Loc,1);
            glUniform1i(t2Loc,2);
            glUniform3fv(lPos,1,glm::value_ptr(lightPosition));
            glUniformMatrix4fv(pPos,1,0,glm::value_ptr(_camera.projMatrix));
            glUniformMatrix4fv(vPos,1,0,glm::value_ptr(_camera.viewMatrix));
            glUniform2f(sPos,width(),height());
            drawTessMesh(_flatMesh[_waterMesh],vvPos,vnPos,vtPos);

            _water[index]->release();

            if (_modes.blendWater && !_wireFrame)
            {
                glDisable(GL_BLEND);
            }

            trisRendered += _flatMesh[_waterMesh].indexCount / 3;

        }

    }


    if (_skysphere.mesh != NULL && _skysphere.index != NULL && !_wireFrame)
    {


        _sky->bind();
        GLint t0Loc = _sky->uniformLocation("cloudTexture");
        GLint oLoc = _sky->uniformLocation("in_Offsets");
        GLint vPos = _sky->uniformLocation("viewMatrix");
        GLint pPos = _sky->uniformLocation("projMatrix");
        GLuint vnPos = _sky->attributeLocation("inNormal");
        GLuint vtPos = _sky->attributeLocation("inTexCoord");
        GLuint vvPos = _sky->attributeLocation("inVertex");

        glUniform1i(t0Loc,0);
        glUniformMatrix4fv(pPos,1,0,glm::value_ptr(_camera.projMatrix));
        glUniformMatrix4fv(vPos,1,0,glm::value_ptr(_camera.viewMatrix));
        glUniform2fv(oLoc,1,glm::value_ptr(_mesh.texOffsets));

        drawMesh(_skysphere,vvPos,vnPos,vtPos);

        _sky->release();

        trisRendered += _skysphere.indexCount / 3;


    }

    glActiveTexture(GL_TEXTURE0);
    glFinish();


    qint64 timeElapsed = renderTimer.nsecsElapsed();
    float msElapsed = timeElapsed / 1000000.0;
    _hud.frameTime = QString("Frame draw time: %1").arg(msElapsed);
    _hud.numTris = QString("Number of triangles: %1").arg(trisRendered);
    /* draw the HUD */

    if (_hud.enabled)
    {
        drawHUD();
    }
}

void RenderWidget::drawHUD()
{
    QString text;
    int offset = 20;

    //    renderText(10,offset,_hud.oglVersion,this->font());
    //    offset += 20;

    renderText(10,offset,_hud.resolution,this->font());
    offset += 20;

    if (_gndShader == 0)
    {
        text = QString("Ground: No Caustics");
    } else {
        text = QString("Ground: Caustics");
    }

    renderText(10,offset,text,this->font());
    offset += 20;

    text = QString("Ground patch size: %1").arg(_hud.meshSizes[_groundMesh]);
    renderText(10,offset,text,this->font());
    offset += 20;

    text = QString("Ground texture size: %1").arg(_hud.texSizes[_gndTexture]);
    renderText(10,offset,text,this->font());
    offset += 20;

    if (_wtrShader == 0)
    {
        text = QString("Water: Diffuse");
    } else if (_wtrShader == 1){
        text = QString("Water: Fresnel");

    }
    renderText(10,offset,text,this->font());
    offset += 20;
    text = QString("Water patch size: %1").arg(_hud.meshSizes[_waterMesh]);
    renderText(10,offset,text,this->font());
    offset += 20;


    text = QString("Water texture size: %1").arg(_hud.texSizes[_wtrTexture]);
    renderText(10,offset,text,this->font());
    offset += 20;

    text = QString("Cloud texture size: %1").arg(_hud.texSizes[_cldTexture]);
    renderText(10,offset,text,this->font());
    offset += 20;

    text = QString("FPS: %1").arg(FramesPerSecond());
    renderText(10,offset,text,this->font());
    offset += 20;

    if (!_modes.blendWater)
    {
        text = QString("Blending disabled");
        renderText(10,offset,text,this->font());
        offset += 20;
    }

    if (!_modes.drawTerrain)
    {
        text = QString("Terrain mesh disabled");
        renderText(10,offset,text,this->font());
        offset += 20;
    }
    if (!_modes.drawWater)
    {
        text = QString("Water mesh disabled");
        renderText(10,offset,text,this->font());
        offset += 20;
    }

    if (_wireFrame)
    {
        text = QString("Wireframe mode");
        renderText(10,offset,text,this->font());
        offset += 20;
    }

    if (_paused)
    {
        text = QString("PAUSED");
        renderText(10,offset,text,this->font());
        offset += 20;
    }
}

void RenderWidget::updateCamera()
{
    // clamp camera values
    if (_camera.pan > 360)
    {
        _camera.pan -= 360;
    } else if (_camera.pan < 0)
    {
        _camera.pan += 360;
    }

    if (_camera.tilt > 90)
    {
        _camera.tilt = 90;
    } else if (_camera.tilt < -90)
    {
        _camera.tilt = -90;
    }

    if (_camera.rotHoriz > 360)
    {
        _camera.rotHoriz -= 360;
    } else if (_camera.rotHoriz < 0)
    {
        _camera.rotHoriz += 360;
    }

    if (_camera.rotVert > 180)
    {
        _camera.rotVert = 180;
    } else if (_camera.rotVert < 0)
    {
        _camera.rotVert = 0;
    }

    if (_camera.radius < 5)
    {
        _camera.radius = 5;
    } else if (_camera.radius > 50)
    {
        _camera.radius = 50;
    }

    if (_camera.mode == 0)
    {
        // generate x, y, z and pan/tilt angles off of rotation around a sphere

        _camera.y = _camera.radius * sin(_camera.rotVert * pi / 180.0) * cos(_camera.rotHoriz * pi / 180.0);
        _camera.x = _camera.radius * sin(_camera.rotVert * pi / 180.0) * sin(_camera.rotHoriz * pi / 180.0);
        _camera.z = _camera.radius * cos(_camera.rotVert * pi / 180.0);

        _camera.pan = 180 - _camera.rotHoriz;
        _camera.tilt = -90 +  _camera.rotVert;
    }

    // load the identity matrix and set up view correctly
    glm::mat4 view = glm::mat4(1);
    view = glm::rotate(view,90.0f,glm::vec3(-1,0,0));
    view = glm::rotate(view,_camera.tilt,glm::vec3(-1,0,0));
    view = glm::rotate(view,_camera.pan,glm::vec3(0,0,-1));
    _camera.viewMatrix = glm::translate(view,glm::vec3(-_camera.x,-_camera.y,-_camera.z));
}



// toggle wireframe on and off
void RenderWidget::toggleWireframe()
{
    _wireFrame = !_wireFrame;
}

// set the scale for the perlin noise
void RenderWidget::setPerlinScale(float scale)
{
    _mesh.perlinScale = scale;
}

// set the time scale for the perlin noise
void RenderWidget::setTimeScale(float timeScale)
{
    _mesh.timeScale = timeScale;
}


// draw a mesh
void RenderWidget::drawMesh(mesh_t &mesh,GLuint vert,GLuint norm,GLuint tex)
{
    if (mesh.vboID != 0)
    {
        glBindBuffer(GL_ARRAY_BUFFER,mesh.vboID);
        glVertexAttribPointer(vert,3,GL_FLOAT,GL_FALSE,sizeof(vertex_t),BUFFER_OFFSET(0 ));
        glVertexAttribPointer(norm,3,GL_FLOAT,GL_FALSE,sizeof(vertex_t),BUFFER_OFFSET(3* sizeof(GLfloat)));
        glVertexAttribPointer(tex,2,GL_FLOAT,GL_FALSE,sizeof(vertex_t),BUFFER_OFFSET(6* sizeof(GLfloat)));
        glEnableVertexAttribArray(vert);
        glEnableVertexAttribArray(norm);
        glEnableVertexAttribArray(tex);
        glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, mesh.index );
        glDisableVertexAttribArray(vert);
        glDisableVertexAttribArray(norm);
        glDisableVertexAttribArray(tex);
        glBindBuffer(GL_ARRAY_BUFFER,0);
    }

}

// draw a mesh
void RenderWidget::drawTessMesh(mesh_t &mesh,GLuint vert,GLuint norm,GLuint tex)
{
    if (mesh.vboID != 0)
    {
        glBindBuffer(GL_ARRAY_BUFFER,mesh.vboID);
        glVertexAttribPointer(vert,3,GL_FLOAT,GL_FALSE,sizeof(vertex_t),BUFFER_OFFSET(0 ));
        glVertexAttribPointer(norm,3,GL_FLOAT,GL_FALSE,sizeof(vertex_t),BUFFER_OFFSET(3* sizeof(GLfloat)));
        glVertexAttribPointer(tex,2,GL_FLOAT,GL_FALSE,sizeof(vertex_t),BUFFER_OFFSET(6* sizeof(GLfloat)));
        glEnableVertexAttribArray(vert);
        glEnableVertexAttribArray(norm);
        glEnableVertexAttribArray(tex);
        glPatchParameteri(GL_PATCH_VERTICES,3);
        glDrawElements(GL_PATCHES, mesh.indexCount, GL_UNSIGNED_INT, mesh.index );
        glDisableVertexAttribArray(vert);
        glDisableVertexAttribArray(norm);
        glDisableVertexAttribArray(tex);
        glBindBuffer(GL_ARRAY_BUFFER,0);
    }

}

// draw a full screen quad
void RenderWidget::drawFullScreenQuad(GLuint vert)
{
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

    glBindBuffer(GL_ARRAY_BUFFER,_quad.vboID);
    glVertexAttribPointer(vert,4,GL_FLOAT,GL_FALSE,0,BUFFER_OFFSET(0));
    glEnableVertexAttribArray(vert);
    glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, _quad.index );
    glDisableVertexAttribArray(vert);

    glBindBuffer(GL_ARRAY_BUFFER,0);
}

// render to a texture
void RenderWidget::generateTexture(texture_t texStruct, ShaderProgram *shader)
{
    // save state
    GLint currentFrameBuffer = 0;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING,&currentFrameBuffer);
    glBindTexture(GL_TEXTURE_2D,texStruct.textureHandle);
    // bind to framebuffer / texture
    glBindFramebuffer(GL_FRAMEBUFFER, texStruct.frameBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texStruct.textureHandle, 0);

    // change viewport size
    glViewport(0,0,texStruct.width,texStruct.height);


    GLenum res = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (res != GL_FRAMEBUFFER_COMPLETE)
    {
        pdebug(QString("Error creating framebuffer object"));
    }

    shader->bind();
    GLint tLoc = shader->uniformLocation("in_Time");
    glUniform1f(tLoc,_mesh.timeElapsed);
    GLint oLoc = shader->uniformLocation("in_Offsets");
    glUniform2fv(oLoc,1,glm::value_ptr(_mesh.texOffsets));
    GLuint vPos = shader->attributeLocation("inVertex");


    GLint texel = shader->uniformLocation("in_texelSize");
    GLint size = shader->uniformLocation("in_sideLength");

    glUniform2f(texel,1.0/texStruct.width,1.0/texStruct.height);
    glUniform2f(size,2048.0,2048.0);

    drawFullScreenQuad(vPos);

    shader->release();

    // restore window to previous state
    glBindFramebuffer(GL_FRAMEBUFFER, currentFrameBuffer);
    glBindTexture(GL_TEXTURE_2D,0);
    glViewport(0, 0, width(), height());

}

void RenderWidget::initTexture(texture_t &texture, int width, int height)
{
    texture.width = width;
    texture.height = height;
    texture.textureHandle = 0;

    glGenTextures(1, &texture.textureHandle);

    // Bind to current texture
    glBindTexture(GL_TEXTURE_2D, texture.textureHandle);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); // Linear Filtering

    // Give an empty image to OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA16F, texture.width, texture.height, 0,GL_RGBA, GL_UNSIGNED_BYTE, 0);

    // generate the framebuffer object
    glGenFramebuffers(1, &texture.frameBuffer);

    pdebug(QString("Generated texture %1 and FBO %2").arg(texture.textureHandle).arg(texture.frameBuffer));

    // unbind all
    glBindTexture(GL_TEXTURE_2D,0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}


void RenderWidget::generateQuad(fsquad_t &mesh)
{
    if (mesh.mesh != NULL)
        free(mesh.mesh);
    if (mesh.index != NULL)
        free(mesh.index);

    pdebug("Generating vertices");
    GLfloat quad[16] = {
        -1,-1,0,1,
        1,-1,0,1,
        -1,1,0,1,
        1,1,0,1
    };

    mesh.mesh = (GLfloat *) malloc(sizeof(GLfloat) * 16);

    for(int i = 0 ; i < 16 ; i++)
    {
        mesh.mesh[i] = quad[i];
    }

    pdebug("Generating Indices");

    //   GLuint indices[4] = {0,1,2,3};
    mesh.index = (GLuint *) malloc(sizeof(GLuint) * 4);
    mesh.index[0] = 0;
    mesh.index[1] = 1;
    mesh.index[2] = 2;
    mesh.index[3] = 3;

    mesh.indexCount = 4;
    mesh.vertexOffset = 0;
    pdebug("Generating VBO");
    glGenBuffers(1,&mesh.vboID);
    pdebug(mesh.vboID);
    if (mesh.vboID > 0)
    {
        glBindBuffer(GL_ARRAY_BUFFER,mesh.vboID);
        glBufferData(GL_ARRAY_BUFFER,sizeof(GLfloat) * 16,mesh.mesh,GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER,0);
    }
}

void RenderWidget::generateFlatMesh(mesh_t &mesh, int width, int height, float scale)
{
    if (mesh.mesh != NULL)
        free(mesh.mesh);
    if (mesh.index != NULL)
        free(mesh.index);

    pdebug("Generating vertices");
    mesh.mesh = (vertex_t *) malloc(sizeof(vertex_t) * (width + 1) * (height + 1));
    if (mesh.mesh != NULL)
    {
        for(int i = 0 ; i <= width; i++)
        {
            for(int j = 0 ; j <= height ; j++)
            {
                //  qpdebug() << i << j;
                vertex_t temp;
                temp.vertex.x = (i - width / 2.0) * scale;
                temp.vertex.y = (j - height / 2.0) * scale;
                temp.vertex.z = 0.0;
                temp.normal.x = 0.0;
                temp.normal.y = 0.0;
                temp.normal.z = 1.0;
                temp.texture.u = i / (float) width;
                temp.texture.v = j / (float) height;
                mesh.mesh[i * (height + 1) + j] = temp;
            }
        }
    }

    int numTris = height * width * 3 * 2;
    pdebug("Generating Indices");

    mesh.index = (GLuint *) malloc(sizeof(int) * numTris);
    int count = 0;
    if (mesh.index != NULL)
    {
        for(int i = 0 ; i < width ; i++)
        {
            for (int j = 0 ; j < height ; j++)
            {
                mesh.index[count] = i * (height + 1) + j;
                count++;
                mesh.index[count] = (i + 1) * (height + 1) + j;
                count++;
                mesh.index[count] = (i) * (height + 1) + (j + 1);
                count++;

                mesh.index[count] = (i) * (height + 1) + (j + 1);
                count++;
                mesh.index[count] = (i + 1) * (height + 1) + j;
                count++;
                mesh.index[count] = (i + 1) * (height + 1) + (j + 1);
                count++;

                /*
                mesh.index[count] = (i + 1) * (height + 1) + (j + 1);
                count++;
                mesh.index[count] = (i + 1) * (height + 1) + (j + 2);
                count++;
                mesh.index[count] = (i) * (height + 1) + (j + 1);
                count++;


                mesh.index[count] = (i) * (height + 1) + (j + 1);
                count++;
                mesh.index[count] = (i + 1) * (height + 1) + (j + 2);
                count++;
                mesh.index[count] = (i) * (height + 1) + (j + 2);
                count++;*/
            }

        }
    }
    mesh.indexCount = count;
    mesh.vertexOffset = 0;
    mesh.normalOffset = 3;
    mesh.texOffset = 6;
    mesh.stride = sizeof(vertex_t);
    pdebug("Generating VBO");
    glGenBuffers(1,&mesh.vboID);
    pdebug(mesh.vboID);
    if (mesh.vboID > 0)
    {
        glBindBuffer(GL_ARRAY_BUFFER,mesh.vboID);
        glBufferData(GL_ARRAY_BUFFER,sizeof(vertex_t) * (width + 1) * (height + 1),mesh.mesh,GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER,0);
    }
}

void RenderWidget::generateSphere(mesh_t &mesh, int width, int height, float radius)
{
    if (mesh.index != NULL)
    {
        free(mesh.index);
    }
    if (mesh.mesh != NULL)
    {
        free(mesh.mesh);
    }

#define PI 3.14159265358979323846f


    float theta, phi;
    int i, j, t, ntri, nvec;

    nvec = (height-2)* width+2;
    ntri = (height-2)*(width-1)*2;

    mesh.mesh = (vertex_t*) malloc( nvec * sizeof(vertex_t) );
    mesh.index =   (GLuint*) malloc( ntri * 3*sizeof(GLuint)   );

    for( t=0, j=1; j<height-1; j++ )
        for(      i=0; i<width; i++ )
        {
            theta = float(j)/(height-1) * PI;
            phi   = float(i)/(width-1 ) * PI*2;
            mesh.mesh[t].vertex.x =  radius * sinf(theta) * cosf(phi);
            mesh.mesh[t].vertex.y  = radius * -sinf(theta) * sinf(phi);
            mesh.mesh[t].vertex.z =  radius * cosf(theta);
            mesh.mesh[t].normal.x =  sinf(theta) * cosf(phi);
            mesh.mesh[t].normal.y  = -sinf(theta) * sinf(phi);
            mesh.mesh[t].normal.z =  cosf(theta);


            // don't generate proper texture coordinates
            mesh.mesh[t].texture.u =   (mesh.mesh[t].normal.x+ 1.0) / 2.0;
            mesh.mesh[t].texture.v =   (mesh.mesh[t].normal.y+ 1.0) / 2.0;
            t++;

        }
    mesh.mesh[t].normal.x = 0;
    mesh.mesh[t].normal.y = 0;
    mesh.mesh[t].normal.z = 1;
    mesh.mesh[t].texture.u =   (mesh.mesh[t].normal.x+ 1.0) / 2.0;
    mesh.mesh[t].texture.v =   (mesh.mesh[t].normal.y+ 1.0) / 2.0;
    mesh.mesh[t].vertex.x = 0;
    mesh.mesh[t].vertex.y = 0;
    mesh.mesh[t++].vertex.z = radius;
    mesh.mesh[t].normal.x = 0;
    mesh.mesh[t].normal.y = 0;
    mesh.mesh[t].normal.z = -1;
    mesh.mesh[t].texture.u =   (mesh.mesh[t].normal.x+ 1.0) / 2.0;
    mesh.mesh[t].texture.v =   (mesh.mesh[t].normal.y+ 1.0) / 2.0;

    mesh.mesh[t].vertex.x = 0;
    mesh.mesh[t].vertex.y = 0;
    mesh.mesh[t++].vertex.z = -radius;

    for( t=0, j=0; j<height-3; j++ )
        for(      i=0; i<width-1; i++ )
        {
            mesh.index[t++] = (j  )*width + i  ;
            mesh.index[t++] = (j+1)*width + i+1;
            mesh.index[t++] = (j  )*width + i+1;
            mesh.index[t++] = (j  )*width + i  ;
            mesh.index[t++] = (j+1)*width + i  ;
            mesh.index[t++] = (j+1)*width + i+1;
        }
    for( i=0; i<width-1; i++ )
    {
        mesh.index[t++] = (height-2)*width;
        mesh.index[t++] = i;
        mesh.index[t++] = i+1;
        mesh.index[t++] = (height-2)*width+1;
        mesh.index[t++] = (height-3)*width + i+1;
        mesh.index[t++] = (height-3)*width + i;
    }

    mesh.indexCount = t;
    mesh.vertexOffset = 0;
    mesh.normalOffset = 3;
    mesh.texOffset = 6;
    mesh.stride = sizeof(vertex_t);
    glGenBuffers(1,&mesh.vboID);
    pdebug(mesh.vboID);
    if (mesh.vboID > 0)
    {
        glBindBuffer(GL_ARRAY_BUFFER,mesh.vboID);
        glBufferData(GL_ARRAY_BUFFER,sizeof(vertex_t) * nvec,mesh.mesh,GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER,0);
    }
}



int RenderWidget::FramesPerSecond()
{
    int sec = _fpsTime.elapsed()/1000;
    if (sec !=_fpsInfo.sec0)
    {
        _fpsInfo.sec0 = sec;
        _fpsInfo.fps = _fpsInfo.count;
        _fpsInfo.count=0;
    }
    _fpsInfo.count++;
    return _fpsInfo.fps;
}


