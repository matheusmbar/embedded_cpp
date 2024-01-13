#pragma once

class Test_CPP {
 public:
  explicit Test_CPP(int value);

  int getValue() const;
  void setValue(int value);

  int checkGlobals(void) const;

 private:
  int value_;
};

int check_inits(void);
