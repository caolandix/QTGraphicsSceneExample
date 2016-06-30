#ifndef PHYSGRAPHICSSCENE_H
#define PHYSGRAPHICSSCENE_H

#include "diagramitem.h"
#include "diagramtextitem.h"

#include <QGraphicsScene>

class QGraphicsSceneMouseEvent;
class QMenu;
class QPointF;
class QGraphicsLineItem;
class QFont;
class QGraphicsTextItem;
class QColor;
class QPainter;

class PhysGraphicsScene : public QGraphicsScene {
    Q_OBJECT
public:
    enum Mode { InsertItem, InsertLine, InsertText, MoveItem, RotateItem };

    explicit PhysGraphicsScene(QMenu *itemMenu, QObject *parent = NULL);
    QFont font() const { return m_Font; }
    QColor textColor() const { return m_TextColor; }
    QColor itemColor() const { return m_ItemColor; }
    QColor lineColor() const { return m_LineColor; }
    void setLineColor(const QColor &color);
    void setTextColor(const QColor &color);
    void setItemColor(const QColor &color);
    void setFont(const QFont &font);

public slots:
    void setMode(Mode mode);
    void setItemType(DiagramItem::DiagramType type);
    void editorLostFocus(DiagramTextItem *);

signals:
    void itemInserted(DiagramItem *);
    void textInserted(QGraphicsTextItem *);
    void itemSelected(QGraphicsItem *);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) Q_DECL_OVERRIDE;
    void drawBackground(QPainter *, const QRectF &) Q_DECL_OVERRIDE;

private:
    bool isItemChange(int type);
    void setupAngleLine(QLineF &, const QPointF, const QPointF, const double);
    void drawAngledPlane(QPainter *);
    void drawThetaPhi(QPainter *);
    QPointF findLineMidPoint(const QLineF );



    DiagramItem::DiagramType m_ItemType;
    QMenu *m_pItemMenu;
    Mode m_Mode;
    bool leftButtonDown;
    QPointF startPoint;
    QGraphicsLineItem *m_pLine;
    QFont m_Font;
    DiagramTextItem *m_pTextItem;
    QColor m_TextColor;
    QColor m_ItemColor;
    QColor m_LineColor;
};

#endif // PHYSGRAPHICSSCENE_H
