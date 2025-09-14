# configuration entries
set(CONFIG_PCAIO_STACKSIZE_DEFAULT 0x2000)
set(CONFIG_PCAIO_MODULES_MAX 8)
set(CONFIG_PCAIO_MODULETIMEOUT_SHORT_US 50000)
set(CONFIG_PCAIO_MODULETIMEOUT_LONG_US 300000)
configure_file(config.h.in config.h @ONLY)


# private include directories
include_directories(
  ${PROJECT_SOURCE_DIR}/pcaio 
  ${PROJECT_BINARY_DIR}/pcaio
)


# core
add_library(threadpool OBJECT threadpool.c threadpool.h)
add_library(context OBJECT context.c context.h)
add_library(worker OBJECT worker.c worker.h)
add_library(task OBJECT task.c task.h)
add_library(master OBJECT master.c master.h)
add_library(pcaio_ OBJECT pcaio.c ../include/pcaio/pcaio.h)


# modules
add_library(modio OBJECT modio.c ../include/pcaio/modio.h)
add_library(modselect OBJECT modselect.c ../include/pcaio/modselect.h)
add_library(modepoll OBJECT modepoll.c ../include/pcaio/modepoll.h)

# utils
add_library(timer OBJECT timer.c ../include/pcaio/timer.h)
