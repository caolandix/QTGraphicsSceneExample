#ifndef PHYSGRAPHICSVIEW_H
#define PHYSGRAPHICSVIEW_H

#include <QtWidgets>

#include "diagramitem.h"


class PhysGraphicsView : public QGraphicsView {
    Q_OBJECT
public:
    enum Mode { InsertItem, InsertLine, InsertText, MoveItem };

    PhysGraphicsView(QMenu *, QGraphicsScene *, QWidget * = NULL);
private:
    void init(QMenu *);


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
    QGraphicsScene *m_pScene;
    DiagramItem::DiagramType myItemType;
    QMenu *myItemMenu;
    Mode myMode;
    bool leftButtonDown;
    QPointF startPoint;
    QGraphicsLineItem *line;
    QFont myFont;
    DiagramTextItem *textItem;
    QColor myTextColor;
    QColor myItemColor;
    QColor myLineColor;

};

#endif // PHYSGRAPHICSVIEW_H
