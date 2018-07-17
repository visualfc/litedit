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
// Module: optionmanager.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "optionmanager.h"
#include "optionsbrowser.h"
#include <QAction>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end


OptionManager::OptionManager()
    : m_browser(0)
{
}

OptionManager::~OptionManager()
{
    if (m_browser) {
        delete m_browser;
    }
}

bool OptionManager::initWithApp(IApplication *app)
{
    if (!IManager::initWithApp(app)) {
        return false;
    }
    return true;
}

void OptionManager::addFactory(IOptionFactory *factory)
{
    m_factoryList.append(factory);
}

void OptionManager::removeFactory(IOptionFactory *factory)
{
    m_factoryList.removeOne(factory);
}

QList<IOptionFactory*> OptionManager::factoryList() const
{
    return m_factoryList;
}

void OptionManager::exec()
{
    if (m_browser == 0) {
        m_browser = new OptionsBrowser(m_liteApp,m_liteApp->mainWindow());
        connect(m_browser,SIGNAL(applyOption(QString)),this,SIGNAL(applyOption(QString)));
        foreach (IOptionFactory *f, m_factoryList) {
            QStringList mimeTypes = f->mimeTypes();
            foreach (QString mimeType, mimeTypes) {
                IOption *opt = f->create(mimeType);
                if (opt) {
                    m_browser->addOption(opt);
                }
            }
        }
    }
    m_browser->execute();
}

void OptionManager::loadOption(const QString &opt)
{
    foreach (IOptionFactory *f, m_factoryList) {
        QStringList mimeTypes = f->mimeTypes();
        foreach (QString mimeType, mimeTypes) {
            if (opt == mimeType) {
                emit applyOption(mimeType);
            }
        }
    }
}
