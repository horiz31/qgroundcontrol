LinuxBuild {
    DEFINES += HAVE_QT_X11 HAVE_QT_EGLFS HAVE_QT_WAYLAND
} else:MacBuild {
    DEFINES += HAVE_QT_MAC
} else:iOSBuild {
    DEFINES += HAVE_QT_IOS
} else:WindowsBuild {
    DEFINES += HAVE_QT_WIN32 HAVE_QT_QPA_HEADER
    LIBS += opengl32.lib user32.lib
} else:AndroidBuild {
    DEFINES += HAVE_QT_ANDROID
}

SOURCES += \
    libs/qmlglsink/gst-plugins-good/ext/qt/gstplugin.cc \
    libs/qmlglsink/gst-plugins-good/ext/qt/gstqtglutility.cc \
    libs/qmlglsink/gst-plugins-good/ext/qt/gstqsgtexture.cc \
    libs/qmlglsink/gst-plugins-good/ext/qt/gstqtsink.cc \
    libs/qmlglsink/gst-plugins-good/ext/qt/gstqtsrc.cc \
    libs/qmlglsink/gst-plugins-good/ext/qt/qtwindow.cc \
    libs/qmlglsink/gst-plugins-good/ext/qt/qtitem.cc



HEADERS += \
    libs/qmlglsink/gst-plugins-good/ext/qt/gstqsgtexture.h \
    libs/qmlglsink/gst-plugins-good/ext/qt/gstqtgl.h \
    libs/qmlglsink/gst-plugins-good/ext/qt/gstqtglutility.h \
    libs/qmlglsink/gst-plugins-good/ext/qt/gstqtsink.h \
    libs/qmlglsink/gst-plugins-good/ext/qt/gstqtsrc.h \
    libs/qmlglsink/gst-plugins-good/ext/qt/qtwindow.h \
    libs/qmlglsink/gst-plugins-good/ext/qt/qtitem.h

#cheating here. I couldn't figure out how to include RidgeRun interpipes as its own thing,
#so I'm sneaking it into the videosink plugin
HEADERS += \
    libs/qmlglsink/gst-plugins-good/ext/qt/gstinterpipe.h \
    libs/qmlglsink/gst-plugins-good/ext/qt/gstinterpipeilistener.h \
    libs/qmlglsink/gst-plugins-good/ext/qt/gstinterpipeinode.h \
    libs/qmlglsink/gst-plugins-good/ext/qt/gstinterpipesink.h \
    libs/qmlglsink/gst-plugins-good/ext/qt/gstinterpipesrc.h

SOURCES += \
libs/qmlglsink/gst-plugins-good/ext/qt/gstinterpipe.c \
libs/qmlglsink/gst-plugins-good/ext/qt/gstinterpipeilistener.c \
libs/qmlglsink/gst-plugins-good/ext/qt/gstinterpipeinode.c \
libs/qmlglsink/gst-plugins-good/ext/qt/gstinterpipesink.c \
libs/qmlglsink/gst-plugins-good/ext/qt/gstinterpipesrc.c

#cheating here. I couldn't figure out how to include mpegtsmux,
#so I'm sneaking it into the videosink plugin
#HEADERS += \
#    libs/qmlglsink/gst-plugins-good/ext/qt/gstatscmux.h \
#    libs/qmlglsink/gst-plugins-good/ext/qt/gstbasetsmux.h \
#    libs/qmlglsink/gst-plugins-good/ext/qt/gstbasetsmuxaac.h \
#    libs/qmlglsink/gst-plugins-good/ext/qt/gstbasetsmuxjpeg2000.h \
#    libs/qmlglsink/gst-plugins-good/ext/qt/gstbasetsmuxopus.h \
#    libs/qmlglsink/gst-plugins-good/ext/qt/gstbasetsmuxtxt.h \
#    libs/qmlglsink/gst-plugins-good/ext/qt/gstmpegtsmux.h \
#    libs/qmlglsink/gst-plugins-good/ext/qt/tsmux/tsmux.h \
#    libs/qmlglsink/gst-plugins-good/ext/qt/tsmux/tsmuxcommon.h \
#    libs/qmlglsink/gst-plugins-good/ext/qt/tsmux/tsmuxstream.h

#SOURCES += \
#libs/qmlglsink/gst-plugins-good/ext/qt/gstatscmux.c \
#libs/qmlglsink/gst-plugins-good/ext/qt/gstbasetsmux.c \
#libs/qmlglsink/gst-plugins-good/ext/qt/gstbasetsmuxaac.c \
#libs/qmlglsink/gst-plugins-good/ext/qt/gstbasetsmuxjpeg2000.c \
#libs/qmlglsink/gst-plugins-good/ext/qt/gstbasetsmuxopus.c \
#libs/qmlglsink/gst-plugins-good/ext/qt/gstbasetsmuxtxt.c \
#libs/qmlglsink/gst-plugins-good/ext/qt/gstmpegtsmux.c \
#libs/qmlglsink/gst-plugins-good/ext/qt/tsmux/tsmux.c \
#libs/qmlglsink/gst-plugins-good/ext/qt/tsmux/tsmuxstream.c
