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
// Module: quickopenfiles.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef QUICKOPENFILES_H
#define QUICKOPENFILES_H

#include "quickopenapi/quickopenapi.h"
#include  "fileutil/findfilesthread.h"

class QStandardItemModel;
class QSortFilterProxyModel;

class QuickOpenFiles : public LiteApi::IQuickOpen
{
    Q_OBJECT
public:
    QuickOpenFiles(LiteApi::IApplication *app, QObject *parent = 0);
    virtual ~QuickOpenFiles();
    virtual QString id() const;
    virtual QString info() const;
    virtual QString placeholderText() const;
    virtual void activate();
    virtual QAbstractItemModel *model() const;
    virtual void updateModel();
    virtual QModelIndex filterChanged(const QString &text);
    virtual void indexChanged(const QModelIndex &index);
    virtual bool selected(const QString &text, const QModelIndex &index);
    virtual void cancel();
    void startFindThread();
public slots:
    void findResult(const QStringList &fileList);
protected:
    LiteApi::IApplication *m_liteApp;
    FindFilesThread *m_thread;
    QStandardItemModel *m_model;
    QStandardItemModel *m_filesModel;
    QSortFilterProxyModel *m_proxyModel;
    QStringList        m_editors;
    Qt::CaseSensitivity m_matchCase;
    int                 m_maxCount;
};

#endif // QUICKOPENFILES_H
