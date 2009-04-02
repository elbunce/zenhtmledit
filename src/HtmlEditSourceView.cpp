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
#include "HtmlEditSourceView.h"
#include "HtmlSourceHighlighter.h"

/*----------------------------------------------------------------------
 * System Headers
 *----------------------------------------------------------------------*/
#include <QPlainTextEdit>
#include <QTimer>
#include <QVBoxLayout>
#include <qwebview.h>
#include <qwebpage.h>
#include <qwebframe.h>

/*----------------------------------------------------------------------
 * Main Code
 *----------------------------------------------------------------------*/
HtmlEditSourceView::HtmlEditSourceView(QWebView* view, QWidget* parent, Qt::WindowFlags flags)
	: QWidget(parent, flags),
		m_view(view),
		m_timer(new QTimer(this)),
		m_dirty(true)
{
	// check for updates once a second
	m_timer->setInterval(1 * 1000);

	// setup the display
	QVBoxLayout* layout = new QVBoxLayout;
	layout->setContentsMargins(0, 0, 0, 0);
	m_display = new QPlainTextEdit;
	m_display->setReadOnly(true);
	QFont font = m_display->font();
	font.setPointSize(10);
	m_display->setFont(font);
	layout->addWidget(m_display);
	setLayout(layout);

	// do some primitive highlighting
	(void)new HtmlSourceHighlighter(m_display->document());

	connect(m_timer, SIGNAL(timeout()), this, SLOT(updateDisplay()));
	connect(m_view, SIGNAL(loadFinished(bool)),
            this, SLOT(handleLoadFinished()));
	connect(m_view->page(), SIGNAL(contentsChanged()),
					this, SLOT(handlePageContentsChanged()));
	
	
	updateDisplay();
	
	// start checking for updates
	m_timer->start();
}

HtmlEditSourceView::~HtmlEditSourceView()
{
}

void HtmlEditSourceView::updateDisplay()
{
	// nothing to do if the display isn't dirty
	if (!m_dirty)
		return;

	QString content = m_view->page()->mainFrame()->toHtml();

	// break it up a bit, otherwise the returned HTML can all end up on one line
	QRegExp lineBreaks("(</div>|</address>|</p>|</pre>|</h[1-6]>|<br>|<br />|</title>|</style>|</table>|</form>|</script>|</head>|</body>)(?!\n)");
	content.replace(lineBreaks, "\\1\n");

	// now set the content
	m_display->setPlainText(content);

	m_dirty = false;
}

void HtmlEditSourceView::handleLoadFinished()
{
	// mark us dirty, and immediately update the display
	m_dirty = true;

	// Now update the display
	updateDisplay();
}

void HtmlEditSourceView::handlePageContentsChanged()
{
	// mark us as dirty
	m_dirty = true;
}


