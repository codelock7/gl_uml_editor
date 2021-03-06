#pragma once
#include <QOpenGLWindow>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <vector>
#include <array>
#include "points.h"
#include "point.h"
#include "rect.h"

class MainWindow : public QOpenGLWindow, protected QOpenGLFunctions_3_3_Core
{
public:
    MainWindow();
    ~MainWindow() override;
    void addRect(Rect  rect);
protected:
    void initializeGL() override;
    void paintGL() override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
private:
    void shift(size_t rectId, Point offset);
    size_t findBoundRect(Points& v, Point p);
    size_t findIntersection(Points& points, size_t rectId);

    QString getWorkDir();
    void initShaderProgram();
    GLfloat widthf() const { return static_cast<GLfloat>(width()); }
    GLfloat heightf() const { return static_cast<GLfloat>(height()); };
    GLfloat getAspectRatio() const { return widthf() / heightf(); }
    bool hasSelection() const { return mSelectedRect != Points::getInvalidIndex(); }
    void unselect() { mSelectedRect = Points::getInvalidIndex(); }
private:
    QOpenGLShaderProgram* mProgram;
    size_t mSelectedRect;

    Points mPoints;

    GLuint mVao;
    struct {
        GLuint vertex;
        GLuint element;
        GLuint colorVbo;
    } mBufObj;

    GLint mPosId;
    GLint mColorId;
    GLint mMatrixId;
    GLint mModelViewMatrixId;
};
