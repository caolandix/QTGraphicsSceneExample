#include "physvectoranglecartesian.h"

PhysVectorAngleCartesian::PhysVectorAngleCartesian()
{

}

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
    m_Color = Qt::black;

}

QPainterPath PhysVectorAngleCartesian::shape() const {
    QPainterPath path;
    path.addEllipse(-10, -10, 20, 20);
    return path;
}

QRectF PhysVectorAngleCartesian::boundingRect() const {
    const qreal adjust = 2.0;
    return QRectF(-10 - adjust, -10 - adjust, 23 + adjust, 23 + adjust);
}

void PhysVectorAngleCartesian::paint(QPainter *pPainter, const QStyleOptionGraphicsItem *pOption, QWidget *) {
    pPainter -> setPen(Qt::NoPen);
    pPainter -> setBrush(Qt::darkGray);
    pPainter -> drawEllipse(-7, -7, 20, 20);

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
    pPainter -> drawEllipse(-10, -10, 20, 20);
}

void PhysVectorAngleCartesian::removeArrow(Arrow *pArrow)  {
    int index = m_lstArrows.indexOf(pArrow);

    if (index != -1)
        m_lstArrows.removeAt(index);
}

void PhysVectorAngleCartesian::removeArrows() {
    foreach (Arrow *pArrow, m_lstArrows) {
        pArrow ->startItem() ->removeArrow(pArrow);
        pArrow ->endItem() ->removeArrow(pArrow);
        scene() ->removeItem(pArrow);
        delete pArrow;
    }
}

void PhysVectorAngleCartesian::addArrow(Arrow *pArrow) {
    m_lstArrows.append(pArrow);
}

QVariant PhysVectorAngleCartesian::itemChange(GraphicsItemChange change, const QVariant &value) {
    if (change == QGraphicsItem::ItemPositionChange) {
        foreach (Arrow *pArrow, m_lstArrows)
            pArrow ->updatePosition();
    }
    return value;
}
