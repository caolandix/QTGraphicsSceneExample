#ifndef PHYSGRAPHICSVIEW_H
#define PHYSGRAPHICSVIEW_H

#include <QtWidgets>

#include "diagramitem.h"
#include "diagramtextitem.h"


class PhysGraphicsView : public QGraphicsView {
    Q_OBJECT
public:
    enum Mode { InsertItem, InsertLine, InsertText, MoveItem };

    PhysGraphicsView(QMenu *, QGraphicsScene *, QWidget * = NULL);

    QFont font() const { return myFont; }
    QColor textColor() const { return myTextColor; }
    QColor itemColor() const { return myItemColor; }
    QColor lineColor() const { return myLineColor; }
    void setLineColor(const QColor &color);
    void setTextColor(const QColor &color);
    void setItemColor(const QColor &color);
    void setFont(const QFont &font);
private:
    void init(QMenu *);

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

    QGraphicsScene *m_pScene;
    DiagramItem::DiagramType myItemType;
    Mode myMode;
    bool leftButtonDown;
    QPointF startPoint;
    QGraphicsLineItem *line;
    QFont myFont;
    DiagramTextItem *textItem;
    QColor myTextColor;
    QColor myItemColor;
    QColor myLineColor;
    QMenu *myItemMenu;
};

#endif // PHYSGRAPHICSVIEW_H
