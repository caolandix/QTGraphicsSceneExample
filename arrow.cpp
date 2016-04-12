#include <QtWidgets>
#include <math.h>

#include "arrow.h"

const qreal Pi = 3.14;

Arrow::Arrow(DiagramItem *startItem, DiagramItem *endItem, QGraphicsItem *parent, QGraphicsScene *scene) : QGraphicsLineItem(parent) {
    myStartItem = startItem;
    myEndItem = endItem;
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    myColor = Qt::black;
    setPen(QPen(myColor, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
}

Arrow::Arrow(QPointF startPoint, QPointF endPoint, QGraphicsItem *parent, QGraphicsScene *scene) : QGraphicsLineItem(parent) {
    myStartItem = myEndItem = NULL;
    m_startPos = startPoint, m_endPos = endPoint;
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    myColor = Qt::black;
    setPen(QPen(myColor, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
}


QRectF Arrow::boundingRect() const {
    qreal extra = (pen().width() + 20) / 2.0;
    return QRectF(line().p1(), QSizeF(line().p2().x() - line().p1().x(), line().p2().y() - line().p1().y())).normalized().adjusted(-extra, -extra, extra, extra);
}

void Arrow::updatePosition() {
    if (myStartItem && myEndItem) {
        QLineF line(mapFromItem(myStartItem, 0, 0), mapFromItem(myEndItem, 0, 0));
        setLine(line);
    }

}

void Arrow::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {

    QPen myPen = pen();
    myPen.setColor(myColor);
    painter ->setPen(myPen);
    painter ->setBrush(myColor);
    QPointF startPos, endPos;

    if (myStartItem && myEndItem) {
        if (myStartItem ->collidesWithItem(myEndItem))
            return;
        QLineF centerLine(myStartItem ->pos(), myEndItem ->pos());
        QPolygonF endPolygon = myEndItem ->polygon();
        QPointF p1 = endPolygon.first() + myEndItem ->pos();
        QPointF p2;
        QPointF intersectPoint;
        QLineF polyLine;
        for (int i = 1; i < endPolygon.count(); ++i) {
            p2 = endPolygon.at(i) + myEndItem ->pos();
            polyLine = QLineF(p1, p2);
            QLineF::IntersectType intersectType = polyLine.intersect(centerLine, &intersectPoint);
            if (intersectType == QLineF::BoundedIntersection)
                break;
            p1 = p2;
        }
        startPos = myStartItem ->pos();
        endPos = intersectPoint;
    }
    else {
        endPos = m_endPos;
        startPos = m_startPos;
    }
    setLine(QLineF(endPos, startPos));
    double angle = ::acos(line().dx() / line().length());

    if (line().dy() >= 0)
        angle = (Pi * 2) - angle;

    QPointF pt1 = line().p1();
    QPointF pt2 = line().p2();
    QLineF angleLine1(pt2, pt1);
    QLineF angleLine2(pt2, pt1);

    double arrowLen = angleLine1.length() * 0.33;

    // Determine arrow lengths -- no more than 10, no less than 5
    if (arrowLen > 10.0)
        arrowLen = 10.0;
    else if (arrowLen < 5.0)
        arrowLen = 5.0;

    QPointF arrow1P1 = line().p1() + QPointF(sin(angle + Pi / 3) * arrowLen, cos(angle + Pi / 3) * arrowLen);
    QPointF arrow2P1 = line().p1() + QPointF(sin(angle + Pi - Pi / 3) * arrowLen, cos(angle + Pi - Pi / 3) * arrowLen);
    angleLine1.setP1(arrow1P1);
    angleLine2.setP1(arrow2P1);
    angleLine1.setLength(arrowLen);
    angleLine2.setLength(arrowLen);
    painter -> drawLine(angleLine1);
    painter -> drawLine(angleLine2);
    painter -> drawLine(line());
    if (isSelected()) {
        painter ->setPen(QPen(Qt::blue, 1, Qt::DashLine));
        QLineF myLine = line();
        myLine.translate(0, 4.0);
        painter ->drawLine(myLine);
        myLine.translate(0,-8.0);
        painter ->drawLine(myLine);
    }
}
