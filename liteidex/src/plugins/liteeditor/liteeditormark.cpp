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
// Module: liteeditormark.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "liteeditormark.h"
#include "qtc_texteditor/basetextdocumentlayout.h"
#include "liteeditorwidget.h"
#include <QTextDocument>
#include <QTextBlock>
#include <QFileInfo>
#include <QDebug>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

LiteEditorMarkNode::LiteEditorMarkNode(LiteEditorMark *editorMark, int type, int indexOfType, int lineNumber, const QTextBlock &block, QObject *parent)
    : TextEditor::ITextMark(parent),
      m_editorMark(editorMark)
{
    m_blockNumber = lineNumber;
    m_type = type;
    m_indexOfType = indexOfType;
    m_block = block;
}

LiteEditorMarkNode::~LiteEditorMarkNode()
{
  //  qDebug() << "~LiteTextMark" << m_type << this->lineNumber();
}

void LiteEditorMarkNode::removedFromEditor()
{
    m_editorMark->removedFromEditor(this);
}

void LiteEditorMarkNode::updateBlockNumber(int lineNumber)
{
    if (m_blockNumber == lineNumber) {
        return;
    }
    int old = m_blockNumber;
    m_blockNumber = lineNumber;
    m_editorMark->updateBlockNumber(this, m_blockNumber, old);
}

void LiteEditorMarkNode::updateBlock(const QTextBlock &block)
{
    m_block = block;
    m_editorMark->updateLineBlock(this);
 }

QTextBlock LiteEditorMarkNode::block() const
{
    return m_block;
}

LiteEditorMarkManager::LiteEditorMarkManager(QObject *parent) :
    LiteApi::IEditorMarkManager(parent)
{
}

LiteEditorMarkManager::~LiteEditorMarkManager()
{

}

void LiteEditorMarkManager::registerMark(int type, const QIcon &icon)
{
    if (m_typeIconMap.contains(type)) {
        qDebug() << "warning!!!" << "LiteEditorMarkTypeManager register mark type" << type << "exist!";
    }
    m_typeIconMap.insert(type,icon);
}

QList<int> LiteEditorMarkManager::markTypeList() const
{
    return m_typeIconMap.keys();
}

QIcon LiteEditorMarkManager::iconForType(int type) const
{
    return m_typeIconMap.value(type);
}

int LiteEditorMarkManager::indexOfType(int type) const
{
    return m_typeIconMap.keys().indexOf(type);
}

QList<LiteApi::IEditorMark *> LiteEditorMarkManager::editorMarkList() const
{
    return m_markList;
}

void LiteEditorMarkManager::addMark(LiteApi::IEditorMark *mark)
{
    m_markList.append(mark);
    emit editorMarkCreated(mark);
    connect(mark,SIGNAL(markListChanged(int)),this,SLOT(markListChanged(int)));
}

void LiteEditorMarkManager::removeMark(LiteApi::IEditorMark *mark)
{
    m_markList.removeAll(mark);
    emit editorMarkRemoved(mark);
}

void LiteEditorMarkManager::addMarkNode(LiteApi::IEditorMark *mark, LiteApi::IEditorMarkNode *node)
{
    emit editorMarkNodeCreated(mark,node);
}

void LiteEditorMarkManager::removeMarkNode(LiteApi::IEditorMark *mark, LiteApi::IEditorMarkNode *node)
{
    emit editorMarkNodeRemoved(mark,node);
}

void LiteEditorMarkManager::updateMarkNode(LiteApi::IEditorMark *mark, LiteApi::IEditorMarkNode *node)
{
    emit editorMarkNodeChanged(mark,node);
}

void LiteEditorMarkManager::markListChanged(int type)
{
    emit editorMarkListChanged((LiteApi::IEditorMark*)sender(),type);
}

LiteEditorMark::LiteEditorMark(LiteEditorMarkManager *manager, LiteEditor *editor) :
    LiteApi::IEditorMark(editor),
    m_manager(manager),
    m_editor(editor),
    m_document(editor->document())
{
    m_manager->addMark(this);
}

LiteEditorMark::~LiteEditorMark()
{
    m_manager->removeMark(this);
}

static TextEditor::ITextMark *findMarkByType(TextEditor::TextBlockUserData *data, int type)
{
    foreach(TextEditor::ITextMark *mark, data->marks()) {
        if ( mark->type() == type ) {
            return mark;
        }
    }
    return 0;
}

LiteEditorMarkNode *LiteEditorMark::createMarkByType(int type, int line, const QTextBlock &block)
{
    int index = m_manager->indexOfType(type);
    if (index < 0) {
        return 0;
    }
    QIcon icon = m_manager->iconForType(type);
    LiteEditorMarkNode *mark = new LiteEditorMarkNode(this,type,index,line,block,this);
    mark->setIcon(icon);
    return mark;
}

void LiteEditorMark::removedFromEditor(LiteEditorMarkNode *mark)
{
    int type = mark->type();
    m_typeLineMarkMap[type].remove(mark->blockNumber());

    m_manager->removeMarkNode(this,mark);

    emit markListChanged(type);

    delete mark;
}

void LiteEditorMark::updateBlockNumber(LiteEditorMarkNode *mark, int newLine, int oldLine)
{
    int type = mark->type();

    m_typeLineMarkMap[type].remove(oldLine);
    m_typeLineMarkMap[type].insert(newLine,mark);

    m_manager->updateMarkNode(this,mark);

    emit markListChanged(type);
}

void LiteEditorMark::updateLineBlock(LiteEditorMarkNode *mark)
{
    m_manager->updateMarkNode(this,mark);
}

QMap<int, QList<int> > LiteEditorMark::saveTypeLinesMap(bool clearAll)
{
    QMap<int, QList<int> > maps;
    QTextBlock block = m_document->firstBlock();
    while(block.isValid()) {
        TextEditor::TextBlockUserData *data = TextEditor::BaseTextDocumentLayout::testUserData(block);
        if (data) {
            foreach (LiteApi::IEditorMarkNode *mark, data->marks()) {
                maps[mark->type()].push_back(mark->blockNumber());
            }
            if (clearAll) {
                qDeleteAll(data->marks());
                data->clearMarks();
            }
        }
        block = block.next();
    }
    if (clearAll) {
        m_typeLineMarkMap.clear();
    }
    return maps;
}

void LiteEditorMark::restoreTypeLinesMap(const QMap<int, QList<int> > &maps)
{
    QMapIterator<int, QList<int> > i(maps);
    while (i.hasNext()) {
        i.next();
        this->addMarkList(i.value(),i.key());
    }
}

void LiteEditorMark::addMark(int line, int type)
{
    addMarkList(QList<int>() << line, type);
}

void LiteEditorMark::addMarkList(const QList<int> &lines, int type)
{
    bool changed = false;
    foreach (int line, lines) {
        const QTextBlock &block = m_document->findBlockByNumber(line);
        if (!block.isValid()) {
            continue;
        }
        TextEditor::TextBlockUserData *data = TextEditor::BaseTextDocumentLayout::userData(block);
        if (!data) {
            continue;
        }
        TextEditor::ITextMark *findMark = findMarkByType(data,type);
        if (findMark) {
            continue;
        }
        LiteEditorMarkNode *mark = createMarkByType(type, line, block);
        data->addMark(mark);

        m_typeLineMarkMap[type].insert(line,mark);
        m_manager->addMarkNode(this,mark);
        changed = true;
    }

    if (changed) {
        emit markListChanged(type);
    }
}


void LiteEditorMark::removeMark(int line, int type)
{
    removeMarkList(QList<int>() << line, type);
}

void LiteEditorMark::removeMarkList(const QList<int> &lines, int type)
{
    bool changed = false;
    foreach (int line, lines) {
        const QTextBlock &block = m_document->findBlockByNumber(line);
        if (!block.isValid()) {
            continue;
        }
        TextEditor::TextBlockUserData *data = TextEditor::BaseTextDocumentLayout::testUserData(block);
        if (!data) {
            continue;
        }
        TextEditor::ITextMark *mark = findMarkByType(data,type);
        if (!mark) {
            continue;
        }
        data->removeMark(mark);

        m_typeLineMarkMap[type].remove(line);
        m_manager->removeMarkNode(this,mark);
        delete mark;
        changed = true;
    }
    if (changed) {
        emit markListChanged(type);
    }
}

QList<int> LiteEditorMark::markLinesByType(int type) const
{
 //   qDebug
    return m_typeLineMarkMap[type].keys();
//    QList<int> lineList;
//    QTextBlock block = m_document->firstBlock();
//    while (block.isValid()) {
//        TextEditor::TextBlockUserData *data = TextEditor::BaseTextDocumentLayout::testUserData(block);
//        if (data) {
//            foreach(TextEditor::ITextMark *mark, data->marks() ) {
//                if ( ((LiteTextMark*)mark)->type() == type ) {
//                    lineList.append(block.blockNumber());
//                    break;
//                }
//            }
//        }
//        block = block.next();
//    }
    //    return lineList;
}

QList<QTextBlock> LiteEditorMark::markBlocksByType(int type) const
{
    QList<QTextBlock> blocks;
    foreach (TextEditor::ITextMark *mark, m_typeLineMarkMap[type].values()) {
        blocks.push_back(((LiteEditorMarkNode*)mark)->block());
    }
    return blocks;
}

QList<int> LiteEditorMark::markTypesByLine(int line) const
{
    QList<int> typeList;
    TypeLineMarkMapIterator i(m_typeLineMarkMap);
    while (i.hasNext()) {
        i.next();
        if (i.value().contains(line)) {
            typeList.push_back(i.key());
        }
    }
    return typeList;
//    QList<int> typeList;
//    const QTextBlock &block = m_document->findBlockByNumber(line);
//    if (!block.isValid()) {
//        return typeList;
//    }
//    TextEditor::TextBlockUserData *data = TextEditor::BaseTextDocumentLayout::testUserData(block);
//    if (data) {
//        foreach(TextEditor::ITextMark *mark, data->marks()) {
//            typeList.append(((LiteTextMark*)mark)->type());
//        }
//    }
//    return typeList;
}

LiteEditor *LiteEditorMark::editor() const
{
    return m_editor;
}

QString LiteEditorMark::filePath() const
{
    return m_editor->filePath();
}
