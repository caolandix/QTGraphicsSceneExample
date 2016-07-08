#include "arrow.h"
#include "diagramitem.h"
#include "physgraphicsscene.h"
#include "diagramtextitem.h"
#include "mainwindow.h"

#include <QtWidgets>

const int InsertTextButton = 10;

MainWindow::MainWindow() {
    createActions();
    createToolBox();
    createMenus();

    m_pScene = new PhysGraphicsScene(itemMenu, this);
    m_pScene ->setSceneRect(QRectF(0, 0, 640, 480));
    connect(m_pScene, SIGNAL(itemInserted(DiagramItem *)), this, SLOT(itemInserted(DiagramItem *)));
    connect(m_pScene, SIGNAL(textInserted(QGraphicsTextItem *)), this, SLOT(textInserted(QGraphicsTextItem *)));
    connect(m_pScene, SIGNAL(itemSelected(QGraphicsItem *)), this, SLOT(itemSelected(QGraphicsItem *)));
    createToolbars();

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(toolBox);
    m_pView = new QGraphicsView(m_pScene);
    layout ->addWidget(m_pView);

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
        m_pScene ->setBackgroundBrush(QPixmap("images/background1.png"));
    else if (text == tr("White Grid"))
        m_pScene ->setBackgroundBrush(QPixmap("images/background2.png"));
    else if (text == tr("Gray Grid"))
        m_pScene ->setBackgroundBrush(QPixmap("images/background3.png"));
    else
        m_pScene ->setBackgroundBrush(QPixmap("images/background4.png"));

    m_pScene ->update();
    m_pView->update();
}

void MainWindow::buttonGroupClicked(int id) {
    QList<QAbstractButton *> buttons = buttonGroup ->buttons();
    foreach (QAbstractButton *button, buttons) {
        if (buttonGroup ->button(id) != button)
            button ->setChecked(false);
    }
    if (id == InsertTextButton) {
        m_pScene ->setMode(PhysGraphicsScene::InsertText);
    }
    else {
        m_pScene ->setItemType(DiagramItem::DiagramType(id));
        m_pScene ->setMode(PhysGraphicsScene::InsertItem);
    }
}

void MainWindow::deleteItem() {
    foreach (QGraphicsItem *item, m_pScene ->selectedItems()) {
        if (item->type() == Arrow::Type) {
            m_pScene ->removeItem(item);
            Arrow *arrow = qgraphicsitem_cast<Arrow *>(item);
            arrow ->startItem() ->removeArrow(arrow);
            arrow ->endItem() ->removeArrow(arrow);
            delete item;
        }
    }

    foreach (QGraphicsItem *item, m_pScene ->selectedItems()) {
         if (item ->type() == DiagramItem::Type)
             qgraphicsitem_cast<DiagramItem *>(item) ->removeArrows();
         m_pScene ->removeItem(item);
         delete item;
     }
}

void MainWindow::pointerGroupClicked(int) {
    m_pScene ->setMode(PhysGraphicsScene::Mode(pointerTypeGroup->checkedId()));
}

void MainWindow::bringToFront() {
    if (m_pScene ->selectedItems().isEmpty())
        return;

    QGraphicsItem *selectedItem = m_pScene ->selectedItems().first();
    QList<QGraphicsItem *> overlapItems = selectedItem ->collidingItems();

    qreal zValue = 0;
    foreach (QGraphicsItem *item, overlapItems) {
        if (item ->zValue() >= zValue && item ->type() == DiagramItem::Type)
            zValue = item ->zValue() + 0.1;
    }
    selectedItem ->setZValue(zValue);
}

void MainWindow::sendToBack() {
    if (m_pScene ->selectedItems().isEmpty())
        return;

    QGraphicsItem *selectedItem = m_pScene ->selectedItems().first();
    QList<QGraphicsItem *> overlapItems = selectedItem ->collidingItems();

    qreal zValue = 0;
    foreach (QGraphicsItem *item, overlapItems) {
        if (item ->zValue() <= zValue && item ->type() == DiagramItem::Type)
            zValue = item ->zValue() - 0.1;
    }
    selectedItem ->setZValue(zValue);
}

void MainWindow::itemInserted(DiagramItem *item) {
    pointerTypeGroup ->button(int(PhysGraphicsScene::MoveItem)) ->setChecked(true);
    m_pScene ->setMode(PhysGraphicsScene::Mode(pointerTypeGroup ->checkedId()));
    buttonGroup ->button(int(item ->diagramType())) ->setChecked(false);
}

void MainWindow::textInserted(QGraphicsTextItem *) {
    buttonGroup ->button(InsertTextButton) ->setChecked(false);
    m_pScene ->setMode(PhysGraphicsScene::Mode(pointerTypeGroup ->checkedId()));
}



void MainWindow::currentFontChanged(const QFont &) {
    handleFontChange();
}

void MainWindow::fontSizeChanged(const QString &) {
    handleFontChange();
}

void MainWindow::sceneScaleChanged(const QString &scale) {
    double newScale = scale.left(scale.indexOf(tr("%"))).toDouble() / 100.0;
    QMatrix oldMatrix = m_pView ->matrix();
    m_pView ->resetMatrix();
    m_pView ->translate(oldMatrix.dx(), oldMatrix.dy());
    m_pView ->scale(newScale, newScale);
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

void MainWindow::textButtonTriggered() {
    m_pScene ->setTextColor(qvariant_cast<QColor>(textAction->data()));
}

void MainWindow::fillButtonTriggered() {
    m_pScene ->setItemColor(qvariant_cast<QColor>(fillAction->data()));
}

void MainWindow::lineButtonTriggered() {
    m_pScene ->setLineColor(qvariant_cast<QColor>(lineAction->data()));
}

void MainWindow::handleFontChange() {
    QFont font = fontCombo->currentFont();
    font.setPointSize(fontSizeCombo->currentText().toInt());
    font.setWeight(boldAction->isChecked() ? QFont::Bold : QFont::Normal);
    font.setItalic(italicAction->isChecked());
    font.setUnderline(underlineAction->isChecked());

    m_pScene ->setFont(font);
}

void MainWindow::itemSelected(QGraphicsItem *item) {
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
    buttonGroup ->setExclusive(false);
    connect(buttonGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(buttonGroupClicked(int)));
    QGridLayout *pLayout = new QGridLayout;
    pLayout ->addWidget(createCellWidget(tr("Conditional"), DiagramItem::Conditional), 0, 0);
    pLayout ->addWidget(createCellWidget(tr("Process"), DiagramItem::Step),0, 1);
    pLayout ->addWidget(createCellWidget(tr("Input/Output"), DiagramItem::Io), 1, 0);

    QToolButton *pTextButton = new QToolButton;
    pTextButton ->setCheckable(true);
    buttonGroup ->addButton(pTextButton, InsertTextButton);
    pTextButton ->setIcon(QIcon(QPixmap("images/textpointer.png")));
    pTextButton ->setIconSize(QSize(50, 50));

    QGridLayout *pTextLayout = new QGridLayout;
    pTextLayout ->addWidget(pTextButton, 0, 0, Qt::AlignHCenter);
    pTextLayout ->addWidget(new QLabel(tr("Text")), 1, 0, Qt::AlignCenter);

    QWidget *pTextWidget = new QWidget;
    pTextWidget ->setLayout(pTextLayout);
    pLayout ->addWidget(pTextWidget, 1, 1);

    pLayout ->setRowStretch(3, 10);
    pLayout ->setColumnStretch(2, 10);

    QWidget *pItemWidget = new QWidget;
    pItemWidget ->setLayout(pLayout);

    backgroundButtonGroup = new QButtonGroup(this);
    connect(backgroundButtonGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(backgroundButtonGroupClicked(QAbstractButton*)));

    QGridLayout *pBackgroundLayout = new QGridLayout;
    pBackgroundLayout ->addWidget(createBackgroundCellWidget(tr("Blue Grid"), "images/background1.png"), 0, 0);
    pBackgroundLayout ->addWidget(createBackgroundCellWidget(tr("White Grid"), "images/background2.png"), 0, 1);
    pBackgroundLayout ->addWidget(createBackgroundCellWidget(tr("Gray Grid"), "images/background3.png"), 1, 0);
    pBackgroundLayout ->addWidget(createBackgroundCellWidget(tr("No Grid"), "images/background4.png"), 1, 1);

    pBackgroundLayout ->setRowStretch(2, 10);
    pBackgroundLayout ->setColumnStretch(2, 10);

    QWidget *pBackgroundWidget = new QWidget;
    pBackgroundWidget -> setLayout(pBackgroundLayout);

    toolBox = new QToolBox;
    toolBox ->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Ignored));
    toolBox ->setMinimumWidth(pItemWidget ->sizeHint().width());
    toolBox ->addItem(pItemWidget, tr("Basic Flowchart Shapes"));
    toolBox ->addItem(pBackgroundWidget, tr("Backgrounds"));
}

void MainWindow::createActions() {
    toFrontAction = new QAction(QIcon("images/bringtofront.png"), tr("Bring to &Front"), this);
    toFrontAction ->setShortcut(tr("Ctrl+F"));
    toFrontAction ->setStatusTip(tr("Bring item to front"));
    connect(toFrontAction, SIGNAL(triggered()), this, SLOT(bringToFront()));

    sendBackAction = new QAction(QIcon("images/sendtoback.png"), tr("Send to &Back"), this);
    sendBackAction ->setShortcut(tr("Ctrl+T"));
    sendBackAction ->setStatusTip(tr("Send item to back"));
    connect(sendBackAction, SIGNAL(triggered()), this, SLOT(sendToBack()));

    deleteAction = new QAction(QIcon("images/delete.png"), tr("&Delete"), this);
    deleteAction ->setShortcut(tr("Delete"));
    deleteAction ->setStatusTip(tr("Delete item from diagram"));
    connect(deleteAction, SIGNAL(triggered()), this, SLOT(deleteItem()));

    exitAction = new QAction(tr("E&xit"), this);
    exitAction ->setShortcuts(QKeySequence::Quit);
    exitAction ->setStatusTip(tr("Quit Scenediagram example"));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

    boldAction = new QAction(tr("Bold"), this);
    boldAction ->setCheckable(true);
    QPixmap pixmap(":/images/bold.png");
    boldAction ->setIcon(QIcon(pixmap));
    boldAction ->setShortcut(tr("Ctrl+B"));
    connect(boldAction, SIGNAL(triggered()), this, SLOT(handleFontChange()));

    italicAction = new QAction(QIcon("images/italic.png"), tr("Italic"), this);
    italicAction ->setCheckable(true);
    italicAction ->setShortcut(tr("Ctrl+I"));
    connect(italicAction, SIGNAL(triggered()), this, SLOT(handleFontChange()));

    underlineAction = new QAction(QIcon("images/underline.png"), tr("Underline"), this);
    underlineAction ->setCheckable(true);
    underlineAction ->setShortcut(tr("Ctrl+U"));
    connect(underlineAction, SIGNAL(triggered()), this, SLOT(handleFontChange()));

    aboutAction = new QAction(tr("A&bout"), this);
    aboutAction ->setShortcut(tr("F1"));
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
    fontSizeCombo ->setEditable(true);
    for (int i = 8; i < 30; i = i + 2)
        fontSizeCombo ->addItem(QString().setNum(i));
    QIntValidator *pValidator = new QIntValidator(2, 64, this);
    fontSizeCombo ->setValidator(pValidator);
    connect(fontSizeCombo, SIGNAL(currentIndexChanged(QString)), this, SLOT(fontSizeChanged(QString)));

    fontColorToolButton = new QToolButton;
    fontColorToolButton ->setPopupMode(QToolButton::MenuButtonPopup);
    fontColorToolButton ->setMenu(createColorMenu(SLOT(textColorChanged()), Qt::black));
    textAction = fontColorToolButton ->menu() ->defaultAction();
    fontColorToolButton ->setIcon(createColorToolButtonIcon("images/textpointer.png", Qt::black));
    fontColorToolButton ->setAutoFillBackground(true);
    connect(fontColorToolButton, SIGNAL(clicked()), this, SLOT(textButtonTriggered()));

    fillColorToolButton = new QToolButton;
    fillColorToolButton ->setPopupMode(QToolButton::MenuButtonPopup);
    fillColorToolButton ->setMenu(createColorMenu(SLOT(itemColorChanged()), Qt::white));
    fillAction = fillColorToolButton ->menu() ->defaultAction();
    fillColorToolButton ->setIcon(createColorToolButtonIcon("images/floodfill.png", Qt::white));
    connect(fillColorToolButton, SIGNAL(clicked()), this, SLOT(fillButtonTriggered()));

    lineColorToolButton = new QToolButton;
    lineColorToolButton ->setPopupMode(QToolButton::MenuButtonPopup);
    lineColorToolButton ->setMenu(createColorMenu(SLOT(lineColorChanged()), Qt::black));
    lineAction = lineColorToolButton->menu()->defaultAction();
    lineColorToolButton ->setIcon(createColorToolButtonIcon("images/linecolor.png", Qt::black));
    connect(lineColorToolButton, SIGNAL(clicked()), this, SLOT(lineButtonTriggered()));

    textToolBar = addToolBar(tr("Font"));
    textToolBar ->addWidget(fontCombo);
    textToolBar ->addWidget(fontSizeCombo);
    textToolBar ->addAction(boldAction);
    textToolBar ->addAction(italicAction);
    textToolBar ->addAction(underlineAction);

    colorToolBar = addToolBar(tr("Color"));
    colorToolBar ->addWidget(fontColorToolButton);
    colorToolBar ->addWidget(fillColorToolButton);
    colorToolBar ->addWidget(lineColorToolButton);

    QToolButton *pPointerButton = new QToolButton;
    pPointerButton ->setCheckable(true);
    pPointerButton ->setChecked(true);
    pPointerButton ->setIcon(QIcon("images/pointer.png"));
    QToolButton *pLinePointerButton = new QToolButton;
    pLinePointerButton ->setCheckable(true);
    pLinePointerButton ->setIcon(QIcon("images/linepointer.png"));

    pointerTypeGroup = new QButtonGroup(this);
    pointerTypeGroup ->addButton(pPointerButton, int(PhysGraphicsScene::MoveItem));
    pointerTypeGroup ->addButton(pLinePointerButton, int(PhysGraphicsScene::InsertLine));
    connect(pointerTypeGroup, SIGNAL(buttonClicked(int)), this, SLOT(pointerGroupClicked(int)));

    sceneScaleCombo = new QComboBox;
    QStringList scales;
    scales << tr("50%") << tr("75%") << tr("100%") << tr("125%") << tr("150%");
    sceneScaleCombo ->addItems(scales);
    sceneScaleCombo ->setCurrentIndex(2);
    connect(sceneScaleCombo, SIGNAL(currentIndexChanged(QString)), this, SLOT(sceneScaleChanged(QString)));

    pointerToolbar = addToolBar(tr("Pointer type"));
    pointerToolbar ->addWidget(pPointerButton);
    pointerToolbar ->addWidget(pLinePointerButton);
    pointerToolbar ->addWidget(sceneScaleCombo);
}

QWidget *MainWindow::createBackgroundCellWidget(const QString &text, const QString &image) {
    QToolButton *pButton = new QToolButton;
    pButton ->setText(text);
    pButton ->setIcon(QIcon(image));
    pButton ->setIconSize(QSize(50, 50));
    pButton ->setCheckable(true);
    backgroundButtonGroup ->addButton(pButton);

    QGridLayout *pLayout = new QGridLayout;
    pLayout ->addWidget(pButton, 0, 0, Qt::AlignHCenter);
    pLayout ->addWidget(new QLabel(text), 1, 0, Qt::AlignCenter);

    QWidget *pWidget = new QWidget;
    pWidget ->setLayout(pLayout);

    return pWidget;
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
