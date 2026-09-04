from iocbuilder import AutoSubstitution
from iocbuilder.modules.streamDevice import AutoProtocol

class mercuryitc(AutoSubstitution, AutoProtocol):
    # Substitution attributes
    TemplateFile = 'mercuryitc.template'

    # AutoProtocol attributes
    ProtocolFiles = ['mercuryitc.proto']
