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
#include "WebActionView.h"

/*----------------------------------------------------------------------
 * System Headers
 *----------------------------------------------------------------------*/
#include <QHBoxLayout>
#include <QAction>
#include <QLabel>
#include <QToolButton>
#include <qwebview.h>
#include <qwebpage.h>

/*----------------------------------------------------------------------
 * Main Code
 *----------------------------------------------------------------------*/
WebActionView::WebActionView(QWebView* view, QWidget* parent, Qt::WindowFlags flags)
    : QWidget(parent, flags)
{
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    QGridLayout* layout = new QGridLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setVerticalSpacing(0);
    layout->setHorizontalSpacing(0);
    layout->setSizeConstraint(QLayout::SetMinimumSize);

    QLabel* label;
    QAction* action;
    QToolButton* button;
    QHBoxLayout* hBoxLayout;
    int baseCol = 0;
    int row = 0;
    for (int i = 0; i < QWebPage::WebActionCount; i++) {
        label = new QLabel(QString::number(i));
        label->setFrameShape(QFrame::StyledPanel);
        layout->addWidget(label, row, baseCol);
        action = view->pageAction(QWebPage::WebAction(i));
        if (action)
        {
            button = new QToolButton;
            button->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
            button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
            
            button->setDefaultAction(action);
            layout->addWidget(button, row, baseCol + 1);
        } else {
            label = new QLabel(tr("Missing Action?!"));
            label->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
            label->setFrameShape(QFrame::StyledPanel);
            layout->addWidget(label, row, baseCol + 1);
        }
        row++;
        if (row > 16) {
            row = 0;
            baseCol += 2;
        }
    }
    
    setLayout(layout);
    setMinimumWidth(500);
    adjustSize();
}

WebActionView::~WebActionView()
{
}

