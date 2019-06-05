import sys
from Crypto.PublicKey import RSA
from Crypto.Cipher import PKCS1_OAEP, AES
from Crypto.Hash import SHA256, SHA1
from Crypto.Signature import PKCS1_v1_5, pss
from Crypto import Random
from base64 import b64encode, b64decode

AES_BLOCK_SIZE = 32
AES_RAND_SIZE = 16

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

    def aes_encrypt(self, key, plaintext):
        plaintext = self._aes_pad(plaintext)
        rand = Random.new().read(AES_RAND_SIZE)
        encrypter = AES.new(b64decode(key), AES.MODE_CBC, rand)
        return b64encode(rand+encrypter.encrypt(plaintext))

    def aes_decrypt(self, key, ciphertext):
        ciphertext = b64decode(ciphertext)
        rand = ciphertext[:AES_RAND_SIZE]
        decrypter = AES.new(b64decode(key), AES.MODE_CBC, rand)
        return self._aes_unpad(decrypter.decrypt(ciphertext[AES_RAND_SIZE:])).decode('utf-8')
        
    def _aes_pad(self, s):
        return bytes(s + (AES_BLOCK_SIZE - len(s) % AES_BLOCK_SIZE) * chr(AES_BLOCK_SIZE - len(s) % AES_BLOCK_SIZE), 'utf-8')
        
    def _aes_unpad(self, c):
        return c[:-ord(c[-1:])]
    
if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Not enough args")
    else:
        crypto = CryptoLib(sys.argv[1])
        plaintext = crypto.rsa_decrypt(sys.argv[2])
        print(plaintext)
