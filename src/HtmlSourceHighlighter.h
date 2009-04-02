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

#ifndef HtmlSourceHighlighter_h
#define HtmlSourceHighlighter_h

/*----------------------------------------------------------------------
 * System Headers
 *----------------------------------------------------------------------*/
#include <QSyntaxHighlighter>
#include <QTextCharFormat>

/*----------------------------------------------------------------------
 * Main Code
 *----------------------------------------------------------------------*/
/*!
  \brief The HtmlSourceHighlighter provides basic highlighting of document source.
*/
class HtmlSourceHighlighter : public QSyntaxHighlighter {
    Q_OBJECT
public:
    //! Constructs a web source highligher on \a parent.
    HtmlSourceHighlighter(QTextDocument* parent);
    
    //! Destroys the object and frees allocated resources.
    virtual ~HtmlSourceHighlighter();
    
protected:
    //! Apply the appropriate highlights to \a text.
    virtual void highlightBlock(const QString& text);
    
private:
    //! The character format to apply to elements.
    QTextCharFormat m_elementFormat;
};

#endif // HtmlSourceHighlighter_h

