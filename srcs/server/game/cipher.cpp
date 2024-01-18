#include "stdafx.h"

#include "cipher.h"

#ifdef _IMPROVED_PACKET_ENCRYPTION_

#include <cryptopp/modes.h>
#include <cryptopp/nbtheory.h>
#include <cryptopp/osrng.h>

// Diffie-Hellman key agreement
#include <cryptopp/dh.h>
#include <cryptopp/dh2.h>

// AES winner and candidates
//#include <cryptopp/aes.h>
#include <cryptopp/cast.h>
#include <cryptopp/rc6.h>
#include <cryptopp/mars.h>
#include <cryptopp/serpent.h>
#include <cryptopp/twofish.h>
// Other block ciphers
#include <cryptopp/blowfish.h>
#include <cryptopp/camellia.h>
#include <cryptopp/des.h>
#include <cryptopp/idea.h>
#include <cryptopp/rc5.h>
#include <cryptopp/seed.h>
#include <cryptopp/shacal2.h>
#include <cryptopp/skipjack.h>
#include <cryptopp/tea.h>

using namespace CryptoPP;

// Block cipher algorithm selector abstract base class.
struct BlockCipherAlgorithm {
  enum {
    kDefault, // to give more chances to default algorithm
    // AES winner and candidates
//    kAES, // Rijndael
    kRC6,
    kMARS,
    kTwofish,
    kSerpent,
    kCAST256,
    // Other block ciphers
    kIDEA,
    k3DES, // DES-EDE2
    kCamellia,
    kSEED,
    kRC5,
    kBlowfish,
    kTEA,
//    kSKIPJACK,
    kSHACAL2,
    // End sentinel
    kMaxAlgorithms
  };

  BlockCipherAlgorithm() {}
  virtual ~BlockCipherAlgorithm() {}

  static BlockCipherAlgorithm* Pick(int hint);

  virtual int GetBlockSize() const = 0;
  virtual int GetDefaultKeyLength() const = 0;
  virtual int GetIVLength() const = 0;

  virtual SymmetricCipher* CreateEncoder(const byte* key, size_t keylen,
                                         const byte* iv) const = 0;
  virtual SymmetricCipher* CreateDecoder(const byte* key, size_t keylen,
                                         const byte* iv) const = 0;
};

// Block cipher (with CTR mode) algorithm selector template class.
template<class T>
struct BlockCipherDetail : public BlockCipherAlgorithm {
  BlockCipherDetail() {}
  virtual ~BlockCipherDetail() {}

  virtual int GetBlockSize() const { return T::BLOCKSIZE; }
  virtual int GetDefaultKeyLength() const { return T::DEFAULT_KEYLENGTH; }
  virtual int GetIVLength() const { return T::IV_LENGTH; }

  virtual SymmetricCipher* CreateEncoder(const byte* key, size_t keylen,
                                         const byte* iv) const {
    return new typename CTR_Mode<T>::Encryption(key, keylen, iv);
  }
  virtual SymmetricCipher* CreateDecoder(const byte* key, size_t keylen,
                                         const byte* iv) const {
    return new typename CTR_Mode<T>::Decryption(key, keylen, iv);
  }
};

// Key agreement scheme abstract class.
class KeyAgreement {
 public:
  KeyAgreement() {}
  virtual ~KeyAgreement() {}

  virtual size_t Prepare(void* buffer, size_t* length) = 0;
  virtual bool Agree(size_t agreed_length, const void* buffer, size_t length) = 0;

  const SecByteBlock& shared() const { return shared_; }

 protected:
  SecByteBlock shared_;
};

// Crypto++ Unified Diffie-Hellman key agreement scheme implementation.
class DH2KeyAgreement : public KeyAgreement {
 public:
  DH2KeyAgreement();
  virtual ~DH2KeyAgreement();

  virtual size_t Prepare(void* buffer, size_t* length);
  virtual bool Agree(size_t agreed_length, const void* buffer, size_t length);

 private:
  DH dh_;
  DH2 dh2_;
  SecByteBlock spriv_key_;
  SecByteBlock epriv_key_;
};

Cipher::Cipher()
    : activated_(false), encoder_(NULL), decoder_(NULL), key_agreement_(NULL) {
}

Cipher::~Cipher() {
  if (activated_) {
    CleanUp();
  }
}

void Cipher::CleanUp() {
  if (encoder_ != NULL) {
    delete encoder_;
    encoder_ = NULL;
  }
  if (decoder_ != NULL) {
    delete decoder_;
    decoder_ = NULL;
  }
  if (key_agreement_ != NULL) {
    delete key_agreement_;
    key_agreement_ = NULL;
  }
  activated_ = false;
}

size_t Cipher::Prepare(void* buffer, size_t* length) {
  assert(key_agreement_ == NULL);
  key_agreement_ = new DH2KeyAgreement();
  assert(key_agreement_ != NULL);
  size_t agreed_length = key_agreement_->Prepare(buffer, length);
  if (agreed_length == 0) {
    delete key_agreement_;
    key_agreement_ = NULL;
  }
  return agreed_length;
}

bool Cipher::Activate(bool polarity, size_t agreed_length,
                      const void* buffer, size_t length) {
  assert(activated_ == false);
  assert(key_agreement_ != NULL);
  if (activated_ != false)
	  return false;

  if (key_agreement_->Agree(agreed_length, buffer, length)) {
    activated_ = SetUp(polarity);
  }
  delete key_agreement_;
  key_agreement_ = NULL;
  return activated_;
}

bool Cipher::SetUp(bool polarity) {
  assert(key_agreement_ != NULL);
  const SecByteBlock& shared = key_agreement_->shared();

  // Pick a block cipher algorithm

  if (shared.size() < 2) {
    return false;
  }
  int hint_0 = shared.BytePtr()[*(shared.BytePtr()) % shared.size()];
  int hint_1 = shared.BytePtr()[*(shared.BytePtr() + 1) % shared.size()];
  BlockCipherAlgorithm* detail_0 = BlockCipherAlgorithm::Pick(hint_0);
  BlockCipherAlgorithm* detail_1 = BlockCipherAlgorithm::Pick(hint_1);
  assert(detail_0 != NULL);
  assert(detail_1 != NULL);
  std::unique_ptr<BlockCipherAlgorithm> algorithm_0(detail_0);
  std::unique_ptr<BlockCipherAlgorithm> algorithm_1(detail_1);

  const size_t key_length_0 = algorithm_0->GetDefaultKeyLength();
  const size_t iv_length_0 = algorithm_0->GetBlockSize();
  if (shared.size() < key_length_0 || shared.size() < iv_length_0) {
    return false;
  }
  const size_t key_length_1 = algorithm_1->GetDefaultKeyLength();
  const size_t iv_length_1 = algorithm_1->GetBlockSize();
  if (shared.size() < key_length_1 || shared.size() < iv_length_1) {
    return false;
  }

  // Pick encryption keys and initial vectors

  SecByteBlock key_0(key_length_0), iv_0(iv_length_0);
  SecByteBlock key_1(key_length_1), iv_1(iv_length_1);

  size_t offset;

  key_0.Assign(shared, key_length_0);
  offset = key_length_0;
#ifdef __GNUC__
  offset = std::min(key_length_0, shared.size() - key_length_1);
#else
  offset = min(key_length_0, shared.size() - key_length_1);
#endif
  key_1.Assign(shared.BytePtr() + offset, key_length_1);

  offset = shared.size() - iv_length_0;
  iv_0.Assign(shared.BytePtr() + offset, iv_length_0);
  offset = (offset < iv_length_1 ? 0 : offset - iv_length_1);
  iv_1.Assign(shared.BytePtr() + offset, iv_length_1);

  // Create encryption/decryption objects

  if (polarity) {
    encoder_ = algorithm_1->CreateEncoder(key_1, key_1.size(), iv_1);
    decoder_ = algorithm_0->CreateDecoder(key_0, key_0.size(), iv_0);
  } else {
    encoder_ = algorithm_0->CreateEncoder(key_0, key_0.size(), iv_0);
    decoder_ = algorithm_1->CreateDecoder(key_1, key_1.size(), iv_1);
  }
  assert(encoder_ != NULL);
  assert(decoder_ != NULL);
  return true;
}

BlockCipherAlgorithm* BlockCipherAlgorithm::Pick(int hint) {
  BlockCipherAlgorithm* detail;
  int selector = hint % kMaxAlgorithms;
  switch (selector) {
//    case kAES:
//      detail = new BlockCipherDetail<AES>();
      break;
    case kRC6:
      detail = new BlockCipherDetail<RC6>();
      break;
    case kMARS:
      detail = new BlockCipherDetail<MARS>();
      break;
    case kTwofish:
      detail = new BlockCipherDetail<Twofish>();
      break;
    case kSerpent:
      detail = new BlockCipherDetail<Serpent>();
      break;
    case kCAST256:
      detail = new BlockCipherDetail<CAST256>();
      break;
    case kIDEA:
      detail = new BlockCipherDetail<IDEA>();
      break;
    case k3DES:
      detail = new BlockCipherDetail<DES_EDE2>();
      break;
    case kCamellia:
      detail = new BlockCipherDetail<Camellia>();
      break;
    case kSEED:
      detail = new BlockCipherDetail<SEED>();
      break;
    case kRC5:
      detail = new BlockCipherDetail<RC5>();
      break;
    case kBlowfish:
      detail = new BlockCipherDetail<Blowfish>();
      break;
    case kTEA:
      detail = new BlockCipherDetail<TEA>();
      break;
//    case kSKIPJACK:
//      detail = new BlockCipherDetail<SKIPJACK>();
//      break;
    case kSHACAL2:
      detail = new BlockCipherDetail<SHACAL2>();
      break;
    case kDefault:
    default:
      detail = new BlockCipherDetail<Twofish>(); // default algorithm
      break;
  }
  return detail;
}

DH2KeyAgreement::DH2KeyAgreement() : dh_(), dh2_(dh_) {
}

DH2KeyAgreement::~DH2KeyAgreement() {
}

size_t DH2KeyAgreement::Prepare(void* buffer, size_t* length) {
	Integer p("0xAD107E1E9123A9D0D660FAA79559C51FA20D64E5683B9FD1"
			  "B54B1597B61D0A75E6FA141DF95A56DBAF9A3C407BA1DF15"
			  "EB3D688A309C180E1DE6B85A1274A0A66D3F8152AD6AC212"
			  "9037C9EDEFDA4DF8D91E8FEF55B7394B7AD5B7D0B6C12207"
			  "C9F98D11ED34DBF6C6BA0B2C8BBC27BE6A00E0A0B9C49708"
			  "B3BF8A317091883681286130BC8985DB1602E714415D9330"
			  "278273C7DE31EFDC7310F7121FD5A07415987D9ADC0A486D"
			  "CDF93ACC44328387315D75E198C641A480CD86A1B9E587E8"
			  "BE60E69CC928B2B9C52172E413042E9B23F10B0E16E79763"
			  "C9B53DCF4BA80A29E3FB73C16B8E75B97EF363E2FFA31F71"
			  "CF9DE5384E71B81C0AC4DFFE0C10E64F");

	Integer g("0xAC4032EF4F2D9AE39DF30B5C8FFDAC506CDEBE7B89998CAF"
			  "74866A08CFE4FFE3A6824A4E10B9A6F0DD921F01A70C4AFA"
			  "AB739D7700C29F52C57DB17C620A8652BE5E9001A8D66AD7"
			  "C17669101999024AF4D027275AC1348BB8A762D0521BC98A"
			  "E247150422EA1ED409939D54DA7460CDB5F6C6B250717CBE"
			  "F180EB34118E98D119529A45D6F834566E3025E316A330EF"
			  "BB77A86F0C1AB15B051AE3D428C8F8ACB70A8137150B8EEB"
			  "10E183EDD19963DDD9E263E4770589EF6AA21E7F5F2FF381"
			  "B539CCE3409D13CD566AFBB48D6C019181E1BCFE94B30269"
			  "EDFE72FE9B6AA4BD7B5A0F1C71CFFF4C19C418E1F6EC0179"
			  "81BC087F2A7065B384B890D3191F2BFA");

	Integer q("0x801C0D34C58D93FE997177101F80535A4738CEBCBF389A99B36371EB");

  // Schnorr Group primes are of the form p = rq + 1, p and q prime. They
  // provide a subgroup order. In the case of 1024-bit MODP Group, the
  // security level is 80 bits (based on the 160-bit prime order subgroup).

  // For a compare/contrast of using the maximum security level, see
  // dh-unified.zip. Also see http://www.cryptopp.com/wiki/Diffie-Hellman
  // and http://www.cryptopp.com/wiki/Security_level .

  AutoSeededRandomPool rnd;

  dh_.AccessGroupParameters().Initialize(p, q, g);

  if(!dh_.GetGroupParameters().ValidateGroup(rnd, 3)) {
    // Failed to validate prime and generator
    return 0;
  }

  p = dh_.GetGroupParameters().GetModulus();
  q = dh_.GetGroupParameters().GetSubgroupOrder();
  g = dh_.GetGroupParameters().GetGenerator();

  // http://groups.google.com/group/sci.crypt/browse_thread/thread/7dc7eeb04a09f0ce
  Integer v = ModularExponentiation(g, q, p);
  if(v != Integer::One()) {
    // Failed to verify order of the subgroup
    return 0;
  }

  //////////////////////////////////////////////////////////////

  spriv_key_.New(dh2_.StaticPrivateKeyLength());
  epriv_key_.New(dh2_.EphemeralPrivateKeyLength());
  SecByteBlock spub_key(dh2_.StaticPublicKeyLength());
  SecByteBlock epub_key(dh2_.EphemeralPublicKeyLength());

  dh2_.GenerateStaticKeyPair(rnd, spriv_key_, spub_key);
  dh2_.GenerateEphemeralKeyPair(rnd, epriv_key_, epub_key);

  // Prepare key agreement data
  const size_t spub_key_length = spub_key.size();
  const size_t epub_key_length = epub_key.size();
  const size_t data_length = spub_key_length + epub_key_length;
  if (*length < data_length) {
    // Not enough data buffer length
    return 0;
  }
  *length = data_length;
  byte* buf = (byte*)buffer;
  memcpy(buf, spub_key.BytePtr(), spub_key_length);
  memcpy(buf + spub_key_length, epub_key.BytePtr(), epub_key_length);

  return dh2_.AgreedValueLength();
}

bool DH2KeyAgreement::Agree(size_t agreed_length, const void* buffer, size_t length) {
  if (agreed_length != dh2_.AgreedValueLength()) {
    // Shared secret size mismatch
    return false;
  }
  const size_t spub_key_length = dh2_.StaticPublicKeyLength();
  const size_t epub_key_length = dh2_.EphemeralPublicKeyLength();
  if (length != (spub_key_length + epub_key_length)) {
    // Wrong data length
    return false;
  }
  shared_.New(dh2_.AgreedValueLength());
  const byte* buf = (const byte*)buffer;
  if (!dh2_.Agree(shared_, spriv_key_, epriv_key_, buf, buf + spub_key_length)) {
    // Failed to reach shared secret
    return false;
  }
  return true;
}

#endif // _IMPROVED_PACKET_ENCRYPTION_

// EOF cipher.cpp
