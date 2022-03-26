#pragma once
#include <QOpenGLWindow>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <vector>
#include <array>

struct Point {
    GLfloat x;
    GLfloat y;

    Point makeSmallest(Point rhs) const {
        Point result;
        result.x = std::min(x, rhs.x);
        result.y = std::min(y, rhs.y);
        return result;
    }

    Point makeLargest(Point rhs) const {
        Point result;
        result.x = std::max(x, rhs.x);
        result.y = std::max(y, rhs.y);
        return result;
    }

    Point operator-(Point rhs) const {
        Point result;
        result.x = x - rhs.x;
        result.y = y - rhs.y;
        return result;
    }
};

struct Size {
    GLfloat width;
    GLfloat height;

    Size operator+(Size rhs) {
        return {width + rhs.width, height + rhs.height};
    }
};

struct Rect {
    GLfloat x;
    GLfloat y;
    GLfloat width;
    GLfloat height;
};

struct PlainRect {
    Point topLeft;
    Point topRight;
    Point bottomRight;
    Point bottomLeft;

    static constexpr size_t getByteSize() {
        return sizeof(PlainRect);
    }

    static constexpr size_t getComponentCount() {
        return 8;
    }

    static const Size& toSize(const Point& point) {
        return *reinterpret_cast<const Size*>(&point);
    }

    Size getBoundRectSize(const PlainRect& rhs) const {
        const Point b = bottomRight.makeLargest(rhs.bottomRight);
        const Point a = topLeft.makeSmallest(rhs.topLeft);
        return toSize(b - a);
    }

    Size getSize() const {
        return toSize(bottomRight - topLeft);
    }

    void fill(Rect rect) {
        topLeft.x = rect.x;
        topLeft.y = rect.y;
        topRight.x = rect.x + rect.width;
        topRight.y = rect.y;
        bottomRight.x = rect.x + rect.width;
        bottomRight.y = rect.y + rect.height;
        bottomLeft.x = rect.x;
        bottomLeft.y = rect.y + rect.height;
    }

    PlainRect& operator+=(Point rhs) {
        topLeft.x += rhs.x;
        topRight.x += rhs.x;
        bottomRight.x += rhs.x;
        bottomLeft.x += rhs.x;
        topLeft.y += rhs.y;
        topRight.y += rhs.y;
        bottomRight.y += rhs.y;
        bottomLeft.y += rhs.y;
        return *this;
    }

    bool isBound(Point point) const {
        return (point.x >= topLeft.x) && (point.x < bottomRight.x) &&
               (point.y >= topLeft.y) && (point.y < bottomRight.y);
    }

    bool isIntersect(const PlainRect& object) const {
        const Size size = getBoundRectSize(object);
        const Size expectedSize = object.getSize() + getSize();
        if (size.width >= expectedSize.width || size.height >= expectedSize.height)
            return false;
        return true;
    }

    std::pair<GLfloat*, size_t> toComponents() {
        return {reinterpret_cast<GLfloat*>(this), getComponentCount()};
    }
};

struct Color {
    GLfloat r;
    GLfloat g;
    GLfloat b;
};

class Points {
public:
    void add(Rect rect) {
        PlainRect plainRect;
        plainRect.fill(rect);
        const auto [ptr, size] = plainRect.toComponents();
        for (size_t i = 0; i < size; ++i)
            mPoints.push_back(ptr[i]);
    }


    template<typename T>
    bool hasItem(size_t i) const {
        constexpr size_t itemCount = sizeof(T) / sizeof(GLfloat);
        return i + itemCount <= mPoints.size();
    }

    template<typename T>
    T& getItem(size_t i) {
        return *reinterpret_cast<T*>(&mPoints[i]);
    }

    static constexpr size_t getInvalidId() { return static_cast<size_t>(-1); }
    GLfloat& getItem(size_t i) { return mPoints[i]; }
    size_t getCount() const { return mPoints.size(); }
    size_t getByteCount() const { return mPoints.size() * sizeof(GLfloat); }
    GLfloat* getData() { return mPoints.data(); }

private:
    std::vector<GLfloat> mPoints;
};


class MainWindow : public QOpenGLWindow, protected QOpenGLFunctions_3_3_Core
{
public:
    MainWindow();
    ~MainWindow() override;
    void addRect(Rect  rect);
protected:
    template<typename T>
    Point toPoint(const T& source) {
        Point p{
            static_cast<GLfloat>(source.x()),
            static_cast<GLfloat>(source.y())
        };
        return p;
    }

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
    bool hasSelection() const { return mSelectedRect != Points::getInvalidId(); }
    void unselect() { mSelectedRect = Points::getInvalidId(); }
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
