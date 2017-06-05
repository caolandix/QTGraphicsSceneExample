#include "physvectoranglecartesian.h"
#include "arrow.h"

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
    pPainter -> setPen(Qt::SolidLine);
    pPainter -> setBrush(Qt::darkGreen);


    QGraphicsItem *pObj = parentItem();
    Arrow *pParent = static_cast<Arrow *>(pObj);

    QPointF parentStartPos = pParent -> startPos();
    QPointF parentEndPos = pParent -> endPos();

    QRectF rc = scene() ->sceneRect();

    // if position rect is outside of drawing rect, do nothing...
    if (!rc.contains(parentStartPos) ||  !rc.contains(parentEndPos)) {
        return;
    }

    QPointF horizLineStart = parentStartPos;
    QPointF horizLineEnd(horizLineStart.x() + parentEndPos.x(), parentStartPos.y());

    QPointF vertLineStart = parentStartPos;
    QPointF vertLineEnd(horizLineStart.x(), horizLineStart.y() + parentEndPos.y());
    QLineF horizLine(horizLineStart, horizLineEnd);
    QLineF vertLine(vertLineStart, vertLineEnd);

    pPainter ->drawLine(horizLine);
    pPainter ->drawLine(vertLine);
}

QVariant PhysVectorAngleCartesian::itemChange(GraphicsItemChange change, const QVariant &value) {
    if (change == QGraphicsItem::ItemPositionChange) {
    }
    return value;
}
