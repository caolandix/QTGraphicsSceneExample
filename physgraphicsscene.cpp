#include "physgraphicsscene.h"
#include "arrow.h"

#include <QtWidgets>

PhysGraphicsScene::PhysGraphicsScene(QMenu *pItemMenu, QObject *parent) : QGraphicsScene(parent) {
    m_pItemMenu = pItemMenu;
    m_Mode = MoveItem;
    m_ItemType = DiagramItem::Step;
    m_pLine = NULL;
    m_pTextItem = NULL;
    m_ItemColor = Qt::white;
    m_TextColor = Qt::black;
    m_LineColor = Qt::black;
}

void PhysGraphicsScene::drawBackground(QPainter *painter, const QRectF &rect) {
    Q_UNUSED(rect);

    QMatrix mtx;
    int w_2 = width() / 2;
    int h_2 = height() / 2;

    painter -> setPen(Qt::blue);
    painter -> drawLine(0, h_2, width(), h_2);     // X-Axis
    painter -> drawLine(w_2, 0 , w_2, height());  // Y-Axis

    mtx.translate(w_2, h_2);
    mtx.scale(1, -1);

    painter -> setMatrix(mtx);
    painter -> setPen(Qt::NoPen);
    painter -> setBrush(QBrush(Qt::blue, Qt::Dense4Pattern));
    painter -> drawRect(-10, -10, 20, 20);

    QLineF y_axis(-10, -10, -10, 10);
    QLineF x_axis(-10, -10, 10, -10);
    QPen pen;
    QLineF angleLine1, angleLine2;

    pen.setWidth(2);
    pen.setColor(Qt::red);
    painter -> setPen(pen);

    setupAngleLine(angleLine1, x_axis.p2(), x_axis.p1(), 135.0);
    setupAngleLine(angleLine2, x_axis.p2(), x_axis.p1(), -135.0);
    painter -> drawLine(x_axis);     // X-Axis
    painter -> drawLine(angleLine1);
    painter -> drawLine(angleLine2);

    pen.setColor(Qt::green);
    setupAngleLine(angleLine1, y_axis.p2(), y_axis.p1(), 45.0);
    setupAngleLine(angleLine2, y_axis.p2(), y_axis.p1(), 135.0);
    painter -> setPen(pen);
    painter -> drawLine(y_axis);  // Y-Axis
    painter -> drawLine(angleLine1);
    painter -> drawLine(angleLine2);

    if (m_Mode == InsertLine && m_pLine) {
        drawAngledPlane(painter);
        drawThetaPhi(painter);
    }

}

QPointF PhysGraphicsScene::findLineMidPoint(const QLineF line) {
    QPointF mid((line.p1().x() + line.p2().x()) / 2, (line.p1().y() + line.p2().y()) / 2);

    return mid;
}

void PhysGraphicsScene::drawAngledPlane(QPainter *painter) {
    QPointF lineMidPoint = findLineMidPoint(m_pLine ->line());
    QRectF lineRect(m_pLine ->line().p1(), m_pLine ->line().p2());

    // Draw the temporary cartesian axis' origin'ed at p1 of the line
    QPen pen(Qt::lightGray);
    pen.setStyle(Qt::DashDotDotLine);
    pen.setWidth(2);
    painter ->setPen(pen);
    QLineF xAxis, yAxis;
    xAxis.setAngle(90);
    xAxis.setP1(m_pLine ->line().p1()); xAxis.setP2(QPointF(m_pLine ->line().dx(), 0));
    xAxis.setLength(m_pLine ->line().dx());

    yAxis.setAngle(0);
    yAxis.setLength(m_pLine ->line().dy());

    painter -> drawLine(xAxis);   // x-axis
    painter -> drawLine(yAxis);   // y-axis


    // Draw the arc from the false cartesian axis to the line
    double angle = m_pLine ->line().angle();
}

void PhysGraphicsScene::drawThetaPhi(QPainter *painter) {

}


void PhysGraphicsScene::setupAngleLine(QLineF &line, const QPointF p1, const QPointF p2, const double angle) {
    line.setP1(p1);
    line.setP2(p2);
    line.setAngle(angle);
    line.setLength(line.length() * 0.33);
}

void PhysGraphicsScene::setLineColor(const QColor &color) {
    m_LineColor = color;
    if (isItemChange(Arrow::Type)) {
        Arrow *pItem = qgraphicsitem_cast<Arrow *>(selectedItems().first());
        pItem ->setColor(m_LineColor);
        update();
    }
}

void PhysGraphicsScene::setTextColor(const QColor &color) {
    m_TextColor = color;
    if (isItemChange(DiagramTextItem::Type)) {
        DiagramTextItem *pItem = qgraphicsitem_cast<DiagramTextItem *>(selectedItems().first());
        pItem ->setDefaultTextColor(m_TextColor);
    }
}

void PhysGraphicsScene::setItemColor(const QColor &color) {
    m_ItemColor = color;
    if (isItemChange(DiagramItem::Type)) {
        DiagramItem *pItem = qgraphicsitem_cast<DiagramItem *>(selectedItems().first());
        pItem ->setBrush(m_ItemColor);
    }
}

void PhysGraphicsScene::setFont(const QFont &font) {
    m_Font = font;

    if (isItemChange(DiagramTextItem::Type)) {
        QGraphicsTextItem *pItem = qgraphicsitem_cast<DiagramTextItem *>(selectedItems().first());
        //At this point the selection can change so the first selected item might not be a DiagramTextItem
        if (pItem)
            pItem ->setFont(m_Font);
    }
}

void PhysGraphicsScene::setMode(Mode mode) {
    m_Mode = mode;
}

void PhysGraphicsScene::setItemType(DiagramItem::DiagramType type) {
    m_ItemType = type;
}

void PhysGraphicsScene::editorLostFocus(DiagramTextItem *pItem) {
    QTextCursor cursor = pItem ->textCursor();
    cursor.clearSelection();
    pItem ->setTextCursor(cursor);

    if (pItem ->toPlainText().isEmpty()) {
        removeItem(pItem);
        pItem ->deleteLater();
    }
}

void PhysGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) {
    if (mouseEvent ->button() != Qt::LeftButton)
        return;

    DiagramItem *pItem = NULL;
    switch (m_Mode) {
        case InsertItem:
            pItem = new DiagramItem(m_ItemType, m_pItemMenu);
            pItem ->setBrush(m_ItemColor);
            addItem(pItem);
            pItem ->setPos(mouseEvent ->scenePos());
            emit itemInserted(pItem);
            break;
        case InsertLine:
            startPoint = mouseEvent ->scenePos();
            m_pLine = new QGraphicsLineItem(QLineF(mouseEvent ->scenePos(), mouseEvent ->scenePos()));
            m_pLine ->setPen(QPen(m_LineColor, 2));
            addItem(m_pLine);
            break;
        case InsertText:
            m_pTextItem = new DiagramTextItem();
            m_pTextItem ->setFont(m_Font);
            m_pTextItem ->setTextInteractionFlags(Qt::TextEditorInteraction);
            m_pTextItem ->setZValue(1000.0);
            connect(m_pTextItem, SIGNAL(lostFocus(DiagramTextItem*)),
                    this, SLOT(editorLostFocus(DiagramTextItem*)));
            connect(m_pTextItem, SIGNAL(selectedChange(QGraphicsItem*)),
                    this, SIGNAL(itemSelected(QGraphicsItem*)));
            addItem(m_pTextItem);
            m_pTextItem ->setDefaultTextColor(m_TextColor);
            m_pTextItem ->setPos(mouseEvent->scenePos());
            emit textInserted(m_pTextItem);
            break;
        case RotateItem:
            break;
    default:
        ;
    }
    QGraphicsScene::mousePressEvent(mouseEvent);
}

void PhysGraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) {
    if (m_Mode == InsertLine && m_pLine) {
        QLineF newLine(m_pLine ->line().p1(), mouseEvent->scenePos());
        m_pLine ->setLine(newLine);
    }
    else {
        if (m_Mode == MoveItem)
            QGraphicsScene::mouseMoveEvent(mouseEvent);
    }
}

void PhysGraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) {
    QPointF endPoint = mouseEvent ->scenePos();

    if (m_pLine && m_Mode == InsertLine) {
        QList<QGraphicsItem *> startItems = items(m_pLine ->line().p1());
        if (startItems.count() && startItems.first() == m_pLine)
            startItems.removeFirst();
        QList<QGraphicsItem *> endItems = items(m_pLine ->line().p2());
        if (endItems.count() && endItems.first() == m_pLine)
            endItems.removeFirst();

        removeItem(m_pLine);
        delete m_pLine;

        if (startItems.count() > 0 && endItems.count() > 0 &&
            startItems.first() ->type() == DiagramItem::Type && endItems.first() ->type() == DiagramItem::Type &&
            startItems.first() != endItems.first()) {

            DiagramItem *startItem = qgraphicsitem_cast<DiagramItem *>(startItems.first());
            DiagramItem *endItem = qgraphicsitem_cast<DiagramItem *>(endItems.first());
            Arrow *arrow = new Arrow(startItem, endItem);
            arrow ->setColor(m_LineColor);
            startItem ->addArrow(arrow);
            endItem ->addArrow(arrow);
            arrow ->setZValue(-1000.0);
            addItem(arrow);
            arrow ->updatePosition();
        }
        else {
            Arrow *arrow = new Arrow(startPoint, endPoint);
            arrow ->setColor(m_LineColor);
            arrow ->setZValue(-1000.0);
            addItem(arrow);
            arrow ->updatePosition();

        }
    }
    m_pLine = NULL;
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

bool PhysGraphicsScene::isItemChange(int type) {
    foreach (QGraphicsItem *pItem, selectedItems()) {
        if (pItem ->type() == type)
            return true;
    }
    return false;
}
