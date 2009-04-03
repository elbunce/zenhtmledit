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

#ifndef HtmlEditMainWindow_h
#define HtmlEditMainWindow_h

/*----------------------------------------------------------------------
 * System Headers
 *----------------------------------------------------------------------*/
#include <QMainWindow>

/*----------------------------------------------------------------------
 * Forward Declarations
 *----------------------------------------------------------------------*/
QT_BEGIN_NAMESPACE
class QProgressBar;
class QLabel;
class QFontComboBox;
QT_END_NAMESPACE

class QWebView;

/*----------------------------------------------------------------------
 * Main Code
 *----------------------------------------------------------------------*/
/*!
  \brief The HtmlEditMainWindow class provides a test harness / example of 
  using Qt WebKit for embedded XHTML/HTML editing.
*/
class HtmlEditMainWindow : public QMainWindow 
{
    Q_OBJECT
public:
    /*!
      \brief Constructs a HtmlEditMainWindow as a child of \a parent with \a flags.
    */
    HtmlEditMainWindow(QWidget* parent = 0, Qt::WindowFlags flags = 0);

    //! Destroys the object and frees allocated resources.
    virtual ~HtmlEditMainWindow();

    //! Returns the web view being edited in the window.
    QWebView* webView() const { return m_view; }

    //! Evaluate the passed in \a script and returns the result.
    QVariant evalJavaScript(const QString& script);
    
public slots:
    //! Creates and shows a new HtmlEditMainWindow containing default XHtml.
    void newWindow();
    
    //! Opens a user specified location within the current window.
    void openLocation();

    //! Opens a local file from the users hard disk within the current window.
    void openFile();

    //! Opens a default HTML file within the current window.
    void openDefaultHtml();

    //! Opens a default XHTML file within the current window.
    void openDefaultXHtml();

    //! Save the contents of the current window.
    void saveFile();

    //! Dumps the contents of the current window as plain text.
    void dumpPlainText();

    //! Dumps the contents of the current window as HTML.
    void dumpHtml();

    //! Dumps the render tree of the current window.
    void dumpRenderTree();

    //! Displays the about box
    void about();

private slots:
    //! Handles the set editable.
    void setEditable(bool on);

    //! Handle title changed.
    void handleTitleChanged(const QString& title);

    //! Handle link hovered.
    void handleLinkHover(const QString &link, const QString &toolTip);

    //! Handle load finished.
    void handleLoadFinished();
    
    //! Handle page contents changed.
    void handlePageContentsChanged();
    
    //! Handles the page action changing state.
    void handlePageActionChanged();

    //! Handle a common exec command action.
    void handleExecCommandAction(QAction* action);

    //! Handle a color change action.
    void handleColorChange();
    
    //! Handles the font name being changed.
    void handleFontChanged(const QFont& font);

    //! Handle view menu about to show.
    void handleViewAboutToShow();

private:
    //! Converts the passed \a rgba color string to a QColor
    QColor toColor(const QString& rgba);
    
    //! Setup the UI.
    void setupUI();

    //! Setup actions
    void setupActions();
    
    //! Setup menus
    void setupMenus();

    //! Setup ToolBars
    void setupToolBars();

    //! Setup the docks
    void setupDocks();

private:
    //! The set of filters to use int he file open dialog.
    QString m_filters;

    //! The web view being operated upon.
    QWebView* m_view;

    //! Progress bar used to indicate the progress of loading content into the view.
    QProgressBar* m_progress;

    //! Action used to control whether the content of the view is editable or not.
    QAction* m_editableAction;

    //! The view menu
    QMenu* m_viewMenu;

    /*! 
      \brief The action used to make the current selection bold (used for 
      tracking if rich text operations are possible.
    */
    QAction* m_boldAction;

    //! The color action
    QAction* m_colorAction;

    //! The background color action
    QAction* m_backgroundColorAction;

    //! Action to trigger dumping the contents as plain text
    QAction* m_dumpPlainTextAction;

    //! Action to trigger dumping the contents as HTML
    QAction* m_dumpHtmlAction;

    //! Action to trigger dumping of the render tree
    QAction* m_dumpRenderTreeAction;

    //! Paragraph Format menu
    QMenu* m_paraMenu;

    //! The font combo box
    QFontComboBox* m_fontComboBox;

    //! Size Format menu
    QMenu* m_sizeMenu;
};

#endif // HtmlEditMainWindow_h

