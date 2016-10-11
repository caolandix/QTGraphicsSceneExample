#ifndef ARROW_H
#define ARROW_H

#include <QGraphicsLineItem>

#include "physbaseitem.h"
#include "diagramitem.h"

class QGraphicsPolygonItem;
class QGraphicsLineItem;
class QGraphicsScene;
class QRectF;
class QGraphicsSceneMouseEvent;
class QPainterPath;

#include "physbaseitem.h"

class PhysParticle;


class Arrow : public QGraphicsLineItem, PhysBaseItem {
public:
    enum { Type = PhysBaseItem::VectorType };

    Arrow(PhysParticle *, PhysParticle *, QGraphicsItem * = NULL);
    Arrow(QPointF, QPointF, QGraphicsItem * = NULL);
    Arrow(QPointF, QPointF, PhysParticle *, PhysParticle *, QGraphicsItem * = NULL);
    int type() const { return Type; }
    QRectF boundingRect() const;
    void setColor(const QColor &color) { m_Color = color; }
    PhysParticle *startItem() const { return m_pStartItem; }
    PhysParticle *endItem() const { return m_pEndItem; }
    void startItem(PhysParticle *pStartItem) { m_pStartItem = pStartItem; }
    void endItem(PhysParticle *pEndItem) { m_pEndItem = pEndItem; }

private:
    void init(PhysParticle * = NULL, PhysParticle * = NULL);

public slots:
    void updatePosition();

protected:
    void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget * = NULL);

private:
    PhysParticle *m_pStartItem, *m_pEndItem;
    QColor m_Color;
    QPointF m_startPos, m_endPos;
};
#endif // ARROW_H
