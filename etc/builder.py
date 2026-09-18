from iocbuilder import AutoSubstitution
from iocbuilder.modules.streamDevice import AutoProtocol

class MercuryGlobal(AutoSubstitution, AutoProtocol):
    # Substitution attributes
    TemplateFile = 'MercuryGlobal.template'

    # AutoProtocol attributes
    ProtocolFiles = ['mercuryitc.proto']

'''
class MercuryTemp(AutoSubstitution, AutoProtocol):
    # Substitution attributes
    TemplateFile = 'MercuryTemp.template'

    # AutoProtocol attributes
    ProtocolFiles = ['mercuryitc.proto']
'''