from iocbuilder import AutoSubstitution
from iocbuilder.modules.streamDevice import AutoProtocol

class MercuryFlow(AutoSubstitution, AutoProtocol):
    # Substitution attributes
    TemplateFile = 'MercuryFlowSPC.template'

    # AutoProtocol attributes
    ProtocolFiles = ['mercuryitc.proto']

class MercuryGlobal(AutoSubstitution, AutoProtocol):
    # Substitution attributes
    TemplateFile = 'MercuryGlobal.template'

    # AutoProtocol attributes
    ProtocolFiles = ['mercuryitc.proto']

class MercuryLevel(AutoSubstitution, AutoProtocol):
    # Substitution attributes
    TemplateFile = 'MercuryLevel.template'

    # AutoProtocol attributes
    ProtocolFiles = ['mercuryitc.proto']

class MercuryPressure(AutoSubstitution, AutoProtocol):
    # Substitution attributes
    TemplateFile = 'MercuryPressure.template'

    # AutoProtocol attributes
    ProtocolFiles = ['mercuryitc.proto']

class MercurySPCAvailable(AutoSubstitution, AutoProtocol):
    # Substitution attributes
    TemplateFile = 'MercurySPCAvailable.template'

    # AutoProtocol attributes
    ProtocolFiles = ['mercuryitc.proto']

class MercuryTemp(AutoSubstitution, AutoProtocol):
    # Substitution attributes
    TemplateFile = 'MercuryTemp.template'

    # AutoProtocol attributes
    ProtocolFiles = ['mercuryitc.proto']

class MercuryVTISPC(AutoSubstitution, AutoProtocol):
    # Substitution attributes
    TemplateFile = 'MercuryVTISPC.template'

    # AutoProtocol attributes
    ProtocolFiles = ['mercuryitc.proto']