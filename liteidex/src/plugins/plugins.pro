# USE .subdir AND .depends !
# OTHERWISE PLUGINS WILL BUILD IN WRONG ORDER (DIRECTORIES ARE COMPILED IN PARALLEL)
include (../../liteidex.pri)

TEMPLATE  = subdirs

SUBDIRS = \
    liteeditor \
    litefind    \
    quickopen \
#    filebrowser \
#    bookmarks   \
#    welcome


contains(DEFINES, LITEIDE_QTWEBKIT) {
    SUBDIRS += webkithtmlwidget
}
