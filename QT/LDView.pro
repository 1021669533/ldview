SOURCES	+= QTMain.cpp ModelViewerWidget.cpp Preferences.cpp LDViewErrors.cpp \
		   LDViewExtraDir.cpp AlertHandler.cpp LDViewSnapshotSettings.cpp \
           LDViewPartList.cpp misc.cpp LDViewJpegOptions.cpp \
		   LDViewModelTree.cpp LDViewBoundingBox.cpp LDViewMainWindow.cpp \
		   LDViewLatitudeLongitude.cpp LDViewMpdModel.cpp LDViewExportOption.cpp

HEADERS	+= ModelViewerWidget.h Preferences.h LDViewErrors.h LDViewExtraDir.h \
		   AlertHandler.h LDViewPartList.h misc.h LDViewJpegOptions.h \
		   LDViewSnapshotSettings.h LDViewModelTree.h \
		   LDViewBoundingBox.h LDViewLatitudeLongitude.h LDViewMpdModel.h \
		   LDViewExportOption.h LDViewMainWindow.h Help.h About.h \
		   OpenGLExtensions.h

FORMS	= AboutPanel.ui BoundingBoxPanel.ui ErrorPanel.ui ExtraDirPanel.ui \
		  HelpPanel.ui JpegOptionsPanel.ui LDView.ui LatitudeLongitude.ui \
		  OpenGLExtensionsPanel.ui PreferencesPanel.ui \
		  SnapshotSettingsPanel.ui ExportOptionPanel.ui \
		  ModelTreePanel.ui MpdModelSelectionPanel.ui PartList.ui

LANGUAGE	= C++
TRANSLATIONS   =  	ldview_en.ts \
					ldview_de.ts \
					ldview_it.ts \
					ldview_cz.ts \
					ldview_hu.ts

RESOURCES 	= resources.qrc

TEMPLATE	= app
CONFIG		+= qt opengl thread warn_on release 
QT  		+= opengl
DEFINES		+= QT_THREAD_SUPPORT _QT
INCLUDEPATH	+= . .. ../include 
DBFILE		= LDView.db

UI_DIR 		= .ui
MOC_DIR 	= .moc
OBJECTS_DIR = .obj

DEFINES 	+= EXPORT_3DS
#DEFINES 	+= _NO_BOOST

QMAKE_CXXFLAGS += $$(TESTING)

unix {
  UNAME = $$system(uname -m)
  LDVDEV64 = $$(LDVDEV64)
  contains(UNAME, x86_64) {
    contains(LDVDEV64, YES) {
    }
    else {
      QMAKE_CFLAGS_RELEASE += -m32
      QMAKE_CFLAGS_DEBUG += -m32
      QMAKE_CXXFLAGS_RELEASE += -m32
      QMAKE_CXXFLAGS_DEBUG += -m32
      QMAKE_LFLAGS_RELEASE += -m32
      QMAKE_LFLAGS_DEBUG += -m32
    }
  }
  BOOSTLIB = -lboost_thread
  exists(/usr/lib/libboost_thread-mt.so*){
    BOOSTLIB = -lboost_thread-mt
  }
  exists(/usr/local/lib/libboost_thread-mt.so*){
    BOOSTLIB = -lboost_thread-mt
  }
  exists(/usr/lib64/libboost_thread-mt.so*){
    BOOSTLIB = -lboost_thread-mt
  }
  exists(/usr/local/lib64/libboost_thread-mt.so*){
    BOOSTLIB = -lboost_thread-mt
  }

  documentation.depends += compiler_translations_make_all
  documentation.path = /usr/local/share/ldview
  documentation.files = ../Readme.txt ../Help.html ../license.txt \
						../m6459.ldr \
						../ChangeHistory.html ../8464.mpd todo.txt \
						../Textures/SansSerif.fnt \
						ldview_de.qm ldview_cz.qm ldview_it.qm ldview_en.qm
  target.path = /usr/local/bin
  INSTALLS += documentation target
  LIBS += -L../TCFoundation -L../LDLib -L../LDLoader -L../TRE -L../boost/lib \
          -lLDraw -L../gl2ps -L../LDExporter 
  ldlib.target = ../LDLib/libLDraw.a
  ldlib.commands = cd ../LDLib ; make all
  ldlib.depends = ../LDLib/*.cpp ../LDLib/*.h
  tre.target = ../TRE/libTRE.a
  tre.commands = cd ../TRE ; make all
  tre.depends = ../TRE/*.cpp ../TRE/*.h
  tcfoundation.target = ../TCFoundation/libTCFoundation.a
  tcfoundation.commands = cd ../TCFoundation ; make all
  tcfoundation.depends = ../TCFoundation/*.cpp ../TCFoundation/*.h
  ldloader.target = ../LDLoader/libLDLoader.a
  ldloader.commands = cd ../LDLoader ; make all
  ldloader.depends = ../LDLoader/*.cpp ../LDLoader/*.h
  ldexporter.target = ../LDExporter/libLDExporter.a
  ldexporter.commands = cd ../LDExporter ; make all
  ldexporter.depends = ../LDExporter/*.cpp ../LDExporter/*.h
  gl2ps.target = ../gl2ps/libgl2ps.a
  gl2ps.commands = cd ../gl2ps ; make all
  gl2ps.depends = ../gl2ps/*.c ../gl2ps/*.h
  QMAKE_EXTRA_UNIX_TARGETS += ldlib tre tcfoundation ldloader ldexporter gl2ps
  PRE_TARGETDEPS += ../LDLib/libLDraw.a ../TRE/libTRE.a \
                    ../TCFoundation/libTCFoundation.a ../LDLoader/libLDLoader.a \
					../LDExporter/libLDExporter.a ../gl2ps/libgl2ps.a
  QMAKE_CLEAN += ../[TLg]*/.obj/*.o ../[TLg]*/lib*.a
  initrans.path = /usr/local/share/ldview
  initrans.extra = cp ../Translations/Hungarian/LDViewMessages.ini \
/usr/local/share/ldview/LDViewMessages_hu.ini ;\
cp ../Translations/Czech/LDViewMessages.ini \
/usr/local/share/ldview/LDViewMessages_cz.ini ;\
cp ../Translations/German/LDViewMessages.ini \
/usr/local/share/ldview/LDViewMessages_de.ini ; \
cp -f ../Translations/Italian/LDViewMessages.ini \
/usr/local/share/ldview/LDViewMessages_it.ini ;\
cat ../LDViewMessages.ini ../LDExporter/LDExportMessages.ini \
>/usr/local/share/ldview/LDViewMessages.ini
  INSTALLS += initrans
  ini.depends = ../LDViewMessages.ini ../LDExporter/LDExportMessages.ini
  ini.target = LDViewMessages.ini
  ini.commands = cat ../LDViewMessages.ini ../LDExporter/LDExportMessages.ini \
                 > LDViewMessages.ini
  QMAKE_EXTRA_UNIX_TARGETS += ini
  PRE_TARGETDEPS += LDViewMessages.ini
}

win32 {
  QMAKE_CXXFLAGS_RELEASE += /FI winsock2.h /FI winsock.h
#
# supress boost 1.42 warnings
#
  QMAKE_CXXFLAGS_RELEASE += /wd4675


  ini.commands = copy /y /a ..\LDViewMessages.ini+..\LDExporter\LDExportMessages.ini LDViewMessages.ini
  ini.target = LDViewMessages.ini 
  ini.depends = ../LDViewMessages.ini ../LDExporter/LDExportMessages.ini
  QMAKE_EXTRA_WIN_TARGETS += ini
  PRE_TARGETDEPS += LDViewMessages.ini

  DEFINES += _TC_STATIC
  INCLUDE += -I../../boost_1_33_1
  LIBS += -L../TCFoundation/Release -L../LDLib/Release -L../LDLoader/Release \
          -L../TRE/Release -lLDLib -L../LDExporter/Release -L../lib -lunzip32 \
		 -L../gl2ps/Release
  exists ($(VCINSTALLDIR)/bin/cl.exe) {
    LIBS    += -ltinyxml_STL -llibboost_thread-vc80-mt-s
  }
  exists ($(VCTOOLKITINSTALLDIR)/bin/cl.exe) {
    LIBS    += -ltinyxml_STL-vc2003 -llibboost_thread-vc71-mt-s
  }
  LIBS += -llib3ds
}

translations.input = TRANSLATIONS
translations.output = ${QMAKE_FILE_BASE}.qm
translations.commands = lrelease ${QMAKE_FILE_IN} -qm ${QMAKE_FILE_BASE}.qm
translations.CONFIG = no_link
QMAKE_EXTRA_COMPILERS += translations
#PRE_TARGETDEPS += compiler_translations_make_all

QMAKE_CLEAN += *.qm

unix {
		LIBS += -ljpeg
}
win32 {
		LIBS += -llibjpeg
}
LIBS	+= -lLDLoader -lTRE -lTCFoundation -lgl2ps -lLDExporter

unix {
# This has to be down here, because -ltinyxml has to come after -lLDExporter.
	LIBS	+= -ltinyxml
	LIBS += -L../lib
	UNAME = $$system(uname -m)
	LDVDEV64 = $$(LDVDEV64)
# Use static boost library on Ubuntu and other systems that have it in
# /usr/lib.
	exists(/usr/lib/libboost_thread-mt.a){
		BOOSTLIB = /usr/lib/libboost_thread-mt.a
	}
	contains(UNAME, x86_64) {
		contains(LDVDEV64, YES) {
			LIBS += -l3ds-64
			exists(/usr/lib64/libboost_thread-mt.a){
				BOOSTLIB = /usr/lib64/libboost_thread-mt.a
			}
		}
		else {
			LIBS += -l3ds
		}
	}
	else {
		LIBS += -l3ds
	}
}

LIBS += $$BOOSTLIB
LIBS += -lz -lpng

VERSION = 4.0.1
macx {
# This has to be down here, because -ltinyxml has to come after -lLDExporter.
	LIBS	+= -ltinyxml
	RC_FILE = images/LDView.icns
	helpfile.target = LDView.app/Contents/MacOS/Help.html
	helpfile.commands = @$(CHK_DIR_EXISTS) LDView.app/Contents/MacOS/ || \
		$(MKDIR) LDView.app/Contents/MacOS/ ; cp ../Help.html \
		LDView.app/Contents/MacOS
	helpfile.depends = ../Help.html
	messagefile.target = LDView.app/Contents/MacOS/LDViewMessages.ini
	messagefile.commands = @$(CHK_DIR_EXISTS) LDView.app/Contents/MacOS/ || \
		$(MKDIR) LDView.app/Contents/MacOS/ ; cp ../LDViewMessages.ini \
		LDView.app/Contents/MacOS
	messagefile.depends = ../LDViewMessages.ini
	fontfile.target = LDView.app/Contents/MacOS/SansSerif.fnt
	fontfile.commands = @$(CHK_DIR_EXISTS) LDView.app/Contents/MacOS/ || \
		$(MKDIR) LDView.app/Contents/MacOS/ ; cp ../Textures/SansSerif.fnt \
		LDView.app/Contents/MacOS
	fontfile.depends = ../Textures/SansSerif.fnt
	QMAKE_EXTRA_UNIX_TARGETS += helpfile messagefile fontfile
	POST_TARGETDEPS += LDView.app/Contents/MacOS/LDViewMessages.ini \
		LDView.app/Contents/MacOS/Help.html \
		LDView.app/Contents/MacOS/SansSerif.fnt
}
