#ifndef ARROW_H
#define ARROW_H

#include <QGraphicsLineItem>

#include "diagramitem.h"

class QGraphicsPolygonItem;
class QGraphicsLineItem;
class QGraphicsScene;
class QRectF;
class QGraphicsSceneMouseEvent;
class QPainterPath;

class Arrow : public QGraphicsLineItem {
public:
    enum { Type = UserType + 4 };

    Arrow(DiagramItem *, DiagramItem *, QGraphicsItem * = NULL, QGraphicsScene * = NULL);
    Arrow(QPointF, QPointF, QGraphicsItem * = NULL, QGraphicsScene * = NULL);
    int type() const { return Type; }
    QRectF boundingRect() const;
    void setColor(const QColor &color) { myColor = color; }
    DiagramItem *startItem() const { return myStartItem; }
    DiagramItem *endItem() const { return myEndItem; }

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
