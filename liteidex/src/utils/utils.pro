# USE .subdir AND .depends !
# OTHERWISE PLUGINS WILL BUILD IN WRONG ORDER (DIRECTORIES ARE COMPILED IN PARALLEL)

TEMPLATE  = subdirs
CONFIG   += ordered

SUBDIRS = \
    fileutil \
    mimetype \
    extension \
    textoutput \
    symboltreeview \
    documentbrowser \
    modelproject \
    processex \
    colorstyle \
    htmlutil \
    editorutil \
    folderview \
    iconutil
