noinst_LIBRARIES = libipc.a

if BUILD_BAYTRAIL
libipc_a_SOURCES = \
	ipc.c \
	intel-ipc.c \
	byt-ipc.c \
	pmc-ipc.c \
	dma-copy.c
endif

if BUILD_CHERRYTRAIL
libipc_a_SOURCES = \
	ipc.c \
	intel-ipc.c \
	byt-ipc.c \
	pmc-ipc.c \
	dma-copy.c
endif

libipc_a_CFLAGS = \
	$(ARCH_CFLAGS) \
	$(ARCH_INCDIR) \
	$(REEF_INCDIR) \
	$(PLATFORM_INCDIR)

