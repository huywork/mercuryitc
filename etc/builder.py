from iocbuilder import AutoSubstitution
from iocbuilder.modules.streamDevice import AutoProtocol

class MercuryTemp(AutoSubstitution, AutoProtocol):
    # Substitution attributes
    TemplateFile = 'MercuryTemp.template'

    # AutoProtocol attributes
    ProtocolFiles = ['mercuryitc.proto']
