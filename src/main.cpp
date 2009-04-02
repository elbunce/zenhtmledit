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

/*----------------------------------------------------------------------
 * System Headers
 *----------------------------------------------------------------------*/
#include <QApplication>
#include <QIcon>
#include <qwebsettings.h>

/*----------------------------------------------------------------------
 * Main Code
 *----------------------------------------------------------------------*/
int main(int argc, char **argv)
{
    QApplication app(argc, argv);
		app.setAttribute(Qt::AA_DontShowIconsInMenus);
		app.setOrganizationDomain("webkit.org");
		app.setApplicationName("QtHtmlEditor");
		app.setOrganizationName("WebKit");
		app.setWindowIcon(QIcon(":/images/application_edit.png"));
#if QT_VERSION >= 0x040400
    app.setApplicationVersion("0.1");
#endif

    QWebSettings::setObjectCacheCapacities((16*1024*1024) / 8, (16*1024*1024) / 8, 16*1024*1024);

		QWebSettings* gSettings = QWebSettings::globalSettings();
		gSettings->setAttribute(QWebSettings::JavascriptEnabled, true);
    gSettings->setAttribute(QWebSettings::PluginsEnabled, true);
    gSettings->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
	
		HtmlEditMainWindow* mw = new HtmlEditMainWindow(0, 0);
		mw->show();

		return app.exec();
};
