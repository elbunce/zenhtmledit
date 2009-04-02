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

#ifndef HtmlEditSourceView_h
#define HtmlEditSourceView_h

/*----------------------------------------------------------------------
 * System Headers
 *----------------------------------------------------------------------*/
#include <QWidget>

/*----------------------------------------------------------------------
 * Forward Declarations
 *----------------------------------------------------------------------*/
QT_BEGIN_NAMESPACE
class QPlainTextEdit;
class QTimer;
QT_END_NAMESPACE

class QWebView;

/*----------------------------------------------------------------------
 * Main Code
 *----------------------------------------------------------------------*/
/*!
  \brief The HtmlEditSourceView provides a view on the contents contained 
  within a QWebView.
*/
class HtmlEditSourceView : public QWidget {
    Q_OBJECT
public:
    //! Constructs a HtmlEditSourceView on \a view as a child of \a parent with \a flags. 
    HtmlEditSourceView(QWebView* view, QWidget* parent = 0, Qt::WindowFlags flags = 0);
    
    //! Destroys the object and frees allcoated resources
    virtual ~HtmlEditSourceView();
    
private slots:
    //! Updates the source display
    void updateDisplay();
    
    //! Handles a load having finished on view.
    void handleLoadFinished();
    
    //! Handles the contents of the contained page having changed.
    void handlePageContentsChanged();
    
private:
    //! The plain text edit to use for the display
    QPlainTextEdit* m_display;

    //! The web view whose source is being displayed
    QWebView* m_view;

    //! Timer used to update the display in a timely fashion
    QTimer* m_timer;

    //! True if the contents of the display is out date with the view.
    bool m_dirty;
};

#endif // HtmlEditSourceView
