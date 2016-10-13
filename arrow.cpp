#include <QtWidgets>
#include <math.h>

#include "physbaseitem.h"
#include "arrow.h"
#include "physparticle.h"

Arrow::Arrow(QPointF startPoint, QPointF endPoint, QGraphicsItem *pParent) :
    QGraphicsLineItem(pParent), PhysBaseItem() {
    init();
    m_startPos = startPoint, m_endPos = endPoint;
}

Arrow::Arrow(PhysParticle *pStartItem, PhysParticle *pEndItem, QGraphicsItem *pParent) :
    QGraphicsLineItem(pParent), PhysBaseItem() {
    init(pStartItem, pEndItem);
}

Arrow::Arrow(QPointF startPoint, QPointF endPoint, PhysParticle *pStartItem, PhysParticle *pEndItem, QGraphicsItem *pParent) :
    QGraphicsLineItem(pParent), PhysBaseItem() {
    init(pStartItem, pEndItem);
    m_startPos = startPoint, m_endPos = endPoint;
}


void Arrow::init(PhysParticle *pStartItem, PhysParticle *pEndItem) {
    m_pStartItem = pStartItem;
    m_pEndItem = pEndItem;
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);
    setFlag(QGraphicsItem::ItemIsMovable, true);
    m_Color = Qt::black;
    setPen(QPen(m_Color, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
}


QRectF Arrow::boundingRect() const {
    qreal extra = (pen().width() + 20) / 2.0;
    return QRectF(line().p1(), QSizeF(line().p2().x() - line().p1().x(), line().p2().y() - line().p1().y())).normalized().adjusted(-extra, -extra, extra, extra);
}

void Arrow::updatePosition() {
    if (m_pStartItem && m_pEndItem) {
        QLineF line(m_startPos = mapFromItem(m_pStartItem, 0, 0), m_endPos = mapFromItem(m_pEndItem, 0, 0));
        setLine(line);
    }
    else if (m_pStartItem) {
        QLineF line(m_startPos = mapFromItem(m_pStartItem, 0, 0), m_endPos);
        setLine(line);
    }
    else if (m_pEndItem) {
        QLineF line(m_startPos, m_endPos = mapFromItem(m_pEndItem, 0, 0));
        setLine(line);
    }
    else {
        QLineF line(m_startPos, m_endPos);
        setLine(line);
    }
}

void Arrow::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
    QPen myPen = pen();
    myPen.setColor(m_Color);
    painter ->setPen(myPen);
    painter ->setBrush(m_Color);
    QPointF startPos, endPos;

    if (m_pStartItem && m_pEndItem) {
        if (m_pStartItem ->collidesWithItem(m_pEndItem))
            return;
    }
    /*
    if (m_pStartItem && m_pEndItem) {
        if (m_pStartItem ->collidesWithItem(m_pEndItem))
            return;
        QLineF centerLine(m_pStartItem ->pos(), m_pEndItem ->pos());
        QPolygonF endPolygon = m_pEndItem ->polygon();
        QPointF p1 = endPolygon.first() + m_pEndItem ->pos();
        QPointF p2;
        QPointF intersectPoint;
        QLineF polyLine;
        for (int i = 1; i < endPolygon.count(); ++i) {
            p2 = endPolygon.at(i) + m_pEndItem ->pos();
            polyLine = QLineF(p1, p2);
            QLineF::IntersectType intersectType = polyLine.intersect(centerLine, &intersectPoint);
            if (intersectType == QLineF::BoundedIntersection)
                break;
            p1 = p2;
        }
        startPos = m_pStartItem ->pos();
        endPos = intersectPoint;

        endPos = m_endPos;
        startPos = m_startPos;

    }
    else {
*/
    endPos = m_endPos;
    startPos = m_startPos;
    setLine(QLineF(endPos, startPos));
    double angle = ::acos(line().dx() / line().length());

    if (line().dy() >= 0)
        angle = (M_PI * 2) - angle;

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

    QPointF arrow1P1 = line().p1() + QPointF(sin(angle + M_PI / 3) * arrowLen, cos(angle + M_PI / 3) * arrowLen);
    QPointF arrow2P1 = line().p1() + QPointF(sin(angle + M_PI - M_PI / 3) * arrowLen, cos(angle + M_PI - M_PI / 3) * arrowLen);
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
        myLine.translate(0, -8.0);
        painter ->drawLine(myLine);
    }
}
