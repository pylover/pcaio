add_library(task OBJECT task.c task.h)
add_library(worker OBJECT worker.c worker.h)
add_library(core OBJECT core.c)

set(CONFIG_PCAIO_STACKSIZE_MAX 32768)
set(CONFIG_PCAIO_TASKQUEUE_BITS 3)
configure_file(config.h.in config.h @ONLY)
include_directories(${PROJECT_SOURCE_DIR}/core ${PROJECT_BINARY_DIR}/core)
