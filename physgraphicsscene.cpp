#include "physgraphicsscene.h"
#include "arrow.h"

#include <QtWidgets>

PhysGraphicsScene::PhysGraphicsScene(QMenu *itemMenu, QObject *parent) : QGraphicsScene(parent) {
    myItemMenu = itemMenu;
    myMode = MoveItem;
    myItemType = DiagramItem::Step;
    line = 0;
    textItem = 0;
    myItemColor = Qt::white;
    myTextColor = Qt::black;
    myLineColor = Qt::black;
}

void PhysGraphicsScene::drawBackground(QPainter *painter, const QRectF &rect) {
    Q_UNUSED(rect);

    QMatrix m;
    int w_2 = width() / 2;
    int h_2 = height() / 2;

    painter -> setPen(Qt::blue);
    painter -> drawLine(0, h_2, width(), h_2);     // X-Axis
    painter -> drawLine(w_2, 0 , w_2, height());  // Y-Axis

    m.translate(w_2, h_2);
    m.scale(1, -1);

    painter -> setMatrix(m);
    painter -> setPen(Qt::NoPen);
    painter -> setBrush(QBrush(Qt::blue, Qt::Dense4Pattern));
    painter -> drawRect(-10, -10, 20, 20);

    QLineF y_axis(-10, -10, -10, 10);
    QLineF x_axis(-10, -10, 10, -10);
    {
        QPen pen_x(Qt::red);
        pen_x.setWidth(2);
        painter -> setPen(pen_x);
        painter -> drawLine(x_axis);     // X-Axis
        QLineF angleLine1(x_axis.p2(), x_axis.p1());
        QLineF angleLine2(x_axis.p2(), x_axis.p1());
        angleLine1.setAngle(135);
        angleLine1.setLength(angleLine1.length() * 0.33);
        angleLine2.setAngle(-135);
        angleLine2.setLength(angleLine2.length() * 0.33);
        painter -> drawLine(angleLine1);
        painter -> drawLine(angleLine2);
    }

    {
        QPen pen_y(Qt::green);
        pen_y.setWidth(2);
        painter -> setPen(pen_y);
        painter -> drawLine(y_axis);  // Y-Axis

        pen_y.setWidth(2);
        painter -> setPen(pen_y);
        painter -> drawLine(y_axis);     // X-Axis
        QLineF angleLine1(y_axis.p2(), y_axis.p1());
        QLineF angleLine2(y_axis.p2(), y_axis.p1());

        angleLine1.setAngle(45);
        angleLine1.setLength(angleLine1.length() * 0.33);
        angleLine2.setAngle(135);
        angleLine2.setLength(angleLine2.length() * 0.33);
        painter -> drawLine(angleLine1);
        painter -> drawLine(angleLine2);
    }
}

void PhysGraphicsScene::setLineColor(const QColor &color) {
    myLineColor = color;
    if (isItemChange(Arrow::Type)) {
        Arrow *item = qgraphicsitem_cast<Arrow *>(selectedItems().first());
        item->setColor(myLineColor);
        update();
    }
}

void PhysGraphicsScene::setTextColor(const QColor &color) {
    myTextColor = color;
    if (isItemChange(DiagramTextItem::Type)) {
        DiagramTextItem *item = qgraphicsitem_cast<DiagramTextItem *>(selectedItems().first());
        item->setDefaultTextColor(myTextColor);
    }
}

void PhysGraphicsScene::setItemColor(const QColor &color) {
    myItemColor = color;
    if (isItemChange(DiagramItem::Type)) {
        DiagramItem *item = qgraphicsitem_cast<DiagramItem *>(selectedItems().first());
        item->setBrush(myItemColor);
    }
}

void PhysGraphicsScene::setFont(const QFont &font) {
    myFont = font;

    if (isItemChange(DiagramTextItem::Type)) {
        QGraphicsTextItem *item = qgraphicsitem_cast<DiagramTextItem *>(selectedItems().first());
        //At this point the selection can change so the first selected item might not be a DiagramTextItem
        if (item)
            item->setFont(myFont);
    }
}

void PhysGraphicsScene::setMode(Mode mode) {
    myMode = mode;
}

void PhysGraphicsScene::setItemType(DiagramItem::DiagramType type) {
    myItemType = type;
}

void PhysGraphicsScene::editorLostFocus(DiagramTextItem *item) {
    QTextCursor cursor = item ->textCursor();
    cursor.clearSelection();
    item ->setTextCursor(cursor);

    if (item ->toPlainText().isEmpty()) {
        removeItem(item);
        item ->deleteLater();
    }
}

void PhysGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) {
    if (mouseEvent ->button() != Qt::LeftButton)
        return;

    DiagramItem *item = NULL;
    switch (myMode) {
        case InsertItem:
            item = new DiagramItem(myItemType, myItemMenu);
            item ->setBrush(myItemColor);
            addItem(item);
            item ->setPos(mouseEvent->scenePos());
            emit itemInserted(item);
            break;
        case InsertLine:
            startPoint = mouseEvent ->scenePos();
            line = new QGraphicsLineItem(QLineF(mouseEvent ->scenePos(), mouseEvent ->scenePos()));
            line ->setPen(QPen(myLineColor, 2));
            addItem(line);
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
            addItem(textItem);
            textItem ->setDefaultTextColor(myTextColor);
            textItem ->setPos(mouseEvent->scenePos());
            emit textInserted(textItem);
    default:
        ;
    }
    QGraphicsScene::mousePressEvent(mouseEvent);
}

void PhysGraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) {
    if (myMode == InsertLine && line != 0) {
        QLineF newLine(line ->line().p1(), mouseEvent->scenePos());
        line ->setLine(newLine);
    }
    else if (myMode == MoveItem) {
        QGraphicsScene::mouseMoveEvent(mouseEvent);
    }
}

void PhysGraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) {
    QPointF endPoint = mouseEvent ->scenePos();

    if (line != 0 && myMode == InsertLine) {
        QList<QGraphicsItem *> startItems = items(line ->line().p1());
        if (startItems.count() && startItems.first() == line)
            startItems.removeFirst();
        QList<QGraphicsItem *> endItems = items(line ->line().p2());
        if (endItems.count() && endItems.first() == line)
            endItems.removeFirst();

        removeItem(line);
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
            addItem(arrow);
            arrow ->updatePosition();
        }
        else {
            Arrow *arrow = new Arrow(startPoint, endPoint);
            arrow ->setColor(myLineColor);
            arrow ->setZValue(-1000.0);
            addItem(arrow);
            arrow ->updatePosition();

        }
    }
    line = 0;
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

bool PhysGraphicsScene::isItemChange(int type) {
    foreach (QGraphicsItem *item, selectedItems()) {
        if (item ->type() == type)
            return true;
    }
    return false;
}
