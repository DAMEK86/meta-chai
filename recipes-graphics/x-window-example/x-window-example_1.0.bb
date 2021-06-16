# set the following variable to your one and only application which should
# be started

INITIAL_APP_PKGS ?= "htop"
INITIAL_PATH ?= "/usr/bin"
X_APPLICATION ?= "${INITIAL_PATH}/htop"

FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"
require recipes-graphics/x-window-example/x-window-example.inc