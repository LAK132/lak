QMAKE_MAKEFILE = lakqt.mk
QMAKE_PROJ = lakqt.pro
TARGET = bin/liblakqt.a

.PHONY: $(TARGET)

$(TARGET): $(QMAKE_MAKEFILE)
	$(MAKE) -f $(QMAKE_MAKEFILE)

$(QMAKE_MAKEFILE): $(QMAKE_PROJ)
	qmake -o $@ $<

clean:
	rm -rf $(QMAKE_MAKEFILE) .qmake.stash obj/ bin/