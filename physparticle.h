#ifndef PHYSPARTICLE_H
#define PHYSPARTICLE_H

#include <QtWidgets>

#include "physbaseitem.h"

class PhysParticle : public PhysBaseItem, public QGraphicsPolygonItem {
    Q_OBJECT
    enum { Type = PhysBaseItem::ParticleType };
public:
    PhysParticle(QGraphicsItem * = NULL, QGraphicsScene * = NULL);
    PhysParticle(QPointF, QGraphicsItem * = NULL, QGraphicsScene * = NULL);
    ~PhysParticle();

    QPainterPath shape() const Q_DECL_OVERRIDE;
    QRectF boundingRect() const;
    void setColor(const QColor &color) { m_Color = color; }
private:
    void init();
    int type() const { return Type; }
    QPointF pos() const { return m_currPos; }

protected:
    void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget * = NULL);

private:
    QColor m_Color;
    QPointF m_currPos;
};
Q_DECLARE_METATYPE(PhysParticle *)
#endif // PHYSPARTICLE_H
