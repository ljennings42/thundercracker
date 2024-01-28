APP = cauldron

include $(SDK_DIR)/Makefile.defs

OBJS = $(ASSETS).gen.o main.o loader.o textsfx.o
ASSETDEPS += images/*.png $(ASSETS).lua
CCFLAGS += -DCUBE_ALLOCATION=6
CDEPS += *.h

# build assets.html to proof stir-processed assets.
# comment out to disable.
ASSETS_BUILD_PROOF := yes

include $(SDK_DIR)/Makefile.rules
