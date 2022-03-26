#include "mainwindow.h"
#include <QOpenGLShader>
#include <QMatrix4x4>
#include <cmath>
#include <QMouseEvent>
#include "point.h"
#include "rect.h"
#include "size.h"
#include "plainrect.h"
#include <type_traits>

static_assert(std::is_same_v<GLfloat, float>);
static_assert(std::is_same_v<GLint, int>);

template<typename T>
Point toPoint(const T& source) {
    Point p {
        static_cast<float>(source.x()),
        static_cast<float>(source.y())
    };
    return p;
}

struct MainShaderProgram {
    QString vertShaderPath;
    QString fragShaderPath;
    int aColor;
    QMatrix4x4 matrix;
    QMatrix4x4 modelViewMatrix;
};

MainShaderProgram gMainShader;

constexpr auto gZeroOffset = static_cast<GLvoid*>(0);
Point gPrevPos;

MainWindow::MainWindow()
{
    gMainShader.vertShaderPath = getWorkDir() + "\\main.vert";
    gMainShader.fragShaderPath = getWorkDir() + "\\main.frag";
    unselect();
}

MainWindow::~MainWindow()
{
}

void MainWindow::addRect(Rect r)
{
    mPoints.add(r);
    if (mVao > -1) {
        glBindVertexArray(mVao);
        glBindBuffer(GL_ARRAY_BUFFER, mBufObj.vertex);
        glBufferData(GL_ARRAY_BUFFER, mPoints.getByteCount(), mPoints.getData(), GL_DYNAMIC_DRAW);
    }
    unselect();
}

void MainWindow::paintGL()
{
    glClearColor(0.01f, 0.36f, 0.86f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT);

    assert(mProgram->bind());

    glBindVertexArray(mVao);
    glDrawArrays(GL_QUADS, 0, static_cast<GLsizei>(mPoints.getCount()) / 2);
//    glDrawElements(GL_QUADS, 4, GL_UNSIGNED_BYTE, gZeroOffset);
    qDebug("Err %d", glGetError());
    glBindVertexArray(0);

    mProgram->release();

    static size_t i = 0;
    qDebug("frame %d", i++);
}

void MainWindow::initializeGL()
{
    initializeOpenGLFunctions();
    initShaderProgram();

    addRect({   0,   0, 100, 100 });
    addRect({ 100, 100, 100, 100 });

    glGenVertexArrays(1, &mVao);
    assert(mVao != 0);

    glGenBuffers(2, static_cast<GLuint*>(static_cast<void*>(&mBufObj)));
    assert((mBufObj.vertex != 0) && (mBufObj.element != 0));

    glBindVertexArray(mVao);

    glBindBuffer(GL_ARRAY_BUFFER, mBufObj.vertex);
    glBufferData(GL_ARRAY_BUFFER, mPoints.getByteCount(), mPoints.getData(), GL_DYNAMIC_DRAW);

//    static constexpr GLubyte rectIndices[] { 0, 1, 2, 3 };
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBufObj.element);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(rectIndices), rectIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, gZeroOffset);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    const qreal scale = devicePixelRatio();
    glViewport(0, 0, width() * scale, height() * scale);

//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

QString MainWindow::getWorkDir()
{
    return "C:\\Users\\d.kolmykov\\Documents\\gl_uml_editor";
}

void MainWindow::initShaderProgram()
{
    mProgram = new QOpenGLShaderProgram(this);
    mProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, gMainShader.vertShaderPath);
    mProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, gMainShader.fragShaderPath);
    assert(mProgram->link());
    assert(mProgram->bind());

    mPosId = mProgram->attributeLocation("aPosition");
    assert(mPosId != -1);

//    mColorId = mProgram->attributeLocation("aColor");

    mMatrixId = mProgram->uniformLocation("uMatrix");
    assert(mMatrixId != -1);

    mModelViewMatrixId = mProgram->uniformLocation("uModelViewMatrix");
    assert(mModelViewMatrixId != -1);

    const auto m = std::min(widthf(), heightf()) / 2.f;
    QMatrix4x4 modelViewMatrix;
    modelViewMatrix.translate(-getAspectRatio(), 1.f);
    modelViewMatrix.scale(-1.f / m, 1.f / m);
    modelViewMatrix.rotate(180.f, 0, 0, 1.f);

    QMatrix4x4 matrix;
    matrix.perspective(90.0f, getAspectRatio(), 0.1f, 100.0f);
    matrix.translate(0, 0, -1.f);

    mProgram->setUniformValue(mMatrixId, matrix);
    mProgram->setUniformValue(mModelViewMatrixId, modelViewMatrix);
}

void MainWindow::mousePressEvent(QMouseEvent* e) {
    const Point mousePos = toPoint(e->pos());
    mSelectedRect = findBoundRect(mPoints, mousePos);
    if (hasSelection()) {
        qDebug("found");
        gPrevPos = mousePos;
        return;
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent* e) {
    if (hasSelection()) {
        unselect();
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent* e) {
    if (hasSelection()) {
        const Point mousePos = toPoint(e->pos());
        shift(mSelectedRect, mousePos - gPrevPos);
        if (const size_t id = findIntersection(mPoints, mSelectedRect);
                id != mPoints.getInvalidIndex()) {
            qDebug("Intersect");
//            if (mSelectedRect < id) {
//                mPoints.swap(mSelectedRect, id)
//                mSelectedRect = id;
//            }
        }
        gPrevPos = mousePos;
        update();
    }
}

void MainWindow::shift(size_t rectId, Point offset)
{
    PlainRect& item = mPoints.getItem<PlainRect>(rectId);
    item += offset;

    glBindVertexArray(mVao);
    glBindBuffer(GL_ARRAY_BUFFER, mBufObj.vertex);
    const GLintptr bufObjOffset = rectId * sizeof(GLfloat);
    glBufferSubData(GL_ARRAY_BUFFER, bufObjOffset, item.getByteSize(), &item);
}

size_t MainWindow::findBoundRect(Points& points, Point mousePos)
{
    constexpr size_t componentCount = PlainRect::getComponentCount();
    for (size_t i = 0; points.hasItem<PlainRect>(i); i += componentCount) {
        const PlainRect& item = points.getItem<PlainRect>(i);
        if (item.isBound(mousePos)) {
            assert(i % componentCount == 0);
            return i;
        }
    }
    return points.getInvalidIndex();
}

size_t MainWindow::findIntersection(Points& points, size_t rectId)
{
    constexpr size_t componentCount = PlainRect::getComponentCount();
    assert(rectId % componentCount == 0);
    const PlainRect& subject = points.getItem<PlainRect>(rectId);
    for (size_t i = 0; points.hasItem<PlainRect>(i); i += componentCount) {
        if (i == rectId)
            continue;
        const auto& object = points.getItem<PlainRect>(i);
        if (!subject.isIntersect(object))
            continue;
        assert(i % componentCount == 0);
        return i;
    }
    return points.getInvalidIndex();
}
