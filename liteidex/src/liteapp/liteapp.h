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
// Module: liteapp.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef LITEAPP_H
#define LITEAPP_H

#include "liteapi/liteapi.h"
#include "textoutput/textoutput.h"
#include "extension/extension.h"
#include "goproxy.h"


using namespace LiteApi;

class MainWindow;
class PluginManager;
class EditorManager;
class FileManager;
class ProjectManager;
class ActionManager;
class MimeTypeManager;
class OptionManager;
class ToolWindowManager;
class HtmlWidgetManager;
class QuickOpenManager;
class RecentManager;
class QSettings;
class QSplitter;
class QComboBox;
class LiteAppOptionFactory;
class AppIdleTimer;

class LiteApp : public IApplication
{
    Q_OBJECT
public:
    static QString getRootPath();
    static QString getToolPath();
    static QString getPluginPath();
    static QString getResoucePath();
    static QString getStoragePath();
    static IApplication* NewApplication(const QString &sessionName, IApplication *base = 0);
    static PluginManager *pluginManager();
    static QList<IApplication*> appList();
public:
    LiteApp();    
    virtual ~LiteApp();    
    virtual IExtension *extension();
    virtual IApplication *newInstance(const QString &sessionName);
    virtual QList<IApplication*> instanceList() const;
    virtual bool hasGoProxy() const;
    virtual IGoProxy *createGoProxy(QObject *parent);
    virtual IProjectManager *projectManager();
    virtual IEditorManager  *editorManager();
    virtual IFileManager    *fileManager();
    virtual IActionManager  *actionManager();
    virtual IMimeTypeManager *mimeTypeManager();
    virtual IOptionManager  *optionManager();
    virtual IToolWindowManager *toolWindowManager();
    virtual IHtmlWidgetManager *htmlWidgetManager();
    virtual IRecentManager *recentManager();

    virtual QMainWindow *mainWindow() const;
    virtual QSettings *settings();
    virtual QMap<QString,QVariant> &globalCookie();

    virtual QString rootPath() const;
    virtual QString applicationPath() const;
    virtual QString toolPath() const;
    virtual QString resourcePath() const;
    virtual QString pluginPath() const;
    virtual QString storagePath() const;

    virtual QString ideVersion() const;
    virtual QString ideFullName() const;
    virtual QString ideName() const;
    virtual QString ideCopyright() const;

    virtual QList<IPlugin*> pluginList() const;

    virtual void loadSession(const QString &session);
    virtual void saveSession(const QString &session);
    virtual QStringList sessionList() const;
    virtual QString currentSession() const;

    virtual void loadState();
    virtual void saveState();

    virtual void appendLog(const QString &model, const QString &log = QString(), bool error = false);
    virtual void sendBroadcast(const QString &module, const QString &id, const QString &param = QString());
public:
    void load(const QString &sessionName, IApplication *baseApp);
    void createActions();
    void createMenus();
    void createToolBars();
    void loadPlugins();
    void loadMimeType();
    void initPlugins();
    void setPluginPath(const QString &path); 
    void setResourcePath(const QString &path);
protected slots:
    void goproxyDone(const QByteArray &reply);
    void dbclickLogOutput(QTextCursor);
    void projectReloaded();
    void currentProjectChanged(LiteApi::IProject *project);
    void currentEditorChanged(LiteApi::IEditor *editor);
    void editorModifyChanged(bool);
    void cleanup();
    void aboutPlugins();
    void escape();
    void newWindow();
    void closeWindow();
    void exit();
    void applyOption(QString id);
protected:
    QString         m_currentSession;
    QString         m_rootPath;
    QString         m_applicationPath;
    QString         m_toolPath;
    QString         m_pluginPath;
    QString         m_resourcePath;
    QString         m_storagePath;
    QSettings       *m_settings;
    Extension     *m_extension;
    MainWindow      *m_mainwindow;
    ToolWindowManager *m_toolWindowManager;
    HtmlWidgetManager *m_htmlWidgetManager;
    RecentManager     *m_recentManager;
    ActionManager  *m_actionManager;
    ProjectManager *m_projectManager;
    EditorManager   *m_editorManager;
    FileManager    *m_fileManager;
    MimeTypeManager *m_mimeTypeManager;
    OptionManager   *m_optionManager;
    TextOutput    *m_logOutput;
    QAction       *m_logAct;
    LiteAppOptionFactory *m_liteAppOptionFactory;
    QList<IPlugin*> m_pluginList;
    AppIdleTimer     *m_idleTimer;
public:
    static QMap<QString,QVariant> s_cookie;
    static QList<IApplication*> s_appList;
protected:
    QAction     *m_newAct;
    QAction     *m_openFileAct;
    QAction     *m_openFolderAct;
    QAction     *m_openFolderNewWindowAct;
    QAction     *m_closeAllFolderAct;
    QAction     *m_newWindow;
    QAction     *m_closeWindow;
    QAction     *m_closeAct;
    QAction     *m_closeAllAct;
    QAction     *m_openProjectAct;
    QAction     *m_saveProjectAct;
    QAction     *m_closeProjectAct;
    QAction     *m_saveAct;
    QAction     *m_saveAsAct;
    QAction     *m_saveAllAct;
    QAction     *m_exitAct;
    QAction     *m_optionAct;
    QAction     *m_aboutAct;
    QAction     *m_aboutPluginsAct;
    QAction     *m_fullScreent;
    QToolBar    *m_stdToolBar;
    QMenu       *m_fileMenu;
    QMenu       *m_viewMenu;
    QMenu       *m_helpMenu;    
protected:
    GoProxy     *m_goProxy;
};

#endif // LITEAPP_H
