#ifndef PHYSGRAPHICSVIEW_H
#define PHYSGRAPHICSVIEW_H

#include <QtWidgets>

#include "diagramitem.h"
#include "diagramtextitem.h"
#include "physparticle.h"
#include "physvectoranglecartesian.h"


class PhysGraphicsView : public QGraphicsView {
    Q_OBJECT
public:
    enum Mode { InsertItem, InsertLine, InsertText, MoveItem, InsertParticle, MoveVector, MoveParticle };

    PhysGraphicsView(QMenu *, QGraphicsScene *, QWidget * = NULL);

    QFont font() const { return m_Font; }
    QColor textColor() const { return m_TextColor; }
    QColor itemColor() const { return m_ItemColor; }
    QColor lineColor() const { return m_LineColor; }
    QColor particleColor() const { return m_particleColor; }
    void setLineColor(const QColor &);
    void setTextColor(const QColor &);
    void setItemColor(const QColor &);
    void setParticleColor(const QColor &);
    void setFont(const QFont &);
private:
    void init(QMenu *);
    Arrow *createVector(QPointF StartPt, QPointF EndPt, PhysParticle * = NULL, PhysParticle * = NULL);


public slots:
    void setMode(Mode mode);
    void setItemType(DiagramItem::DiagramType type);
    void editorLostFocus(DiagramTextItem *item);

signals:
    void itemInserted(DiagramItem *item);
    void textInserted(QGraphicsTextItem *item);
    void itemSelected(QGraphicsItem *item);


protected:
    void mousePressEvent(QMouseEvent *mouseEvent) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *mouseEvent) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *mouseEvent) Q_DECL_OVERRIDE;
    void drawBackground(QPainter *, const QRectF &) Q_DECL_OVERRIDE;

private:
    bool isItemChange(int type);

    bool leftButtonDown;
    QGraphicsScene *m_pScene;
    DiagramItem::DiagramType myItemType;
    Mode m_Mode;
    QPointF m_pStartPoint;
    QGraphicsLineItem *m_pLine;
    QGraphicsPolygonItem *m_pPolyItem;
    PhysParticle *m_pParticle;
    PhysVectorAngleCartesian *m_pAngleDisplay;
    QFont m_Font;
    DiagramTextItem *textItem;
    QColor m_TextColor;
    QColor m_ItemColor;
    QColor m_LineColor;
    QColor m_particleColor;
    QMenu *myItemMenu;
};

#endif // PHYSGRAPHICSVIEW_H
