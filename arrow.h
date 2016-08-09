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

class Arrow : public QGraphicsLineItem, PhysBaseItem {
public:
    enum { Type = PhysBaseItem::ParticleType };

    Arrow(DiagramItem *, DiagramItem *, QGraphicsItem * = NULL, QGraphicsScene * = NULL);
    Arrow(QPointF, QPointF, QGraphicsItem * = NULL, QGraphicsScene * = NULL);
    int type() const { return Type; }
    QRectF boundingRect() const;
    void setColor(const QColor &color) { m_Color = color; }
    DiagramItem *startItem() const { return m_pStartItem; }
    DiagramItem *endItem() const { return m_pEndItem; }

public slots:
    void updatePosition();

protected:
    void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget * = NULL);

private:
    DiagramItem *m_pStartItem;
    DiagramItem *m_pEndItem;
    QColor m_Color;
    QPointF m_startPos, m_endPos;
};
#endif // ARROW_H
