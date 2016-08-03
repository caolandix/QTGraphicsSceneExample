#include <QtWidgets>
#include "physgraphicsview.h"
#include "arrow.h"

PhysGraphicsView::PhysGraphicsView(QMenu *pMenu, QGraphicsScene *pScene, QWidget *pParent) : QGraphicsView(pScene, pParent) {
    init(pMenu);
}

void PhysGraphicsView::init(QMenu *pMenu) {
    m_pScene = scene();
    myItemMenu = pMenu;
    myMode = MoveItem;
    myItemType = DiagramItem::Step;
    m_pLine = NULL;
    textItem = NULL;
    myItemColor = Qt::white;
    myTextColor = Qt::black;
    myLineColor = Qt::black;
    m_particleColor = Qt::darkGreen;
}

bool PhysGraphicsView::isItemChange(int type) {
    foreach (QGraphicsItem *item, m_pScene -> selectedItems()) {
        if (item ->type() == type)
            return true;
    }
    return false;
}

void PhysGraphicsView::setLineColor(const QColor &color) {
    myLineColor = color;
    if (isItemChange(Arrow::Type)) {
        Arrow *item = qgraphicsitem_cast<Arrow *>(m_pScene -> selectedItems().first());
        item ->setColor(myLineColor);
        update();
    }
}

void PhysGraphicsView::setTextColor(const QColor &color) {
    myTextColor = color;
    if (isItemChange(DiagramTextItem::Type)) {
        DiagramTextItem *item = qgraphicsitem_cast<DiagramTextItem *>(m_pScene ->selectedItems().first());
        item ->setDefaultTextColor(myTextColor);
    }
}

void PhysGraphicsView::setMode(Mode mode) {
    myMode = mode;
}

void PhysGraphicsView::setItemType(DiagramItem::DiagramType type) {
    myItemType = type;
}

void PhysGraphicsView::setItemColor(const QColor &color) {
    myItemColor = color;
    if (isItemChange(DiagramItem::Type)) {
        DiagramItem *item = qgraphicsitem_cast<DiagramItem *>(m_pScene -> selectedItems().first());
        item->setBrush(myItemColor);
    }
}

void PhysGraphicsView::setFont(const QFont &font) {
    myFont = font;

    if (isItemChange(DiagramTextItem::Type)) {
        QGraphicsTextItem *item = qgraphicsitem_cast<DiagramTextItem *>(m_pScene ->selectedItems().first());
        //At this point the selection can change so the first selected item might not be a DiagramTextItem
        if (item)
            item ->setFont(myFont);
    }
}


void PhysGraphicsView::editorLostFocus(DiagramTextItem *item) {
    QTextCursor cursor = item ->textCursor();
    cursor.clearSelection();
    item ->setTextCursor(cursor);

    if (item ->toPlainText().isEmpty()) {
        m_pScene ->removeItem(item);
        item ->deleteLater();
    }
}

void PhysGraphicsView::mousePressEvent(QMouseEvent *mouseEvent) {
    if (mouseEvent ->button() != Qt::LeftButton)
        return;
    QPointF scenePos = mapToScene(mouseEvent ->pos());
    DiagramItem *item = NULL;

    switch (myMode) {
        case InsertItem:
            item = new DiagramItem(myItemType, myItemMenu);
            item ->setBrush(myItemColor);
            m_pScene ->addItem(item);
            item ->setPos(scenePos);
            emit itemInserted(item);
            break;
        case InsertLine:
            startPoint = scenePos;
            m_pLine = new QGraphicsLineItem(QLineF(scenePos, scenePos));
            m_pLine ->setPen(QPen(myLineColor, 2));
            m_pScene ->addItem(m_pLine);
            break;
        case InsertParticle:
            startPoint = scenePos;
            m_pPolyItem = new QGraphicsPolygonItem(QPolygonF(QVector<QPointF>() << QPointF(10, 10) << QPointF(0, 90) << QPointF(40, 70) << QPointF(80, 110) << QPointF(70, 20)));
            m_pPolyItem ->setPen(QPen(m_particleColor));
            m_pPolyItem ->setBrush(Qt::yellow);
            m_pScene ->addItem(m_pPolyItem);
            break;
        case InsertText:
            textItem = new DiagramTextItem();
            textItem ->setFont(myFont);
            textItem ->setTextInteractionFlags(Qt::TextEditorInteraction);
            textItem ->setZValue(1000.0);
            connect(textItem, SIGNAL(lostFocus(DiagramTextItem*)), this, SLOT(editorLostFocus(DiagramTextItem*)));
            connect(textItem, SIGNAL(selectedChange(QGraphicsItem*)), this, SIGNAL(itemSelected(QGraphicsItem*)));
            m_pScene ->addItem(textItem);
            textItem ->setDefaultTextColor(myTextColor);
            textItem ->setPos(scenePos);
            emit textInserted(textItem);
    default:
        ;
    }
}

void PhysGraphicsView::mouseMoveEvent(QMouseEvent *mouseEvent) {
    QPointF scenePos = mapToScene(mouseEvent ->pos());

    if (myMode == InsertLine && m_pLine) {
        QLineF newLine(m_pLine ->line().p1(), scenePos);
        m_pLine ->setLine(newLine);
    }
    else if (myMode == MoveItem) {
        PhysGraphicsView::mouseMoveEvent(mouseEvent);
    }
}

void PhysGraphicsView::mouseReleaseEvent(QMouseEvent *mouseEvent) {
    QPointF endPoint = mapToScene(mouseEvent ->pos());

    if (m_pLine != NULL && myMode == InsertLine) {
        QList<QGraphicsItem *> startItems = m_pScene ->items(m_pLine ->line().p1());
        if (startItems.count() && startItems.first() == m_pLine)
            startItems.removeFirst();
        QList<QGraphicsItem *> endItems = m_pScene ->items(m_pLine ->line().p2());
        if (endItems.count() && endItems.first() == m_pLine)
            endItems.removeFirst();

        m_pScene ->removeItem(m_pLine);
        delete m_pLine;

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
            Arrow *pArrow = new Arrow(startPoint, endPoint);
            pArrow ->setColor(myLineColor);
            pArrow ->setZValue(-1000.0);
            m_pScene ->addItem(pArrow);
            pArrow ->updatePosition();

        }
        m_pLine = NULL;
    }
    else if (m_pPolyItem && myMode == InsertParticle) {
        m_pPolyItem = NULL;
    }
}

void PhysGraphicsView::drawBackground(QPainter *painter, const QRectF &rect) {
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


