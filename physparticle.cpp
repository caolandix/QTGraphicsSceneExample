#include "physparticle.h"
#include "physbaseitem.h"

PhysParticle::PhysParticle(QGraphicsItem *pParent, QGraphicsScene *pScene) :
    PhysBaseItem(), QGraphicsPolygonItem(pParent) {
    init();
}

PhysParticle::PhysParticle(QPointF pos, QGraphicsItem *pParent, QGraphicsScene *pScene) :
    PhysBaseItem(), QGraphicsPolygonItem(pParent) {
    init();
    setPos(m_currPos = pos);
}

PhysParticle::~PhysParticle() {

}

void PhysParticle::init() {
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    m_Color = Qt::black;

}

QPainterPath PhysParticle::shape() const {
    QPainterPath path;
    path.addEllipse(-10, -10, 20, 20);
    return path;
}

QRectF PhysParticle::boundingRect() const {
    const qreal adjust = 2.0;
    return QRectF(-10 - adjust, -10 - adjust, 23 + adjust, 23 + adjust);
}

void PhysParticle::paint(QPainter *pPainter, const QStyleOptionGraphicsItem *pOption, QWidget *) {
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
