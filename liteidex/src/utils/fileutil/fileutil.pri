LIBS *= -l$$qtLibraryName(fileutil)
win32 {
    LIBS += -lole32 -lshell32
}



