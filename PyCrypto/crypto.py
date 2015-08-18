""" Crypto.py:
        Set of crypto functions and operations for Google Maps APIs
"""
__author__ = "Faissal Elamraoui"
__copyright__ = "Copyright 2015, amr.faissal@gmail.com"
__license__ = "The MIT Licence"
__version__ = "1.0"
__maintainer__ = "Faissal Elamraoui"
__email__ = "amr.faissal@gmail.com"
__status__ = "Production"

import base64
import hashlib
import hmac

""" Signs the path+query part of the URL using the provided private key.
    :param urlToSign: the path+query part of the URL
    :param privateKey: the base64 encoded binary secret
    :return string: base64 encoded signature
"""
def url_signer(urlToSign, privateKey):
    signature = hmac.new(base64.urlsafe_b64decode(privateKey), urlToSign, hashlib.sha1)
    return base64.urlsafe_b64encode(signature.digest())

""" URL encodes the parameters.
    :param params: The parameters
    :type string: URL encoded parameters
"""
def urlencode_params(params):
    params = sorted(params.items())
    return "&".join("%s=%s" % (k,v) for k,v in params)
