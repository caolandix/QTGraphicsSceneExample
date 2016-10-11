#ifndef PHYSPARTICLE_H
#define PHYSPARTICLE_H

#include <QtWidgets>

#include "physbaseitem.h"
#include "arrow.h"

class PhysParticle : public PhysBaseItem, public QGraphicsPolygonItem {
    Q_OBJECT
public:
    enum { Type = PhysBaseItem::ParticleType };

    PhysParticle(QGraphicsItem * = NULL, QGraphicsScene * = NULL);
    PhysParticle(QPointF, QGraphicsItem * = NULL, QGraphicsScene * = NULL);
    ~PhysParticle();

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
    QList<Arrow *> m_lstArrows;
};
Q_DECLARE_METATYPE(PhysParticle *)
#endif // PHYSPARTICLE_H
