#Copyright: (C) 2016 Universidad Carlos III de Madrid
# Author: Juan G. Victores, Raul Fernandez-Fernandez
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

#IF (ENABLE_openraveplugins)

add_subdirectory(OpenraveYarpPaintSquares)
add_subdirectory(OpenraveWorldRpcResponder)
add_subdirectory(OpenraveYarpForceEstimator)
add_subdirectory(OpenraveYarpPluginLoader)

add_subdirectory(OpenraveYarpCoupled)

#ENDIF (ENABLE_openraveplugins)

