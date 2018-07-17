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
// Module: liteappoption.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "liteappoption.h"
#include "actionmanager.h"
#include "liteapp_global.h"
#include "ui_liteappoption.h"
#include <QDir>
#include <QFileInfo>
#include <QLocale>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QFileDialog>
#include <QMessageBox>
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

LiteAppOption::LiteAppOption(LiteApi::IApplication *app,QObject *parent) :
    LiteApi::IOption(parent),
    m_liteApp(app),
    m_widget(new QWidget),
    ui(new Ui::LiteAppOption)
{
    ui->setupUi(m_widget);

    QSettings global(m_liteApp->resourcePath()+"/liteapp/config/global.ini",QSettings::IniFormat);
    bool storeLocal = global.value(LITEIDE_STORELOCAL,false).toBool();
    ui->storeLocalCheckBox->setChecked(storeLocal);

    const QString &liteideTrPath = m_liteApp->resourcePath()+"/translations";
    QLocale eng(QLocale::English);
    ui->langComboBox->addItem(QLocale::languageToString(QLocale::English),eng.name());
    QDir dir(liteideTrPath);
    if (dir.exists()) {
        foreach (QFileInfo info,dir.entryInfoList(QStringList() << "liteide_*.qm")) {
            QString base = info.baseName();
            QLocale lc(base.right(base.length()-8));
            if (lc.name().isEmpty()) {
                continue;
            }
            QLocale::Language lang = lc.language();
            QString text = QString("%1 (%2)").arg(QLocale::languageToString(lang)).arg(lc.name());
            ui->langComboBox->addItem(text,lc.name());
        }
    }
    QString locale = QLocale::system().name();
    locale = m_liteApp->settings()->value(LITEAPP_LANGUAGE,locale).toString();
    if (!locale.isEmpty()) {
        for (int i = 0; i < ui->langComboBox->count(); i++) {
            if (locale == ui->langComboBox->itemData(i).toString()) {
                ui->langComboBox->setCurrentIndex(i);
                break;
            }
        }
    }
    ui->styleComboBox->addItem(tr("SideBarStyle"),"sidebar");
    ui->styleComboBox->addItem(tr("SplitterStyle"),"splitter");
    QString style = m_liteApp->settings()->value(LITEAPP_STYLE,"sidebar").toString();
    for (int i = 0; i < ui->styleComboBox->count(); i++) {
        if (style == ui->styleComboBox->itemData(i).toString()) {
            ui->styleComboBox->setCurrentIndex(i);
            break;
        }
    }

    const QString &liteQssPath = m_liteApp->resourcePath()+"/liteapp/qss";
    QDir qssDir(liteQssPath);
    if (qssDir.exists()) {
        foreach (QFileInfo info, qssDir.entryInfoList(QStringList() << "*.qss")) {
            ui->qssComboBox->addItem(info.fileName());
        }
    }
    QString qss = m_liteApp->settings()->value(LITEAPP_QSS,"default.qss").toString();
    int index = ui->qssComboBox->findText(qss,Qt::MatchFixedString);
    if (index >= 0 && index < ui->qssComboBox->count()) {
        ui->qssComboBox->setCurrentIndex(index);
    }

    int max = m_liteApp->settings()->value(LITEAPP_MAXRECENTFILES,32).toInt();
    //ui->maxRecentLineEdit->setText(QString("%1").arg(max));
    ui->maxRecentFilesSpinBox->setValue(max);
    max = m_liteApp->settings()->value(LITEAPP_MAXEDITORCOUNT,64).toInt();
    ui->maxEditorCountSpinBox->setValue(max);
    //bool b = m_liteApp->settings()->value(LITEAPP_AUTOCLOSEPROEJCTFILES,true).toBool();
    //ui->autoCloseProjecEditorsCheckBox->setChecked(b);
    bool b1 = m_liteApp->settings()->value(LITEAPP_AUTOLOADLASTSESSION,true).toBool();
    ui->autoLoadLastSessionCheckBox->setChecked(b1);
    bool b2 = m_liteApp->settings()->value(LITEAPP_SPLASHVISIBLE,true).toBool();
    ui->splashVisibleCheckBox->setChecked(b2);
    bool b3 = m_liteApp->settings()->value(LITEAPP_WELCOMEPAGEVISIBLE,true).toBool();
    ui->welcomeVisibleCheckBox->setChecked(b3);

    bool b4 = m_liteApp->settings()->value(LITEAPP_EDITTABSCLOSABLE,true).toBool();
    ui->editorTabsClosableCheckBox->setChecked(b4);

//    bool b5 = m_liteApp->settings()->value(LITEAPP_STARTUPRELOADFOLDERS,true).toBool();
//    ui->startupReloadFoldersCheckBox->setChecked(b5);

    bool b6 = m_liteApp->settings()->value(LITEAPP_STARTUPRELOADFILES,true).toBool();
    ui->startupReloadFilesCheckBox->setChecked(b6);

    bool b7 = m_liteApp->settings()->value(LITEAPP_FILEWATCHERAUTORELOAD,false).toBool();
    ui->fileWatcherAutoReloadCheckBox->setChecked(b7);

    bool b8 = m_liteApp->settings()->value(LITEAPP_EDITTABSENABLEWHELL,true).toBool();
    ui->editorTabsEnableWhellCheckBox->setChecked(b8);

    int id = m_liteApp->settings()->value(LITEAPP_TOOLBARICONSIZE,0).toInt();
    if (id >= 0 && id < ui->buttonGroup->buttons().size()) {
        ui->buttonGroup->buttons().at(id)->setChecked(true);
    }

    connect(ui->autoIdleSaveDocumentsCheckBox,SIGNAL(toggled(bool)),ui->autoIdleSaveDocumentsTimeSpinBox,SLOT(setEnabled(bool)));

    bool b9 = m_liteApp->settings()->value(LITEAPP_AUTOIDLESAVEDOCUMENTS,false).toBool();
    ui->autoIdleSaveDocumentsCheckBox->setChecked(b9);

    int time = m_liteApp->settings()->value(LITEAPP_AUTOIDLESAVEDOCUMENTS_TIME,3).toInt();
    if (time < 1) {
        time = 1;
    }
    ui->autoIdleSaveDocumentsTimeSpinBox->setValue(time);

    bool toolwndshortcuts = m_liteApp->settings()->value(LITEAPP_TOOLWINDOW_SHORTCUTS,true).toBool();
    ui->toolWindowShortcutsCheckBox->setChecked(toolwndshortcuts);

    connect(ui->customIconCheckBox,SIGNAL(toggled(bool)),ui->iconPathComboBox,SLOT(setEnabled(bool)));

    bool customeIcon = m_liteApp->settings()->value(LITEIDE_CUSTOMEICON,false).toBool();
    ui->customIconCheckBox->setChecked(customeIcon);
    ui->iconPathComboBox->setEnabled(customeIcon);

    QDir iconDir(m_liteApp->resourcePath()+"/liteapp/qrc");
    foreach (QFileInfo info, iconDir.entryInfoList(QDir::Dirs|QDir::NoDotAndDotDot)) {
        ui->iconPathComboBox->addItem(info.fileName());
    }
    QString iconPath = m_liteApp->settings()->value(LITEIDE_CUSTOMEICONPATH,"default").toString();
    index = ui->iconPathComboBox->findText(iconPath,Qt::MatchFixedString);
    if (index >= 0 && index < ui->iconPathComboBox->count()) {
        ui->iconPathComboBox->setCurrentIndex(index);
    }

    m_keysModel = new QStandardItemModel(0,5,this);
    m_keysModel->setHeaderData(0,Qt::Horizontal,tr("Command"));
    m_keysModel->setHeaderData(1,Qt::Horizontal,tr("Label"));
    m_keysModel->setHeaderData(2,Qt::Horizontal,tr("Shortcuts"));
    m_keysModel->setHeaderData(3,Qt::Horizontal,tr("NativeText"));
    m_keysModel->setHeaderData(4,Qt::Horizontal,tr("Standard"));
    ui->keysTreeView->setModel(m_keysModel);  
#if QT_VERSION >= 0x050000
    ui->keysTreeView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
#else
    ui->keysTreeView->header()->setResizeMode(QHeaderView::ResizeToContents);
#endif
#ifndef Q_OS_MAC
    ui->keysTreeView->header()->hideSection(3);
#endif

    ui->standardCheckBox->setChecked(true);

//    if (libgopher.isValid()) {
//        ui->gopherInfoLabel->setText(tr("libgopher is valid"));
//    } else {
//        ui->gopherInfoLabel->setText(tr("libgopher is invalid!"));
//    }
//    bool useGopher = m_liteApp->settings()->value(LITEAPP_USE_LIBGOPHER,false).toBool();
//    ui->useLibgopherCheckBox->setChecked(useGopher);

    connect(m_keysModel,SIGNAL(itemChanged(QStandardItem*)),this,SLOT(shortcutsChanaged(QStandardItem*)));
    connect(ui->resetAllButton,SIGNAL(clicked()),this,SLOT(resetAllShortcuts()));
    connect(ui->resetButton,SIGNAL(clicked()),this,SLOT(resetShortcuts()));
    connect(ui->importButton,SIGNAL(clicked()),this,SLOT(importShortcuts()));
    connect(ui->exportButton,SIGNAL(clicked()),this,SLOT(exportShortcuts()));
    connect(ui->standardCheckBox,SIGNAL(toggled(bool)),this,SLOT(reloadShortcuts()));
    connect(ui->autoLoadLastSessionCheckBox,SIGNAL(toggled(bool)),this,SLOT(autoLoadLastSessionToggled(bool)));    
    connect(ui->autoIdleSaveDocumentsCheckBox,SIGNAL(toggled(bool)),this,SLOT(autoIdleSaveDocumentsToggled(bool)));
    autoLoadLastSessionToggled(ui->autoLoadLastSessionCheckBox->isChecked());
}

LiteAppOption::~LiteAppOption()
{
    delete m_widget;
    delete ui;
}

QWidget *LiteAppOption::widget()
{
    return m_widget;
}

QString LiteAppOption::name() const
{
    return "LiteApp";
}

QString LiteAppOption::mimeType() const
{
    return OPTION_LITEAPP;
}
void LiteAppOption::apply()
{
    bool storeLocal = ui->storeLocalCheckBox->isChecked();
    QSettings global(m_liteApp->resourcePath()+"/liteapp/config/global.ini",QSettings::IniFormat);
    global.setValue(LITEIDE_STORELOCAL,storeLocal);

    int index = ui->langComboBox->currentIndex();
    if (index >= 0 && index < ui->langComboBox->count()) {
        QString lc = ui->langComboBox->itemData(index).toString();
        m_liteApp->settings()->setValue(LITEAPP_LANGUAGE,lc);
    }

    index = ui->styleComboBox->currentIndex();
    if (index >= 0 && index < ui->styleComboBox->count()) {
        QString style = ui->styleComboBox->itemData(index).toString();
        m_liteApp->settings()->setValue(LITEAPP_STYLE,style);
    }

    //QString max = ui->maxRecentLineEdit->text();
    int max = ui->maxRecentFilesSpinBox->value();
    m_liteApp->settings()->setValue(LITEAPP_MAXRECENTFILES,max);
    max = ui->maxEditorCountSpinBox->value();
    m_liteApp->settings()->setValue(LITEAPP_MAXEDITORCOUNT,max);
    //bool b = ui->autoCloseProjecEditorsCheckBox->isChecked();
   // m_liteApp->settings()->setValue(LITEAPP_AUTOCLOSEPROEJCTFILES,b);
    bool b1 = ui->autoLoadLastSessionCheckBox->isChecked();
    m_liteApp->settings()->setValue(LITEAPP_AUTOLOADLASTSESSION,b1);
    bool b2 = ui->splashVisibleCheckBox->isChecked();
    m_liteApp->settings()->setValue(LITEAPP_SPLASHVISIBLE,b2);
    bool b3 = ui->welcomeVisibleCheckBox->isChecked();
    m_liteApp->settings()->setValue(LITEAPP_WELCOMEPAGEVISIBLE,b3);
    bool b4 = ui->editorTabsClosableCheckBox->isChecked();
    m_liteApp->settings()->setValue(LITEAPP_EDITTABSCLOSABLE,b4);
    bool b5 = ui->startupReloadFilesCheckBox->isChecked();
    m_liteApp->settings()->setValue(LITEAPP_STARTUPRELOADFILES,b5);
//    bool b6 = ui->startupReloadFoldersCheckBox->isChecked();
//    m_liteApp->settings()->setValue(LITEAPP_STARTUPRELOADFOLDERS,b6);
    bool b7 = ui->fileWatcherAutoReloadCheckBox->isChecked();
    m_liteApp->settings()->setValue(LITEAPP_FILEWATCHERAUTORELOAD,b7);
    bool b8 = ui->editorTabsEnableWhellCheckBox->isChecked();
    m_liteApp->settings()->setValue(LITEAPP_EDITTABSENABLEWHELL,b8);

    bool b9 = ui->autoIdleSaveDocumentsCheckBox->isChecked();
    m_liteApp->settings()->setValue(LITEAPP_AUTOIDLESAVEDOCUMENTS,b9);

    int time = ui->autoIdleSaveDocumentsTimeSpinBox->value();
    m_liteApp->settings()->setValue(LITEAPP_AUTOIDLESAVEDOCUMENTS_TIME,time);

    bool toolwindowshortcuts = ui->toolWindowShortcutsCheckBox->isChecked();
    m_liteApp->settings()->setValue(LITEAPP_TOOLWINDOW_SHORTCUTS,toolwindowshortcuts);

    int size = ui->buttonGroup->buttons().size();
    for (int i = 0; i < size; i++) {
        if (ui->buttonGroup->buttons().at(i)->isChecked()) {
            m_liteApp->settings()->setValue(LITEAPP_TOOLBARICONSIZE,i);
            break;
        }
    }

//    bool useGopher = ui->useLibgopherCheckBox->isChecked();
//    bool oldUseGopher = m_liteApp->settings()->value(LITEAPP_USE_LIBGOPHER,false).toBool();
//    if (useGopher != oldUseGopher) {
//        m_liteApp->settings()->setValue(LITEAPP_USE_LIBGOPHER,useGopher);
//        if (!libgopher.isValid()) {
//            m_liteApp->appendLog("LiteApp",QString("libgopher is invalid"));
//        } else {
//            m_liteApp->appendLog("LiteApp",useGopher ? QString("enable use libgopher"):QString("disable use libgopher"));
//        }
//    }

    QString qss = ui->qssComboBox->currentText();
    if (!qss.isEmpty()) {
        QFile f(m_liteApp->resourcePath()+"/liteapp/qss/"+qss);
        if (f.open(QFile::ReadOnly)) {
            m_liteApp->settings()->setValue(LITEAPP_QSS,qss);
            QString styleSheet = QLatin1String(f.readAll());
            qApp->setStyleSheet(styleSheet);
        }
    }

    bool customelIcon = ui->customIconCheckBox->isChecked();
    m_liteApp->settings()->setValue(LITEIDE_CUSTOMEICON,customelIcon);

    QString iconPath = ui->iconPathComboBox->currentText();
    m_liteApp->settings()->setValue(LITEIDE_CUSTOMEICONPATH,iconPath);

    for (int i = 0; i < m_keysModel->rowCount(); i++) {
        QStandardItem *root = m_keysModel->item(i,0);
        if (!root) {
            continue;
        }
        LiteApi::IActionContext *actionContext = m_liteApp->actionManager()->actionContextForName(root->text());
        if (!actionContext) {
            continue;
        }

        for (int j = 0; j < root->rowCount(); j++) {
            QStandardItem *id = root->child(j,0);
            if (!id) {
                continue;
            }
            QStandardItem *bind = root->child(j,2);
            if (!bind) {
                continue;
            }
            m_liteApp->actionManager()->setActionShourtcuts(id->text(),bind->text());
       }
    }
}

void LiteAppOption::active()
{
    this->reloadShortcuts();
}

void LiteAppOption::reloadShortcuts()
{
    m_keysModel->removeRows(0,m_keysModel->rowCount());
    bool bCheckStandard = ui->standardCheckBox->isChecked();
    foreach(QString name, m_liteApp->actionManager()->actionContextNameList() ) {
        LiteApi::IActionContext *actionContext = m_liteApp->actionManager()->actionContextForName(name);
        if (actionContext) {
            QStandardItem *root = new QStandardItem(name);
            root->setEditable(false);
            foreach(QString id, actionContext->actionKeys()) {
                LiteApi::ActionInfo *info = actionContext->actionInfo(id);
                if (!info) {
                    continue;
                }
                if (bCheckStandard && info->standard && (info->ks == info->defks)) {
                    continue;
                }
                QStandardItem *item = new QStandardItem(id);
                item->setEditable(false);
                QStandardItem *label = new QStandardItem(info->label);
                label->setEditable(false);
                QStandardItem *std = new QStandardItem;
                std->setCheckable(true);
                std->setEnabled(false);
                std->setCheckState(info->standard?Qt::Checked:Qt::Unchecked);
                QStandardItem *bind = new QStandardItem(info->ks);
                bind->setEditable(true);
                if (info->ks != info->defks) {
                    QFont font = bind->font();
                    font.setBold(true);
                    bind->setFont(font);
                }
                QStandardItem *native = new QStandardItem(ActionManager::formatShortcutsNativeString(info->ks));
                native->setEditable(false);

                root->appendRow(QList<QStandardItem*>() << item << label << bind << native << std);
            }
            m_keysModel->appendRow(root);
        }
    }
    ui->keysTreeView->expandAll();
}

void LiteAppOption::shortcutsChanaged(QStandardItem *bind)
{
    if (!bind) {
        return;
    }
    QStandardItem *root = bind->parent();
    if (!root) {
        return;
    }
    LiteApi::IActionContext *actionContext = m_liteApp->actionManager()->actionContextForName(root->text());
     if (!actionContext) {
         return;
    }
    QStandardItem *item = root->child(bind->row(),0);
    if (!item) {
        return;
    }
    QStandardItem *native = root->child(bind->row(),3);
    LiteApi::ActionInfo *info = actionContext->actionInfo(item->text());
    if (!info) {
        return;
    }
    m_keysModel->blockSignals(true);
    bind->setText(ActionManager::formatShortcutsString(bind->text()));
    native->setText(ActionManager::formatShortcutsNativeString(bind->text()));
    m_keysModel->blockSignals(false);
    QFont font = bind->font();
    if (info->defks != bind->text()) {
        font.setBold(true);
    } else {
        font.setBold(false);
    }
    bind->setFont(font);
}

void LiteAppOption::resetAllShortcuts()
{
    for (int i = 0; i < m_keysModel->rowCount(); i++) {
        QStandardItem *root = m_keysModel->item(i,0);
        if (!root) {
            continue;
        }
        LiteApi::IActionContext *actionContext = m_liteApp->actionManager()->actionContextForName(root->text());
        if (!actionContext) {
            continue;
        }
        for (int j = 0; j < root->rowCount(); j++) {
            QStandardItem *id = root->child(j,0);
            if (!id) {
                continue;
            }
            QStandardItem *bind = root->child(j,2);
            if (!bind) {
                continue;
            }
            LiteApi::ActionInfo *info = actionContext->actionInfo(id->text());
            if (!info) {
                continue;
            }
            bind->setText(info->defks);
            QFont font = bind->font();
            font.setBold(false);
            bind->setFont(font);
        }
    }
}

void LiteAppOption::resetShortcuts()
{
    QModelIndex index = ui->keysTreeView->currentIndex();
    if (!index.isValid()) {
        return;
    }    
    QModelIndex rootIndex = index.parent();
    if (!rootIndex.isValid()) {
        return;
    }
    QStandardItem *root = m_keysModel->item(rootIndex.row());
    if (!root) {
        return;
    }
    LiteApi::IActionContext *actionContext = m_liteApp->actionManager()->actionContextForName(root->text());
    if (!actionContext) {
        return;
    }
    QStandardItem *id = root->child(index.row(),0);
    if (!id) {
        return;
    }
    QStandardItem *bind = root->child(index.row(),2);
    if (!bind) {
        return;
    }
    LiteApi::ActionInfo *info = actionContext->actionInfo(id->text());
    if (!info) {
        return;
    }
    bind->setText(info->defks);
    QFont font = bind->font();
    font.setBold(false);
    bind->setFont(font);
}

void LiteAppOption::importShortcuts()
{
    QString dir = m_liteApp->resourcePath()+"/liteapp/kms";
    QString filePath = QFileDialog::getOpenFileName(m_liteApp->mainWindow(),tr("Import Keyboard Mapping Scheme"),dir,QString(tr("Keyboard Mapping Scheme (%1)")).arg("*.kms"));
    if (filePath.isEmpty()) {
        return;
    }
    QSettings read(filePath,QSettings::IniFormat);
    int version = read.value("liteidex/version",0).toInt();
    if (version < 1) {
        QMessageBox::critical(m_liteApp->mainWindow(),"Import Error",QString(tr("Could not read scheme from %1!")).arg(filePath));
        return;
    }

    for (int i = 0; i < m_keysModel->rowCount(); i++) {
        QStandardItem *root = m_keysModel->item(i,0);
        for (int j = 0; j < root->rowCount(); j++) {
            QStandardItem *id = root->child(j,0);
            if (!id) {
                continue;
            }
            QStandardItem *bind = root->child(j,2);
            if (!bind) {
                continue;
            }
            QVariant val = read.value(root->text()+"/"+id->text());
            if (!val.isValid()) {
                continue;
            }
            bind->setText(ActionManager::formatShortcutsString(val.toString()));
        }
    }
}

void LiteAppOption::exportShortcuts()
{
    QString dir = m_liteApp->resourcePath()+"/liteapp/kms";
    QString filePath = QFileDialog::getSaveFileName(m_liteApp->mainWindow(),tr("Export Keyboard Mapping Scheme"),dir,QString(tr("Keyboard Mapping Scheme (%1)")).arg("*.kms"));
    if (filePath.isEmpty()) {
        return;
    }
    QFileInfo info(filePath);
    if (info.suffix() != "kms") {
        filePath += ".kms";
    }

    QSettings write(filePath,QSettings::IniFormat);
    if (!write.isWritable()) {
        QMessageBox::critical(m_liteApp->mainWindow(),"Export Error",QString(tr("Could not write scheme to %1!")).arg(filePath));
        return;
    }
    write.clear();
    write.setValue("liteidex/version",1);
    for (int i = 0; i < m_keysModel->rowCount(); i++) {
        QStandardItem *root = m_keysModel->item(i,0);
        for (int j = 0; j < root->rowCount(); j++) {
            QStandardItem *id = root->child(j,0);
            if (!id) {
                continue;
            }
            QStandardItem *bind = root->child(j,2);
            if (!bind) {
                continue;
            }
            write.setValue(root->text()+"/"+id->text(),bind->text());
        }
    }
}

void LiteAppOption::autoLoadLastSessionToggled(bool b)
{
    //ui->startupReloadFoldersCheckBox->setEnabled(b);
    ui->startupReloadFilesCheckBox->setEnabled(b);
}

void LiteAppOption::autoIdleSaveDocumentsToggled(bool /*b*/)
{

}
