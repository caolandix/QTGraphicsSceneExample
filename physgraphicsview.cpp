#include <QtWidgets>
#include "physgraphicsview.h"

PhysGraphicsView::PhysGraphicsView(QMenu *pMenu, QGraphicsScene *pScene, QWidget *pParent) : QGraphicsView(pScene, pParent) {
    init(pMenu);
}

void PhysGraphicsView::init(QMenu *pMenu) {
    m_pScene = scene();
    myItemMenu = pMenu;
}

void PhysGraphicsView::mousePressEvent(QMouseEvent *mouseEvent) {
    if (mouseEvent ->button() != Qt::LeftButton)
        return;

    DiagramItem *item = NULL;
    switch (myMode) {
        case InsertItem:
            item = new DiagramItem(myItemType, myItemMenu);
            item ->setBrush(myItemColor);
            m_pScene ->addItem(item);
            item ->setPos(mouseEvent->scenePos());
            emit itemInserted(item);
            break;
        case InsertLine:
            startPoint = mouseEvent ->scenePos();
            line = new QGraphicsLineItem(QLineF(mouseEvent ->scenePos(), mouseEvent ->scenePos()));
            line ->setPen(QPen(myLineColor, 2));
            m_pScene ->addItem(line);
            break;
        case InsertText:
            textItem = new DiagramTextItem();
            textItem ->setFont(myFont);
            textItem ->setTextInteractionFlags(Qt::TextEditorInteraction);
            textItem ->setZValue(1000.0);
            connect(textItem, SIGNAL(lostFocus(DiagramTextItem*)),
                    this, SLOT(editorLostFocus(DiagramTextItem*)));
            connect(textItem, SIGNAL(selectedChange(QGraphicsItem*)),
                    this, SIGNAL(itemSelected(QGraphicsItem*)));
            m_pScene ->addItem(textItem);
            textItem ->setDefaultTextColor(myTextColor);
            textItem ->setPos(mouseEvent->scenePos());
            emit textInserted(textItem);
    default:
        ;
    }
    PhysGraphicsView::mousePressEvent(mouseEvent);
}

void PhysGraphicsView::mouseMoveEvent(QMouseEvent *mouseEvent) {
    if (myMode == InsertLine && line != 0) {
        QLineF newLine(line ->line().p1(), mouseEvent ->scenePos());
        line ->setLine(newLine);
    }
    else if (myMode == MoveItem) {
        PhysGraphicsView::mouseMoveEvent(mouseEvent);
    }
}

void PhysGraphicsView::mouseReleaseEvent(QMouseEvent *mouseEvent) {
    QPointF endPoint = mouseEvent ->scenePos();

    if (line != 0 && myMode == InsertLine) {
        QList<QGraphicsItem *> startItems = m_pScene ->items(line ->line().p1());
        if (startItems.count() && startItems.first() == line)
            startItems.removeFirst();
        QList<QGraphicsItem *> endItems = m_pScene ->items(line ->line().p2());
        if (endItems.count() && endItems.first() == line)
            endItems.removeFirst();

        m_pScene ->removeItem(line);
        delete line;

        if (startItems.count() > 0 && endItems.count() > 0 &&
            startItems.first() ->type() == DiagramItem::Type && endItems.first() ->type() == DiagramItem::Type &&
            startItems.first() != endItems.first()) {

            DiagramItem *startItem = qgraphicsitem_cast<DiagramItem *>(startItems.first());
            DiagramItem *endItem = qgraphicsitem_cast<DiagramItem *>(endItems.first());
            Arrow *arrow = new Arrow(startItem, endItem);
            arrow ->setColor(myLineColor);
            startItem ->addArrow(arrow);
            endItem ->addArrow(arrow);
            arrow ->setZValue(-1000.0);
            m_pScene ->addItem(arrow);
            arrow ->updatePosition();
        }
        else {
            Arrow *arrow = new Arrow(startPoint, endPoint);
            arrow ->setColor(myLineColor);
            arrow ->setZValue(-1000.0);
            m_pScene ->addItem(arrow);
            arrow ->updatePosition();

        }
    }
    line = 0;
    PhysGraphicsView::mouseReleaseEvent(mouseEvent);
}

