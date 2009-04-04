/*
 * Copyright (C) 2009 Erik Bunce <elbunce@xendom.com>
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met:
 * 1. Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright 
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder(s) nor the names of any 
 *    contributors may be used to endorse or promote products derived from 
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*----------------------------------------------------------------------
 * Application Headers
 *----------------------------------------------------------------------*/
#include "HtmlEditMainWindow.h"
#include "HtmlEditSourceView.h"
#include "WebActionView.h"

/*----------------------------------------------------------------------
 * System Headers
 *----------------------------------------------------------------------*/
#include <qwebview.h>
#include <qwebpage.h>
#include <qwebframe.h>
#include <QProgressBar>
#include <QStatusBar>
#include <QFileDialog>
#include <QLabel>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QToolTip>
#include <QFontComboBox>
#include <QStringList>
#include <QVariant>
#include <QMessageBox>
#include <QInputDialog>
#include <QColorDialog>
#include <QFile>
#include <QSettings>
#include <QDockWidget>
#include <QScrollArea>
#include <QApplication>
#include <QtDebug>

/*----------------------------------------------------------------------
 * Feature Test Switches
 *----------------------------------------------------------------------*/
//#define DEBUG_WEBEDITMAINWINDOW_JS
//#define DEBUG_WEBEDITMAINWINDOW_FONTCHANGE

/*----------------------------------------------------------------------
 * Contants
 *----------------------------------------------------------------------*/
static QRegExp rgbaExpression("rgba\\(\\s*(\\d+)\\s*,\\s*(\\d+)\\s*,\\s*(\\d+)\\s*,\\s*(\\d+)\\s*\\)");
static QRegExp rgbExpression("rgb\\(\\s*(\\d+)\\s*,\\s*(\\d+)\\s*,\\s*(\\d+)\\s*\\)");

/*----------------------------------------------------------------------
 * Main Code
 *----------------------------------------------------------------------*/
HtmlEditMainWindow::HtmlEditMainWindow(QWidget* parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags),
      m_view(0),
      m_progress(0),
      m_editableAction(0),
      m_boldAction(0),
      m_dumpPlainTextAction(0),
      m_dumpHtmlAction(0),
      m_dumpRenderTreeAction(0),
      m_paraMenu(0),
      m_fontComboBox(0),
      m_sizeMenu(0)
{
    // our filters
    m_filters = tr("HTML Files (*.html *.htm);; XHTML Files (*.xhtml *.xhtm);; XML Files (*.xml);; Text Files (*.txt);; All Files (*.*)");

    setupUI();
}

HtmlEditMainWindow::~HtmlEditMainWindow()
{
}

QVariant HtmlEditMainWindow::evalJavaScript(const QString& script)
{
    QWebFrame* currentFrame = m_view->page()->currentFrame();

#ifdef DEBUG_WEBEDITMAINWINDOW_JS
    qDebug() << "evalJavaScript(" << script << ")";
#endif

    QVariant res = currentFrame->evaluateJavaScript(script);

#ifdef DEBUG_WEBEDITMAINWINDOW_JS
    qDebug() << "\tres=" << res;
#endif

    return res;
}

void HtmlEditMainWindow::newWindow()
{
    // create / show a new edit window
    HtmlEditMainWindow* mw = new HtmlEditMainWindow;
    mw->show();
}

void HtmlEditMainWindow::openLocation()
{
    QSettings settings;
    // keep track of the recently visited URL's
    QStringList recents = settings.value("/urls/recent").toStringList();
    
    bool ok;
    QString urlStr = 
        QInputDialog::getItem(this, tr("Open URL"),
                              tr("Please type the URL you wish to load"), 
                              recents, 0, true, &ok);

    // nothing to see here, move along
    if (!ok || urlStr.isEmpty())
        return;

    QUrl url(urlStr, QUrl::TolerantMode);
    if (!url.isValid()) {
        qWarning() << "URL isn't valid: " << urlStr;
        statusBar()->showMessage(tr("URL '%1' is not valid!").arg(urlStr));
        return;
    }

    // replace with the parsed / cleaned version
    urlStr = url.toString();

    // remove the text of the URL from recents, then prepend it
    recents.removeAll(urlStr);
    recents.prepend(urlStr);
    
    // now prune the list down to the last 10
    while (recents.count() > 10)
        recents.removeLast();

    // now save it for next time
    settings.setValue("/urls/recent", recents);

    // now load the URL
    m_view->load(url);
}

void HtmlEditMainWindow::openFile()
{
    QString fileName = 
        QFileDialog::getOpenFileName(this, tr("Open File"), QString(), m_filters);

    if (fileName.isEmpty())
        return;
    
    m_view->load(QUrl::fromLocalFile(fileName));
    m_view->setFocus(Qt::OtherFocusReason);
}

void HtmlEditMainWindow::openDefaultHtml()
{
    m_view->load(QUrl("qrc:/defaults/clean.html"));
    m_view->setFocus(Qt::OtherFocusReason);
}

void HtmlEditMainWindow::openDefaultXHtml()
{
    m_view->load(QUrl("qrc:/defaults/clean.xhtml"));
    m_view->setFocus(Qt::OtherFocusReason);
}

void HtmlEditMainWindow::saveFile()
{
    // get the filename
    QString fileName =
        QFileDialog::getSaveFileName(this, tr("Save File"), QString(), m_filters);

    if (fileName.isEmpty())
        return;

    // attempt to open the file for write
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Truncate)) {
        QString message = tr("Failed to open %1 for write: %2").arg(file.fileName()).arg(file.errorString());
        qWarning() << message;
        QMessageBox::warning(this, tr("Save Failed"), message);
        return;
    }

    // get the content
    QString content = m_view->page()->mainFrame()->toHtml();

    // write the content to the file in UTF8
    file.write(content.toUtf8());
    file.close();
}

void HtmlEditMainWindow::dumpPlainText()
{
    qDebug() << "Plain Text: " << m_view->page()->mainFrame()->toPlainText();
}

void HtmlEditMainWindow::dumpHtml()
{
    qDebug() << "HTML: " << m_view->page()->mainFrame()->toHtml();
}

void HtmlEditMainWindow::dumpRenderTree()
{
    qDebug() << "Render Tree: " << m_view->page()->mainFrame()->renderTreeDump();
}

void HtmlEditMainWindow::about()
{
    QString html;
    QFile file(":/defaults/about.html");
    if (file.open(QFile::ReadOnly))
        html = file.readAll();
    else
    {
        qWarning() << "Failed to open " << file.fileName() << " error:" 
                   << file.errorString();
        html = "About %1\nVersion: %2\nBuild: %3 %4";
    }
    
    QMessageBox::about(this,
                       tr("About %1")
                       .arg(QApplication::applicationName()), 
                       html.arg(QApplication::applicationName())
                       .arg(QApplication::applicationVersion())
                       .arg(GITVERSION).arg(GITCHANGENUMBER));
}

void HtmlEditMainWindow::setEditable(bool on)
{
    m_view->page()->setContentEditable(on);
}

void HtmlEditMainWindow::handleTitleChanged(const QString& title)
{
    if (!title.isEmpty())
        setWindowTitle(tr("%1 - %2").arg(QApplication::applicationName())
                       .arg(title));
    else 
        setWindowTitle(tr("%1").arg(QApplication::applicationName()));
}

void HtmlEditMainWindow::handleLinkHover(const QString &link, const QString &toolTip)
{
    statusBar()->showMessage(link);

#ifndef QT_NO_TOOLTIP
    if (!toolTip.isEmpty())
        QToolTip::showText(QCursor::pos(), toolTip);
#endif
}

void HtmlEditMainWindow::handleLoadFinished()
{
    qDebug("HtmlEditMainWindow::handleLoadFinished()");
    // make sure to preserve the editable state
    m_view->page()->setContentEditable(m_editableAction->isChecked());

    // make sure to give the view focus
    m_view->setFocus();
}

void HtmlEditMainWindow::handlePageContentsChanged()
{
    if (m_boldAction->isEnabled()) {
        // attempt to get the current font
        QVariant res = evalJavaScript("document.queryCommandValue('FontName');");
        if (res.isValid()) {
            // 'fix' the font name
            QString fontName = res.toString();
            fontName.remove(QChar('\''));
            
            m_fontComboBox->setCurrentFont(QFont(fontName));
        }
    }
}

void HtmlEditMainWindow::handlePageActionChanged()
{
    bool enable = m_boldAction->isEnabled();
    m_paraMenu->setEnabled(enable);
    m_fontComboBox->setEnabled(enable);
    m_sizeMenu->setEnabled(enable);
}

void HtmlEditMainWindow::handleExecCommandAction(QAction* action)
{
    QStringList sl = action->data().toStringList();
		Q_ASSERT(sl.count() >= 2);
    if (sl.count() < 2)
        return;

    evalJavaScript(QString("document.execCommand('%1', false, '%2');")
                   .arg(sl[0]).arg(sl[1]));
}

void HtmlEditMainWindow::handleColorChange()
{
    QAction* action = ::qobject_cast<QAction*>(sender());
    Q_ASSERT(action);
    if (!action)
        return;
    
    // figure out which color we're getting
    QString whichColor = action->data().toString();
    Q_ASSERT(!whichColor.isEmpty());
    if (whichColor.isEmpty())
        return;
    
    QVariant res = evalJavaScript(QString("document.queryCommandValue('%1');").arg(whichColor));
    QColor color = toColor(res.toString());
    
    color = QColorDialog::getColor(color, m_view);
    
    if (color.isValid())
        evalJavaScript(QString("document.execCommand('%1', false, '%2');").arg(whichColor).arg(color.name()));
}

void HtmlEditMainWindow::handleFontChanged(const QFont& font)
{
#ifdef DEBUG_WEBEDITMAINWINDOW_FONTCHANGE
    qDebug() << "HtmlEditMainWindow::handleFontChanged(" << font << ")";
#endif
    evalJavaScript(QString("document.execCommand('FontName', false, '%1');").arg(font.family()));
}

void HtmlEditMainWindow::handleViewAboutToShow()
{
    m_viewMenu->clear();
    m_viewMenu->addAction(m_view->pageAction(QWebPage::InspectElement));
    QMenu* menu = createPopupMenu();
    if (menu) {
        m_viewMenu->addSeparator();
        m_viewMenu->addActions(menu->actions());
        delete menu;
    }

    m_viewMenu->addSeparator();
    m_viewMenu->addAction(m_dumpPlainTextAction);
    m_viewMenu->addAction(m_dumpHtmlAction);
    m_viewMenu->addAction(m_dumpRenderTreeAction);

}

QColor HtmlEditMainWindow::toColor(const QString& rgba)
{
    // try to match rgba(r, g, b, a)
    if (rgbaExpression.exactMatch(rgba)) {
        QStringList ct = rgbaExpression.capturedTexts();
        return QColor(ct.value(1).toInt(), ct.value(2).toInt(), ct.value(3).toInt(), ct.value(4).toInt());
    }
    
    // try to match rgb(r, g, b)
    if (rgbExpression.exactMatch(rgba)) {
        QStringList ct = rgbExpression.capturedTexts();
        return QColor(ct.value(1).toInt(), ct.value(2).toInt(), ct.value(3).toInt());
    }
    
    // convert from name
    return QColor(rgba);
}

void HtmlEditMainWindow::setupUI()
{
    // Set base UI attributes
    setWindowTitle(tr("%1").arg(QApplication::applicationName()));
    setUnifiedTitleAndToolBarOnMac(true);
    setIconSize(QSize(16, 16));

    m_view = new QWebView(this);
    setCentralWidget(m_view);

    // setup a progress bar to display while we're loading
    m_progress = new QProgressBar(this);
    m_progress->setRange(0, 100);
    m_progress->setMinimumSize(100, 20);
    m_progress->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    m_progress->hide();
    statusBar()->addPermanentWidget(m_progress);

    connect(m_view, SIGNAL(loadProgress(int)), m_progress, SLOT(show()));
    connect(m_view, SIGNAL(loadProgress(int)), m_progress, SLOT(setValue(int)));
    connect(m_view, SIGNAL(loadFinished(bool)), m_progress, SLOT(hide()));

    setupActions();
    setupMenus();
    setupToolBars();
    setupDocks();

    connect(m_view, SIGNAL(loadFinished(bool)),
            this, SLOT(handleLoadFinished()));
    connect(m_view, SIGNAL(titleChanged(const QString&)),
            this, SLOT(handleTitleChanged(const QString&)));
    connect(m_view->page(), SIGNAL(linkHovered(const QString&, const QString&, const QString&)),
            this, SLOT(handleLinkHover(const QString&, const QString&)));
    connect(m_view->page(), SIGNAL(selectionChanged()),
            this, SLOT(handlePageContentsChanged()));
    connect(m_view->page(), SIGNAL(contentsChanged()),
            this, SLOT(handlePageContentsChanged()));
    connect(m_boldAction, SIGNAL(changed()), 
            this, SLOT(handlePageActionChanged()));
   
    openDefaultXHtml();
}

void HtmlEditMainWindow::setupActions()
{
    QAction* action;
    m_colorAction = new QAction(QIcon(":/images/palette.png"), tr("Color"), this);
    m_colorAction->setData("ForeColor");
    m_colorAction->setIconVisibleInMenu(false);
    connect(m_colorAction, SIGNAL(triggered(bool)),
            this, SLOT(handleColorChange()));
    m_backgroundColorAction = new QAction(QIcon(":/images/palette.png"), tr("Background Color"), this);
    m_backgroundColorAction->setData("BackColor");
    m_backgroundColorAction->setIconVisibleInMenu(false);
    connect(m_backgroundColorAction, SIGNAL(triggered(bool)),
            this, SLOT(handleColorChange()));

    m_view->pageAction(QWebPage::Undo)->setShortcut(QKeySequence::Undo);
    m_view->pageAction(QWebPage::Redo)->setShortcut(QKeySequence::Redo);
    m_view->pageAction(QWebPage::Cut)->setShortcut(QKeySequence::Cut);
    m_view->pageAction(QWebPage::Copy)->setShortcut(QKeySequence::Copy);
    m_view->pageAction(QWebPage::Paste)->setShortcut(QKeySequence::Paste);
#ifdef WEBKIT_TRUNK
    m_view->pageAction(QWebPage::PasteAndMatchStyle)->setShortcut(QKeySequence(tr("Ctrl+Alt+Shift+v")));
#endif
#if QT_VERSION >= 0x040500
    action = m_view->pageAction(QWebPage::SelectAll);
    if (action)
        action->setShortcut(QKeySequence::SelectAll);
#endif

    m_boldAction = m_view->pageAction(QWebPage::ToggleBold);
    m_boldAction->setIcon(QIcon(":/images/bold.png"));
    m_boldAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_B));
    action = m_view->pageAction(QWebPage::ToggleItalic);
    action->setIcon(QIcon(":/images/italic.png"));
    action->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_I));
    action = m_view->pageAction(QWebPage::ToggleUnderline);
    action->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_U));
    action->setIcon(QIcon(":/images/underline.png"));
#ifdef WEBKIT_TRUNK
    action = m_view->pageAction(QWebPage::ToggleStrikethrough);
    action->setIcon(QIcon(":/images/text_strikethrough.png"));
    action = m_view->pageAction(QWebPage::ToggleSubscript);
    action->setIcon(QIcon(":/images/text_subscript.png"));
    action = m_view->pageAction(QWebPage::ToggleSuperscript);
    action->setIcon(QIcon(":/images/text_superscript.png"));
    action = m_view->pageAction(QWebPage::RemoveFormat);
    action->setShortcut(QKeySequence(tr("Ctrl+Shift+t")));
    action = m_view->pageAction(QWebPage::AlignLeft);
    action->setShortcut(QKeySequence("Ctrl+{"));
    action->setIcon(QIcon(":/images/text_align_left.png"));
    action = m_view->pageAction(QWebPage::AlignCenter);
    action->setShortcut(QKeySequence("Ctrl+|"));
    action->setIcon(QIcon(":/images/text_align_center.png"));
    action = m_view->pageAction(QWebPage::AlignJustified);
    action->setShortcut(QKeySequence("Ctrl+Alt+|"));
    action->setIcon(QIcon(":/images/text_align_justify.png"));
    action = m_view->pageAction(QWebPage::AlignRight);
    action->setShortcut(QKeySequence("Ctrl+}"));
    action->setIcon(QIcon(":/images/text_align_right.png"));
    action = m_view->pageAction(QWebPage::InsertUnorderedList);
    action->setIcon(QIcon(":/images/text_list_bullets.png"));
    action = m_view->pageAction(QWebPage::InsertOrderedList);
    action->setIcon(QIcon(":/images/text_list_numbers.png"));
    action = m_view->pageAction(QWebPage::Indent);
    action->setShortcut(QKeySequence("Ctrl+]"));
    action->setIcon(QIcon(":/images/text_indent.png"));
    action = m_view->pageAction(QWebPage::Outdent);
    action->setShortcut(QKeySequence("Ctrl+["));
    action->setIcon(QIcon(":/images/text_outdent.png"));
#endif
}

void HtmlEditMainWindow::setupMenus()
{
    // setup the menus
    QMenu* fileMenu = menuBar()->addMenu(tr("&File"));
    QAction* action;
    action = fileMenu->addAction(tr("&New Window"), this, SLOT(newWindow()), QKeySequence(Qt::CTRL | Qt::Key_N));
    fileMenu->addSeparator();
    fileMenu->addAction(tr("Open File..."), this, SLOT(openFile()), QKeySequence(Qt::CTRL | Qt::Key_O));
    fileMenu->addAction(tr("Open Location..."), this, SLOT(openLocation()), QKeySequence(Qt::CTRL | Qt::Key_L));
    fileMenu->addAction(tr("Open Default &HTML"), this, SLOT(openDefaultHtml()));
    fileMenu->addAction(tr("Open Default &XHTML"), this, SLOT(openDefaultXHtml())); 
    fileMenu->addSeparator();
    fileMenu->addAction(tr("&Close"), this, SLOT(close()), QKeySequence(Qt::CTRL | Qt::Key_W));
    fileMenu->addAction(tr("&Save Page As..."), this, SLOT(saveFile()), QKeySequence(Qt::CTRL | Qt::Key_S));

    QMenu* editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(m_view->pageAction(QWebPage::Undo));
    editMenu->addAction(m_view->pageAction(QWebPage::Redo));
    editMenu->addSeparator();
    editMenu->addAction(m_view->pageAction(QWebPage::Cut));
    editMenu->addAction(m_view->pageAction(QWebPage::Copy));
    editMenu->addAction(m_view->pageAction(QWebPage::Paste));
#ifdef WEBKIT_TRUNK
    editMenu->addAction(m_view->pageAction(QWebPage::PasteAndMatchStyle));
#endif
#if QT_VERSION >= 0x040500
    action = m_view->pageAction(QWebPage::SelectAll);
    if (action)
        editMenu->addAction(action);
#endif
    editMenu->addSeparator();
    m_editableAction = editMenu->addAction(QIcon(":/images/page_edit.png"), tr("Editable"), this, SLOT(setEditable(bool)));
    m_editableAction->setCheckable(true);
    m_editableAction->setChecked(true);

    m_viewMenu = menuBar()->addMenu(tr("&View"));
    connect(m_viewMenu, SIGNAL(aboutToShow()),
            this, SLOT(handleViewAboutToShow()));
    m_dumpPlainTextAction = new QAction(tr("Dump Plain Text"), this);
    connect(m_dumpPlainTextAction, SIGNAL(triggered(bool)),
            this, SLOT(dumpPlainText()));
    m_dumpHtmlAction = new QAction(tr("Dump HTML"), this);
    connect(m_dumpHtmlAction, SIGNAL(triggered(bool)),
            this, SLOT(dumpHtml()));
    m_dumpRenderTreeAction = new QAction(tr("Dump RenderTree"), this);
    connect(m_dumpRenderTreeAction, SIGNAL(triggered(bool)),
            this, SLOT(dumpRenderTree()));

    QMenu* formatMenu = menuBar()->addMenu(tr("F&ormat"));
    QMenu* fontMenu;
#ifdef WEBKIT_TRUNK
    fontMenu = formatMenu->addMenu(tr("&Font"));
#else
    fontMenu = formatMenu;
#endif

    fontMenu->addAction(m_boldAction);
    fontMenu->addAction(m_view->pageAction(QWebPage::ToggleItalic));
    fontMenu->addAction(m_view->pageAction(QWebPage::ToggleUnderline));
#ifdef WEBKIT_TRUNK
    fontMenu->addAction(m_view->pageAction(QWebPage::ToggleStrikethrough));
    fontMenu->addAction(m_view->pageAction(QWebPage::ToggleSubscript));
    fontMenu->addAction(m_view->pageAction(QWebPage::ToggleSuperscript));
#endif

    m_sizeMenu = new QMenu(tr("Size"), 0);
    m_sizeMenu->setIcon(QIcon(":/images/style.png"));
    formatMenu->addMenu(m_sizeMenu);
    connect(m_sizeMenu, SIGNAL(triggered(QAction*)), 
            this, SLOT(handleExecCommandAction(QAction*)));
    action = m_sizeMenu->addAction(tr("X Small"));
    action->setData(QStringList() << "FontSize" << "1");
    action = m_sizeMenu->addAction(tr("Small"));
    action->setData(QStringList() << "FontSize" << "2");
    action = m_sizeMenu->addAction(tr("Medium"));
    action->setData(QStringList() << "FontSize" << "3");
    m_sizeMenu->setDefaultAction(action);
    connect(m_sizeMenu->menuAction(), SIGNAL(triggered(bool)),
            action, SIGNAL(triggered(bool)));
    action = m_sizeMenu->addAction(tr("Large"));
    action->setData(QStringList() << "FontSize" << "5");
    action = m_sizeMenu->addAction(tr("X Large"));
    action->setData(QStringList() << "FontSize" << "6");
    action = m_sizeMenu->addAction(tr("XX Large"));
    action->setData(QStringList() << "FontSize" << "7");

    m_paraMenu = new QMenu(tr("&Style"), 0);
    m_paraMenu->menuAction()->setIcon(QIcon(":/images/pilcrow.png"));
    m_paraMenu->menuAction()->setIconVisibleInMenu(false);
    m_paraMenu->setToolTip(tr("Choose a paragraph style."));
    formatMenu->addMenu(m_paraMenu);
    connect(m_paraMenu, SIGNAL(triggered(QAction*)), 
            this, SLOT(handleExecCommandAction(QAction*)));
    action = m_paraMenu->addAction(tr("Default (div)"));
    action->setData(QStringList() << "FormatBlock" << "div");
    connect(m_paraMenu->menuAction(), SIGNAL(triggered(bool)),
            action, SIGNAL(triggered(bool)));
    action = m_paraMenu->addAction(tr("Paragraph"));
    action->setData(QStringList() << "FormatBlock" << "p");
    action = m_paraMenu->addAction(tr("Preformatted"));
    action->setData(QStringList() << "FormatBlock" << "pre");
    action = m_paraMenu->addAction(tr("Heading 1"));
    action->setData(QStringList() << "FormatBlock" << "h1");
    action = m_paraMenu->addAction(tr("Heading 2"));
    action->setData(QStringList() << "FormatBlock" << "h2");
    action = m_paraMenu->addAction(tr("Heading 3"));
    action->setData(QStringList() << "FormatBlock" << "h3");
    action = m_paraMenu->addAction(tr("Heading 4"));
    action->setData(QStringList() << "FormatBlock" << "h4");
    action = m_paraMenu->addAction(tr("Heading 5"));
    action->setData(QStringList() << "FormatBlock" << "h5");
    action = m_paraMenu->addAction(tr("Heading 6"));
    action->setData(QStringList() << "FormatBlock" << "h6");
    action = m_paraMenu->addAction(tr("Address"));
    action->setData(QStringList() << "FormatBlock" << "address");
    action = m_paraMenu->addAction(tr("Span"));
    action->setData(QStringList() << "FormatBlock" << "span");

#ifdef WEBKIT_TRUNK
    QMenu* listMenu = formatMenu->addMenu(tr("&List"));
    listMenu->addAction(m_view->pageAction(QWebPage::InsertUnorderedList));
    listMenu->addAction(m_view->pageAction(QWebPage::InsertOrderedList));
    listMenu->addSeparator();
    listMenu->addAction(m_view->pageAction(QWebPage::Indent));
    listMenu->addAction(m_view->pageAction(QWebPage::Outdent));

    QMenu* alignmentMenu = formatMenu->addMenu(tr("&Alignment"));
    alignmentMenu->addAction(m_view->pageAction(QWebPage::AlignLeft));
    alignmentMenu->addAction(m_view->pageAction(QWebPage::AlignCenter));
    alignmentMenu->addAction(m_view->pageAction(QWebPage::AlignJustified));
    alignmentMenu->addAction(m_view->pageAction(QWebPage::AlignRight));

    QMenu* writingMenu = alignmentMenu->addMenu(tr("Writing Direction"));
#else
    QMenu* writingMenu = formatMenu->addMenu(tr("Writing Direction"));
#endif
    writingMenu->addAction(m_view->pageAction(QWebPage::SetTextDirectionDefault));
    writingMenu->addAction(m_view->pageAction(QWebPage::SetTextDirectionLeftToRight));
    writingMenu->addAction(m_view->pageAction(QWebPage::SetTextDirectionRightToLeft));

    formatMenu->addAction(m_colorAction);
    formatMenu->addAction(m_backgroundColorAction);
#ifdef WEBKIT_TRUNK
    formatMenu->addSeparator();
    formatMenu->addAction(m_view->pageAction(QWebPage::RemoveFormat));
#endif
    QMenu* helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(tr("About %1").arg(QApplication::applicationName()), this, SLOT(about()));
    helpMenu->addAction(tr("About Qt"), QApplication::instance(), SLOT(aboutQt()));
}

void HtmlEditMainWindow::setupToolBars()
{
    QFont font;
    font.setPointSize(10);

    // setup the toolbar
    QToolBar* bar = addToolBar("Edit ToolBar");
    bar->setFont(font);
    bar->addAction(m_paraMenu->menuAction());
    m_fontComboBox = new QFontComboBox;
    connect(m_fontComboBox, SIGNAL(currentFontChanged(const QFont&)), 
            this, SLOT(handleFontChanged(const QFont&)));
    m_fontComboBox->setMaximumWidth(180);
    bar->addWidget(m_fontComboBox);
    bar->addAction(m_sizeMenu->menuAction());
    bar->addAction(m_boldAction);
    bar->addAction(m_view->pageAction(QWebPage::ToggleItalic));
    bar->addAction(m_view->pageAction(QWebPage::ToggleUnderline));
#ifdef WEBKIT_TRUNK
    bar->addAction(m_view->pageAction(QWebPage::ToggleStrikethrough));
    bar->addAction(m_view->pageAction(QWebPage::ToggleSubscript));
    bar->addAction(m_view->pageAction(QWebPage::ToggleSuperscript));
    bar->addSeparator();
#endif
    bar->addAction(m_colorAction);
    bar->addAction(m_backgroundColorAction);
#ifdef WEBKIT_TRUNK
    addToolBarBreak();
    bar = addToolBar("Layout ToolBar");
    bar->addAction(m_view->pageAction(QWebPage::InsertUnorderedList));
    bar->addAction(m_view->pageAction(QWebPage::InsertOrderedList));
    bar->addAction(m_view->pageAction(QWebPage::Indent));
    bar->addAction(m_view->pageAction(QWebPage::Outdent));
    bar->addSeparator();
    bar->addAction(m_view->pageAction(QWebPage::AlignLeft));
    bar->addAction(m_view->pageAction(QWebPage::AlignCenter));
    bar->addAction(m_view->pageAction(QWebPage::AlignJustified));
    bar->addAction(m_view->pageAction(QWebPage::AlignRight));
#endif
}

void HtmlEditMainWindow::setupDocks()
{
    // create / setup the source view dock widget
    QDockWidget* sourceDock = new QDockWidget(tr("Source View"), this);
    sourceDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    sourceDock->setFeatures(QDockWidget::AllDockWidgetFeatures);
    HtmlEditSourceView* sourceView = new HtmlEditSourceView(m_view);
    sourceDock->setWidget(sourceView);
    addDockWidget(Qt::BottomDockWidgetArea, sourceDock, Qt::Horizontal);

    WebActionView* actionView = new WebActionView(m_view);
    QDockWidget* actionDock = new QDockWidget(tr("WebAction View"), this);
    actionDock->setAllowedAreas(Qt::TopDockWidgetArea|Qt::BottomDockWidgetArea);
    actionDock->setFeatures(QDockWidget::AllDockWidgetFeatures);
    actionDock->setWidget(actionView);
    addDockWidget(Qt::BottomDockWidgetArea, actionDock, Qt::Horizontal);
    actionDock->setFloating(true);
//    tabifyDockWidget(actionDock, sourceDock);
    actionDock->hide();
    m_view->setFocus();
}

