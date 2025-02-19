#include "triangle.h"

Triangle::Triangle(int id, bool direction, QGraphicsItem* parent)
    : t_id(id), t_direction(direction), status(TriangleStatus::EMPTY), QGraphicsObject(parent) {
    setAcceptHoverEvents(true);  // Enable hover events
    updateColor();
}

// Define button shape (bounding rectangle)
QRectF Triangle::boundingRect() const {
    return QRectF(0, 0, TRIANGLE_LENGTH, TRIANGLE_HEIGHT);  // Triangle size
}

// Paint the triangle shape
void Triangle::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)  {
    QPolygon triangle;
    if(t_direction){
        triangle << QPoint(TRIANGLE_LENGTH / 2, 0) << QPoint(TRIANGLE_LENGTH, TRIANGLE_HEIGHT) << QPoint(0, TRIANGLE_HEIGHT);
    }
    else{
        triangle << QPoint(TRIANGLE_LENGTH / 2, TRIANGLE_HEIGHT) << QPoint(TRIANGLE_LENGTH, 0) << QPoint(0, 0);
    }

    painter->setBrush(t_color);
    painter->setPen(Qt::NoPen);
    painter->drawPolygon(triangle);
}

// Define precise shape (triangle) for mouse detection
QPainterPath Triangle::shape() const {
    QPainterPath path;
    if(t_direction){
        path.moveTo(TRIANGLE_LENGTH / 2, 0);  // Top point
        path.lineTo(TRIANGLE_LENGTH, TRIANGLE_HEIGHT); // Bottom right
        path.lineTo(0, TRIANGLE_HEIGHT);   // Bottom left
    }
    else{
        path.moveTo(TRIANGLE_LENGTH / 2, TRIANGLE_HEIGHT);  // Bottom point
        path.lineTo(TRIANGLE_LENGTH, 0); // Top right
        path.lineTo(0, 0);   // Top left
    }
    path.closeSubpath();  // Close triangle
    return path;
}

int Triangle::getID() const { return t_id; }

void Triangle::changeStatus(TriangleStatus status) {
    this->status = status;
    updateColor();
    update();
}

void Triangle::changeColor(QColor color) {
    t_color = color;
    update();
}

void Triangle::updateColor() {
    switch (status){
    case TriangleStatus::EMPTY:
        t_color = QColorConstants::Svg::orange;
        break;
    case TriangleStatus::BLACK:
        t_color = QColorConstants::Svg::black;
        break;
    case TriangleStatus::BLACK_LAST:
        t_color = QColorConstants::Svg::midnightblue;
        break;
    case TriangleStatus::WHITE:
        t_color = QColorConstants::Svg::white;
        break;
    case TriangleStatus::WHITE_LAST:
        t_color = QColorConstants::Svg::lemonchiffon;
        break;
    case TriangleStatus::NEXT:
        t_color = QColorConstants::Svg::skyblue;
        break;
    default:
        t_color = QColorConstants::Svg::brown;
        break;
    }
    update();
}

// Handle mouse clicks
void Triangle::mousePressEvent(QGraphicsSceneMouseEvent *event)  {
    // Only triggers make move when the current square is a legal move
    if(status == TriangleStatus::NEXT){
        emit clicked(t_id);
    }
}

// Handle hover effects
void Triangle::hoverEnterEvent(QGraphicsSceneHoverEvent *)  {
    switch (status){
    case TriangleStatus::EMPTY:
        t_color = QColorConstants::Svg::orangered;
        break;
    case TriangleStatus::BLACK:
        t_color = QColorConstants::Svg::darkslategrey;
        break;
    case TriangleStatus::BLACK_LAST:
        t_color = QColorConstants::Svg::darkslateblue;
        break;
    case TriangleStatus::WHITE:
        t_color = QColorConstants::Svg::lightyellow;
        break;
    case TriangleStatus::WHITE_LAST:
        t_color = QColorConstants::Svg::gold;
        break;
    case TriangleStatus::NEXT:
        t_color = QColorConstants::Svg::royalblue;
        break;
    default:
        t_color = QColorConstants::Svg::red;
        break;
    }
    update();
}

void Triangle::hoverLeaveEvent(QGraphicsSceneHoverEvent *)  {
    updateColor();
    update();
}
