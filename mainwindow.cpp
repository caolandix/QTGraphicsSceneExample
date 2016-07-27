#include "arrow.h"
#include "diagramitem.h"
// #include "physgraphicsscene.h"
#include "diagramtextitem.h"
#include "mainwindow.h"

#include <QtWidgets>

const int InsertTextButton = 10;

MainWindow::MainWindow() {
    createActions();
    createToolBox();
    createMenus();

    // scene = new PhysGraphicsScene(itemMenu, this);
    scene = new QGraphicsScene(this);
    scene ->setSceneRect(QRectF(0, 0, 640, 480));

    /*
    connect(scene, SIGNAL(itemInserted(DiagramItem *)), this, SLOT(itemInserted(DiagramItem *)));
    connect(scene, SIGNAL(textInserted(QGraphicsTextItem *)), this, SLOT(textInserted(QGraphicsTextItem *)));
    connect(scene, SIGNAL(itemSelected(QGraphicsItem *)), this, SLOT(itemSelected(QGraphicsItem *)));
    */
    createToolbars();

    view = new QGraphicsView(scene);
    connect(this, SIGNAL(itemInserted(DiagramItem *)), this, SLOT(onItemInserted(DiagramItem *)));
    connect(this, SIGNAL(textInserted(QGraphicsTextItem *)), this, SLOT(onTextInserted(QGraphicsTextItem *)));
    connect(this, SIGNAL(itemSelected(QGraphicsItem *)), this, SLOT(onItemSelected(QGraphicsItem *)));

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(toolBox);
    layout->addWidget(view);

    QWidget *widget = new QWidget;
    widget->setLayout(layout);

    setCentralWidget(widget);
    setWindowTitle(tr("PhysGraphicsScene"));
    setUnifiedTitleAndToolBarOnMac(true);
}

void MainWindow::backgroundButtonGroupClicked(QAbstractButton *button)
{
    QList<QAbstractButton *> buttons = backgroundButtonGroup->buttons();
    foreach (QAbstractButton *myButton, buttons) {
        if (myButton != button)
            button->setChecked(false);
    }
    QString text = button->text();
    if (text == tr("Blue Grid"))
        scene->setBackgroundBrush(QPixmap("images/background1.png"));
    else if (text == tr("White Grid"))
        scene->setBackgroundBrush(QPixmap("images/background2.png"));
    else if (text == tr("Gray Grid"))
        scene->setBackgroundBrush(QPixmap("images/background3.png"));
    else
        scene->setBackgroundBrush(QPixmap("images/background4.png"));

    scene->update();
    view->update();
}

void MainWindow::buttonGroupClicked(int id) {
    QList<QAbstractButton *> buttons = buttonGroup ->buttons();
    foreach (QAbstractButton *button, buttons) {
        if (buttonGroup ->button(id) != button)
            button ->setChecked(false);
    }
    if (id == InsertTextButton) {
        setMode(InsertText);
    }
    else {
        setItemType(DiagramItem::DiagramType(id));
        setMode(InsertItem);
    }
}

void MainWindow::deleteItem() {
    foreach (QGraphicsItem *item, scene ->selectedItems()) {
        if (item->type() == Arrow::Type) {
            scene ->removeItem(item);
            Arrow *arrow = qgraphicsitem_cast<Arrow *>(item);
            arrow ->startItem() ->removeArrow(arrow);
            arrow ->endItem() ->removeArrow(arrow);
            delete item;
        }
    }

    foreach (QGraphicsItem *item, scene ->selectedItems()) {
         if (item ->type() == DiagramItem::Type)
             qgraphicsitem_cast<DiagramItem *>(item) ->removeArrows();
         scene ->removeItem(item);
         delete item;
     }
}

void MainWindow::pointerGroupClicked(int) {
    setMode(Mode(pointerTypeGroup->checkedId()));
}

void MainWindow::bringToFront() {
    if (scene->selectedItems().isEmpty())
        return;

    QGraphicsItem *selectedItem = scene ->selectedItems().first();
    QList<QGraphicsItem *> overlapItems = selectedItem ->collidingItems();

    qreal zValue = 0;
    foreach (QGraphicsItem *item, overlapItems) {
        if (item ->zValue() >= zValue && item ->type() == DiagramItem::Type)
            zValue = item ->zValue() + 0.1;
    }
    selectedItem ->setZValue(zValue);
}

void MainWindow::sendToBack() {
    if (scene ->selectedItems().isEmpty())
        return;

    QGraphicsItem *selectedItem = scene ->selectedItems().first();
    QList<QGraphicsItem *> overlapItems = selectedItem ->collidingItems();

    qreal zValue = 0;
    foreach (QGraphicsItem *item, overlapItems) {
        if (item ->zValue() <= zValue && item ->type() == DiagramItem::Type)
            zValue = item ->zValue() - 0.1;
    }
    selectedItem ->setZValue(zValue);
}

void MainWindow::onItemInserted(DiagramItem *item) {
    pointerTypeGroup ->button(int(MoveItem)) ->setChecked(true);
    setMode(Mode(pointerTypeGroup ->checkedId()));
    buttonGroup ->button(int(item ->diagramType())) ->setChecked(false);
}

void MainWindow::onTextInserted(QGraphicsTextItem *) {
    buttonGroup ->button(InsertTextButton) ->setChecked(false);
    setMode(Mode(pointerTypeGroup ->checkedId()));
}



void MainWindow::currentFontChanged(const QFont &) {
    handleFontChange();
}

void MainWindow::fontSizeChanged(const QString &) {
    handleFontChange();
}

void MainWindow::sceneScaleChanged(const QString &scale) {
    double newScale = scale.left(scale.indexOf(tr("%"))).toDouble() / 100.0;
    QMatrix oldMatrix = view ->matrix();
    view ->resetMatrix();
    view ->translate(oldMatrix.dx(), oldMatrix.dy());
    view ->scale(newScale, newScale);
}

void MainWindow::textColorChanged() {
    textAction = qobject_cast<QAction *>(sender());
    fontColorToolButton->setIcon(createColorToolButtonIcon(
                                     "images/textpointer.png",
                                     qvariant_cast<QColor>(textAction->data())));
    textButtonTriggered();
}

void MainWindow::itemColorChanged() {
    fillAction = qobject_cast<QAction *>(sender());
    fillColorToolButton->setIcon(createColorToolButtonIcon(
                                     "images/floodfill.png",
                                     qvariant_cast<QColor>(fillAction->data())));
    fillButtonTriggered();
}

void MainWindow::lineColorChanged() {
    lineAction = qobject_cast<QAction *>(sender());
    lineColorToolButton->setIcon(createColorToolButtonIcon(
                                     "images/linecolor.png",
                                     qvariant_cast<QColor>(lineAction->data())));
    lineButtonTriggered();
}

bool MainWindow::isItemChange(int type) {
    foreach (QGraphicsItem *item, scene ->selectedItems()) {
        if (item ->type() == type)
            return true;
    }
    return false;
}

void MainWindow::setFont(const QFont &font) {
    myFont = font;

    if (isItemChange(DiagramTextItem::Type)) {
        QGraphicsTextItem *item = qgraphicsitem_cast<DiagramTextItem *>(scene ->selectedItems().first());
        //At this point the selection can change so the first selected item might not be a DiagramTextItem
        if (item)
            item->setFont(myFont);
    }
}

void MainWindow::setMode(Mode mode) {
    myMode = mode;
}

void MainWindow::setItemType(DiagramItem::DiagramType type) {
    myItemType = type;
}

void MainWindow::setLineColor(const QColor &color) {
    myLineColor = color;
    if (isItemChange(Arrow::Type)) {
        Arrow *item = qgraphicsitem_cast<Arrow *>(scene -> selectedItems().first());
        item->setColor(myLineColor);
        update();
    }
}

void MainWindow::setTextColor(const QColor &color) {
    myTextColor = color;
    if (isItemChange(DiagramTextItem::Type)) {
        DiagramTextItem *item = qgraphicsitem_cast<DiagramTextItem *>(scene -> selectedItems().first());
        item->setDefaultTextColor(myTextColor);
    }
}

void MainWindow::setItemColor(const QColor &color) {
    myItemColor = color;
    if (isItemChange(DiagramItem::Type)) {
        DiagramItem *item = qgraphicsitem_cast<DiagramItem *>(scene -> selectedItems().first());
        item->setBrush(myItemColor);
    }
}

void MainWindow::editorLostFocus(DiagramTextItem *item) {
    QTextCursor cursor = item ->textCursor();
    cursor.clearSelection();
    item->setTextCursor(cursor);

    if (item->toPlainText().isEmpty()) {
        scene ->removeItem(item);
        item->deleteLater();
    }
}

void MainWindow::textButtonTriggered() {
    setTextColor(qvariant_cast<QColor>(textAction->data()));
}

void MainWindow::fillButtonTriggered() {
    setItemColor(qvariant_cast<QColor>(fillAction->data()));
}

void MainWindow::lineButtonTriggered() {
    setLineColor(qvariant_cast<QColor>(lineAction->data()));
}

void MainWindow::handleFontChange() {
    QFont font = fontCombo->currentFont();
    font.setPointSize(fontSizeCombo->currentText().toInt());
    font.setWeight(boldAction->isChecked() ? QFont::Bold : QFont::Normal);
    font.setItalic(italicAction->isChecked());
    font.setUnderline(underlineAction->isChecked());

    scene->setFont(font);
}

void MainWindow::onItemSelected(QGraphicsItem *item) {
    DiagramTextItem *textItem =
    qgraphicsitem_cast<DiagramTextItem *>(item);

    QFont font = textItem->font();
    fontCombo->setCurrentFont(font);
    fontSizeCombo->setEditText(QString().setNum(font.pointSize()));
    boldAction->setChecked(font.weight() == QFont::Bold);
    italicAction->setChecked(font.italic());
    underlineAction->setChecked(font.underline());
}

void MainWindow::about() {
    QMessageBox::about(this, tr("About Diagram Scene"), tr("The <b>Diagram Scene</b> example shows use of the graphics framework."));
}

void MainWindow::createToolBox() {
    buttonGroup = new QButtonGroup(this);
    buttonGroup->setExclusive(false);
    connect(buttonGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(buttonGroupClicked(int)));
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(createCellWidget(tr("Conditional"), DiagramItem::Conditional), 0, 0);
    layout->addWidget(createCellWidget(tr("Process"), DiagramItem::Step),0, 1);
    layout->addWidget(createCellWidget(tr("Input/Output"), DiagramItem::Io), 1, 0);

    QToolButton *textButton = new QToolButton;
    textButton->setCheckable(true);
    buttonGroup->addButton(textButton, InsertTextButton);
    textButton->setIcon(QIcon(QPixmap("images/textpointer.png")));
    textButton->setIconSize(QSize(50, 50));
    QGridLayout *textLayout = new QGridLayout;
    textLayout->addWidget(textButton, 0, 0, Qt::AlignHCenter);
    textLayout->addWidget(new QLabel(tr("Text")), 1, 0, Qt::AlignCenter);
    QWidget *textWidget = new QWidget;
    textWidget->setLayout(textLayout);
    layout->addWidget(textWidget, 1, 1);

    layout->setRowStretch(3, 10);
    layout->setColumnStretch(2, 10);

    QWidget *itemWidget = new QWidget;
    itemWidget->setLayout(layout);

    backgroundButtonGroup = new QButtonGroup(this);
    connect(backgroundButtonGroup, SIGNAL(buttonClicked(QAbstractButton*)),
            this, SLOT(backgroundButtonGroupClicked(QAbstractButton*)));

    QGridLayout *backgroundLayout = new QGridLayout;
    backgroundLayout->addWidget(createBackgroundCellWidget(tr("Blue Grid"),
                                                           "images/background1.png"), 0, 0);
    backgroundLayout->addWidget(createBackgroundCellWidget(tr("White Grid"),
                                                           "images/background2.png"), 0, 1);
    backgroundLayout->addWidget(createBackgroundCellWidget(tr("Gray Grid"),
                                                           "images/background3.png"), 1, 0);
    backgroundLayout->addWidget(createBackgroundCellWidget(tr("No Grid"),
                                                           "images/background4.png"), 1, 1);

    backgroundLayout->setRowStretch(2, 10);
    backgroundLayout->setColumnStretch(2, 10);

    QWidget *backgroundWidget = new QWidget;
    backgroundWidget->setLayout(backgroundLayout);

    toolBox = new QToolBox;
    toolBox->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Ignored));
    toolBox->setMinimumWidth(itemWidget->sizeHint().width());
    toolBox->addItem(itemWidget, tr("Basic Flowchart Shapes"));
    toolBox->addItem(backgroundWidget, tr("Backgrounds"));
}

void MainWindow::createActions() {
    toFrontAction = new QAction(QIcon("images/bringtofront.png"),
                                tr("Bring to &Front"), this);
    toFrontAction->setShortcut(tr("Ctrl+F"));
    toFrontAction->setStatusTip(tr("Bring item to front"));
    connect(toFrontAction, SIGNAL(triggered()), this, SLOT(bringToFront()));

    sendBackAction = new QAction(QIcon("images/sendtoback.png"), tr("Send to &Back"), this);
    sendBackAction->setShortcut(tr("Ctrl+T"));
    sendBackAction->setStatusTip(tr("Send item to back"));
    connect(sendBackAction, SIGNAL(triggered()), this, SLOT(sendToBack()));

    deleteAction = new QAction(QIcon("images/delete.png"), tr("&Delete"), this);
    deleteAction->setShortcut(tr("Delete"));
    deleteAction->setStatusTip(tr("Delete item from diagram"));
    connect(deleteAction, SIGNAL(triggered()), this, SLOT(deleteItem()));

    exitAction = new QAction(tr("E&xit"), this);
    exitAction->setShortcuts(QKeySequence::Quit);
    exitAction->setStatusTip(tr("Quit Scenediagram example"));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

    boldAction = new QAction(tr("Bold"), this);
    boldAction->setCheckable(true);
    QPixmap pixmap(":/images/bold.png");
    boldAction->setIcon(QIcon(pixmap));
    boldAction->setShortcut(tr("Ctrl+B"));
    connect(boldAction, SIGNAL(triggered()), this, SLOT(handleFontChange()));

    italicAction = new QAction(QIcon("images/italic.png"), tr("Italic"), this);
    italicAction->setCheckable(true);
    italicAction->setShortcut(tr("Ctrl+I"));
    connect(italicAction, SIGNAL(triggered()), this, SLOT(handleFontChange()));

    underlineAction = new QAction(QIcon("images/underline.png"), tr("Underline"), this);
    underlineAction->setCheckable(true);
    underlineAction->setShortcut(tr("Ctrl+U"));
    connect(underlineAction, SIGNAL(triggered()), this, SLOT(handleFontChange()));

    aboutAction = new QAction(tr("A&bout"), this);
    aboutAction->setShortcut(tr("F1"));
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));
}

void MainWindow::createMenus() {
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(exitAction);

    itemMenu = menuBar()->addMenu(tr("&Item"));
    itemMenu->addAction(deleteAction);
    itemMenu->addSeparator();
    itemMenu->addAction(toFrontAction);
    itemMenu->addAction(sendBackAction);

    aboutMenu = menuBar()->addMenu(tr("&Help"));
    aboutMenu->addAction(aboutAction);
}

void MainWindow::createToolbars() {
    editToolBar = addToolBar(tr("Edit"));
    editToolBar->addAction(deleteAction);
    editToolBar->addAction(toFrontAction);
    editToolBar->addAction(sendBackAction);

    fontCombo = new QFontComboBox();
    connect(fontCombo, SIGNAL(currentFontChanged(QFont)),
            this, SLOT(currentFontChanged(QFont)));

    fontSizeCombo = new QComboBox;
    fontSizeCombo->setEditable(true);
    for (int i = 8; i < 30; i = i + 2)
        fontSizeCombo->addItem(QString().setNum(i));
    QIntValidator *validator = new QIntValidator(2, 64, this);
    fontSizeCombo->setValidator(validator);
    connect(fontSizeCombo, SIGNAL(currentIndexChanged(QString)),
            this, SLOT(fontSizeChanged(QString)));

    fontColorToolButton = new QToolButton;
    fontColorToolButton->setPopupMode(QToolButton::MenuButtonPopup);
    fontColorToolButton->setMenu(createColorMenu(SLOT(textColorChanged()), Qt::black));
    textAction = fontColorToolButton->menu()->defaultAction();
    fontColorToolButton->setIcon(createColorToolButtonIcon("images/textpointer.png", Qt::black));
    fontColorToolButton->setAutoFillBackground(true);
    connect(fontColorToolButton, SIGNAL(clicked()),
            this, SLOT(textButtonTriggered()));

    fillColorToolButton = new QToolButton;
    fillColorToolButton->setPopupMode(QToolButton::MenuButtonPopup);
    fillColorToolButton->setMenu(createColorMenu(SLOT(itemColorChanged()), Qt::white));
    fillAction = fillColorToolButton->menu()->defaultAction();
    fillColorToolButton->setIcon(createColorToolButtonIcon(
                                     "images/floodfill.png", Qt::white));
    connect(fillColorToolButton, SIGNAL(clicked()),
            this, SLOT(fillButtonTriggered()));

    lineColorToolButton = new QToolButton;
    lineColorToolButton->setPopupMode(QToolButton::MenuButtonPopup);
    lineColorToolButton->setMenu(createColorMenu(SLOT(lineColorChanged()), Qt::black));
    lineAction = lineColorToolButton->menu()->defaultAction();
    lineColorToolButton->setIcon(createColorToolButtonIcon(
                                     "images/linecolor.png", Qt::black));
    connect(lineColorToolButton, SIGNAL(clicked()),
            this, SLOT(lineButtonTriggered()));

    textToolBar = addToolBar(tr("Font"));
    textToolBar->addWidget(fontCombo);
    textToolBar->addWidget(fontSizeCombo);
    textToolBar->addAction(boldAction);
    textToolBar->addAction(italicAction);
    textToolBar->addAction(underlineAction);

    colorToolBar = addToolBar(tr("Color"));
    colorToolBar->addWidget(fontColorToolButton);
    colorToolBar->addWidget(fillColorToolButton);
    colorToolBar->addWidget(lineColorToolButton);

    QToolButton *pointerButton = new QToolButton;
    pointerButton->setCheckable(true);
    pointerButton->setChecked(true);
    pointerButton->setIcon(QIcon("images/pointer.png"));
    QToolButton *linePointerButton = new QToolButton;
    linePointerButton->setCheckable(true);
    linePointerButton->setIcon(QIcon("images/linepointer.png"));

    pointerTypeGroup = new QButtonGroup(this);
    pointerTypeGroup->addButton(pointerButton, int(MoveItem));
    pointerTypeGroup->addButton(linePointerButton, int(InsertLine));
    connect(pointerTypeGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(pointerGroupClicked(int)));

    sceneScaleCombo = new QComboBox;
    QStringList scales;
    scales << tr("50%") << tr("75%") << tr("100%") << tr("125%") << tr("150%");
    sceneScaleCombo->addItems(scales);
    sceneScaleCombo->setCurrentIndex(2);
    connect(sceneScaleCombo, SIGNAL(currentIndexChanged(QString)),
            this, SLOT(sceneScaleChanged(QString)));

    pointerToolbar = addToolBar(tr("Pointer type"));
    pointerToolbar->addWidget(pointerButton);
    pointerToolbar->addWidget(linePointerButton);
    pointerToolbar->addWidget(sceneScaleCombo);
}

QWidget *MainWindow::createBackgroundCellWidget(const QString &text, const QString &image) {
    QToolButton *button = new QToolButton;
    button->setText(text);
    button->setIcon(QIcon(image));
    button->setIconSize(QSize(50, 50));
    button->setCheckable(true);
    backgroundButtonGroup->addButton(button);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(button, 0, 0, Qt::AlignHCenter);
    layout->addWidget(new QLabel(text), 1, 0, Qt::AlignCenter);

    QWidget *widget = new QWidget;
    widget->setLayout(layout);

    return widget;
}

QWidget *MainWindow::createCellWidget(const QString &text, DiagramItem::DiagramType type) {

    DiagramItem item(type, itemMenu);
    QIcon icon(item.image());

    QToolButton *button = new QToolButton;
    button->setIcon(icon);
    button->setIconSize(QSize(50, 50));
    button->setCheckable(true);
    buttonGroup->addButton(button, int(type));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(button, 0, 0, Qt::AlignHCenter);
    layout->addWidget(new QLabel(text), 1, 0, Qt::AlignCenter);

    QWidget *widget = new QWidget;
    widget->setLayout(layout);

    return widget;
}

QMenu *MainWindow::createColorMenu(const char *slot, QColor defaultColor) {
    QList<QColor> colors;
    colors << Qt::black << Qt::white << Qt::red << Qt::blue << Qt::yellow;
    QStringList names;
    names << tr("black") << tr("white") << tr("red") << tr("blue")
          << tr("yellow");

    QMenu *colorMenu = new QMenu(this);
    for (int i = 0; i < colors.count(); ++i) {
        QAction *action = new QAction(names.at(i), this);
        action->setData(colors.at(i));
        action->setIcon(createColorIcon(colors.at(i)));
        connect(action, SIGNAL(triggered()), this, slot);
        colorMenu->addAction(action);
        if (colors.at(i) == defaultColor)
            colorMenu->setDefaultAction(action);
    }
    return colorMenu;
}

QIcon MainWindow::createColorToolButtonIcon(const QString &imageFile, QColor color) {
    QPixmap pixmap(50, 80);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    QPixmap image(imageFile);
    // Draw icon centred horizontally on button.
    QRect target(4, 0, 42, 43);
    QRect source(0, 0, 42, 43);
    painter.fillRect(QRect(0, 60, 50, 80), color);
    painter.drawPixmap(target, image, source);

    return QIcon(pixmap);
}

QIcon MainWindow::createColorIcon(QColor color) {
    QPixmap pixmap(20, 20);
    QPainter painter(&pixmap);
    painter.setPen(Qt::NoPen);
    painter.fillRect(QRect(0, 0, 20, 20), color);

    return QIcon(pixmap);
}


void MainWindow::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) {
    if (mouseEvent ->button() != Qt::LeftButton)
        return;

    DiagramItem *item = NULL;
    switch (myMode) {
        case InsertItem:
            item = new DiagramItem(myItemType, myItemMenu);
            item ->setBrush(myItemColor);
            scene ->addItem(item);
            item ->setPos(mouseEvent->scenePos());
            emit itemInserted(item);
            break;
        case InsertLine:
            startPoint = mouseEvent ->scenePos();
            line = new QGraphicsLineItem(QLineF(mouseEvent ->scenePos(), mouseEvent ->scenePos()));
            line ->setPen(QPen(myLineColor, 2));
            scene ->addItem(line);
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
            scene ->addItem(textItem);
            textItem ->setDefaultTextColor(myTextColor);
            textItem ->setPos(mouseEvent->scenePos());
            emit textInserted(textItem);
    default:
        ;
    }
    MainWindow::mousePressEvent(mouseEvent);
}

void MainWindow::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) {
    if (myMode == InsertLine && line != 0) {
        QLineF newLine(line ->line().p1(), mouseEvent->scenePos());
        line ->setLine(newLine);
    }
    else if (myMode == MoveItem) {
        MainWindow::mouseMoveEvent(mouseEvent);
    }
}

void MainWindow::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) {
    QPointF endPoint = mouseEvent ->scenePos();

    if (line != 0 && myMode == InsertLine) {
        QList<QGraphicsItem *> startItems = scene ->items(line ->line().p1());
        if (startItems.count() && startItems.first() == line)
            startItems.removeFirst();
        QList<QGraphicsItem *> endItems = scene ->items(line ->line().p2());
        if (endItems.count() && endItems.first() == line)
            endItems.removeFirst();

        scene ->removeItem(line);
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
            scene ->addItem(arrow);
            arrow ->updatePosition();
        }
        else {
            Arrow *arrow = new Arrow(startPoint, endPoint);
            arrow ->setColor(myLineColor);
            arrow ->setZValue(-1000.0);
            scene ->addItem(arrow);
            arrow ->updatePosition();

        }
    }
    line = 0;
    MainWindow::mouseReleaseEvent(mouseEvent);
}
