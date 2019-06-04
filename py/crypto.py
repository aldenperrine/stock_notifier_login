import sys
from Crypto.PublicKey import RSA
from Crypto.Cipher import PKCS1_OAEP
from Crypto.Hash import SHA256, SHA1
from Crypto.Signature import PKCS1_v1_5, pss
from base64 import b64encode, b64decode

class CryptoLib:
    def __init__(self, keypath):
        f = open(keypath, 'r')
        rsa_private = RSA.importKey(f.read())
        self.decrypter = PKCS1_OAEP.new(rsa_private, hashAlgo=SHA256, mgfunc= lambda x, y: pss.MGF1(x, y, SHA1))
        self.signer = PKCS1_v1_5.new(rsa_private)

    def rsa_decrypt(self, ciphertext):
        return self.decrypter.decrypt(b64decode(ciphertext))

    def rsa_sign(self, message):
        digest = SHA256.new()
        digest.update(message)
        return self.signer.sign(digest)
        
        
if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Not enough args")
    else:
        crypto = CryptoLib(sys.argv[1])
        plaintext = crypto.rsa_decrypt(sys.argv[2])
        print(plaintext)
