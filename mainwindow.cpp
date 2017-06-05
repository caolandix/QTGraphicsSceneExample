#include <QtWidgets>

#include "physparticle.h"
#include "arrow.h"
#include "diagramitem.h"
#include "diagramtextitem.h"
#include "mainwindow.h"


const int InsertTextButton = 10;

MainWindow::MainWindow() {
    createActions();
    createToolBox();
    createMenus();

    scene = new QGraphicsScene(this);
    scene ->setSceneRect(QRectF(0, 0, 640, 480));
    createToolbars();

    view = new PhysGraphicsView(itemMenu, scene, this);
    connect(view, SIGNAL(itemInserted(DiagramItem *)), this, SLOT(onItemInserted(DiagramItem *)));
    connect(view, SIGNAL(textInserted(QGraphicsTextItem *)), this, SLOT(onTextInserted(QGraphicsTextItem *)));
    connect(view, SIGNAL(itemSelected(QGraphicsItem *)), this, SLOT(onItemSelected(QGraphicsItem *)));

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
        scene ->setBackgroundBrush(QPixmap("images/background1.png"));
    else if (text == tr("White Grid"))
        scene ->setBackgroundBrush(QPixmap("images/background2.png"));
    else if (text == tr("Gray Grid"))
        scene ->setBackgroundBrush(QPixmap("images/background3.png"));
    else
        scene ->setBackgroundBrush(QPixmap("images/background4.png"));

    scene ->update();
    view ->update();
}

void MainWindow::buttonGroupClicked(int id) {
    QList<QAbstractButton *> buttons = buttonGroup ->buttons();
    foreach (QAbstractButton *button, buttons) {
        if (buttonGroup ->button(id) != button)
            button ->setChecked(false);
    }
    switch (id) {
    case InsertTextButton:
        view ->setMode(PhysGraphicsView::InsertText);
        break;
    default:
        view ->setItemType(DiagramItem::DiagramType(id));
        view ->setMode(PhysGraphicsView::InsertItem);
        break;
    }
}

void MainWindow::deleteItem() {
    foreach (QGraphicsItem *pItem, scene ->selectedItems()) {
        if (pItem ->type() == Arrow::Type) {
            scene ->removeItem(pItem);
            Arrow *pArrow = qgraphicsitem_cast<Arrow *>(pItem);
            if (pArrow ->startItem())
                pArrow ->startItem() ->removeArrow(pArrow);
            if (pArrow ->endItem())
                pArrow ->endItem() ->removeArrow(pArrow);
            delete pItem;
        }
    }

    foreach (QGraphicsItem *pItem, scene ->selectedItems()) {
         if (pItem ->type() == DiagramItem::Type)
             qgraphicsitem_cast<DiagramItem *>(pItem) ->removeArrows();
         scene ->removeItem(pItem);
         delete pItem;
     }
}

void MainWindow::pointerGroupClicked(int) {
    view ->setMode(PhysGraphicsView::Mode(pointerTypeGroup->checkedId()));
}

void MainWindow::bringToFront() {
    if (scene ->selectedItems().isEmpty())
        return;

    QGraphicsItem *pSelectedItem = scene ->selectedItems().first();
    QList<QGraphicsItem *> overlapItems = pSelectedItem ->collidingItems();

    qreal zValue = 0;
    foreach (QGraphicsItem *pItem, overlapItems) {
        if (pItem ->zValue() >= zValue && pItem ->type() == DiagramItem::Type)
            zValue = pItem ->zValue() + 0.1;
    }
    pSelectedItem ->setZValue(zValue);
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
    pointerTypeGroup ->button(int(PhysGraphicsView::MoveItem)) ->setChecked(true);
    view ->setMode(PhysGraphicsView::Mode(pointerTypeGroup ->checkedId()));
    buttonGroup ->button(int(item ->diagramType())) ->setChecked(false);
}

void MainWindow::onTextInserted(QGraphicsTextItem *) {
    buttonGroup ->button(InsertTextButton) ->setChecked(false);
    view ->setMode(PhysGraphicsView::Mode(pointerTypeGroup ->checkedId()));
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
    fontColorToolButton ->setIcon(createColorToolButtonIcon("images/textpointer.png", qvariant_cast<QColor>(textAction ->data())));
    textButtonTriggered();
}

void MainWindow::itemColorChanged() {
    fillAction = qobject_cast<QAction *>(sender());
    fillColorToolButton ->setIcon(createColorToolButtonIcon("images/floodfill.png", qvariant_cast<QColor>(fillAction ->data())));
    fillButtonTriggered();
}

void MainWindow::lineColorChanged() {
    lineAction = qobject_cast<QAction *>(sender());
    lineColorToolButton ->setIcon(createColorToolButtonIcon("images/linecolor.png", qvariant_cast<QColor>(lineAction ->data())));
    lineButtonTriggered();
}

bool MainWindow::isItemChange(int type) {
    foreach (QGraphicsItem *item, scene ->selectedItems()) {
        if (item ->type() == type)
            return true;
    }
    return false;
}


void MainWindow::textButtonTriggered() {
    view ->setTextColor(qvariant_cast<QColor>(textAction ->data()));
}

void MainWindow::fillButtonTriggered() {
    view ->setItemColor(qvariant_cast<QColor>(fillAction ->data()));
}

void MainWindow::lineButtonTriggered() {
    view ->setLineColor(qvariant_cast<QColor>(lineAction ->data()));
}

void MainWindow::handleFontChange() {
    QFont font = fontCombo ->currentFont();
    font.setPointSize(fontSizeCombo ->currentText().toInt());
    font.setWeight(boldAction ->isChecked() ? QFont::Bold : QFont::Normal);
    font.setItalic(italicAction ->isChecked());
    font.setUnderline(underlineAction ->isChecked());

    scene->setFont(font);
}

void MainWindow::onItemSelected(QGraphicsItem *item) {
    DiagramTextItem *textItem = qgraphicsitem_cast<DiagramTextItem *>(item);

    QFont font = textItem->font();
    fontCombo ->setCurrentFont(font);
    fontSizeCombo ->setEditText(QString().setNum(font.pointSize()));
    boldAction ->setChecked(font.weight() == QFont::Bold);
    italicAction ->setChecked(font.italic());
    underlineAction ->setChecked(font.underline());
}

void MainWindow::about() {
    QMessageBox::about(this, tr("About Diagram Scene"), tr("The <b>Diagram Scene</b> example shows use of the graphics framework."));
}

void MainWindow::createToolBox() {
    buttonGroup = new QButtonGroup(this);
    buttonGroup ->setExclusive(false);
    connect(buttonGroup, SIGNAL(buttonClicked(int)), this, SLOT(buttonGroupClicked(int)));
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(createCellWidget(tr("Conditional"), DiagramItem::Conditional), 0, 0);
    layout->addWidget(createCellWidget(tr("Process"), DiagramItem::Step),0, 1);
    layout->addWidget(createCellWidget(tr("Input/Output"), DiagramItem::Io), 1, 0);

    QToolButton *textButton = new QToolButton;
    textButton ->setCheckable(true);
    buttonGroup ->addButton(textButton, InsertTextButton);
    textButton ->setIcon(QIcon(QPixmap("images/textpointer.png")));
    textButton ->setIconSize(QSize(50, 50));
    QGridLayout *textLayout = new QGridLayout;
    textLayout ->addWidget(textButton, 0, 0, Qt::AlignHCenter);
    textLayout ->addWidget(new QLabel(tr("Text")), 1, 0, Qt::AlignCenter);
    QWidget *textWidget = new QWidget;
    textWidget ->setLayout(textLayout);
    layout ->addWidget(textWidget, 1, 1);

    layout ->setRowStretch(3, 10);
    layout ->setColumnStretch(2, 10);

    QWidget *itemWidget = new QWidget;
    itemWidget ->setLayout(layout);

    backgroundButtonGroup = new QButtonGroup(this);
    connect(backgroundButtonGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(backgroundButtonGroupClicked(QAbstractButton*)));

    QGridLayout *backgroundLayout = new QGridLayout;
    backgroundLayout ->addWidget(createBackgroundCellWidget(tr("Blue Grid"), "images/background1.png"), 0, 0);
    backgroundLayout ->addWidget(createBackgroundCellWidget(tr("White Grid"), "images/background2.png"), 0, 1);
    backgroundLayout ->addWidget(createBackgroundCellWidget(tr("Gray Grid"), "images/background3.png"), 1, 0);
    backgroundLayout ->addWidget(createBackgroundCellWidget(tr("No Grid"), "images/background4.png"), 1, 1);

    backgroundLayout ->setRowStretch(2, 10);
    backgroundLayout ->setColumnStretch(2, 10);

    QWidget *backgroundWidget = new QWidget;
    backgroundWidget ->setLayout(backgroundLayout);

    toolBox = new QToolBox;
    toolBox ->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Ignored));
    toolBox ->setMinimumWidth(itemWidget->sizeHint().width());
    toolBox ->addItem(itemWidget, tr("Basic Flowchart Shapes"));
    toolBox ->addItem(backgroundWidget, tr("Backgrounds"));
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
    italicAction->setCheckable(true);
    italicAction->setShortcut(tr("Ctrl+I"));
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
    fileMenu = menuBar() ->addMenu(tr("&File"));
    fileMenu ->addAction(exitAction);

    itemMenu = menuBar()->addMenu(tr("&Item"));
    itemMenu ->addAction(deleteAction);
    itemMenu ->addSeparator();
    itemMenu ->addAction(toFrontAction);
    itemMenu ->addAction(sendBackAction);

    aboutMenu = menuBar()->addMenu(tr("&Help"));
    aboutMenu ->addAction(aboutAction);
}

void MainWindow::createToolbars() {
    editToolBar = addToolBar(tr("Edit"));
    editToolBar ->addAction(deleteAction);
    editToolBar ->addAction(toFrontAction);
    editToolBar ->addAction(sendBackAction);

    fontCombo = new QFontComboBox();
    connect(fontCombo, SIGNAL(currentFontChanged(QFont)), this, SLOT(currentFontChanged(QFont)));

    fontSizeCombo = new QComboBox;
    fontSizeCombo ->setEditable(true);
    for (int i = 8; i < 30; i = i + 2)
        fontSizeCombo->addItem(QString().setNum(i));
    QIntValidator *validator = new QIntValidator(2, 64, this);
    fontSizeCombo->setValidator(validator);
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
    lineAction = lineColorToolButton ->menu() ->defaultAction();
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

    // Pointer Type toolbar
    pointerToolbar = addToolBar(tr("Pointer type"));

    QToolButton *pointerButton = new QToolButton;
    pointerButton ->setCheckable(true);
    pointerButton ->setChecked(true);
    pointerButton ->setIcon(QIcon("images/pointer.png"));
    pointerToolbar ->addWidget(pointerButton);

    QToolButton *linePointerButton = new QToolButton;
    linePointerButton ->setCheckable(true);
    linePointerButton ->setIcon(QIcon("images/vector.png"));
    pointerToolbar ->addWidget(linePointerButton);

    QToolButton *particleButton = new QToolButton;
    particleButton ->setCheckable(true);
    particleButton ->setIcon(QIcon("images/particle.png"));
    pointerToolbar ->addWidget(particleButton);

    pointerTypeGroup = new QButtonGroup(this);
    pointerTypeGroup ->addButton(pointerButton, int(PhysGraphicsView::MoveItem));
    pointerTypeGroup ->addButton(linePointerButton, int(PhysGraphicsView::InsertLine));
    pointerTypeGroup ->addButton(particleButton, int(PhysGraphicsView::InsertParticle));
    connect(pointerTypeGroup, SIGNAL(buttonClicked(int)), this, SLOT(pointerGroupClicked(int)));

    sceneScaleCombo = new QComboBox;
    QStringList scales;
    scales << tr("50%") << tr("75%") << tr("100%") << tr("125%") << tr("150%");
    sceneScaleCombo ->addItems(scales);
    sceneScaleCombo ->setCurrentIndex(2);
    connect(sceneScaleCombo, SIGNAL(currentIndexChanged(QString)), this, SLOT(sceneScaleChanged(QString)));

    pointerToolbar ->addWidget(sceneScaleCombo);
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
