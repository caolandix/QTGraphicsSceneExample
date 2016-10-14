#ifndef PHYSVECTORANGLECARTESIAN_H
#define PHYSVECTORANGLECARTESIAN_H

#include "physbaseitem.h"
#include "arrow.h"

class PhysVectorAngleCartesian : public PhysBaseItem, public QGraphicsPolygonItem {
    Q_OBJECT
public:
    enum { Type = PhysBaseItem::PhysVectorAngleType };

    PhysVectorAngleCartesian(QGraphicsItem * = NULL, QGraphicsScene * = NULL);
    PhysVectorAngleCartesian(QPointF, QGraphicsItem * = NULL, QGraphicsScene * = NULL);
    ~PhysVectorAngleCartesian();

    QPainterPath shape() const Q_DECL_OVERRIDE;
    QRectF boundingRect() const;
    void setColor(const QColor &color) { m_Color = color; }

    void removeArrow(Arrow *pArrow);
    void removeArrows();
    void addArrow(Arrow *pArrow);
    QPointF pos() const { return m_currPos; }

private:
    void init();
    int type() const { return Type; }

protected:
    void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget * = NULL);
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

private:
    QColor m_Color;
    QPointF m_currPos;
};
Q_DECLARE_METATYPE(PhysVectorAngleCartesian *)

#endif // PHYSVECTORANGLECARTESIAN_H
