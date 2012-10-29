lib_LTLIBRARIES = npchewing.la
npchewing_la_SOURCES = \
        np_chewing.cpp \
        np_chewing_class.cpp \
        $(NULL)
npchewing_la_CPPFLAGS = \
        -I$(top_srcdir)/npapi \
        -DXP_UNIX=1 \
        $(NULL)
npchewing_la_CXXFLAGS = -fno-rtti -fno-exceptions
npchewing_la_LDFLAGS = -module -release $(VERSION)
