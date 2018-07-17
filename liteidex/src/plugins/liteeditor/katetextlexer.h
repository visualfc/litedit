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
// Module: katetextlexer.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef KATETEXTLEXER_H
#define KATETEXTLEXER_H

#include "liteeditorapi/liteeditorapi.h"

class KateTextLexer : public LiteApi::ITextLexer
{
public:
    KateTextLexer(QObject *parent);
    virtual bool isLangSupport() const;
    virtual bool isInComment(const QTextCursor &cursor) const;
    virtual bool isInString(const QTextCursor &cursor) const;
    virtual bool isInEmptyString(const QTextCursor &cursor) const;
    virtual bool isEndOfString(const QTextCursor &cursor) const;
    virtual bool isInStringOrComment(const QTextCursor &cursor) const;
    virtual bool isCanAutoCompleter(const QTextCursor &cursor) const;
    virtual bool isInImport(const QTextCursor &cursor) const;
    virtual int startOfFunctionCall(const QTextCursor &cursor) const;
    virtual QString fetchFunctionTip(const QString &func, const QString &kind, const QString &info);
    virtual bool fetchFunctionArgs(const QString &str, int &argnr, int &parcount);
    virtual QString stringQuoteList() const;
    virtual bool hasStringBackslash() const;
};

#endif // KATETEXTLEXER_H
