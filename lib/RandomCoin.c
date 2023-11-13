#include "RandomCoin.h"

void random_coin_new(RandomCoin **p_this) {
  RandomCoin *this;

  this = newo(p_this);

  random_new(&this->_random);
}

void random_coin_new_seed(RandomCoin **p_this, uint seed) {
  RandomCoin *this;

  this = newo(p_this);

  random_new_seed(&this->_random, seed);
}

void random_coin_new_offset(RandomCoin **p_this, uint offset) {
  RandomCoin *this;

  this = newo(p_this);

  random_new_offset(&this->_random, offset);
}

void random_coin_delete(RandomCoin **p_this) {
  RandomCoin *this;

  if ((this = *p_this) == null) {
    return;
  }

  random_delete(&this->_random);

  deleteo(p_this);
}

bool random_coin_flip(RandomCoin *this) {
  bool b;

  if (this->bitsLeft == 0) {
    this->bits = random_ulong(this->_random);
    this->bitsLeft = 64;
  }

  b = (this->bits & 0x1) == 0x1;

  this->bits >>= 1;
  this->bitsLeft--;

  return b;
}
