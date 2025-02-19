#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <QGraphicsObject>
#include <QPainter>

class Triangle : public QGraphicsObject {
    Q_OBJECT

public:
    enum class TriangleStatus{
        EMPTY,
        BLACK, BLACK_LAST,
        WHITE, WHITE_LAST,
        NEXT
    };


    Triangle(int id, bool direction, QGraphicsItem* parent = nullptr);
    // Define button shape (bounding rectangle)
    QRectF boundingRect() const override;

    // Paint the triangle shape
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override;

    QPainterPath shape() const override;

    int getID() const;
    void changeStatus(TriangleStatus state);
    void changeColor(QColor color);
    void updateColor();

signals:
    void clicked(int id);  // Custom click signal

protected:
    // Handle mouse clicks
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

    // Handle hover effects
    void hoverEnterEvent(QGraphicsSceneHoverEvent *) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *) override;

public:
    const static int TRIANGLE_LENGTH = 64;
    const static int TRIANGLE_HEIGHT = 0.866 * TRIANGLE_LENGTH;



private:
    int t_id;
    bool t_direction;
    TriangleStatus status;
    QColor t_color;
};


#endif // TRIANGLE_H
