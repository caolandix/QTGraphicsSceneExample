#include "physvectoranglecartesian.h"

PhysVectorAngleCartesian::PhysVectorAngleCartesian(QGraphicsItem *pParent, QGraphicsScene *pScene) :
    PhysBaseItem(), QGraphicsPolygonItem(pParent) {
    init();
}

PhysVectorAngleCartesian::PhysVectorAngleCartesian(QPointF pos, QGraphicsItem *pParent, QGraphicsScene *pScene) :
    PhysBaseItem(), QGraphicsPolygonItem(pParent) {
    init();
    setPos(m_currPos = pos);
}

PhysVectorAngleCartesian::~PhysVectorAngleCartesian() {

}

void PhysVectorAngleCartesian::init() {
    setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);
    m_Color = Qt::gray;
    m_ellipseBounds = { -5, -5, 15, 15 };

}

QPainterPath PhysVectorAngleCartesian::shape() const {
    QPainterPath path;
    path.addEllipse(m_ellipseBounds);
    return path;
}

QRectF PhysVectorAngleCartesian::boundingRect() const {
    const qreal adjust = 5.0;
    return QRectF(
                m_ellipseBounds.left() - adjust,
                m_ellipseBounds.top() - adjust,
                m_ellipseBounds.right() + adjust,
                m_ellipseBounds.bottom() + adjust
                );
}

void PhysVectorAngleCartesian::paint(QPainter *pPainter, const QStyleOptionGraphicsItem *pOption, QWidget *) {
    const qreal offset = -2;
    pPainter -> setPen(Qt::NoPen);
    pPainter -> setBrush(Qt::darkGray);
    pPainter -> drawEllipse(
                m_ellipseBounds.top() + offset,
                m_ellipseBounds.left() + offset,
                m_ellipseBounds.right(),
                m_ellipseBounds.bottom());

    QRadialGradient gradient(-3, -3, 10);
    if (pOption -> state & QStyle::State_Sunken) {
        gradient.setCenter(3, 3);
        gradient.setFocalPoint(3, 3);
        gradient.setColorAt(1, QColor(Qt::green).light(120));
        gradient.setColorAt(0, QColor(Qt::darkGreen).light(120));
    }
    else {
        gradient.setColorAt(0, Qt::green);
        gradient.setColorAt(1, Qt::darkGreen);
    }
    pPainter -> setBrush(gradient);
    pPainter -> setPen(QPen(Qt::black, 0));
    pPainter -> drawEllipse(m_ellipseBounds);
}

QVariant PhysVectorAngleCartesian::itemChange(GraphicsItemChange change, const QVariant &value) {
    if (change == QGraphicsItem::ItemPositionChange) {
    }
    return value;
}
