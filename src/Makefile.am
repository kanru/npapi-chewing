lib_LTLIBRARIES = npchewing.la
npchewing_la_SOURCES = \
  chewing_dso.cpp \
  chewing_dso.h \
  np_chewing.cpp \
  np_chewing_class.cpp \
  np_chewing_class.h \
  $(NULL)
npchewing_la_CPPFLAGS = \
  -I$(top_srcdir)/npapi \
  -DXP_UNIX=1 \
  $(NULL)
npchewing_la_CXXFLAGS = -fno-rtti -fno-exceptions -fvisibility=hidden
npchewing_la_LDFLAGS = -module -release $(VERSION)
