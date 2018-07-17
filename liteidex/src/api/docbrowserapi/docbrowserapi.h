/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2016 LiteIDE Team. All rights reserved.
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License as published by the Free Software Foundation; either
** version 2.1 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Lesser General Public License for more details.
**
** In addition, as a special exception,  that plugins developed for LiteIDE,
** are allowed to remain closed sourced and can be distributed under any license .
** These rights are included in the file LGPL_EXCEPTION.txt in this package.
**
**************************************************************************/
// Module: docbrowserapi.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef LITEDOCBROWSERAPI_H
#define LITEDOCBROWSERAPI_H

#include "liteapi/liteapi.h"
#include "liteapi/litehtml.h"
#include <QTextBrowser>
#include <QComboBox>
#include <QToolBar>

namespace LiteApi {

class IDocumentBrowser : public IBrowserEditor
{
    Q_OBJECT
public:
    IDocumentBrowser(QObject *parent) : IBrowserEditor(parent) {}
    virtual void setSearchPaths(const QStringList &paths) = 0;
    virtual void setUrlHtml(const QUrl &url,const QString &html) = 0;
    virtual void scrollToAnchor(const QString &text) = 0;
    virtual QToolBar *toolBar() = 0;
    virtual QComboBox *urlComboBox() = 0;
    virtual IHtmlWidget *htmlWidget() = 0;
signals:
    void linkHovered(const QUrl &url);
    void requestUrl(const QUrl &url);
    void forwardAvailable(bool available);
    void backwardAvailable(bool available);
    void documentLoaded();
    void anchorChanged(const QString &anchor);
public slots:
    virtual void backward() = 0;
    virtual void forward() = 0;
};

}

#endif //LITEDOCBROWSERAPI_H

