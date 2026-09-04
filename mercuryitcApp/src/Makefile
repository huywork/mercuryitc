TOP=../..

include $(TOP)/configure/CONFIG

# -------------------------------
# Build an Diamond Support Module
# -------------------------------

LIBRARY_IOC += mercuryitc

# xxxRecord.h will be created from xxxRecord.dbd
#DBDINC += xxx.h

# mercuryitcRecord.dbd will be installed into <top>/dbd
#DBD += xxx.dbd
DBD += mercuryitc.dbd

# mercuryitc.dbd will be created from these files
mercuryitc_DBD += base.dbd
mercuryitc_DBD += asyn.dbd
lakeshore340_DBD += drvVxi11.dbd
mercuryitc_DBD += stream.dbd

#Include files
#INC += xxx.h

# The following are compiled and added to the support library
#xxx_SRCS += xxxCodeA.c
#xxx_SRCS += xxxCodeB.c

# We need to link against the EPICS Base libraries
#xxx_LIBS += $(EPICS_BASE_IOC_LIBS)
mercuryitc_LIBS += $(EPICS_BASE_IOC_LIBS)

# The following adds object code from base/src/vxWorks
currAmp_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

include $(TOP)/configure/RULES
